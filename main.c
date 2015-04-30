/*
*******************************************************************************
*
* 文件名：main.c
* 描  述：基于帧模式的实用串口蓝牙模块防盗报警程序
* 版本号：v1.0
* 备  注：
*         基于命令帧模式，通过UART串口控制蜂鸣器开关和液晶显示字符
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
#include <stdio.h>
#include <string.h>

bit flagBuzzOn = 0;   //蜂鸣器启动标志
unsigned int onLine = 0;   //设备离开监测区时间
unsigned int stime = 0;		//发送告警命令间隔时间
unsigned char T0RH = 0;  //T0重载值的高字节
unsigned char T0RL = 0;  //T0重载值的低字节

void ConfigTimer0(unsigned int ms);
void DevOfflineMonitor();
extern void UartDriver();
extern void ConfigUART();
extern void UartRxMonitor(unsigned char ms);
extern void UartWrite(unsigned char *buf, unsigned char len);
extern void PlayAlarm();
extern void clearbufRxd();
extern bit isBuzzOn();
extern bit HC08_Init(void);
extern void HC08_Reset(void);
extern void Delay8000ms();
//extern unsigned char idata bufRxd;
//extern void InitLcd1602();
//extern void LcdShowStr(unsigned char x, unsigned char y, unsigned char *str);
//extern void LcdAreaClear(unsigned char x, unsigned char y, unsigned char len);

void main()
{
    EA = 1;            //开总中断
	ConfigUART();  		//配置波特率为9600
    ConfigTimer0(1);   //配置T0定时1ms	r 
	HC08_Reset();
	//HC08_Init();
//	UartWrite("HC08_init\r\n", 10);
	//Delay8000ms();
    while (1)
    {
	   DevOfflineMonitor(); 
       UartDriver();  //调用串口驱动

	   if(flagBuzzOn)
	   {
	   		UartWrite("bon\r\n", 4);
			PlayAlarm();
	   }
	   if(stime > 800)  //每隔1s，发送一次buzz on
	   {
	   		stime = 0;
		 	UartWrite("bon\r\n", 4);
	   }	   														      
    }
}
/* 内存比较函数，比较两个指针所指向的内存数据是否相同，
   ptr1-待比较指针1，ptr2-待比较指针2，len-待比较长度
   返回值-两段内存数据完全相同时返回1，不同返回0 */
bit CmpMemory(unsigned char *ptr1, unsigned char *ptr2, unsigned char len)
{
    while (len--)
    {
        if (*ptr1++ != *ptr2++)  //遇到不相等数据时即刻返回0
        {
            return 0;
        }
    }
    return 1;  //比较完全部长度数据都相等则返回1
}
/* 串口动作函数，根据接收到的命令帧执行响应的动作
   buf-接收到的命令帧指针，len-命令帧长度 */
void UartAction(unsigned char *buf, unsigned char len)
{
    unsigned char i;
	unsigned char bufRxd[20] = {0};
    unsigned char code cmd0[] = "bon";   //开蜂鸣器命令
    unsigned char code cmd1[] = "buzz off";  //关蜂鸣器命令
    unsigned char code cmd2[] = "showstr ";  //字符串显示命令
	//unsigned char code cmd3[] = "OK";  //字符串显示命令
    unsigned char code cmdLen[] = {          //命令长度汇总表
        sizeof(cmd0)-1, sizeof(cmd1)-1, sizeof(cmd2)-1,
    };
    unsigned char code *cmdPtr[] = {         //命令指针汇总表
        &cmd0[0],  &cmd1[0],  &cmd2[0],
    };

    for (i=0; i<sizeof(cmdLen); i++)  //遍历命令列表，查找相同命令
    {
        if (len >= cmdLen[i])  //首先接收到的数据长度要不小于命令长度
        {
            if (CmpMemory(buf, cmdPtr[i], cmdLen[i]))  //比较相同时退出循环
            {
                break;
            }
        }
    }
    switch (i)  //循环退出时i的值即是当前命令的索引值
    {
        case 0:
            flagBuzzOn = 1; //开启蜂鸣器
            break;
        case 1:
            flagBuzzOn = 0; //关闭蜂鸣器
            break;
       /* case 2:
            buf[len] = '\0';  //为接收到的字符串添加结束符
            LcdShowStr(0, 0, buf+cmdLen[2]);  //显示命令后的字符串
            i = len - cmdLen[2];              //计算有效字符个数
            if (i < 16)  //有效字符少于16时，清除液晶上的后续字符位
            {
                LcdAreaClear(i, 0, 16-i);
            }
            break;	  */
        /*case 3:
           // flagAtAsk = 1; //AT应答
		   	UartWrite(buf, strlen(buf));
            break;	*/
        default:   //未找到相符命令时，给上机发送“错误命令”的提示
			sprintf(bufRxd,"bad command:%s\r\n",buf);
            //UartWrite("bad command:\r\n", sizeof("bad command.\r\n")-1);
			UartWrite(bufRxd, sizeof(bufRxd)-1);
            return;
    }
    buf[len++] = '\r';  //有效命令被执行后，在原命令帧之后添加
    buf[len++] = '\n';  //回车换行符后返回给上位机，表示已执行
    //UartWrite(buf, len);
}

void DevOfflineMonitor()
{
	//unsigned char bufRxd[20] = {0};
	if(flagBuzzOn)
	{
		if(isBuzzOn())
		{
		   onLine = 0;
		}
		else if(onLine > 3000)//3S内没收到设备发送消息，认为设备已离开监控范围
		{
		   flagBuzzOn = 0;
		   onLine = 0;
		}		
	}
	//sprintf(bufRxd,"onLine:%d\r\n",onLine);
	//UartWrite(bufRxd, strlen(bufRxd));
}

/* 配置并启动T0，ms-T0定时时间 */
void ConfigTimer0(unsigned int ms)
{
    unsigned long tmp;  //临时变量
    
    tmp = 11059200 / 12;      //定时器计数频率
    tmp = (tmp * ms) / 1000;  //计算所需的计数值
    tmp = 65536 - tmp;        //计算定时器重载值
    tmp = tmp + 33;           //补偿中断响应延时造成的误差
    T0RH = (unsigned char)(tmp>>8);  //定时器重载值拆分为高低字节
    T0RL = (unsigned char)tmp;
    //TMOD &= 0xF0;   //清零T0的控制位
    TMOD = 0x01;   //配置T0为模式1
    TH0 = T0RH;     //加载T0重载值
    TL0 = T0RL;
    ET0 = 1;        //使能T0中断
    TR0 = 1;        //启动T0
}
/* T0中断服务函数，执行串口接收监控和蜂鸣器驱动 */
void InterruptTimer0() interrupt 1
{
    TH0 = T0RH;  //重新加载重载值
    TL0 = T0RL;

	if (flagBuzzOn)
		onLine++; 

	stime++;
    UartRxMonitor(1);  //串口接收监控
}
