#ifndef __AI_H__
#define __AI_H__


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <termios.h>
#include <time.h>


/**********************************************
 *          室外
 * DHT1111       温度传感器
 *               火焰传感器
 *
 *
 *          室内
 *
 * DS18B20       温度传感器  
 * SHT21         温湿度传感器
 * BH1750        光照传感器
 *
 *
 * ********************************************/

#define NUM_PTHREAD 20          // 线程
pthread_t pthreads[NUM_PTHREAD];
int ret_pthread;
void *host_qt_send_fun(void *arg);
void *host_qt_recv_fun(void *arg);
void *world_fun(void *arg);
void *qt_host_msg_fun(void *arg);
void *event_alarm(void *arg);

#define SA struct sockaddr      // 网络
//struct timeval tv;
struct sockaddr_in my_addr, qt_addr;
int fd_socket_tcp, fd_socket_udp, fd_accept;
int ret_bind, ret_recv;




float sht21_tem_value, sht21_hum_value;     // SHT21 温湿度
int tem, hum;


int light_bh;                        // BH1750光照
float tmp_bh;


#define MAX_LEN 128                  // STM32
char send_stm32[MAX_LEN];
char recv_stm32[MAX_LEN];
char *p_stm32;
int tem_stm32, hum_stm32, fire_stm32;
int nread;



#define LED_ON _IO('x',2)             // 灯控
#define LED_OFF _IO('x',3)

#define BEEP_ON _IO('x',0)            // 蜂鸣器控
#define BEEP_OFF _IO('x',1)


#define SHT21_T_HOLD 0xe3             // ¶ÁÎÂ¶È
#define SHT21_RH_HOLD 0xe5            // ¶ÁÊª¶È

#define TEMP _IOR('x',4,int)          // 温控
#define HUM _IOR('x',5,int)           // 湿控
#define K 0.0625                      // 数字温度传感器

#define READ_LIGHT _IOR('x',6,int)    // 光照传感器



//***********************************************

struct alarm
{
	char e_fire;
	char e_dstem;
	char e_item;
	char e_ihum;
	char e_otem;
	char e_ohum;
	char e_lihgt;
}e;

//***********************************************



struct led_mutex 
{
	char led_fire;
	char led_tem;    // DS18B20
	char led_sht21_hum;
	char led_sht21_tem;
	char led_stm32_hum;
	char led_stm32_tem;
	char led_light;
	char starting_led;
	pthread_mutex_t led;

}myled;

struct beep_mutex
{
	char beep_fire;
	char beep_tem;   // DS18B20
	char beep_sht21_hum;
	char beep_sht21_tem;
	char beep_stm32_hum;
	char beep_stm32_tem;
	char beep_light;
	char starting_beep;
	pthread_mutex_t beep;

}mybeep;

struct sht21_mutexa         // 温湿度传感器
{
	
	pthread_mutex_t sht;

}mysht;

struct ds18_mutex           // 数字温度传感器
{
	pthread_mutex_t ds;

}myds;

struct bh17_mutex            // 光传感器
{
	pthread_mutex_t bh;

}mybh;

struct stm32_mutex            // STM32感器
{
	pthread_mutex_t stm;

}mystm;


struct qt_mutex
{
	pthread_mutex_t qt;

}myqt;

pthread_mutex_t myrecv;

pthread_mutex_t udp;

pthread_mutex_t Neuron;

pthread_mutex_t wocao;

pthread_cond_t cond;

pthread_cond_t cond1;


/***************** 共享内存 ***************/

typedef struct ipc_shm 
{
	int light_value;
	float ds_value; 
	float sht21_tem_value;    		
	float sht21_hum_value;
	int stm32_tem_value;
	int stm32_hum_value;
	int fire_stm32_value;

    int led_ctl;
	int beep_ctl;


}SHM;


/****************** TCP数据包 ***************/

typedef struct data_interaction
{
	int type;
	float ds_tem;
	float sht21_hum;
	float sht21_tem;
	float stm32_hum;
	float stm32_tem;
	float light;
    int low_stm32_tem;
	int high_stm32_tem;
}tcp_info;

tcp_info cs_buff;



#endif 
