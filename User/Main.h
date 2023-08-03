#ifndef MAIN_H
#define MAIN_H

#include "stm32f10x.h"


#define HARDVER 100
#define SOFTVER 100
#define PROTOCOL 101 //����λ����ͨ��Э��

#define LEDB_Off()  GPIO_SetBits(GPIOB, GPIO_Pin_9)
#define LEDB_On()   GPIO_ResetBits(GPIOB, GPIO_Pin_9)

void TIM2_NVIC_Configuration(void);
void TIM2_Configuration(void);

extern unsigned char g_bBmqStop;//��ȡ������ֵ��ͣ��־
extern unsigned short g_ustime;
extern unsigned char g_bSending;
extern unsigned char g_bSendEn;
extern unsigned char g_bSelBigRange ;//��С���̱�ʶ  0��С����  1��������
extern unsigned char g_bUartRead;//��������׼���ñ�ʶ
extern int g_iad1_dat,g_iad2_dat,g_iad3_dat,g_iad4_dat,g_iad5_dat,g_iad6_dat;
extern unsigned char g_ucAdcResultFlag;
extern int	g_iAdc1ZeroBuf;							//ch1 AD�ɼ����ݻ���ֵ���壨�����������㣩
extern int	g_iAdc2ZeroBuf;							//ch2 AD�ɼ����ݻ���ֵ���壨�����������㣩
void Delay(unsigned short int idly);
//extern unsigned char g_bReadAdcEN;//��ȡADֵ����
#endif
