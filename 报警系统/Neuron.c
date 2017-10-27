#include "AI.h"


int hz = 1000;
extern int fd_led;
extern int fd_beep ;
extern int fd_sht21;
extern int fd_ds18;
extern int fd_bh17; 
extern int fd_stm;
extern SHM *shm_buf;


void *world_fun(void *arg)
{


	char count = 5;                // DS18B20 温度
	unsigned int tmp_ds = 0;
	float ds_value = 0;

	pthread_mutex_lock(&Neuron);

	while(1)
	{

// ************ 读取 DS18B20 温度数据 *******************	
#if 1
		printf("########## Get Sensor Data ##########\n");
		pthread_mutex_lock(&myds.ds);

		printf("##### DS18B20 Data ##### !!!\n");

		read(fd_ds18, &tmp_ds, sizeof(tmp_ds));
		ds_value = tmp_ds * K;
//		printf("ds_value = %f\n", ds_value);
		shm_buf->ds_value = ds_value;
//		printf("ds_value = %f\n", shm_buf->ds_value);
		pthread_mutex_unlock(&myds.ds);	
#endif



// *********** 读取 SHT21 温湿度数据 ********************
#if 0
		pthread_mutex_lock(&mysht.sht);

		printf("##### SHT21 Data #####\n");

		ioctl(fd_sht21, TEMP, &tem);
		sht21_tem_value = -46.85 +175.72 * tem / 65536;
		printf("tem_value = %f\n", sht21_tem_value);
		shm_buf->sht21_tem_value = sht21_tem_value;

		ioctl(fd_sht21, HUM, &hum);
		sht21_hum_value = -6 + 125 * hum / 65536;
		printf("sht21_hum_value = %f\n", sht21_hum_value);
		shm_buf->sht21_hum_value = sht21_hum_value;

		pthread_mutex_unlock(&mysht.sht);

#endif


// ************* 读取 BH1750 光照数据 *******************
#if 0
		pthread_mutex_lock(&mybh.bh);

		printf("##### BH1750 Data #####\n");

		ioctl(fd_bh17, READ_LIGHT, &light_bh);
		tmp_bh = light_bh / 1.2;
		light_bh = (int)tmp_bh;
		printf("tmp_bh = %d\n",light_bh);
		shm_buf->light_value = light_bh;

		pthread_mutex_unlock(&mybh.bh);

//		sleep(1);
#endif


// ************** 读取 STM32 数据 ************************


		printf("##### STM32 Data #####\n");

			bzero(recv_stm32, sizeof(recv_stm32));
			nread = read(fd_stm, recv_stm32, MAX_LEN-1);

			if (nread < 0)
			{
//				printf("serial read failed\n");
			//	pthread_exit(NULL);
			}

			recv_stm32[nread] = '\0';
			p_stm32 = recv_stm32;

			while(*p_stm32 != 't' && *p_stm32 != '\0')
			{
				p_stm32++;
			}

			tem_stm32 = transfer(p_stm32);
//			printf("tem_stm32 = %d\n",tem_stm32);
	        shm_buf->stm32_tem_value = tem_stm32;
//	    	printf("shm_buf->stm32_tem_value = %d\n", shm_buf->stm32_tem_value);

			while(*p_stm32 != 'h' && *p_stm32 != '\0')
			{
				p_stm32++;
			}

			hum_stm32 = transfer(p_stm32);
//			printf("hum_stm32 =%d\n",hum_stm32);
			shm_buf->stm32_hum_value = hum_stm32;
//	    	printf("shm_buf->stm32_hum_value = %d\n", shm_buf->stm32_hum_value);

			while(*p_stm32 != 'f' && *p_stm32 != '\0')
			{
				p_stm32++;
			}

			fire_stm32 = transfer(p_stm32);
//			printf("fire_stm32 = %d\n", fire_stm32);
			shm_buf->fire_stm32_value = fire_stm32;
//			printf("shm_buf->fire_stm32_value = %d\n", shm_buf->fire_stm32_value);


		pthread_mutex_unlock(&mystm.stm);

		pthread_mutex_unlock(&Neuron);
		pthread_cond_signal(&cond);
		sleep(1);
	}
		pthread_exit(NULL);
}

