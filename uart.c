/*
*******************************************************************************
*
* �ļ�����Uart.c
* ��  ��������֡ģʽ��UART��������ģ��
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
#include <string.h>

bit flagFrame = 0;  //֡������ɱ�־�������յ�һ֡������
bit flagTxd = 0;    //���ֽڷ�����ɱ�־���������TXD�жϱ�־λ
unsigned char cntRxd = 0;   //�����ֽڼ�����
unsigned char idata bufRxd[32];  //�����ֽڻ�����  ����STC15W204Sû���ⲿ�洢����ʹ��256B�ڲ�RAM
//sbit BEEPT = P1^3;
extern void UartAction(unsigned char *buf, unsigned char len);
extern bit CmpMemory(unsigned char *ptr1, unsigned char *ptr2, unsigned char len);
extern void mdelay(unsigned int ms );
//extern bit flagBuzzOn;
/* �������ú�����baud-ͨ�Ų����� 9600bps@11.0592MHz */
void ConfigUART()
{
	SCON = 0x40;		//8λ����,�ɱ䲨����
	
	AUXR |= 0x04;		//��ʱ��2ʱ��ΪFosc,��1T
	T2L = 0xE0;			//�趨��ʱ��ֵ
	T2H = 0xFE;			//�趨��ʱ��ֵ
	AUXR |= 0x01;		//����1ѡ��ʱ��2Ϊ�����ʷ�����
	AUXR |= 0x10;		//������ʱ��2
	P_SW1 = 0x40;  	//����Ӳ�������л���P3.7��P3.6
	//EA = 1;	   		//����CPU�ж�
	ES = 1;
	REN = 1;			//ʹ�ܽ���
}
/* ��������д�룬�����ڷ��ͺ�����buf-���������ݵ�ָ�룬len-ָ���ķ��ͳ��� */
void UartWrite(unsigned char *buf, unsigned char len)
{
    while (len--)  //ѭ�����������ֽ�
    {
        flagTxd = 0;      //���㷢�ͱ�־
        SBUF = *buf++;    //����һ���ֽ�����
        while (!flagTxd); //�ȴ����ֽڷ������
    }
}
/* �������ݶ�ȡ������buf-����ָ�룬len-ָ���Ķ�ȡ���ȣ�����ֵ-ʵ�ʶ����ĳ��� */
unsigned char UartRead(unsigned char *buf, unsigned char len)
{
    unsigned char i;
    unsigned char recvlen = 0;
    if (len > cntRxd)  //ָ����ȡ���ȴ���ʵ�ʽ��յ������ݳ���ʱ��
    {                  //��ȡ��������Ϊʵ�ʽ��յ������ݳ���
        len = cntRxd;
    }

    for (i=0; i<len; i++)  //�������յ������ݵ�����ָ����
    {
        *buf++ = bufRxd[i];
		bufRxd[i] = '\0';
    }
    cntRxd = 0;  //���ռ���������

    return len;  //����ʵ�ʶ�ȡ����
}

void clearbufRxd()
{
	unsigned char len,i;
	len = sizeof(bufRxd);

    for (i=0; i<len; i++)  
    {
		bufRxd[i] = '\0';
    }
}
/* ���ڽ��ռ�أ��ɿ���ʱ���ж�֡���������ڶ�ʱ�ж��е��ã�ms-��ʱ��� */
void UartRxMonitor(unsigned char ms)
{
    static unsigned char cntbkp = 0;
    static unsigned char idletmr = 0;

    if (cntRxd > 0)  //���ռ�����������ʱ��������߿���ʱ��
    {
        if (cntbkp != cntRxd)  //���ռ������ı䣬���ս��յ�����ʱ��������м�ʱ
        {
            cntbkp = cntRxd;
            idletmr = 0;
        }
        else                   //���ռ�����δ�ı䣬�����߿���ʱ���ۻ�����ʱ��
        {
            if (idletmr < 30)  //���м�ʱС��30msʱ�������ۼ�
            {
                idletmr += ms;
                if (idletmr >= 30)  //����ʱ��ﵽ30msʱ�����ж�Ϊһ֡�������
                {
                    flagFrame = 1;  //����֡������ɱ�־
                }
            }
        }
    }
    else
    {
        cntbkp = 0;
    }
}
/* ���������������������֡�Ľ��գ����ȹ��ܺ�����������ѭ���е��� */
void UartDriver()
{
    unsigned char len;			
    unsigned char idata buf[32]; 		//����STC15W204Sû���ⲿ�洢����ʹ��256B�ڲ�RAM

    if (flagFrame) //�������ʱ����ȡ���������
    {
        flagFrame = 0;
        len = UartRead(buf, sizeof(buf));  //�����յ��������ȡ����������
		
        UartAction(buf, len);  //��������֡�����ö���ִ�к���
    }  
}

/* �����жϷ����� */
void InterruptUART() interrupt 4
{
	//flagBuzzOn = 1;
    if (RI)  //���յ����ֽ�
    {
        RI = 0;  //��������жϱ�־λ
        if (cntRxd < sizeof(bufRxd)) //���ջ�������δ����ʱ��
        {                            //��������ֽڣ�������������
            bufRxd[cntRxd++] = SBUF;
        }
    }
    if (TI)  //�ֽڷ������
    {
        TI = 0;   //���㷢���жϱ�־λ
        flagTxd = 1;  //�����ֽڷ�����ɱ�־
    }
}
/* �ǽ��յ��澯�������� */
bit isBuzzOn()
{
	unsigned char recvlen = 0;
	unsigned char code cmd0[] = "bon";   //������������

	recvlen = strlen(bufRxd);
	if((recvlen > 0)&&(recvlen >= (sizeof(cmd0)-1)))
	{
		if (CmpMemory(bufRxd, cmd0,sizeof(cmd0)-1)) 
		{
		    return 1;
		}
	}

	return 0;
}
/* ����AT����ʱ���ڽ���Ӧ������reStr-Ӧ���յ����ݵ�ָ�룬len-ָ��Ӧ�������ݵĳ��� */
bit UartAtAskResult(unsigned char *reStr, unsigned char len)
{
    unsigned char relen;
	unsigned char retry=10;			
    unsigned char idata buf[32]; 		//����STC15W204Sû���ⲿ�洢����ʹ��256B�ڲ�RAM

	while(retry--)
	{
	    if (flagFrame) //�������ʱ����ȡ���������
	    {
	        flagFrame = 0;
	        relen = UartRead(buf, sizeof(buf));  //�����յ��������ȡ����������
	
			if(relen >= len)
			{
				UartWrite(buf, relen);		
				if (CmpMemory(buf, reStr,len)) 
				{
				    return 1;
				}
			}
	    }
		mdelay(100);
	}
	return 0;
}