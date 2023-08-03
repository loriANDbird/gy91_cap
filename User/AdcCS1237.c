//******************************************************************************//
//Project:	
//File:			CS1237驱动
//Date:			2017.03.10
//Version:	1.0
//Author:		康泉
//Function: 单通道24位ADC数据读取，1倍增益，外部参考REF3125，10Hz输出，可实现17位非常稳定
//					40Hz输出时，精度略微差些。
//
//需注意！  在进行读取数据时，CLK数据线不能长时间置高，不能大于100uS，
//          也即不能有长时间的中断程序占用，否则采集数据会不稳定。
//Remark:   	
//2017.03.10:	实现数据读取、参数配置等功能 	
//2017.09.19    将CS1237数据速率改为40Hz，传输速度提高4倍   40包/s  波特率9600
//2017.10.09    调整AD值的取值窗口，解决在10mm厚度以下AD值过大的问题 

//******************************************************************************//
#include "stm32f10x.h"
#include <string.h>
#include <stdio.h>
#include "AdcCS1237.h"
#include "Main.h"

//***************************************************************** 
// DWT

#define  DWT_CR      *(volatile uint32_t *)0xE0001000
#define  DWT_CYCCNT  *(volatile uint32_t *)0xE0001004
#define  DEM_CR      *(volatile uint32_t *)0xE000EDFC
#define  DEM_CR_TRCENA                   (1 << 24)
#define  DWT_CR_CYCCNTENA                (1 <<  0)
void DelayInit(void) 
{ 
	 
    DEM_CR         |=  DEM_CR_TRCENA; 
		// DWT_CYCCNT = 0u;     
    DWT_CR         |= DWT_CR_CYCCNTENA;
}

// 1us
void delay_us(uint32_t us)
{
		uint32_t temp;
		temp= DWT_CYCCNT; //DWT_CYCCNT
		us *=  SystemCoreClock/1000000;      //1us
		while((uint32_t)( DWT_CYCCNT - temp) < us) //
	  { 
		}
}

void delay_ms(uint32_t ms)
{
		uint32_t temp;
		temp= DWT_CYCCNT;
		ms *=  SystemCoreClock/1000;                   
		while((uint32_t)( DWT_CYCCNT - temp) < ms){ 
		}  
}


void AdcPortInit(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;    

		//jtag管脚重映射，作为普通io口用
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE);  						   
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11
																	|GPIO_Pin_14
																	|GPIO_Pin_4
																	|GPIO_Pin_9
																	|GPIO_Pin_3
																	|GPIO_Pin_5; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
		GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_13
																	|GPIO_Pin_10
																	|GPIO_Pin_8; 	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);	
		
	  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);  								   
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8
																	|GPIO_Pin_9
																	|GPIO_Pin_4; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);	
	 
	  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_15;   		
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);	
	 
//		RangeSw_Small();  // 针对 Coil8V1.0 线圈板所有接收线圈均同向 所以此处设置 RangeSw_Small
	  
	    
		L_SA_Off();
		L_SB_Off();
		M_SA_Off();
		M_SB_Off();
//		R_SA_Off();
//		R_SB_Off();
		
//		L1_SW_L();
//		L2_SW_L();
//		L3_SW_L();

		L_BIG_SW_L();
	 
	  Adc1Sclk_L();
		Adc2Sclk_L();
		Adc3Sclk_L();
		Adc4Sclk_L();
//		Adc5Sclk_L();
//		Adc6Sclk_L();
		
		 
}

//TIM3 PWM部分初始化 
//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数

//HCLK=48
/* PCLK1 = HCLK/2 */
//48
void TIM3_PWM_Init(u16 arr,u16 psc)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//使能定时器3时钟
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  //使能GPIO外设和AFIO复用功能模块时钟 
   //设置该引脚为复用输出功能,输出TIM3 CH2的PWM脉冲波形	GPIOB.5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; //TIM_CH3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIO
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7; //TIM_CH1 2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIO
 
	
	
   //初始化TIM3
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值   
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //基本定时器没有此功能  设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	//初始化TIM3 Channel2 PWM模式	                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM3 OC2
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM3 OC2
  TIM_OC3Init(TIM3, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM3 OC2
	
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);  //使能TIM3在CCR2上的预装载寄存器
  TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);  //使能TIM3在CCR2上的预装载寄存器
	
	TIM_Cmd(TIM3, ENABLE);  //使能TIM3
	

}

