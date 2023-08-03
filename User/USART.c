
#include "stm32f10x.h"  
#include "USART.h"
#include "stm32f10x_dma.h"
unsigned char g_ucUart2RecBuf[30];
unsigned char g_ucUart2SendBuf[30];
unsigned short g_usUart2RecCnt;//接收的字节数
unsigned short g_usUart2SendCnt;//发送字节数
unsigned short g_usUart2SendMaxVal;//发送最大字节数
unsigned char g_Uart2RecValid;//串口数据接收好标识
 /**
  * @brief  USART1 GPIO 配置,工作模式配置。9600 8-N-1
  * @param  无
  * @retval 无
  */
// void USART1_Config(void)
//{
//    GPIO_InitTypeDef GPIO_InitStructure;
//    USART_InitTypeDef USART_InitStructure;
//    NVIC_InitTypeDef NVIC_InitStructure; 

//    /* config USART1 clock */
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

//    /* USART1 GPIO config */
//    /* Configure USART2 Tx (PA.09) as alternate function push-pull */
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
//    GPIO_Init(GPIOA, &GPIO_InitStructure);

//    /* Configure USART1 Rx (PA.10) as input floating */
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//    GPIO_Init(GPIOA, &GPIO_InitStructure);
//        
//    /* USART2 mode config */
//    USART_InitStructure.USART_BaudRate = 9600;
//    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//数据长度 8bit
//    USART_InitStructure.USART_StopBits = USART_StopBits_1;//停止位 1位
//    USART_InitStructure.USART_Parity = USART_Parity_No ;//无校验位
//    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件流控制
//    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//
//    USART_Init(USART1, &USART_InitStructure); 

//    //	USART_ClearITPendingBit(USART2, USART_IT_TC);
//    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//使能接收中断
//    //	USART_ITConfig(USART2, USART_IT_TC, ENABLE);
//    //	USART_ITConfig(USART2, USART_IT_RXNE |USART_IT_TC , ENABLE);		//使能发送接收中断
//    USART_Cmd(USART1, ENABLE);


//    // Configure the NVIC Preemption Priority Bits //  
//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

//    // Enable the USARTy Interrupt //
//    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	 
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&NVIC_InitStructure);
//}
void USART2_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure; 

    /* config USART2 clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    /* USART2 GPIO config */
    /* Configure USART2 Tx (PA.02) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure USART1 Rx (PA.03) as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
        
    /* USART2 mode config */
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//数据长度 8bit
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//停止位 1位
    USART_InitStructure.USART_Parity = USART_Parity_No ;//无校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//
    USART_Init(USART2, &USART_InitStructure); 

    //	USART_ClearITPendingBit(USART2, USART_IT_TC);
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//使能接收中断
    //	USART_ITConfig(USART2, USART_IT_TC, ENABLE);
    //	USART_ITConfig(USART2, USART_IT_RXNE |USART_IT_TC , ENABLE);		//使能发送接收中断
    USART_Cmd(USART2, ENABLE);


    // Configure the NVIC Preemption Priority Bits //  
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

    // Enable the USARTy Interrupt //
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;	 
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*此处函数放于中断中
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
}*/
/*********************************************END OF FILE**********************/
void USARTx_DMA_Config(void)
{
		DMA_InitTypeDef DMA_InitStructure;
	
		// 开启DMA时钟
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
		// 设置DMA源地址：串口数据寄存器地址*/
    DMA_InitStructure.DMA_PeripheralBaseAddr = USART_DR_ADDRESS;
		// 内存地址(要传输的变量的指针)
		DMA_InitStructure.DMA_MemoryBaseAddr = (u32)g_ucUart2SendBuf;
		// 方向：从内存到外设	
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
		// 传输大小	
		DMA_InitStructure.DMA_BufferSize = SENDBUFF_SIZE;
		// 外设地址不增	    
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		// 内存地址自增
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		// 外设数据单位	
		DMA_InitStructure.DMA_PeripheralDataSize = 
	  DMA_PeripheralDataSize_Byte;
		// 内存数据单位
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	 
		// DMA模式，一次或者循环模式
		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal ;
		//DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	
		// 优先级：中	
		DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; 
		// 禁止内存到内存的传输
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
		// 配置DMA通道		   
		DMA_Init(USART_TX_DMA_CHANNEL, &DMA_InitStructure);		
		// 使能DMA
		DMA_Cmd (USART_TX_DMA_CHANNEL,ENABLE);
		
		USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);
}

//开启一次DMA传输
void MYDMA_Enable(DMA_Channel_TypeDef*DMA_CHx)
{ 
	DMA_Cmd(DMA_CHx, DISABLE );  //关闭USART1 TX DMA1 所指示的通道      
 	DMA_SetCurrDataCounter(DMA_CHx,SENDBUFF_SIZE);//DMA通道的DMA缓存的大小
 	DMA_Cmd(DMA_CHx, ENABLE);  //使能USART1 TX DMA1 所指示的通道 
}	  
