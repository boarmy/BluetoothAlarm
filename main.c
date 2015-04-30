/*
*******************************************************************************
*
* �ļ�����main.c
* ��  ��������֡ģʽ��ʵ�ô�������ģ�������������
* �汾�ţ�v1.0
* ��  ע��
*         ��������֡ģʽ��ͨ��UART���ڿ��Ʒ��������غ�Һ����ʾ�ַ�
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
#include <stdio.h>
#include <string.h>

bit flagBuzzOn = 0;   //������������־
unsigned int onLine = 0;   //�豸�뿪�����ʱ��
unsigned int stime = 0;		//���͸澯������ʱ��
unsigned char T0RH = 0;  //T0����ֵ�ĸ��ֽ�
unsigned char T0RL = 0;  //T0����ֵ�ĵ��ֽ�

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
    EA = 1;            //�����ж�
	ConfigUART();  		//���ò�����Ϊ9600
    ConfigTimer0(1);   //����T0��ʱ1ms	r 
	HC08_Reset();
	//HC08_Init();
//	UartWrite("HC08_init\r\n", 10);
	//Delay8000ms();
    while (1)
    {
	   DevOfflineMonitor(); 
       UartDriver();  //���ô�������

	   if(flagBuzzOn)
	   {
	   		UartWrite("bon\r\n", 4);
			PlayAlarm();
	   }
	   if(stime > 800)  //ÿ��1s������һ��buzz on
	   {
	   		stime = 0;
		 	UartWrite("bon\r\n", 4);
	   }	   														      
    }
}
/* �ڴ�ȽϺ������Ƚ�����ָ����ָ����ڴ������Ƿ���ͬ��
   ptr1-���Ƚ�ָ��1��ptr2-���Ƚ�ָ��2��len-���Ƚϳ���
   ����ֵ-�����ڴ�������ȫ��ͬʱ����1����ͬ����0 */
bit CmpMemory(unsigned char *ptr1, unsigned char *ptr2, unsigned char len)
{
    while (len--)
    {
        if (*ptr1++ != *ptr2++)  //�������������ʱ���̷���0
        {
            return 0;
        }
    }
    return 1;  //�Ƚ���ȫ���������ݶ�����򷵻�1
}
/* ���ڶ������������ݽ��յ�������ִ֡����Ӧ�Ķ���
   buf-���յ�������ָ֡�룬len-����֡���� */
void UartAction(unsigned char *buf, unsigned char len)
{
    unsigned char i;
	unsigned char bufRxd[20] = {0};
    unsigned char code cmd0[] = "bon";   //������������
    unsigned char code cmd1[] = "buzz off";  //�ط���������
    unsigned char code cmd2[] = "showstr ";  //�ַ�����ʾ����
	//unsigned char code cmd3[] = "OK";  //�ַ�����ʾ����
    unsigned char code cmdLen[] = {          //����Ȼ��ܱ�
        sizeof(cmd0)-1, sizeof(cmd1)-1, sizeof(cmd2)-1,
    };
    unsigned char code *cmdPtr[] = {         //����ָ����ܱ�
        &cmd0[0],  &cmd1[0],  &cmd2[0],
    };

    for (i=0; i<sizeof(cmdLen); i++)  //���������б�������ͬ����
    {
        if (len >= cmdLen[i])  //���Ƚ��յ������ݳ���Ҫ��С�������
        {
            if (CmpMemory(buf, cmdPtr[i], cmdLen[i]))  //�Ƚ���ͬʱ�˳�ѭ��
            {
                break;
            }
        }
    }
    switch (i)  //ѭ���˳�ʱi��ֵ���ǵ�ǰ���������ֵ
    {
        case 0:
            flagBuzzOn = 1; //����������
            break;
        case 1:
            flagBuzzOn = 0; //�رշ�����
            break;
       /* case 2:
            buf[len] = '\0';  //Ϊ���յ����ַ�����ӽ�����
            LcdShowStr(0, 0, buf+cmdLen[2]);  //��ʾ�������ַ���
            i = len - cmdLen[2];              //������Ч�ַ�����
            if (i < 16)  //��Ч�ַ�����16ʱ�����Һ���ϵĺ����ַ�λ
            {
                LcdAreaClear(i, 0, 16-i);
            }
            break;	  */
        /*case 3:
           // flagAtAsk = 1; //ATӦ��
		   	UartWrite(buf, strlen(buf));
            break;	*/
        default:   //δ�ҵ��������ʱ�����ϻ����͡������������ʾ
			sprintf(bufRxd,"bad command:%s\r\n",buf);
            //UartWrite("bad command:\r\n", sizeof("bad command.\r\n")-1);
			UartWrite(bufRxd, sizeof(bufRxd)-1);
            return;
    }
    buf[len++] = '\r';  //��Ч���ִ�к���ԭ����֮֡�����
    buf[len++] = '\n';  //�س����з��󷵻ظ���λ������ʾ��ִ��
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
		else if(onLine > 3000)//3S��û�յ��豸������Ϣ����Ϊ�豸���뿪��ط�Χ
		{
		   flagBuzzOn = 0;
		   onLine = 0;
		}		
	}
	//sprintf(bufRxd,"onLine:%d\r\n",onLine);
	//UartWrite(bufRxd, strlen(bufRxd));
}

/* ���ò�����T0��ms-T0��ʱʱ�� */
void ConfigTimer0(unsigned int ms)
{
    unsigned long tmp;  //��ʱ����
    
    tmp = 11059200 / 12;      //��ʱ������Ƶ��
    tmp = (tmp * ms) / 1000;  //��������ļ���ֵ
    tmp = 65536 - tmp;        //���㶨ʱ������ֵ
    tmp = tmp + 33;           //�����ж���Ӧ��ʱ��ɵ����
    T0RH = (unsigned char)(tmp>>8);  //��ʱ������ֵ���Ϊ�ߵ��ֽ�
    T0RL = (unsigned char)tmp;
    //TMOD &= 0xF0;   //����T0�Ŀ���λ
    TMOD = 0x01;   //����T0Ϊģʽ1
    TH0 = T0RH;     //����T0����ֵ
    TL0 = T0RL;
    ET0 = 1;        //ʹ��T0�ж�
    TR0 = 1;        //����T0
}
/* T0�жϷ�������ִ�д��ڽ��ռ�غͷ��������� */
void InterruptTimer0() interrupt 1
{
    TH0 = T0RH;  //���¼�������ֵ
    TL0 = T0RL;

	if (flagBuzzOn)
		onLine++; 

	stime++;
    UartRxMonitor(1);  //���ڽ��ռ��
}