void AdcInit(void)
{
		u8 i = 0;
		
	
		
		
		TIM3_PWM_Init(26,0);	 //不分频。PWM频率=48000000/26=1.8Mhz
	  TIM_SetCompare1(TIM3,13);
	  TIM_SetCompare2(TIM3,13);	
	  TIM_SetCompare3(TIM3,13);
  
		delay_ms(5);
	
		Adc1Sclk_L();
		Adc2Sclk_L();
		Adc3Sclk_L();
		Adc4Sclk_L();
//		Adc5Sclk_L();
//		Adc6Sclk_L();
	
		delay_ms(150);
	
		for(i=0;i<26;i++)
		{  
				Adc1Sclk_H();
				Adc2Sclk_H();
				Adc3Sclk_H();
				Adc4Sclk_H();
//				Adc5Sclk_H();
//				Adc6Sclk_H();
				
				delay_us(1);
				
				Adc1Sclk_L();
				Adc2Sclk_L();
				Adc3Sclk_L();
				Adc4Sclk_L();
//				Adc5Sclk_L();
//				Adc6Sclk_L();
				
				delay_us(1);
		}
		
		u8 bflag = 0;
		unsigned int timeout;
		for(timeout=0;timeout<300;timeout++){
				delay_ms(30); 
				if(0 == GetAdc1Dat()){
						bflag += 1;
				}
				if(0 == GetAdc2Dat()){
						bflag += 1;
				}
				if(0 == GetAdc3Dat()){
						bflag += 1;
				}
				if(0 == GetAdc4Dat()){
						bflag += 1;
				}
//				if(0 == GetAdc5Dat()){
//						bflag += 1;
//				}
//				if(0 == GetAdc6Dat()){
//						bflag += 2;
//				}
				
				if(bflag > 6){
						timeout = 300;
				}
		}	
}




unsigned char ReadAdc1()   
{
	unsigned char i = 0 ,j = 123;
		int iAdcValue = 0;
		
		if(GetAdc1Dat()){
			return 0;
		}
		Adc1Sclk_L(); 
		g_iad1_dat = 0;
		for(i=0;i<24;i++){
			iAdcValue <<= 1;
			
			Adc1Sclk_H();
		  //__nop();__nop();__nop();  //48MHz 416ns  
			
			Adc1Sclk_L();
			if(GetAdc1Dat()){
				iAdcValue |=1;
			}
		}
		
	  //__nop();__nop();__nop();   
		Adc1Sclk_H();
		//__nop();__nop();__nop();   
		Adc1Sclk_L(); 
		 
		 
		g_iad1_dat = iAdcValue >> 6;
		g_ucAdcResultFlag |= (1<<0);	
	 
	return 1;
	 
}

unsigned char ReadAdc2()   
{
	unsigned char i = 0 ,j = 123;
		int iAdcValue = 0;
		
		if(GetAdc2Dat()){
			return 0;
		}
		Adc2Sclk_L(); 
		g_iad2_dat = 0;
		for(i=0;i<24;i++){
			iAdcValue <<= 1;
			
			Adc2Sclk_H();
		  //__nop();__nop();__nop();  //48MHz 416ns  
			
			Adc2Sclk_L();
			if(GetAdc2Dat()){
				iAdcValue |=1;
			}
		}
		
	  //__nop();__nop();__nop();   
		Adc2Sclk_H();
		//__nop();__nop();__nop();   
		Adc2Sclk_L(); 
		 
		 
		g_iad2_dat = iAdcValue >> 6;
		g_ucAdcResultFlag |= (1<<1);	
	 
	return 1;
	 
}

