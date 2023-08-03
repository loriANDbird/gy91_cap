#ifndef __USART1_H
#define	__USART1_H

#include "stm32f10x.h"
#include "Main.h"
#include <stdio.h>



// 串口对应的DMA请求通道
#define  USART_TX_DMA_CHANNEL     DMA1_Channel7
// 外设寄存器地址
#define  USART_DR_ADDRESS        (USART2_BASE+0x04)
// 一次发送的数据量
#define  SENDBUFF_SIZE            22

void USART1_Config(void);
void USART2_Config(void);
void NVIC_Configuration(void);
extern unsigned char g_ucUart2RecBuf[30];
extern unsigned char g_ucUart2SendBuf[30];
extern unsigned short g_usUart2RecCnt;//接收的字节数
extern unsigned short g_usUart2SendCnt;//发送字节数
extern unsigned short g_usUart2SendMaxVal;//发送最大字节数
extern unsigned char g_Uart2RecValid;
extern void USARTx_DMA_Config(void);
extern void MYDMA_Enable(DMA_Channel_TypeDef*DMA_CHx);
#endif /* __USART1_H */
