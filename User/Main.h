#ifndef MAIN_H
#define MAIN_H

#include "stm32f10x.h"


#define HARDVER 100
#define SOFTVER 100
#define PROTOCOL 101 //与上位机的通信协议

#define LEDB_Off()  GPIO_SetBits(GPIOB, GPIO_Pin_9)
#define LEDB_On()   GPIO_ResetBits(GPIOB, GPIO_Pin_9)

void TIM2_NVIC_Configuration(void);
void TIM2_Configuration(void);

extern unsigned char g_bBmqStop;//读取编码器值暂停标志
extern unsigned short g_ustime;
extern unsigned char g_bSending;
extern unsigned char g_bSendEn;
extern unsigned char g_bSelBigRange ;//大小量程标识  0：小量程  1：大量程
extern unsigned char g_bUartRead;//串口数据准备好标识
extern int g_iad1_dat,g_iad2_dat,g_iad3_dat,g_iad4_dat,g_iad5_dat,g_iad6_dat;
extern unsigned char g_ucAdcResultFlag;
extern int	g_iAdc1ZeroBuf;							//ch1 AD采集数据基础值缓冲（用作调试清零）
extern int	g_iAdc2ZeroBuf;							//ch2 AD采集数据基础值缓冲（用作调试清零）
void Delay(unsigned short int idly);
//extern unsigned char g_bReadAdcEN;//读取AD值允许
#endif
