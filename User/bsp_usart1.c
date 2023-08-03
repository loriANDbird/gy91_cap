/**
  ******************************************************************************
  * @file    bsp_usart1.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   usart应用bsp
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO-MINI STM32 开发板 
  * 论坛    :http://www.chuxue123.com
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
#include "stm32f10x.h"  
#include "bsp_usart1.h"
#include "LcdMenu.h"
#include "Main.h"


 /**
  * @brief  USART1 GPIO 配置,工作模式配置。9600 8-N-1
  * @param  无
  * @retval 无
  */
void USART1_Config(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure; 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	
	/* config USART1 clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	/* USART1 GPIO config */
	/* Configure USART1 Tx (PA.09/PA.02) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);    
	/* Configure USART1 Rx (PA.10) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/*串口2配置*/

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure);
	
	USART_ClearITPendingBit(USART2, USART_IT_TC);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART2, USART_IT_TC, ENABLE);
	
	
	USART_Cmd(USART2, ENABLE);
	while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
	USART_ClearITPendingBit(USART2, USART_IT_TC);

	/* Enable the USARTy Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
void USART2_Data(void)
{
	if(USART_GetFlagStatus(USART2, USART_FLAG_ORE) != RESET){
		USART_ClearFlag(USART2, USART_FLAG_ORE);
		USART_ReceiveData(USART2);
	}
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{ 				
		g_Uart0RecBuf[g_Uart0RecCnt] = USART_ReceiveData(USART2);
		if(g_Uart0RecBuf[0] == 0xA0)
		{
			g_Uart0RecCnt += 1;
			if(g_Uart0RecCnt >= 6){  //A5 x3 x2 x1 x0 A5
				g_Uart0RecCnt = 0;
				if(g_Uart0RecBuf[5] == 0xA5){				
					g_Uart0RecValid = 1;
					g_Uart0RecTimeOut = 0;	
				}
				else{
					 g_Uart0RecCnt = 0;
					 g_Uart0RecTimeOut = 5;
				}
			}
			else{
				g_Uart0RecTimeOut = 5;
			}
		}
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
	}
	if(USART_GetITStatus(USART2, USART_IT_TC) != RESET)
	{
		USART_ClearITPendingBit(USART2, USART_IT_TC);

		g_Uart0SendCnt += 1;
		if(g_Uart0SendCnt < g_Uart0SendMaxVal)	{			
			USART_SendData(USART2, g_Uart0SendBuf[g_Uart0SendCnt]);
		}
		
	}
}
/*********************************************END OF FILE**********************/
