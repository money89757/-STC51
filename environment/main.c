#include "head.h"

int  goout = 0;

int shmid;                         //共享内存id
sensor_data *shmaddr;              //指向共享内存区域指针(修改为结构体指针)
key_t key;
int msgid;  

MSG msg;

pthread_t mythread;
int *thread_arg;

struct sockaddr_in ser_addr ;

int count;                             //接收数据个数
int sockfd ;                       //定义套接字
unsigned char recvdata[N] = {0};             //接收数据缓冲区
int main(int argc , char **argv)
{
	
	ser_addr.sin_family = PF_INET;
	ser_addr.sin_port = htons(50000);
	ser_addr.sin_addr.s_addr = inet_addr("192.168.0.5");

/************************************************************
 *                  创建套接字并绑定套接字 
 * *********************************************************/
	if((sockfd = socket(PF_INET , SOCK_STREAM , 0)) < 0)
	{
		printf("%s%s%d%s\n" , __FILE__ ,__FUNCTION__, __LINE__ , strerror(errno));
		exit(1);
	}

	if(connect(sockfd , (struct sockaddr*)&ser_addr , sizeof(ser_addr)) < 0)
	{
		printf("%s%s%d%s\n" , __FILE__ ,__FUNCTION__, __LINE__ , strerror(errno));
		exit(1);
	}	

	printf("port = %d , addr = %s\n" , ntohs(ser_addr.sin_port) , inet_ntoa(ser_addr.sin_addr));

/************************************************************
 *                     创建共享内存
 * *********************************************************/

	if((key = ftok("/etc" , 'r')) == -1)     //创建key值id
	{
		perror("ftok");
		exit(-1);
	}

	if((shmid = shmget(key , sizeof(sensor_data) , 0666 | IPC_CREAT | IPC_EXCL)) < 0)  //获得共享内存id
	{
		if((shmid = shmget(key , sizeof(sensor_data) , 0666)) < 0)
		{
			perror("shmget");
			exit(-1);
		}
	}

	if((shmaddr = (sensor_data*)shmat(shmid , NULL , 0)) == NULL)           //映射共享内存首地址
	{
		perror("shmat");
		exit(-1);
	}

		signal(SIGINT,handler);
		signal(SIGUSR1,handler);

	/************************************************************
	 *                       创建线程
	 * *********************************************************/
	if(pthread_create(&mythread , NULL , thread_function , (void*)&sockfd) < 0) //创建进程
	{
		perror("pthread_create failed");
		exit(-1);
	}


	while((count = recv(sockfd , recvdata ,sizeof(recvdata) , 0)) >= 0)                        //接收数据
	{
		datadeal(recvdata , count);
		bzero(recvdata , sizeof(recvdata));

		if(count == 0)
		{
			printf("Client logout............\n");
			kill(getpid() , SIGUSR1);
		}
		if( goout == 1)
			break;

	}



	if(pthread_join(mythread , (void **)&thread_arg) < 0)                              //等待线程结束
	{
		perror("fail to pthread_join");
		exit(-1);
	}
	printf("wait thread %d\n",*thread_arg);


	return 0;
}


/************************************************************
 *                       线程处理函数
 ************************************************************/
