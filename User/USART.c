
#include "stm32f10x.h"  
#include "USART.h"
#include "stm32f10x_dma.h"
unsigned char g_ucUart2RecBuf[30];
unsigned char g_ucUart2SendBuf[30];
unsigned short g_usUart2RecCnt;//���յ��ֽ���
unsigned short g_usUart2SendCnt;//�����ֽ���
unsigned short g_usUart2SendMaxVal;//��������ֽ���
unsigned char g_Uart2RecValid;//�������ݽ��պñ�ʶ
 /**
  * @brief  USART1 GPIO ����,����ģʽ���á�9600 8-N-1
  * @param  ��
  * @retval ��
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
//    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//���ݳ��� 8bit
//    USART_InitStructure.USART_StopBits = USART_StopBits_1;//ֹͣλ 1λ
//    USART_InitStructure.USART_Parity = USART_Parity_No ;//��У��λ
//    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ��������
//    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//
//    USART_Init(USART1, &USART_InitStructure); 

//    //	USART_ClearITPendingBit(USART2, USART_IT_TC);
//    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//ʹ�ܽ����ж�
//    //	USART_ITConfig(USART2, USART_IT_TC, ENABLE);
//    //	USART_ITConfig(USART2, USART_IT_RXNE |USART_IT_TC , ENABLE);		//ʹ�ܷ��ͽ����ж�
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
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//���ݳ��� 8bit
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//ֹͣλ 1λ
    USART_InitStructure.USART_Parity = USART_Parity_No ;//��У��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ��������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//
    USART_Init(USART2, &USART_InitStructure); 

    //	USART_ClearITPendingBit(USART2, USART_IT_TC);
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//ʹ�ܽ����ж�
    //	USART_ITConfig(USART2, USART_IT_TC, ENABLE);
    //	USART_ITConfig(USART2, USART_IT_RXNE |USART_IT_TC , ENABLE);		//ʹ�ܷ��ͽ����ж�
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

/*�˴����������ж���
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
	
		// ����DMAʱ��
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
		// ����DMAԴ��ַ���������ݼĴ�����ַ*/
    DMA_InitStructure.DMA_PeripheralBaseAddr = USART_DR_ADDRESS;
		// �ڴ��ַ(Ҫ����ı�����ָ��)
		DMA_InitStructure.DMA_MemoryBaseAddr = (u32)g_ucUart2SendBuf;
		// ���򣺴��ڴ浽����	
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
		// �����С	
		DMA_InitStructure.DMA_BufferSize = SENDBUFF_SIZE;
		// �����ַ����	    
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		// �ڴ��ַ����
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		// �������ݵ�λ	
		DMA_InitStructure.DMA_PeripheralDataSize = 
	  DMA_PeripheralDataSize_Byte;
		// �ڴ����ݵ�λ
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	 
		// DMAģʽ��һ�λ���ѭ��ģʽ
		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal ;
		//DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	
		// ���ȼ�����	
		DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; 
		// ��ֹ�ڴ浽�ڴ�Ĵ���
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
		// ����DMAͨ��		   
		DMA_Init(USART_TX_DMA_CHANNEL, &DMA_InitStructure);		
		// ʹ��DMA
		DMA_Cmd (USART_TX_DMA_CHANNEL,ENABLE);
		
		USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);
}

//����һ��DMA����
void MYDMA_Enable(DMA_Channel_TypeDef*DMA_CHx)
{ 
	DMA_Cmd(DMA_CHx, DISABLE );  //�ر�USART1 TX DMA1 ��ָʾ��ͨ��      
 	DMA_SetCurrDataCounter(DMA_CHx,SENDBUFF_SIZE);//DMAͨ����DMA����Ĵ�С
 	DMA_Cmd(DMA_CHx, ENABLE);  //ʹ��USART1 TX DMA1 ��ָʾ��ͨ�� 
}	  
