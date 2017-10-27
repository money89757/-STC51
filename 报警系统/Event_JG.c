#include "AI.h"


extern int lock;
extern SHM *shm_buf;
extern int fd_led;
extern int fd_alarm[3];
extern int fd_beep ;
extern int fd_sht21;
extern int fd_ds18;
extern int fd_bh17;
extern int fd_stm;
extern zigbee_serial_init(int fd_stm);
extern transfer(char *p_stm32);


void *event_alarm(void *arg)
{
	pthread_mutex_lock(&wocao);
//	pthread_cond_wait(&cond1, &wocao);

	while(1)
	{
	int j;

	while (e.e_fire ==1 || e.e_dstem==1 || e.e_item==1 || e.e_ihum==1 || e.e_otem==1 || e.e_ohum==1 || e.e_lihgt==1)
	{
		for (j = 0;j < 3;j++)
		{
			ioctl(fd_alarm[j], LED_ON);
			sleep(1);		
		}
	
		for (j = 0;j < 3;j++)
		{
			ioctl(fd_alarm[j], LED_OFF);
		//	sleep(1);		
		}

		ioctl(fd_beep, BEEP_ON);
	}

	for (j = 0;j < 3;j++)
	{
		ioctl(fd_alarm[j], LED_OFF);
		sleep(1);		
	}
	ioctl(fd_beep, BEEP_OFF);
	pthread_mutex_unlock(&wocao);
	sleep(1);
	}
}
















