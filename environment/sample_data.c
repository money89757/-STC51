#include"head.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
//cgi
#include "cgic.h"
#include <stdlib.h>
//ipc
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <signal.h>

extern char *cgiQueryString;
extern cgiUnescapeResultType cgiUnescapeChars(char **sp,char *cp,int len);


 
int cgiMain()
{ 
	key_t key;
	int shmid;                         //共享内存id
	sensor_data *shmaddr;              //指向共享内存区域指针(修改为结构体指针)
	char *buffer;
	
	if((key = ftok("/etc" , 'r')) == -1)     //创建key值id
	{
		perror("ftok");
		exit(-1);
	}
	fprintf(cgiOut,"key:%d\n",key);
	if((shmid = shmget(key , sizeof(sensor_data) , 0666 | IPC_CREAT | IPC_EXCL)) < 0)  //获得共享内存id
	{
		if((shmid = shmget(key , sizeof(sensor_data) , 0666)) < 0)
		{
			perror("shmget");
			exit(-1);
		}
	}

	fprintf(cgiOut,"%d\n",shmid);
	if((shmaddr = (sensor_data*)shmat(shmid , NULL , 0)) == NULL)           //映射共享内存首地址
	{
		perror("shmat");
		exit(-1);
	}
	
	fprintf(cgiOut,"shmaddr:%p\n",shmaddr);
	



	cgiHeaderContentType("text/html");
	fprintf(cgiOut, "<HTML><HEAD>\n");
    fprintf(cgiOut, "<TITLE>智能农场管理系统—环境</TITLE></HEAD>\n");
	cgiUnescapeChars(&buffer,cgiQueryString,strlen(cgiQueryString));
	fprintf(cgiOut, "<BODY>");

	


	fprintf(cgiOut," <H4>土壤温度 = %d</H4>",shmaddr->air_temp);
	fprintf(cgiOut," <H4>土壤湿度 = %d</H4>",shmaddr->air_hum);
	fprintf(cgiOut," <H4>空气温度 = %d</H4>",shmaddr->soil_temp);
	fprintf(cgiOut," <H4>空气湿度 = %d</H4>",shmaddr->soil_hum);
	fprintf(cgiOut," <H4>CO2 浓度 = %d</H4>",shmaddr->co2);
	fprintf(cgiOut," <H4>光照强度 = %d</Hd>",shmaddr->light);
//	fprintf(cgiOut," <H4>风扇转速 = %s</H4>",shmaddr->D_buf);
	fprintf(cgiOut," <H4>水　  位 = %d</H4>",shmaddr->liquid_level);

	fprintf(cgiOut,"</BODY>\n");
	fprintf(cgiOut,"</HTML>\n");	

	free(buffer);

	return 0;
}
