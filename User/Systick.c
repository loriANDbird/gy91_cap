#include "stm32f10x.h"
#include "Systick.h"
#include "main.h"
 

unsigned char g_uc1msTick = 0;  
//unsigned char g_uc1msTick; 2021.04.08 WA

int tmp, tmp1, tmp2;
#define  DWT_CYCCNT  *(volatile uint32_t *)0xE0001004
//*****************************************************************  
/**-------------------------------------------------------
  * @函数名 SZ_STM32_SysTickInit
  * @功能   初始化系统定时器SysTick
  *         用户可以根据需要修改
  * @参数   每秒中断次数
  * @返回值 无
***------------------------------------------------------*/
void SysTickInit(void)
{
	
  if (SysTick_Config(SystemCoreClock / 715)){ 	    //    823
		while (1);
	}
	SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk; 
	
	
} 
 

////时序1   定时器 715  滤波30
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
 
 
// //时序2   定时器 715    滤波20
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



 //时序3   定时器 频率715    滤波系数20     周期0.00139s = 1390us
void SysTick_Handler(void)
{ 
		unsigned int i ; 
				
			   
						L_SA_On();          //基线采样
						M_SA_On();
						// R_SA_On();
	
						delay_us(355);   		// 
	
						L_SA_Off(); 
						M_SA_Off(); 
						// R_SA_Off(); 
					
			      delay_us(10);       // 
             
					  L_BIG_SW_H();       //发射                     	   
						delay_us(150);      //                             ****重点调整****
						L_BIG_SW_L();							
	
						delay_us(50);       //发射 与 信号采样 间隔   50   ****重点调整****  
				  
					  L_SB_On();	        //信号采样
					  M_SB_On();
					//   R_SB_On();						
						
						delay_us(35);       //信号采样 保持时间            ****重点调整****
						
						L_SB_Off();
						M_SB_Off();
						// R_SB_Off();						
        //共 585us
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

// //时序4   定时器 715    滤波20     主要针对 加厚版线圈
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




 