void *thread_function(void *arg)
{

	printf("This is %d thread!\n", *(int*)arg - 2); 
	unsigned char msgdata;


	if((msgid = msgget(key , 0666 | IPC_CREAT)) < 0)                                //创建消息队列
	{
		perror("msgget");
		exit(-1);
	}
	printf("msgget success!\n");

	while(1)
	{
		

			msgrcv(msgid , &msg , MSGLEN , TYPEA , 0);

			printf("thread working ....\n");
			printf("*****************************************\n");		

			switch(msg.commend)
			{
			case fan_speed_0:
				printf("fan_speed_0\n");
				msgdata = msg.commend;
				break;

			case fan_speed_1:
				printf("fan_speed_1\n");
				msgdata = msg.commend;
				break;

			case fan_speed_2:
				printf("fan_speed_2\n");
				msgdata = msg.commend;
				break;
				
			case fan_speed_3:
				printf("fan_speed_3\n");
				msgdata = msg.commend;
				break;

			case pump_off:
				printf("pump_off\n");
				msgdata = msg.commend;
				break;

			case pump_on:
				printf("pump_on\n");
				msgdata = msg.commend;
				break;

			case led1_off:
				printf("led1_off\n");
				msgdata = msg.commend;
				break;

			case led1_on:
				printf("led1_on\n");
				msgdata = msg.commend;
				break;


			case led2_off:
				printf("led2_off\n");
				msgdata = msg.commend;
				break;

			case led2_on:
				printf("led2_on\n");
				msgdata = msg.commend;
				break;

			default:
				break;
			}

			printf("Begin To Send Commend...........\n");

			send(sockfd , &msgdata , sizeof(char) , 0);

			if( goout == 1)
				break;

	}

}

/************************************************************
*                      接收到数据处理函数 
************************************************************/
void datadeal(unsigned char *arg , int count)
{
	int i;
	sensor_data data = {
		.air_temp = 0 ,
		.air_hum = 0,
		.soil_temp = 0,
		.soil_hum = 0,
		.liquid_level = 0 ,
		.co2 = 0 ,
		.light = 0
	};

	for(i = 0 ; i < count ; i++)
		printf("recvdata[%d] = %d\n" , i , arg[i]);

	
/***********************处理数据****************************/
	data.air_temp    = arg[0]*10 + arg[1];
	data.air_hum     = arg[2]*10 + arg[3];
	data.soil_temp   = arg[4]*10 + arg[5];
	data.soil_hum    = arg[6]*10 + arg[7];
	data.liquid_level= arg[8];
	data.co2         = arg[9]*100 + arg[10];
	data.light       = arg[11]*100 + arg[12];

/*******************将数据放入共享内存***********************/
	*shmaddr = data;

#if 1
	printf("air_temp = %d\n" , data.air_temp);
	printf("air_hum = %d\n" , data.air_hum);
	printf("soil_temp = %d\n" ,  data.soil_temp);
	printf("soil_hum = %d\n" , data.soil_hum );
	printf("liquid_level = %d\n" ,data.liquid_level);
	printf("co2 density = %d\n" , data.co2);
	printf("light = %d\n" , data.light);
#endif

}

/************************************************************
*                      信号处理函数 
************************************************************/
void handler(int sigio)
{
	if(sigio = SIGINT)
	{ 
		
		
		goout = 1;
		if(shmdt(shmaddr) < 0)                                                           //取消内存映射
		{
			perror("shmdt");
			exit(-1);
		}

		if(shmctl(shmid , IPC_RMID , NULL) < 0)                                         //删除对象
		{
			perror("shmctl");
			exit(-1);
		}

		printf("shmctl remove!!!\n");

		if(msgctl(msgid , IPC_RMID , NULL) < 0)
		{
			perror("msgctl");
			exit(-1);
		}
		printf("msgctl remove!!!\n");

		close(sockfd);

		
	}

	if(sigio == SIGUSR1)
	{
		close(sockfd);
		printf("****************connecting***********\n");
		if((sockfd = socket(PF_INET , SOCK_STREAM , 0)) < 0)
		{
			printf("%s%s%d%s\n" , __FILE__ ,__FUNCTION__, __LINE__ , strerror(errno));
			exit(1);
		}
		sleep(1000);
		if(connect(sockfd , (struct sockaddr*)&ser_addr , sizeof(ser_addr)) < 0)
		{
			printf("%s%s%d%s\n" , __FILE__ ,__FUNCTION__, __LINE__ , strerror(errno));
			exit(1);
		}	

		printf("***********connect sucessful!********\n");
		printf("port = %d , addr = %s\n" , ntohs(ser_addr.sin_port) , inet_ntoa(ser_addr.sin_addr));

	}


}
