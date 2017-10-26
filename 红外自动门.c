/ * ************************************************ *************************
copyright：2017，money89757
name：main.c
explain：Infrared automatic door
author：money89757
date：2017/9/18
************************************************** ********************* * * /
#include <reg51.h>
#define uchar unsigned char
#define uint unsigned int
sbit PWRON=P2^4;//POWER
sbit FR=P2^5;//Motor
sbit SPK=P2^6;//Buzzer
sbit LP=P2^7;//LowPulse
sbit RevSpeedCal=P2^3;//MotorSpeed
sbit W1=P2^2;//DPY1
sbit W2=P2^1;//DPY2
sbit W3=P2^0;//DPY3
uchar code DPY[]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0xBF,0x8E,0x8C,0xAF,0xFF};
//0~9 - F P r None
uchar data DPY3[3]={0x0A,0x0A,0x0A};//---
uchar num,MotorStatus,Second,Clock;
uint counter1,counter2,counter3,RT,UT;//Remaining Time Used Time
bit FH,STFlag;//ShortTime

void Delayms(uint ms)
{uchar i;
 while(ms--)
    {for(i=123;i>0;i--);}
}

/*
 * Delay effect
 *
 * @param[in]  Delay time
 * 
 */
void Display()
{P0=DPY[DPY3[0]];
 W1=W2=1;W3=0;
 Delayms(2);
 P0=DPY[DPY3[1]];
 W1=W3=1;W2=0;
 Delayms(2);
 P0=DPY[DPY3[2]];
 W2=W3=1;W1=0;
 Delayms(2);
}

/*
 * Main
 * 
 */
void main()
{TMOD=0x01;
 EA=1;
 ET0=1;
 while(1)
    {if(MotorStatus==0)
        if(LP==0)
            {FR=0;
             PWRON=0;//Forward
             TH0=(65536-50000)/256;
             TL0=(65536-50000)%256;//50ms
             MotorStatus=1;//Forward Status
             DPY3[2]=0x0B;//F
             DPY3[1]=0x0A;//-
             DPY3[0]=Second;
             TR0=1;}
     if(MotorStatus==3)
        if(LP==0)
            {TR0=0;
             PWRON=1;//Stop
             FR=0;
             Delayms(2000);
             MotorStatus=4;
             Clock=Second;
             DPY3[2]=0x0B;//F
             DPY3[1]=0x0A;//-
             DPY3[0]=7-Second;
             RT=65536-(TH0*256+TL0);
             UT=50000-RT;
             TH0=(65536-UT)/256;
             TL0=(65536-UT)%256;//50ms
             STFlag=1;
             PWRON=0;
             TR0=1;}
     if(MotorStatus==1)
        if(RevSpeedCal==0)
            {counter1++;}
     if(MotorStatus==3)
        if(RevSpeedCal==0)
            {counter2++;}
     if(MotorStatus==4)
        if(RevSpeedCal==0)
            {counter3++;}
     Display();
    }
}

/*
 * Timer 0 interrupt 1
 * 
 */
void Timer0()interrupt 1
{TH0=(65536-50000)/256;
 TL0=(65536-50000)%256;
 num++;
 if(STFlag==1)
    {num=0;STFlag=0;}
 if(MotorStatus!=4)
    if(num==20)
    {num=0;
     Second++;
     DPY3[0]=Second;}
 if(MotorStatus==4)
    if(num==20)
    {num=0;
     Second--;
     DPY3[0]=7-Second;}
 if(MotorStatus==1)
    if(Second==7)
        {PWRON=1;//Motor Stop
         FR=1;
         Second=0;
         counter1=0;
         MotorStatus=2;//Pause Status
         DPY3[2]=0x0C;//P
         DPY3[1]=0x0A;//-
         DPY3[0]=Second;}
 if(MotorStatus==2)
    if(Second==4)
        {Second=0;
         MotorStatus=3;//Reversal Status
         DPY3[2]=0x0D;//r
         DPY3[1]=0x0A;//-
         DPY3[0]=Second;
         PWRON=0;}//Reversal
 if(MotorStatus==3)
    if(Second==7)
        {PWRON=1;//Motor Stop
         FR=0;
         Second=0;
         MotorStatus=0;
         counter2=0;
         TR0=0;
         DPY3[2]=0x0A;//-
         DPY3[1]=0x0A;//-
         DPY3[0]=0x0A;//-
        }
 if(MotorStatus==4)
    if(Second==0)
        {PWRON=1;//Stop
         FR=1;
         MotorStatus=2;
         DPY3[2]=0x0C;//P
         DPY3[1]=0x0A;//-
         DPY3[0]=Second;
         counter2=counter3=0;}
 if(MotorStatus==5)
    if(Second==1)
    {Second=0;FH=~FH;
     if(FH){DPY3[2]=DPY3[1]=0x01;DPY3[0]=0x00;SPK=0;}
        else{DPY3[2]=DPY3[1]=DPY3[0]=0x0E;SPK=1;}
    }
 if(MotorStatus==1)
    if(Second>1)
        if((counter1/Second)<48)
            {PWRON=1;Second=0;counter1=0;MotorStatus=5;}
 if(MotorStatus==3)
    if(Second>1)
        if((counter2/Second)<42)
            {PWRON=1;Second=0;counter2=0;MotorStatus=5;}
 if(MotorStatus==4)
    if(Second<(Clock-1))
        if(counter3/(Clock-Second)<32)
            {PWRON=1;Second=0;counter3=0;MotorStatus=5;}
}