unsigned char ReadAdc3()   
{
	unsigned char i = 0 ,j = 123;
		int iAdcValue = 0;
		
		if(GetAdc3Dat()){
			return 0;
		}
		Adc3Sclk_L(); 
		g_iad3_dat = 0;
		for(i=0;i<24;i++){
			iAdcValue <<= 1;
			
			Adc3Sclk_H();
		  //__nop();__nop();__nop();  //48MHz 416ns  
			
			Adc3Sclk_L();
			if(GetAdc3Dat()){
				iAdcValue |=1;
			}
		}
		
	  //__nop();__nop();__nop();   
		Adc3Sclk_H();
		//__nop();__nop();__nop();   
		Adc3Sclk_L(); 
		 
		 
		g_iad3_dat = iAdcValue >> 6;
		g_ucAdcResultFlag |= (1<<2);	
	 
	return 1;
	 
}
unsigned char ReadAdc4()   
{
	unsigned char i = 0 ,j = 123;
		int iAdcValue = 0;
		
		if(GetAdc4Dat()){
			return 0;
		}
		Adc4Sclk_L(); 
		g_iad4_dat = 0;
		for(i=0;i<24;i++){
			iAdcValue <<= 1;
			
			Adc4Sclk_H();
		  //__nop();__nop();__nop();  //48MHz 416ns  
			__nop();
			Adc4Sclk_L();
			if(GetAdc4Dat()){
				iAdcValue |=1;
			}
		}
		
	  //__nop();__nop();__nop();   
		Adc4Sclk_H();
		//__nop();__nop();__nop();   
		Adc4Sclk_L(); 
		 
		 
		g_iad4_dat = iAdcValue >> 6;
		g_ucAdcResultFlag |= (1<<3);	
	 
	return 1;
	 
}

#if 0
unsigned char ReadAdc5()   
{
	unsigned char i = 0 ,j = 123;
		int iAdcValue = 0;
		
		if(GetAdc5Dat()){
			return 0;
		}
		Adc5Sclk_L(); 
		g_iad5_dat = 0;
		for(i=0;i<24;i++){
			iAdcValue <<= 1;
			
			Adc5Sclk_H();
		  //__nop();__nop();__nop();  //48MHz 416ns  
			
			Adc5Sclk_L();
			if(GetAdc5Dat()){
				iAdcValue |=1;
			}
		}
		
	  //__nop();__nop();__nop();   
		Adc5Sclk_H();
		//__nop();__nop();__nop();   
		Adc5Sclk_L(); 
		 
		 
		g_iad5_dat = iAdcValue >> 6;
		g_ucAdcResultFlag |= (1<<4);	
	 
	return 1;
	 
}
unsigned char ReadAdc6()   
{
	unsigned char i = 0 ,j = 123;
		int iAdcValue = 0;
		
		if(GetAdc6Dat()){
			return 0;
		}
		Adc6Sclk_L(); 
		g_iad6_dat = 0;
		for(i=0;i<24;i++){
			iAdcValue <<= 1;
			
			Adc6Sclk_H();
		  //__nop();__nop();__nop();  //48MHz 416ns  
			
			Adc6Sclk_L();
			if(GetAdc6Dat()){
				iAdcValue |=1;
			}
		}
		
	  //__nop();__nop();__nop();   
		Adc6Sclk_H();
		//__nop();__nop();__nop();   
		Adc6Sclk_L(); 
		 
		 
		g_iad6_dat = iAdcValue >> 6;
		g_ucAdcResultFlag |= (1<<5);	
	 
	return 1;
	 
}
#endif






















//#define AdcA_CLK_High()  	( GPIOB->ODR |= (1<<12)  )   //GPIO_SetBits(GPIOB, GPIO_Pin_12)
//#define AdcA_CLK_Low()   	( GPIOB->ODR &= ~(1<<12) )   //GPIO_ResetBits(GPIOB, GPIO_Pin_12)
//#define AdcA_DAT_High()  	( GPIOB->ODR |= (1<<13)  )   //GPIO_SetBits(GPIOB, GPIO_Pin_13)
//#define AdcA_DAT_Low()   	( GPIOB->ODR &= ~(1<<13) )   //GPIO_ResetBits(GPIOB, GPIO_Pin_13)
//#define GetAdcDatA()    	( GPIOB->IDR & (1<<13) )     //GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13)

//#define AdcB_CLK_High()  	( GPIOB->ODR |= (1<<14)  )   //GPIO_SetBits(GPIOB, GPIO_Pin_14)
//#define AdcB_CLK_Low()   	( GPIOB->ODR &= ~(1<<14) )   //GPIO_ResetBits(GPIOB, GPIO_Pin_14)
//#define AdcB_DAT_High()  	( GPIOB->ODR |= (1<<15)  )   //GPIO_SetBits(GPIOB, GPIO_Pin_15)
//#define AdcB_DAT_Low()   	( GPIOB->ODR &= ~(1<<15) )   //GPIO_ResetBits(GPIOB, GPIO_Pin_15)
//#define GetAdcDatB()    	( GPIOB->IDR & (1<<15) )     //GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_15)



///////////////////////////////////////////////////////////////////////////////////////////
////多通道ADC模式使用的变量
///*GPIO_TypeDef* arrClk_GPIOx[14];
//uint16_t arrClk_GPIO_Pin[14];

