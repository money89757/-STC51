#include "AI.h"

int lock = 0;
int fd_led =-1;
int fd_alarm[3];
int fd_beep = -1;
int fd_sht21 = -1;
int fd_ds18 = -1;
int fd_bh17 = -1;
int fd_stm = -1;
extern int i;
SHM *p, *shm_buf;
extern stm32_alarm(void);
extern ds18b20_alarm(void);
extern sht21_alarm(void);
extern bh1750_alarm(void);

extern int low_tem;
extern int high_tem;

void zigbee_serial_init(int fd_stm);
int  transfer(char *p_stm32);


int main(int argc, const char *argv[])
{

    int i,j;
	key_t key;
	int shmid;
	char str[100];

//	system("dmesg -c" );
//	system("insmod led_sysfs.ko" );
//	system("insmod beep_misc.ko" );
//	system("insmod ds18b20.ko" );
//	system("mjpg_streamer -i "/mjpg/input_uvc.so -y" -o "/mjpg/output_http.so -w /www" -o "/mjpg/output_file.so -f /pice -d 3000"" );

	if (pthread_mutex_init(&myled.led, NULL) != 0)
	{
		printf("pthread_mutex_init is error\n");
	}

	e.e_fire = 0;
	e.e_dstem = 0;
	e.e_item = 0;
	e.e_ihum = 0;
	e.e_otem = 0;
	e.e_ohum = 0;
	e.e_lihgt = 0;
	
	if (pthread_mutex_init(&mybeep.beep, NULL) != 0)
	{
		printf("pthread_mutex_init is error\n");
	}

	pthread_mutex_init(&myqt.qt, NULL);
	pthread_mutex_init(&mysht.sht, NULL);
	pthread_mutex_init(&myds.ds, NULL);
	pthread_mutex_init(&mybh.bh, NULL);
	pthread_mutex_init(&mystm.stm, NULL);
	pthread_mutex_init(&Neuron, NULL);
	pthread_mutex_init(&udp, NULL);
	pthread_mutex_init(&myrecv, NULL);
	pthread_mutex_init(&wocao, NULL);

	pthread_cond_init(&cond, NULL);
	pthread_cond_init(&cond1, NULL);


/*********************打开所有设备***************************/

#if 1
	if ((fd_led = open("/dev/led2_sysfs", O_RDWR)) < 0)
	{
		perror("fail to open leeeeed !!!");
		exit(1);
	}
#endif

#if 1
	for (j = 0;j < 3;j++)
	{
		bzero(str, sizeof(str));
		sprintf(str, "/dev/led%d_sysfs",j+3);
		if ((fd_alarm[j] = open(str, O_RDWR)) < 0 )
		{
			perror(" fail to open ledddddd!!!!");
			exit(1);
		}
	}
#endif

#if 1
	if ((fd_beep = open("/dev/beep_misc", O_RDWR)) < 0)
	{
		perror("fail to open beeeeep !!!");
		exit(1);
	}
#endif

#if 0
	if ((fd_sht21 = open("/dev/sht21_sysfs", O_RDWR)) < 0)
	{
		perror("fail to open sht2222222211111 !!!");
		exit(1);
	}
#endif

#if 1
	if ((fd_ds18 = open("/dev/ds18b20", O_RDWR)) < 0)
	{
		perror("fail to open ds18b222222000000 !!!");
		exit(1);
	}
#endif

#if 0
	if ((fd_bh17 = open("/dev/bh1750_sysfs", O_RDWR)) < 0)
	{
		perror("fail to open bh1750000000000 !!!");
		exit(1);
	}
#endif

#if 1
	if ((fd_stm = open("/dev/ttyUSB0", O_RDWR)) < 0)
	{
		perror("fail to open sttttttyyyyyyyyy !!!");
		exit(1);
	}
#endif

		pthread_mutex_lock(&mystm.stm);
		zigbee_serial_init(fd_stm);

#if 1
/*************** 创建 key 值******************/

	key = ftok("/dev", 'a');    
	if (key < 0)
	{
		perror("fail to ftok!!!");
		exit(1);
	}

/************  创建共享内存  ****************/

	shmid = shmget(key, sizeof(SHM),IPC_CREAT | IPC_EXCL | 0664);
	if(shmid < 0)
	{
		if(errno == EEXIST)
		{
			shmid = shmget(key, sizeof(SHM), 0664);
			p = (SHM *)shmat(shmid, NULL, 0);
			if (p == NULL)
			{
				perror("fail to shmat !!!");
				exit(1);
			}
			shm_buf = p;			
		}
		else
		{
			perror("fail to shmget !!!");
			exit(1);
		}
	}
	else 
	{
			p = (SHM *)shmat(shmid, NULL, 0);
			if (p == NULL)
			{
				perror("fail to shmat !!!");
				exit(1);
			}
			shm_buf = p;				
	}

#endif 

/***************** TCP COMMUNITAION ********************/


//	pthread_mutex_lock(&myqt.qt);
	printf("host_qt_msg_fun  is oooooook !!!\n");

	fd_socket_tcp = socket(AF_INET, SOCK_STREAM, 0);
	if(fd_socket_tcp < 0)
	{
		perror("fail to socket !!!");
		printf("socket = %d\n",fd_socket_tcp);
	//	pthread_exit(NULL);
	}
	printf("fd_socket_tcp =%d\n",fd_socket_tcp);

	bzero(&my_addr, sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(55556);
	my_addr.sin_addr.s_addr = inet_addr("172.26.1.106");

	if((ret_bind = bind(fd_socket_tcp, (SA *)&my_addr, sizeof(my_addr))) < 0)
	{
		perror("fail to bind !!!");
		printf("ret_bind = %d\n",ret_bind);
//		pthread_exit(NULL);
	}

	if(listen(fd_socket_tcp,5) < 0)
	{
		perror("fail to listen !!!");
	}

#if 0

	bzero(&qt_addr, sizeof(qt_addr));
	qt_addr.sin_family = AF_INET;
	qt_addr.sin_port = htons(55556);
	qt_addr.sin_addr.s_addr = inet_addr("172.1.26.111");
#endif

/****************************************************************************/


/***********************创建线程************************/

#if 1
	ret_pthread = pthread_create(&pthreads[0], NULL, world_fun, NULL);
	if (ret_pthread != 0)
	{
		perror(" fail to pthread_create -> world_fun !!!");
		exit(1);	
	}
#endif


#if 1
	ret_pthread = pthread_create(&pthreads[1], NULL, host_qt_send_fun, NULL);
	if (ret_pthread != 0)
	{
		perror(" fail to pthread_create -> host_qt_send_fun !!!");
		exit(1);	
	}

#endif


#if 1
	ret_pthread = pthread_create(&pthreads[2], NULL,host_qt_recv_fun, NULL);
	if (ret_pthread != 0)
	{
		perror(" fail to pthread_create -> host_qt_recv_fun !!!");
		exit(1);	
	}
#endif


#if 1
	ret_pthread = pthread_create(&pthreads[3], NULL, qt_host_msg_fun, NULL);
	if (ret_pthread != 0)
	{ 
		perror(" fail to pthread_create -> qt_host_msg_fun!!!");
		exit(1);	
	}

#endif


#if 1
	ret_pthread = pthread_create(&pthreads[4], NULL, event_alarm, NULL);
	if (ret_pthread != 0)
	{ 
		perror(" fail to pthread_create -> event_alarm!!!");
		exit(1);	
	}

#endif

/*************************循环监测************************************/

#if 1	
	while(1)
	{
		//  sleep(1);
    	pthread_mutex_lock(&Neuron);
		pthread_cond_wait(&cond, &Neuron);
 
/*********************检测 DS18B20 温度传感器***************************/

#if 0
		pthread_mutex_lock(&myds.ds);


			if ((shm_buf->ds_value < 20 ) || (shm_buf->ds_value > 50 )) 		// 温度异常
			{
				e.e_dstem = 1;
				printf("********** DS18B20 TEM WARNING ********** \n");
			}
			else
			{
				e.e_dstem = 0;
			}

		pthread_mutex_unlock(&myds.ds);	
#endif

/*********************监测 SHT21 温湿度传感器***************************/
#if 0
		pthread_mutex_lock(&mysht.sht);

		if ((shm_buf->sht21_tem_value < 20 ) || (shm_buf->sht21_tem_value > 30 ))
		{
			e.e_item = 1;
			printf("********** SHT21 warning ***********\n");
		}
		else
		{
			e.e_item = 0;
		}

		if ((shm_buf->sht21_hum_value < 30 ) || (shm_buf->sht21_hum_value >100 ))
		{
			e.e_ihum = 1;
			printf("********** SHT21 warning **********\n");
		}
		else
		{
			e.e_ihum = 0;
		}a

		pthread_mutex_unlock(&mysht.sht);

#endif


/*********************监测 BH17501 光照传感器***************************/
#if 0
		pthread_mutex_lock(&mybh.bh);

		if ((shm_buf->light_value < 0) || (shm_buf->light_value > 1000 ))
		{
			e.e_lihgt = 1;
			printf("********** BH1750 warning **********\n");
		}
		else
		{
			e.e_lihgt = 0;
		}

		pthread_mutex_unlock(&mybh.bh);

#endif		



/*********************** 监测 STM32 数据 **********************************************/
#if 1
		pthread_mutex_lock(&mystm.stm);

		printf("low_tem = %d, high_tem = %d\n", low_tem, high_tem);
		
	    	if ((shm_buf->stm32_tem_value < low_tem) || (shm_buf->stm32_tem_value > high_tem ))
			{
				e.e_otem = 1;
			
				printf("********** STM32 TEM WARNING **********\n");
			}	
			else
			{
				e.e_otem = 0;
			
			}

			if ((shm_buf->stm32_hum_value < 30 ) || (shm_buf->stm32_hum_value > 90 ))
			{
		    	e.e_ohum = 1;
				
				printf("********** STM32 HUM WARNING ************\n");
			}	
			else
			{
				e.e_ohum = 0;
			
			}

			if (shm_buf->fire_stm32_value > 100)
			{
				e.e_fire = 1;
				printf("********** STM32 FIRE WARNING **********\n");
			}	
			else
			{
				e.e_fire = 0;
			}

		pthread_mutex_unlock(&mystm.stm);
		pthread_mutex_unlock(&Neuron);
	//	pthread_cond_signal(&cond1);
#endif
			
		sleep(1);
	}

#endif


#if 1
	for(i=0; i < 6; i++ )
	{
		if (pthread_join(pthreads[i], NULL) != 0)
		{
			perror("fail to pthread_join !!!");
			exit(1);
		}
	}
#endif

	close(fd_led);
	close(fd_beep);
	close(fd_sht21);
	close(fd_ds18);
	close(fd_bh17);
	close(fd_stm);
	for (j = 0; j < 3; j++)
	{
		clsoe(fd_alarm[j]);
	}

	sleep(3);
	shmctl(shmid, IPC_RMID, NULL);

	return 0;
}


/************** STM32 ****************/

void zigbee_serial_init(int fd_stm)
{
	struct termios options;

	tcgetattr(fd_stm, &options);
	
	options.c_cflag |= (CLOCAL | CREAD);
	options.c_cflag &= ~CSIZE;
	options.c_cflag &= ~CRTSCTS;
	options.c_cflag |= CS8;
	options.c_cflag &= ~CSTOPB; 
	options.c_iflag |= IGNPAR;
	options.c_iflag &= ~(BRKINT | INPCK | ISTRIP | ICRNL | IXON);

	options.c_cc[VMIN] = 12;
	options.c_oflag = 0;
	options.c_lflag = 0;

	cfsetispeed(&options, B115200);
	cfsetospeed(&options, B115200);
	    
	tcsetattr(fd_stm, TCSANOW, &options);
//	sleep(1);

	 printf("===== zigbee serial ok! =====\n");
}

 int transfer(char *p_stm32)
{
	int data = 0, tmp = 0;
	
	p_stm32++;

	while(*p_stm32 >= '0' && *p_stm32 <= '9' && *p_stm32 != '\0')
	{
		tmp = *p_stm32 - '0';
		data = data * 10 + tmp;
		p_stm32++;
	}
	return data;
}
				
