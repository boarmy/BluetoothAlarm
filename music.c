/*
*******************************************************************************
*
* �ļ�����music.c
* ��  �����������������ֳ���ʹ����ʱʵ�֡�
* �汾�ţ�v1.0
* ��  ע��
* History: 				// ��ʷ�޸ļ�¼
* <author>     <time>     <version >     <desc>
* boojun   2015-04-23 	   v1.0 		 build this moudle
* 
* ��Ȩ���У�����ؾ�
* Copyright(C) �Ͼ�����������ӿƼ����޹�˾ 2015-2025
* All rights reserved
*******************************************************************************
*/

#include <STC15W20XS.h>
#include <intrins.h>
#include "config.h"

//#define DEMON
#ifdef 	DEMON
sbit CTR_BEEP =P1^3;   //��������������
sbit BEEP=P1^2;		   //������BEEP
#else
sbit LED=P1^2;		   //����
sbit BEEP=P1^3;		   //������DEMON
#endif

void delay250(unsigned int k);		//������ʱ����

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

void delay250(unsigned int k)		//��ʱ����
{
	unsigned int a;
	for(a=0;a<k;a++)
	{
	//	for(b=0;b<25;b++);
		Delay250us();
	}
}

void PlayAlarm()							//�����澯����
{
	unsigned int a,b;				//�����޷������ͱ���i��j

#ifdef 	DEMON
	CTR_BEEP = 1;
#endif

	for(a=0;a<2000;a++)		//ѭ��2000�Σ�1khz��1s
	{
		BEEP=~BEEP;			//����
		//LED=~LED;
		delay250(2);		//��ʱ500us
	}
//	LED=~LED;
	for(b=0;b<2000;b++)		//ѭ��2000�Σ�1khz��1s
	{
 		BEEP=~BEEP;			//����
		
 		delay250(1);		//��ʱ250us
	}				
}  		 
