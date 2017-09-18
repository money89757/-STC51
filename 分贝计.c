/ * ************************************************ *************************
版权所有：2017年，money89757
文件名：main.c
说明：运用STC51单片机做成的分贝仪
作者：money89757
版本：V0.1
日期：2017年9月18日
历史：2017-9-18：更新一些注释和端口
************************************************** ********************* * * /
#include <STC12C5A60S2.H>
#include<INTRINS.H>

#define uchar unsigned char
#define uint unsigned int

#define ADC_ON 1//KEY S2
#define ADC_OFF 0//KEY S2
#define ADC_POWER 0x80//ADC_CONTR-7
#define ADC_FLAG 0x10//ADC_CONTR-4
#define ADC_START 0x08//ADC_CONTR-3
#define ADC_SPEEDLL 0x00//ADC_CONTR-540CLOCK
#define ADC_CH0 0x00//ADC_CONTR-ADC0
#define SET_ON 1//KEY S3
#define SET_OFF 0//KEY S3
#define CHECK_ON 1//KEY S4
#define CHECK_OFF 0//KEY S4
#define BSW_ON 1//KEY S5
#define BSW_OFF 0//KEY S5
#define GW_ON 1//KEY S6
#define GW_OFF 0//KEY S6
#define LOCKED 1
#define UNLOCK 0

sbit WK_LED=P1^6;//WK_LED
sbit S2=P1^7;//start
sbit S3=P3^2;//set
sbit S4=P3^3;//limit
sbit S5=P3^4;//bsw
sbit S6=P3^5;//gw
sbit S7=P3^6;//add
sbit S8=P3^7;//sub
sbit DIG1=P2^2;//DPY
sbit DIG2=P2^1;//DPY
sbit DIG3=P2^0;//DPY
sbit Buzzer_EN=P2^7;//Buzzer

bit adcbit,setbit,checkbit,bswbit,gwbit,FH,bswlock,gwlock,S2keylock,S3keylock,S4keylock;

uchar code DPY[]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0xBF,0xFF};//0~9 -  
uchar display[3]={0x0A,0x0A,0x0A};//"---"
uchar maxvalue=80,minvalue=20,value=80,t0_num,t1_num,digit,adc_code[3];

void DELAYMS(uchar nms)
{
	uchar i;
	while(nms--)
	{
		for(i=123;i>0;i--);
	}
}

void ADC_INIT()//OK
{
	P1ASF=0x01;//P1.0 ADC0
	ADC_CONTR=ADC_POWER|ADC_SPEEDLL|ADC_CH0;
	DELAYMS(1);
}

void ADC_POWEROFF()//OK
{
	ADC_CONTR&=~ADC_POWER;
	DELAYMS(1);
}

