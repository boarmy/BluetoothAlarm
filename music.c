/*
*******************************************************************************
*
* 文件名：music.c
* 描  述：警车报警声音乐程序，使用延时实现。
* 版本号：v1.0
* 备  注：
* History: 				// 历史修改记录
* <author>     <time>     <version >     <desc>
* boojun   2015-04-23 	   v1.0 		 build this moudle
* 
* 版权所有，盗版必究
* Copyright(C) 南京市屹泓达电子科技有限公司 2015-2025
* All rights reserved
*******************************************************************************
*/

#include <STC15W20XS.h>
#include <intrins.h>
#include "config.h"

//#define DEMON
#ifdef 	DEMON
sbit CTR_BEEP =P1^3;   //蜂鸣器工作开关
sbit BEEP=P1^2;		   //蜂鸣器BEEP
#else
sbit LED=P1^2;		   //警灯
sbit BEEP=P1^3;		   //蜂鸣器DEMON
#endif

void delay250(unsigned int k);		//声明延时函数

void Delay250us()		//@11.0592MHz
{
	unsigned char i, j;

	_nop_();
	i = 3;
	j = 173;
	do
	{
		while (--j);
	} while (--i);
}

void delay250(unsigned int k)		//延时函数
{
	unsigned int a;
	for(a=0;a<k;a++)
	{
	//	for(b=0;b<25;b++);
		Delay250us();
	}
}

void PlayAlarm()							//启动告警函数
{
	unsigned int a,b;				//声明无符号整型变量i、j

#ifdef 	DEMON
	CTR_BEEP = 1;
#endif

	for(a=0;a<2000;a++)		//循环2000次，1khz响1s
	{
		BEEP=~BEEP;			//反相
		//LED=~LED;
		delay250(2);		//延时500us
	}
//	LED=~LED;
	for(b=0;b<2000;b++)		//循环2000次，1khz响1s
	{
 		BEEP=~BEEP;			//反相
		
 		delay250(1);		//延时250us
	}				
}  		 
