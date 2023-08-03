/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTI
  
  AL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "Main.h"
#include "Systick.h"
#include "USART.h"
#include <stdio.h>
#include "AdcCS1237.h"

extern int SEGGER_RTT_printf(unsigned BufferIndex, const char * sFormat, ...);

extern unsigned char g_ucKeyVal;
static __IO u32 TimingDelay = 20;//定义20ms一次

/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
//void SysTick_Handler(void)
//{

//}



void USB_LP_CAN1_RX0_IRQHandler(void)
{
//  USB_Istr();
}

/*
 * 函数名：USB_HP_CAN1_TX_IRQHandler
 * 描述  ：USB高优先级中断请求 或发送中断
 * 输入  ：无		 
 * 输出  ：无
 */
void USB_HP_CAN1_TX_IRQHandler(void)
{
 // CTR_HP();
}


void EXTI2_IRQHandler(void)
{
		
    
}

void EXTI9_5_IRQHandler(void)
{

}

void EXTI15_10_IRQHandler(void)
{
    
}
/**
  * @brief  This function handles TIM2 interrupt request.
  * @param  None
  * @retval None
  */
void TIM2_IRQHandler(void)
{
	 
}
/**
  * @brief  This function handles TIM2 interrupt request.
  * @param  None
  * @retval None
  */
void TIM3_IRQHandler(void)
{    

}
void TIM4_IRQHandler(void)
{ 
  if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
  }
}
void USART1_IRQHandler(void)
{
   if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{ 				
		g_ucUart2RecBuf[g_usUart2RecCnt] = USART_ReceiveData(USART1);
		if(g_ucUart2RecBuf[0] == 0xA0)
		{
			g_usUart2RecCnt += 1;
			if(g_usUart2RecCnt >= 6)
			{  //A5 x3 x2 x1 x0 A5
				//g_usUart2RecCnt = 0;
				if(g_ucUart2RecBuf[5] == 0xA5){				
					g_Uart2RecValid = 1;
//					if((g_ucUart2RecBuf[1] == 0x11)||(g_ucUart2RecBuf[1] == 0x22)){
//						SEGGER_RTT_printf(0,"%s\r\n","Hello");
//					}
				//	g_Uart0RecTimeOut = 0;	
				}
				else{
					 g_usUart2RecCnt = 0;
					// g_Uart0RecTimeOut = 5;
				}
			}			
		}
    USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
	if(USART_GetITStatus(USART1, USART_IT_TC) != RESET)
	{
		USART_ClearITPendingBit(USART1, USART_IT_TC);

		g_usUart2SendCnt += 1;
		if(g_usUart2SendCnt < g_usUart2SendMaxVal)	{           
			USART_SendData(USART1, g_ucUart2SendBuf[g_usUart2SendCnt]);
		}
		else
		{
			g_bSending = 0;
		}
		
	}
}
void USART2_IRQHandler(void)
{	
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{ 				
		g_ucUart2RecBuf[g_usUart2RecCnt] = USART_ReceiveData(USART2);
		if(g_ucUart2RecBuf[0] == 0xA0)
		{
			g_usUart2RecCnt += 1;
			if(g_usUart2RecCnt >= 6)
			{  //A5 x3 x2 x1 x0 A5
				//g_usUart2RecCnt = 0;
				if(g_ucUart2RecBuf[5] == 0xA5){				
					g_Uart2RecValid = 1;
				//	g_Uart0RecTimeOut = 0;	
				}
				else{
					 g_usUart2RecCnt = 0;
					// g_Uart0RecTimeOut = 5;
				}
			}			
		}
    USART_ClearITPendingBit(USART2, USART_IT_RXNE);
	}
	if(USART_GetITStatus(USART2, USART_IT_TC) != RESET)
	{
		USART_ClearITPendingBit(USART2, USART_IT_TC);

		g_usUart2SendCnt += 1;
		if(g_usUart2SendCnt < g_usUart2SendMaxVal)	{           
			USART_SendData(USART2, g_ucUart2SendBuf[g_usUart2SendCnt]);
		}
		else
		{
			g_bSending = 0;
		}
		
	}
}
/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
