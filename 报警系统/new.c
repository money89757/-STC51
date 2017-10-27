#include "AI.h"


extern SHM *shm_buf;
extern int fd_led;
extern int fd_beep ;
extern int fd_sht21;
extern int fd_ds18;
extern int fd_bh17;
extern int fd_stm;

int low_tem = 20;
int high_tem = 33;
void *host_qt_send_fun(void *arg)
{

	int ret_recv = 10;

	while(ret_recv !=0)    // 循环发送数据
	{
		bzero(&cs_buff, sizeof(tcp_info));
#if 1
		//	cs_buff.type;
		cs_buff.ds_tem = shm_buf->ds_value;
		cs_buff.sht21_tem = shm_buf->sht21_tem_value;
		cs_buff.sht21_hum = shm_buf->sht21_hum_value;
		cs_buff.stm32_hum = shm_buf->stm32_hum_value;
		cs_buff.stm32_tem = shm_buf->stm32_tem_value;
		//	cs_buff.light = 1000;
#endif
		//	recv(fd_accept, (char *)&cs_buff, sizeof(tcp_info),0);
		send(fd_accept, (char *)&cs_buff, sizeof(tcp_info), 0);

		
		sleep(2);
	}

}



void *host_qt_recv_fun(void *arg)
{

	socklen_t len_qtaddr = sizeof(qt_addr);
	while(1)    // 循环jieshou
	{		

		if ((fd_accept = accept(fd_socket_tcp, (SA *)&qt_addr, &len_qtaddr)) < 0)
		{
			perror("fail to accept !!! ");
		}

		printf("@@@@@@ ip:%s,port:%d is connected @@@@@@ \n",inet_ntoa(qt_addr.sin_addr),ntohs(qt_addr.sin_port));

		bzero(&cs_buff, sizeof(tcp_info));

		while(1)
		{
			
			char wo[64]={0};

			ret_recv = recv(fd_accept, wo, sizeof(tcp_info), 0);
			if (ret_recv < 0)
			{
				perror(" fail to recv !!!");
			}
			else if (ret_recv == 0)
			{
				printf("QT is shutdow !!!!");
				close(fd_accept);
				break;
			}
			else
			{
				memcpy(&cs_buff,wo,sizeof(cs_buff));
			    low_tem  = cs_buff.low_stm32_tem;
				high_tem = cs_buff.high_stm32_tem;

				printf("&&&&&&&&&&&&&&&&&&&&& low_tem = %d, high_tem = %d\n", low_tem, high_tem);
			}

		}
		sleep(1);
	}
}



