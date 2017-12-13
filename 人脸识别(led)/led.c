#include "wiringPi.h"
#include <stdio.h>

int main(void)
{
	int i=0;
	wiringPiSetup();
	pinMode(21,OUTPUT);
	//for(;;)
	//{
		digitalWrite(21,HIGH);
		delay(1000);
		int vol = digitalRead(21); printf("vol1:%d\n",vol);
		for(i=0;i<1;i++){
			digitalWrite(21,LOW);
		}
		delay(1000);
		vol = digitalRead(21); printf("vol:%d\n",vol);
	//}

}
