#include "AI.h"


extern SHM *shm_buf;
extern int fd_led;
extern int fd_beep ;
extern int fd_sht21;
extern int fd_ds18;
extern int fd_bh17;
extern int fd_stm;



void *qt_host_msg_fun(void *arg)
{

	int on1 = 1;

	pthread_mutex_lock(&udp);


	printf("main_send_qt is oooooook !!!\n");

	fd_socket_udp = socket(AF_INET, SOCK_DGRAM, 0);
	if(fd_socket_udp < 0)
	{
		perror("fail to socket !!!");
		exit(1);
	}

#if 1
	bzero(&my_addr, sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(55555);
	my_addr.sin_addr.s_addr = inet_addr("172.26.1.106");

	if((ret_bind = bind(fd_socket_udp, (SA *)&my_addr, sizeof(my_addr))) < 0)
	{
		perror("fail to bind ");
//		pthread_exit(NULL);
	}
#endif 
	
//	bzero(&qt_addr, sizeof(qt_addr));
//	qt_addr.sin_family = AF_INET;
//	qt_addr.sin_port = htons(55555);
//	qt_addr.sin_addr.s_addr = inet_addr("172.1.26.143");


	setsockopt(fd_socket_udp, SOL_SOCKET, SO_BROADCAST, &on1, sizeof(on1));

	while(1)
	{
		socklen_t len_qtaddr = sizeof(qt_addr);
		bzero(&qt_addr, len_qtaddr);

	//	bzero(&cs_buff, sizeof(tcp_info));
#if 1
    	recvfrom(fd_socket_udp, &shm_buf->led_ctl, sizeof(shm_buf->led_ctl), 0, (SA *)&qt_addr, &len_qtaddr);
		printf("shm led_ctl:%d\n",shm_buf->led_ctl);
		if ( shm_buf->led_ctl == 0)
		{
			ioctl(fd_led, LED_OFF);
			printf("******* Led is offffff *******\n");
//			ioctl(fd_beep,BEEP_OFF);
//			printf("******* Beep is offffff *******\n");
		}
		else
		{
			ioctl(fd_led, LED_ON);
			printf("****** Led is ooooooon *******\n");
//			ioctl(fd_beep,BEEP_ON);
//			printf("****** Beep is onnnnnnn *******\n");
		}
#endif

	//	sendto(fd_socket, &shm_buf->led_ctl, sizeof(shm_buf->led_ctl), 0, (SA *)&qt_addr, len_qtaddr);


//		sleep(1);

	}

	pthread_mutex_unlock(&udp);
	//pthread_exit(NULL);
}