//GPIO_TypeDef* arrDat_GPIOx[14];
//uint16_t arrDat_GPIO_Pin[14];


//GPIO_TypeDef* g_Clk_GPIOx;
//uint16_t g_Clk_GPIO_Pin;

//GPIO_TypeDef* g_Dat_GPIOx;
//uint16_t g_Dat_GPIO_Pin;*/

//////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
////单通道模式 读取 ADC数据

//void ConfDatIOModeA(int bInput){
//	GPIO_InitTypeDef GPIO_InitStructure; 
//	if(bInput > 0){
//		GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IPD;  //ADC_Dat 输入 弱下拉
//	}
//	else{
//		GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP; //输出  
//	}
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
//	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;	
//	GPIO_Init(GPIOB, &GPIO_InitStructure);	

//}

//void InitAdcCS1237A(void)
//{
//	AdcA_CLK_Low();
//	
//    ConfDatIOModeA(1);
//	
//}


// 
// unsigned char ReadAdcCS1237A()  //int为 四字节长度
//{
//	unsigned char i = 0 ,j = 123;
//		int iAdcValue = 0;
//		
//		if(GetAdcDatA()){
//			return 0;
//		}
//		
//		g_iad1_dat = 0;
//		for(i=0;i<24;i++){
//			iAdcValue <<= 1;
//			
//			AdcA_CLK_High();
//			__nop();__nop();__nop();__nop();__nop();__nop();  //48MHz 480ns
//       
//			AdcA_CLK_Low(); 
//		 
//			if(GetAdcDatA()){
//				iAdcValue ++;
//			}
//		}
//	 
//		AdcA_CLK_High();
//		__nop();__nop();__nop();__nop();__nop();__nop();  //48MHz 480ns 
//		AdcA_CLK_Low(); 
//		__nop();__nop();__nop();__nop();__nop();__nop();  //48MHz 480ns 
//		
//		AdcA_CLK_High();
//		__nop();__nop();__nop();__nop();__nop();__nop();  //48MHz 480ns 
//		AdcA_CLK_Low();  
//    __nop();__nop();__nop();__nop();__nop();__nop();  //48MHz 480ns 
//		
//		AdcA_CLK_High();
//		__nop();__nop();__nop();__nop();__nop();__nop();  //48MHz 480ns 
//		AdcA_CLK_Low(); 
//		__nop();__nop();__nop();__nop();__nop();__nop();  //48MHz 480ns 
//		
//		g_iad1_dat = iAdcValue >> 7;
//		g_ucAdcResultFlag |= 0x01;	
//		 
//	return 1;
//}
// 


//void ConfDatIOModeB(int bInput)
//{
//	GPIO_InitTypeDef GPIO_InitStructure; 
//	if(bInput > 0){
//		GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IPD;  //ADC_Dat 输入 弱下拉
//	}
//	else{
//		GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP; //输出  
//	}
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
//	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;	
//	GPIO_Init(GPIOB, &GPIO_InitStructure);	

//}






//void InitAdcCS1237B(void)
//{
//	AdcB_CLK_Low();
//	//ADC_SCLK = 0;
//    ConfDatIOModeB(1);

//}


// 
// unsigned char ReadAdcCS1237B()  //int为 四字节长度
//{
//	unsigned char i = 0 ,j = 123;
//		int iAdcValue = 0;
//		
//		if(GetAdcDatB()){
//			return 0;
//		}
//		
//		g_iad2_dat = 0;
//		for(i=0;i<24;i++){
//			iAdcValue <<= 1;
//			
//			AdcB_CLK_High();
//		  __nop();__nop();__nop();__nop();__nop();__nop();  //48MHz 480ns 
//			 
//			AdcB_CLK_Low(); 
//			 
//			if(GetAdcDatB()){
//				iAdcValue ++;
//			}
//		}
//	 
//		AdcB_CLK_High();
//		__nop();__nop();__nop();__nop();__nop();__nop();  //48MHz 480ns 
//		AdcB_CLK_Low(); 
//		__nop();__nop();__nop();__nop();__nop();__nop();  //48MHz 480ns 
//		
//		AdcB_CLK_High();
//		__nop();__nop();__nop();__nop();__nop();__nop();  //48MHz 480ns 
//		AdcB_CLK_Low(); 
//		__nop();__nop();__nop();__nop();__nop();__nop();  //48MHz 480ns 
//		
//		AdcB_CLK_High();
//		__nop();__nop();__nop();__nop();__nop();__nop();  //48MHz 480ns 
//		AdcB_CLK_Low(); 
//		__nop();__nop();__nop();__nop();__nop();__nop();  //48MHz 480ns 
//		 
//		g_iad2_dat = iAdcValue >> 7;
//		g_ucAdcResultFlag |= 0x02;	
//	 
//	return 1;
//	 
//}
// 