void KEYSCAN()
{
	if((S2keylock==UNLOCK)&&(S2==0))
	{
		DELAYMS(20);
		if(S2==0)
		{
			adcbit=~adcbit;
			if(adcbit==ADC_ON)
			{
				WK_LED=0;
				S3keylock=LOCKED;//SET DISABLE
				Buzzer_EN=0;
				DELAYMS(250);
				DELAYMS(250);
				DELAYMS(250);
				DELAYMS(250);//1S
				Buzzer_EN=1;
				ADC_INIT();
				TH1=0x3C;//T_BASE=
				TL1=0xB0;//50ms
				TR1=1;
			}//ADC
			else if(adcbit==ADC_OFF)
			{
				TR1=0;
				ADC_POWEROFF();
				WK_LED=1;
				display[2]=display[1]=display[0]=0x0B;//"   "
				S3keylock=UNLOCK;
			}
			while(S2==0);
		}
	}//OK
	if((S3keylock==UNLOCK)&&(S3==0))
	{
		DELAYMS(20);
		if(S3==0)
		{
			setbit=~setbit;
			if(setbit==SET_ON)
			{
				S2keylock=LOCKED;
				display[2]=value/100;
				if(display[2]==0)
					display[2]=0x0B;
				display[1]=value%100/10;
				display[0]=value%10;
			}
			else if(setbit==SET_OFF)
			{
				display[2]=display[1]=display[0]=0x0B;//"   "
				S2keylock=UNLOCK;
			}
			while(S3==0);
		}
	}//OK
	if(setbit==SET_ON)
	{
		if((S4keylock==UNLOCK)&&(S4==0))
		{
			DELAYMS(20);
			if(S4==0)
			{
				checkbit=~checkbit;
				if(checkbit==CHECK_ON)
				{
					S3keylock=LOCKED;
					bswlock=LOCKED;
					gwlock=LOCKED;
					TH0=0x3C;//T_BASE=
					TL0=0xB0;//50ms
					TR0=1;
				}
				else if(checkbit==CHECK_OFF)
				{
					TR0=0;
					display[2]=value/100;
					if(display[2]==0)
						display[2]=0x0B;
					display[1]=value%100/10;
					display[0]=value%10;
					S3keylock=UNLOCK;
					bswlock=UNLOCK;
					gwlock=UNLOCK;
					Buzzer_EN=0;
					DELAYMS(250);
					DELAYMS(250);
					DELAYMS(250);					
					DELAYMS(250);
					Buzzer_EN=1;
				}
				while(S4==0);
			}
		}//OK
		if((bswlock==UNLOCK)&&(S5==0))//bsw
		{
			DELAYMS(20);
			if(S5==0)
			{
				bswbit=~bswbit;
				if(bswbit==BSW_ON)
				{
					TH0=0x3C;//T_BASE=
					TL0=0xB0;//50ms
					TR0=1;
					gwlock=LOCKED;
					S3keylock=LOCKED;
					S4keylock=LOCKED;
				}
				else if(bswbit==BSW_OFF)
				{
					TR0=0;
					gwlock=UNLOCK;
					S3keylock=UNLOCK;
					S4keylock=UNLOCK;
					display[2]=value/100;
					if(display[2]==0)
						display[2]=0x0B;
					display[1]=value%100/10;
					display[0]=value%10;
				}
				while(S5==0);
			}
		}//OK
		if((gwlock==UNLOCK)&&(S6==0))
		{
			DELAYMS(20);
			if(S6==0)
			{
				gwbit=~gwbit;
				if(gwbit==GW_ON)
				{
					TH0=0x3C;//T_BASE=
					TL0=0xB0;//50ms
					TR0=1;
					bswlock=LOCKED;
					S3keylock=LOCKED;
					S4keylock=LOCKED;
				}
				else if(gwbit==GW_OFF)
				{
					TR0=0;
					bswlock=UNLOCK;
					S3keylock=UNLOCK;
					S4keylock=UNLOCK;
					display[2]=value/100;
					if(display[2]==0)
						display[2]=0x0B;
					display[1]=value%100/10;
					display[0]=value%10;
				}
				while(S6==0);
			}
		}//OK
	}//OK
	if(checkbit==CHECK_ON)
	{
		if(S7==0)//+
		{
			DELAYMS(20);
			if(S7==0)
			{
				if(value>=60)
				{
					maxvalue=value;
					Buzzer_EN=0;
					DELAYMS(250);
					DELAYMS(250);
					DELAYMS(250);					
					DELAYMS(250);
					Buzzer_EN=1;
					TR0=0;
					display[2]=maxvalue/100;
					if(display[2]==0)
						display[2]=0x0B;
					display[1]=maxvalue%100/10;
					display[0]=maxvalue%10;
				}
				while(S7==0);
			}
		}
		if(S8==0)//-
		{
			DELAYMS(20);
			if(S8==0)
			{
				if(value<=30)
				{
					minvalue=value;
					Buzzer_EN=0;
					DELAYMS(250);
					DELAYMS(250);
					DELAYMS(250);					
					DELAYMS(250);
					Buzzer_EN=1;
					TR0=0;
					display[2]=minvalue/100;
					if(display[2]==0)
						display[2]=0x0B;
					display[1]=minvalue%100/10;
					display[0]=minvalue%10;
				}
				while(S8==0);
			}
		}
	}
	if(bswbit==BSW_ON)
	{
		if(S7==0)
		{
			DELAYMS(20);
			if(S7==0)
			{
				if(value<110)//110dB
				{
					value+=10;
				}
				while(S7==0);
			}
		}
		if(S8==0)
		{
			DELAYMS(20);
			if(S8==0)
			{
				if(value>20)//20dB
				{
					value-=10;
				}
				while(S8==0);
			}
		}
	}//OK
	if(gwbit==GW_ON)
	{
		if(S7==0)
		{
			DELAYMS(20);
			if(S7==0)
			{
				if(digit<9)
				{
					digit++;
					value++;
				}
				while(S7==0);
			}
		}
		if(S8==0)
		{
			DELAYMS(20);
			if(S8==0)
			{
				if(digit>0)
				{
					digit--;
					value--;
				}
				while(S8==0);
			}			
		}
	}//OK
}//OK

