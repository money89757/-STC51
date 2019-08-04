#include"cgic.h"
#include "cgic.c"
#include <stdio.h>
#include <fcntl.h>
#include <sys/ipc.h>


int cgiMain()
{
	key_t key;
//	char buffer;
	int shmid;
	sensor_data *shm_buf;
	
	if((key=ftok("/etc",'r')) == -1)
	{
	
		perror("ftok");
		exit(-1);
	}
	if((shmid=shmget(key,sizeof(sensor_data),0666 | IPC_CREAT)) < 0)
	{
		if(errno == EEXIST)
		{
			shmid=shmget(key,sizeof(sensor_data),0664);
			shm_buf = shmat(shmid,NULL,0);
		}
		else
		{
			perror("shmget");
			exit(-1);
		}
	}else
	{
		shm_buf = shmat(shmid,NULL,0);
	}
	
/***************************************************************
 * 将 环静信息直接打印到 网页上
 * **************************************************************/
	cgiHeaderContentType("text/html\n\n");
	fprintf(cgiOut,"<HTML ><HEAD>\n");
	fprintf(cgiOut,"<TITLE>Environment data</TITLE></HEAD>\n");	
	fprintf(cgiOut,"<meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\"/>\n");

//	cgiUnescapeChars(&buffer,cgiQueryString,strlen(cgiQueryString));//反 转 义
	fprintf(cgiOut,"<BODY background=\"../image/air.jpg\">");
//	fprintf(cgiOut,"<H1>environment date send sucess</H1>\n");

	fprintf(cgiOut,"<p></p>\n");
	fprintf(cgiOut,"<H1 align=\"center\">智能农场环境信息</H1>\n");
	fprintf(cgiOut,"<H1 align=\"right\">--------天 河 二 号</H1>\n");

	fprintf(cgiOut,"<table width=\"400 height=\"600 border=\"0 align=\"center \">\n");

	fprintf(cgiOut,"<tr>\n");
	fprintf(cgiOut,"<div align=\"center\" >\n");
	fprintf(cgiOut,"<H2>空气温度:  %d.%d 摄氏度 </H2>\n",shm_buf->air_temp/10,shm_buf->air_temp%10);
	fprintf(cgiOut,"<H2>空气湿度:  %d.%d RH     </H2>\n",shm_buf->air_hum/10,shm_buf->air_hum%10);	
   fprintf(cgiOut,"</div></tr>\n");

	fprintf(cgiOut,"<p></p>\n");

	fprintf(cgiOut,"<tr>\n");
	fprintf(cgiOut,"<div align=\"center\" >\n");
	fprintf(cgiOut,"<H2>土壤温度:  %d.%d 摄氏度 </H2>\n",shm_buf->soil_temp/10,shm_buf->soil_temp%10);
	fprintf(cgiOut,"<H2>土壤湿度:  %d.%d RH     </H2>\n",shm_buf->soil_hum/10,shm_buf->soil_hum%10);
	fprintf(cgiOut,"</div>\n");
	fprintf(cgiOut,"</tr>\n");

	fprintf(cgiOut,"<p></p>\n");
	fprintf(cgiOut,"<tr>\n");
//	fprintf(cgiOut,"<td align=\"center >\n");
	fprintf(cgiOut,"<div align=\"center\" >\n");
	fprintf(cgiOut,"<H2>光照强度:  %d.%d 勒克斯</H2>\n",shm_buf->light/10,shm_buf->light%10);
	fprintf(cgiOut,"<H2>CO2浓度 :  0.000%d     </H2>\n",shm_buf->co2);
	fprintf(cgiOut,"</div>\n");
//	fprintf(cgiOut,"</td>\n");
	fprintf(cgiOut,"</tr>\n");

	fprintf(cgiOut,"<p></p>\n");
	fprintf(cgiOut,"<tr>\n");
//	fprintf(cgiOut,"<td align=\"center\" >\n");
	fprintf(cgiOut,"<div align=\"center\" >\n");
	if(shm_buf->liquid_level == 2)
		fprintf(cgiOut,"<H2>水位状态 : 高 </H2>\n");
	else
		fprintf(cgiOut,"<H2>水位状态 : 低 </H2>\n");
	fprintf(cgiOut,"</div>\n");
//	fprintf(cgiOut,"</td>\n");
	fprintf(cgiOut,"</tr>\n");


	fprintf(cgiOut,"</table>\n");

	fprintf(cgiOut,"<meta http-equiv=\"refresh\" content=\"1;url= \">");//间隔0s刷新一次 
	fprintf(cgiOut,"</BODY>\n");
	fprintf(cgiOut,"</HTML>\n");
	
return 0;

}


