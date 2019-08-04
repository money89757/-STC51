#ifndef __HEAD_H__
#define __HEAD_H__


#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdio.h>
#include "cgic.h"
#include <string.h>
#include <stdlib.h>



#define N 16 
#define MSGLEN (sizeof(msg) - sizeof(long))
#define TYPEA 100
#define TYPEB 200


#define fan_speed_0 16
#define fan_speed_1 17
#define fan_speed_2 18
#define fan_speed_3 19
#define pump_off 32
#define pump_on 33
#define led1_off 48
#define led1_on 49 
#define led2_off 50
#define led2_on 51




/************************************************************
 *                   定义消息队列结构体 
 * *********************************************************/
typedef struct message{
	long mtype;
	unsigned char commend;
}MSG;


/************************************************************
 *                   定义传感器数据结构体 
 * *********************************************************/
typedef struct sensor{
	unsigned int air_temp;                //空气温度
	unsigned int air_hum;                 //空气湿度
	unsigned int soil_temp;               //土壤温度
	unsigned int soil_hum;                //土壤湿度
	unsigned int liquid_level;            //水位
	unsigned int co2;                     //CO2浓度/
	unsigned int light;                   //光照强度
}sensor_data;

                       //消息队列id

void *thread_function(void *arg);         //线程处理函数
void datadeal(unsigned char *arg , int count);     //数据处理函数
void handler(int arg);                    //信号处理函数





#endif
