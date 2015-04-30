/*
*******************************************************************************
*
* 文件名：hc08.c
* 描  述：串口蓝牙模块程序
* 版本号：v1.0
* 备  注：
*         
* History: 				// 历史修改记录
* <author>     <time>     <version >     <desc>
* boojun   2015-04-27 	   v1.0 		 build this moudle
* 
* 版权所有，盗版必究
* Copyright(C) 南京市屹泓达电子科技有限公司 2015-2025
* All rights reserved
*******************************************************************************
*/
#include <STC15W20XS.h>
#include <stdio.h>
#include <string.h>
#include <intrins.h>
#include "config.h"

sbit HC08_RESET = P1^0;		//蓝牙复位，低电平复位，至少5ms   
sbit HC08_LED  = P3^2;		//蓝牙连接状态信号
extern void UartWrite(unsigned char *buf, unsigned char len);
extern bit CmpMemory(unsigned char *ptr1, unsigned char *ptr2, unsigned char len);
extern unsigned char UartRead(unsigned char *buf, unsigned char len);
extern bit UartAtAskResult(unsigned char *reStr, unsigned char len);

//@11.0592MHz 1ms延时函数
void mdelay(unsigned int ms )	   
{
	unsigned char i, j, m;
	for(m=0; m<ms; m++)
	{
		_nop_();
		_nop_();
		_nop_();
		i = 11;
		j = 190;
		do
		{
			while (--j);
		} while (--i);
	}
}

void Delay8000ms()		//@11.0592MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 81;
	j = 47;
	k = 124;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}

void Delay16000ms()		//@11.0592MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 161;
	j = 93;
	k = 254;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}

/* HC08模块测试函数，测试蓝牙模块是否正常工作 */
bit HC08_AtTest()
{
    bit result = 0;	
	unsigned char retry=4;		
//	unsigned char bufRxd[20] = {0};

	UartWrite("AT\r\n", 3);

	while(retry--)	   //最多延时retry X 50ms
	{
		mdelay(100); 
	 	result = UartAtAskResult("OK",2);
		if(result==1)
		{
			UartWrite("Test AT OK\r\n", 11);
			return 1;
		}
	}

	return 0;	  
}

/*
设置发射信号强度函数
设置：AT+TXPW[para] 
返回：OK+Set:[para]   Para: 0 ~ 3; 0: -23dbm、1: -6dbm; 2: 0dbm、 3: 4dbm; Default: 2 暂时不支持 4dbm 的发射信号强度。
如：设置发射信号强度为-6dbm
*/
bit HC08_setPower(unsigned int power)
{
    bit result = 0;	
	unsigned char retry=4;		
	unsigned char bufRxd[20] = {0};

	sprintf(bufRxd,"AT+TXPW%d\r\n",power);
	UartWrite(bufRxd, strlen(bufRxd)-1);

	while(retry--)	   //最多延时retry X 50ms
	{
		mdelay(50); 
	 	result = UartAtAskResult("OK+Set",6);
		if(result==1)
		{
			UartWrite("Set Power OK\r\n", 13);
			return 1;
		}
	}

	return 0;	
}
/*
设置设备角色：主从模式
设置：AT+ROLE1  1: 主设备 0: 从设备  Default: 0
返回：OK+Set:1  4F 4B 2B 53 65 74 3A 31 0D 0A
*/
bit HC08_setRole(unsigned int role)
{
 //   bit result = 0;	
//	unsigned char retry=4;		
	unsigned char bufRxd[20] = {0};

	sprintf(bufRxd,"AT+ROLE%d\r\n",role);
	UartWrite(bufRxd, strlen(bufRxd)-1);

	/*while(retry--)	   //最多延时retry X 50ms
	{
		mdelay(50); 
	 	result = UartAtAskResult("OK+Set",6);
		if(result==1)
		{
			UartWrite("Set Role OK\r\n", 13);
			return 1;
		}
	}	*/

	return 0;	
}

/* HC08模块复位函数*/
void HC08_Reset(void)
{
	HC08_RESET = 0;	//低电平复位，至少5ms 
	mdelay(8);
	HC08_RESET = 1;
	Delay8000ms();
	//Delay16000ms();
	//Delay16000ms();	 //等待蓝牙模块启动，时间为闪烁5次，800ms on,800ms off
}

bit HC08_Init(void)
{	
	//unsigned char ret = 0;
	HC08_setPower(1);
//	HC08_Reset();
	
/*	UartWrite("HC08_Reset\r\n", 11);

	if(!HC08_AtTest())
		return 0;

#ifdef 	DEMON
	if(HC08_setRole(0)) //从设备	
	{
		if(HC08_setPower(1)) // 0: -23dbm、1: -6dbm; 2: 0dbm
			return 1;
	}
#else
	if(HC08_setRole(1)) //主设备
		return 1;	
#endif	 */
	return 0; 
}