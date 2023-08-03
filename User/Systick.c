#include "stm32f10x.h"
#include "Systick.h"
#include "main.h"
 

unsigned char g_uc1msTick = 0;  
//unsigned char g_uc1msTick; 2021.04.08 WA

int tmp, tmp1, tmp2;
#define  DWT_CYCCNT  *(volatile uint32_t *)0xE0001004
//*****************************************************************  
/**-------------------------------------------------------
  * @������ SZ_STM32_SysTickInit
  * @����   ��ʼ��ϵͳ��ʱ��SysTick
  *         �û����Ը�����Ҫ�޸�
  * @����   ÿ���жϴ���
  * @����ֵ ��
***------------------------------------------------------*/
void SysTickInit(void)
{
	
  if (SysTick_Config(SystemCoreClock / 715)){ 	    //    823
		while (1);
	}
	SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk; 
	
	
} 
 

////ʱ��1   ��ʱ�� 715  �˲�30
//void SysTick_Handler(void)
//{ 
//		unsigned int i ; 
//				
//			   
//						L_SA_On();
//						M_SA_On();
//						R_SA_On();
//	
//						delay_us(355);   		// 
//	
//						L_SA_Off(); 
//						M_SA_Off(); 
//						R_SA_Off(); 
//					
//			      delay_us(585);       // 
//             
//					  L_BIG_SW_H(); 
//						delay_us(150);       //  
//						L_BIG_SW_L();							
//	
//						delay_us(35);       //   
//				  
//					  L_SB_On();	
//					  M_SB_On();
//					  R_SB_On();						
//						
//						delay_us(35);       //20     995
//						
//						L_SB_Off();
//						M_SB_Off();
//						R_SB_Off();						
//       
//				//tmp1 = DWT_CYCCNT;
//				ReadAdc1();    //153.3us
//				ReadAdc2();
//				ReadAdc3();
//				ReadAdc4();
//				ReadAdc5();
//				ReadAdc6();
//				//tmp2 = DWT_CYCCNT;
//				//tmp = tmp2 - tmp1;
//				//1148.3
//				g_uc1msTick = 1;	
//				
//} 
 
 
// //ʱ��2   ��ʱ�� 715    �˲�20
//void SysTick_Handler(void)
//{ 
//		unsigned int i ; 
//				
//			   
//						L_SA_On();
//						M_SA_On();
//						R_SA_On();
//	
//						delay_us(355);   		// 
//	
//						L_SA_Off(); 
//						M_SA_Off(); 
//						R_SA_Off(); 
//					
//			      delay_us(585);       // 
//             
//					  L_BIG_SW_H(); 
//						delay_us(150);       //  
//						L_BIG_SW_L();							
//	
//						delay_us(35);       //   
//				  
//					  L_SB_On();	
//					  M_SB_On();
//					  R_SB_On();						
//						
//						delay_us(50);       //20     995
//						
//						L_SB_Off();
//						M_SB_Off();
//						R_SB_Off();						
//       
//				//tmp1 = DWT_CYCCNT;
//				ReadAdc1();    //153.3us
//				ReadAdc2();
//				ReadAdc3();
//				ReadAdc4();
//				ReadAdc5();
//				ReadAdc6();
//				//tmp2 = DWT_CYCCNT;
//				//tmp = tmp2 - tmp1;
//				//1148.3
//				g_uc1msTick = 1;	
//				
//} 



 //ʱ��3   ��ʱ�� Ƶ��715    �˲�ϵ��20     ����0.00139s = 1390us
void SysTick_Handler(void)
{ 
		unsigned int i ; 
				
			   
						L_SA_On();          //���߲���
						M_SA_On();
						// R_SA_On();
	
						delay_us(355);   		// 
	
						L_SA_Off(); 
						M_SA_Off(); 
						// R_SA_Off(); 
					
			      delay_us(10);       // 
             
					  L_BIG_SW_H();       //����                     	   
						delay_us(150);      //                             ****�ص����****
						L_BIG_SW_L();							
	
						delay_us(50);       //���� �� �źŲ��� ���   50   ****�ص����****  
				  
					  L_SB_On();	        //�źŲ���
					  M_SB_On();
					//   R_SB_On();						
						
						delay_us(35);       //�źŲ��� ����ʱ��            ****�ص����****
						
						L_SB_Off();
						M_SB_Off();
						// R_SB_Off();						
        //�� 585us
				//tmp1 = DWT_CYCCNT; 
				ReadAdc1();
				ReadAdc2();
				ReadAdc3();    //153.3us
				ReadAdc4();
				// ReadAdc5();
				// ReadAdc6();
				
				//tmp2 = DWT_CYCCNT;
				//tmp = tmp2 - tmp1;
				//1148.3
				g_uc1msTick = 1;	
				
} 

// //ʱ��4   ��ʱ�� 715    �˲�20     ��Ҫ��� �Ӻ����Ȧ
//void SysTick_Handler(void)
//{ 
//		unsigned int i ; 
//				
//			   
//						L_SA_On();
//						M_SA_On();
//						R_SA_On();
//	
//						delay_us(355);   		// 
//	
//						L_SA_Off(); 
//						M_SA_Off(); 
//						R_SA_Off(); 
//					
//			      delay_us(10);       // 
//             
//					  L_BIG_SW_H(); 
//						delay_us(150);       //  
//						L_BIG_SW_L();							
//	
//						delay_us(40);       //   35
//				  
//					  L_SB_On();	
//					  M_SB_On();
//					  R_SB_On();						
//						
//						delay_us(50);       //50
//						
//						L_SB_Off();
//						M_SB_Off();
//						R_SB_Off();						
//      
//				//tmp1 = DWT_CYCCNT;
//				ReadAdc1();    //153.3us
//				ReadAdc2();
//				ReadAdc3();
//				ReadAdc4();
//				ReadAdc5();
//				ReadAdc6();
//				//tmp2 = DWT_CYCCNT;
//				//tmp = tmp2 - tmp1;
//				//1148.3
//				g_uc1msTick = 1;	
//				
//} 




 