void ADC()//OK
{
	uchar i,k,bw,sw,gw,dbvalue;
	uint ADCR=0;
	for(i=0;i<3;i++)//three times
	{
		ADC_CONTR=ADC_CONTR|ADC_START;
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		while(!(ADC_CONTR&ADC_FLAG));
		ADC_CONTR&=~ADC_FLAG;
		adc_code[i]=ADC_RES;
	}
	for(k=0;k<3;k++)
	{
		ADCR=ADCR+adc_code[k];
	}
	ADCR=ADCR/3;//average value
	bw=ADCR*5/256;
	sw=ADCR*5%256*10/256;
	gw=ADCR*5%256*10%256*10/256;
	dbvalue=(bw*100+sw*10+gw)/3.5;
	if(dbvalue<40)
		dbvalue=40;
	if(dbvalue>maxvalue)
		Buzzer_EN=0;//alarm
	else
		Buzzer_EN=1;
	display[2]=dbvalue/100;
	if(display[2]==0)
		display[2]=0x0B;
	display[1]=dbvalue%100/10;
	display[0]=dbvalue%10;
}

void DPY_ON()//OK
{
	P0=DPY[display[0]];
	DIG3=0;DIG2=DIG1=1;
	DELAYMS(2);
	P0=DPY[display[1]];
	DIG2=0;DIG3=DIG1=1;
	DELAYMS(2);
	P0=DPY[display[2]];
	DIG1=0;DIG3=DIG2=1;
	DELAYMS(2);
}

void main()//OK
{
	TMOD=0x11;
	EA=1;
	ET0=1;
	ET1=1;
	while(1)
	{
		KEYSCAN();
		DPY_ON();
	}
}

void T0_IRQ()interrupt 1//OK
{
	TR0=0;
	TH0=0x3C;//T_BASE=
	TL0=0xB0;//50ms
	t0_num++;
	if(t0_num==10)//0.5s
	{
		t0_num=0;
		FH=~FH;
		if(FH)
		{
			if(bswbit==BSW_ON)
			{
				display[2]=display[1]=0x0B;
			}
			if(gwbit==GW_ON)
			{
				display[0]=0x0B;
			}
			if(checkbit==CHECK_ON)
			{
				display[2]=display[1]=display[0]=0x0B;
			}
		}
		else
		{
			if(bswbit==BSW_ON)
			{
				display[2]=value/100;
				if(display[2]==0)
					display[2]=0x0B;
				display[1]=value%100/10;
			}
			if(gwbit==GW_ON)
			{
				display[0]=value%10;
			}
			if(checkbit==CHECK_ON)
			{
				display[2]=value/100;
				if(display[2]==0)
					display[2]=0x0B;
				display[1]=value%100/10;
				display[0]=value%10;
			}
		}
	}//OK
	TR0=1;
}

void T1_IRQ()interrupt 3//OK
{
	TR1=0;
	TH1=0x3C;//T_BASE=
	TL1=0xB0;//50ms
	t1_num++;
	if(t1_num==10)//0.5s
	{
		t1_num=0;
		ADC();
	}
	TR1=1;
}
		
	