//static void AdcDelay(int idly)
//{
//		int i;
//		for(i=0;i<idly;i++){}
//}
//static void Adc1Cs1237Clk(void)
//{
//		AdcB_CLK_High();
//		AdcDelay(1); 
//		AdcB_CLK_Low(); 
//}
//static void Adc2Cs1237Clk(void)
//{
//		AdcA_CLK_High();
//		AdcDelay(1); 
//		AdcA_CLK_Low(); 
//} 
// 
//static void SetAdc1Reg(unsigned char res_conf)
//{
//		unsigned char i = 0,ucCmdByte = 0;
//			
//		for(i=0;i<27;i++){
//			Adc1Cs1237Clk();		
//		}
//		 
//		Adc1Cs1237Clk();//28 29
//		Adc1Cs1237Clk();
//		
//		ucCmdByte = 0xca;//0x65 ????
//		for(i=0;i<7;i++){  //30 ~ 36
//			if(ucCmdByte & 0x80){
//				AdcB_DAT_High();
//			}
//			else{
//				AdcB_DAT_Low();
//			}
//			ucCmdByte = ucCmdByte << 1;
//			Adc1Cs1237Clk();	
//		}
//		
//		Adc1Cs1237Clk();	//37
//		
//		for(i=0;i<8;i++){  //38 ~ 45
//			if(res_conf & 0x80){
//				AdcB_DAT_High();
//			}
//			else{
//				AdcB_DAT_Low();
//			}
//			res_conf = res_conf << 1;
//			Adc1Cs1237Clk();	
//		}

//		Adc1Cs1237Clk();	//46
//	  
//}

//static void SetAdc2Reg(unsigned char res_conf)
//{
//		unsigned char i = 0,ucCmdByte = 0;
//			
//		for(i=0;i<27;i++){
//			Adc2Cs1237Clk();		
//		}
//		 
//		Adc2Cs1237Clk();//28 29
//		Adc2Cs1237Clk();
//		
//		ucCmdByte = 0xca;//0x65 ????
//		for(i=0;i<7;i++){  //30 ~ 36
//			if(ucCmdByte & 0x80){
//				AdcA_DAT_High();
//			}
//			else{
//				AdcA_DAT_Low();
//			}
//			ucCmdByte = ucCmdByte << 1;
//			Adc2Cs1237Clk();	
//		}
//		
//		Adc2Cs1237Clk();	//37
//		
//		for(i=0;i<8;i++){  //38 ~ 45
//			if(res_conf & 0x80){
//				AdcA_DAT_High();
//			}
//			else{
//				AdcA_DAT_Low();
//			}
//			res_conf = res_conf << 1;
//			Adc2Cs1237Clk();	
//		}

//		Adc2Cs1237Clk();	//46
//	  
//}
//  
// 
//void AdcCs1237Init(void)
//{
//		unsigned char ucRegConf = 0;
//		
//		InitAdcCS1237A();
//		InitAdcCS1237B();
//	
//	
//		//??REF??
//		ucRegConf |= (1<<6);
//		
//		//ADC??????       // res_conf[5:4]    00 10Hz, 01 40Hz, 10 640Hz, 11 1280Hz
//		//ucRegConf |= (1<<5);  
//		ucRegConf |= (1<<4);      
//		 
//		//PGA??  00 1?         ??128?
//		 
//		//???? 00 ????A
//		 
//		//res_conf |= 0x0c;//  ?????  
//		
//		while(GetAdcDatA() != 0);
//		while(GetAdcDatB() != 0); 
//		SetAdc1Reg(ucRegConf); 
//		SetAdc2Reg(ucRegConf);
//}

////配置参数
////0x40  -> 10Hz 关闭内部参考，采用外部参考  1倍放大   
////0x50  -> 40Hz 关闭内部参考，采用外部参考  1倍放大  	
////0x00  -> 10Hz 使用内部参考输出  1倍放大 
////0x0C  -> 10Hz 使用内部参考输出  128倍放大 //芯片默认值，用于测量压力传感器
 


