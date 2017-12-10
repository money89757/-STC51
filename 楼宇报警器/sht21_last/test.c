#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>

#define TEMP _IOR('s',0,int)
#define HUM  _IOR('s',1,int)

int main(int argc, const char *argv[])
{
	float i,j;
	int fd = open("/dev/sht21",O_RDWR);
	int temp,hum;
	if(fd < 0)
	{
		perror("open\n");
		exit(1);
	}

	while(1)
	{
		ioctl(fd,TEMP,&temp);
		i = -46.85+(175.72*temp/65536);
		printf("temp : %f\n",i);
		sleep(1);
		ioctl(fd,HUM,&hum);
		j = -6 + (125*hum/65536);
		printf("hum : %f\n",j);
		sleep(1);
	}
	return 0;
}
