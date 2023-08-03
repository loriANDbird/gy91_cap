/********************************************************************************
GY31程序   2017.4.21开始
以GY31SampV1.0硬件上为基础
上传协议：A0 D0 D1 D2 D3 D4 D5 A5  D0-D3为2路AD值，高位在前，低位在后；D4，D5为编码器计数值
下传指令：A0 xx 00 00 00 A5  其中xx  10：将AD值清零   11：切换成小量程  22：切换成大量程
2018.02.02日在接收后采样处区分大量程和小量程的间隔时间    原先此处和大量程一样   因V1.01采集板接收平台延长较大 特做此修改  平台较大原因未找到
2018.02.10日 找到V1.01采集板接收平台延长较大的原因，因Q4和Q8焊错成了BAT54A（应该焊BAT54S） 程序改2.02日之前，版本号不变
2018.06.11日 增加编码器停止计数指令  0x66 停止编码器计数  0x55 ：计数器清零  开始计数
LJQ

**********************************************************************************/

#include <string.h>
#include <stdio.h>  
#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_dma.h"
#include "Systick.h"
#include "Main.h"
#include "USART.h"
#include "AdcCS1237.h"
extern int SEGGER_RTT_printf(unsigned BufferIndex, const char * sFormat, ...);
//#define digitalToggle(p,i)		{p->ODR ^=i;}			//输出反转状态


int vValue = 0;
unsigned int adcValue = 0x800000;
/* 定义控制IO的宏 */
//#define LED1_TOGGLE		digitalToggle(GPIOA,GPIO_Pin_10)
#define MAXDATA  25

unsigned char g_bBmqStop=0;//读取编码器值暂停标志
unsigned char g_bReadAdcEN;//读取AD值允许
unsigned char g_bSending;//正在发送中标识
unsigned char g_bSendEn;//发送数据标识
unsigned char g_bDatProc;//标定标志 =1则减去初始值  =0，原始值显示
unsigned char g_bSelBigRange ;//大小量程标识  0：小量程  1：大量程
unsigned char g_bRangeTransFlag ;//大小量程标识  0：小量程  1：大量程
unsigned char g_URangeTransCount ;//大小量程标识  0：小量程  1：大量程
unsigned short	g_usAdcZeroBuf;				//AD采集数据缓冲值  用于清零
unsigned short g_usAdcBuf[10];//中间变量
unsigned short g_usAdcAvg;
unsigned char g_ucAdcNum=0;

unsigned short g_ustime;
int g_iad1_dat,g_iad2_dat,g_iad3_dat,g_iad4_dat,g_iad5_dat,g_iad6_dat;

unsigned long g_lADCBufSUM[6]; //ch1 AD采集数据缓冲区数据总和
 

int g_iAdFilterOut[6];						//ch1 AD毛刺滤波数据
 

int g_iAdc1buf[MAXDATA];						//ch1 AD采集数据缓冲区
int g_iAdc2buf[MAXDATA];						//ch2 AD采集数据缓冲区
int g_iAdc3buf[MAXDATA];
int g_iAdc4buf[MAXDATA];
int g_iAdc5buf[MAXDATA];
int g_iAdc6buf[MAXDATA];


char g_cAdcPutOffset;						//当前AD采集值存储位置
char g_cStartSendFlag;						//开始发送数据标志，采集数据毛刺滤波需要开机时先采集10次数据，之后才可上传数据

char g_uCount;

unsigned char g_ucAdcResultFlag;
int	g_iAdcZeroBuf[6];							//ch1 AD采集数据基础值缓冲（用作调试清零）
 

int  g_iCarCount;   //小车位置 单位 mm
//unsigned char g_ucCarDir;

void Delay(unsigned short int idly)
{
	u16 i,k;
	for(i=0;i<idly;i++)
        for(k=0;k<110;k++);
}

void PORT_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;    

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE); 	// RX_BigSamll_SW LEDA LEDB AD_SB AD_SA ADS2_SCLK  ADS2_Dout ADS1_SCLK	ADS1_Dout										   
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_5|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;//PB7用于测试	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);	

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//L1_SW L2_SW  
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_12;//|GPIO_Pin_10 PA10测试定时器	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    //jtag管脚重映射，作为普通io口用
   // RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
   // GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
    //无用管脚设置为输入降低功耗
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOB, &GPIO_InitStructure);	

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);//新增
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 |GPIO_Pin_14 |GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}
 

 
/*编码器接口初始化*/
void ENC_Init(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_ICInitTypeDef TIM_ICInitStructure;
//  NVIC_InitTypeDef NVIC_InitStructure;
  
/* Encoder unit connected to TIM3, 4X mode */    
  GPIO_InitTypeDef GPIO_InitStructure;  
  /* TIM4 clock source enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
  /* Enable GPIOB, clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  
  GPIO_StructInit(&GPIO_InitStructure);
  /* Configure PB.6,7 as encoder input */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  TIM_DeInit(TIM4);
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  
  TIM_TimeBaseStructure.TIM_Prescaler = 0x00;  // No prescaling 
  TIM_TimeBaseStructure.TIM_Period = 65535;  
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

  TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI1,TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
  TIM_ICStructInit(&TIM_ICInitStructure);
  TIM_ICInitStructure.TIM_ICFilter = 15; 
  TIM_ICInit(TIM4, &TIM_ICInitStructure);
  
  TIM4->CNT = 0;          
  
  TIM_Cmd(TIM4, ENABLE); 
  
  g_iCarCount = 0; 
}
void BMQRead(void){
	//int tmp = 0;
	//g_iCarCount=0;
	//tmp = TIM_GetCounter(TIM4);
	g_iCarCount = 65535-TIM_GetCounter(TIM4); 
//////	//g_iCarCount = TIM_GetCounter(TIM4);
//////	if(g_iCarCount < 200){
//////    //	g_ucCarDir = 0;
//////	//	g_iLastCarCount = 0;
//////		g_iCarCount = 0;
//////		TIM4->CNT = 0;
//////    }
//////    else if(g_iCarCount < 5000){
//////		g_iCarCount = 5000;
//////        TIM4->CNT = 5000;
//////	}
//////    else if(g_iCarCount <= 0){
//////    	//g_ucCarDir = 0;
//////		//g_iLastCarCount = 0;
//////		g_iCarCount = 0;
//////		TIM4->CNT = 0;
//////    }

}
//毛刺滤波
void MaoCiFilter(void) 
{
//    char i;
//    unsigned long Temp[2];  //???0--3;  ??[0] ????? 

//    Temp[0]=0;
//    Temp[1]=0;
//    for(i=0;i<MAXDATA;i++){ 
//		Temp[0]+=g_iAdc1buf[i];	
//		Temp[1]+=g_iAdc2buf[i];	
//    }  
	if(g_cStartSendFlag > MAXDATA){
		g_iAdFilterOut[0] = (9*(g_lADCBufSUM[0]/MAXDATA)+g_iAdFilterOut[0])/10;
		g_iAdFilterOut[1] = (9*(g_lADCBufSUM[1]/MAXDATA)+g_iAdFilterOut[1])/10;
		g_iAdFilterOut[2] = (9*(g_lADCBufSUM[2]/MAXDATA)+g_iAdFilterOut[2])/10;
		g_iAdFilterOut[3] = (9*(g_lADCBufSUM[3]/MAXDATA)+g_iAdFilterOut[3])/10;
		g_iAdFilterOut[4] = (9*(g_lADCBufSUM[4]/MAXDATA)+g_iAdFilterOut[4])/10;
		g_iAdFilterOut[5] = (9*(g_lADCBufSUM[5]/MAXDATA)+g_iAdFilterOut[5])/10;
	}
	else{
		g_iAdFilterOut[0] = (g_lADCBufSUM[0]/MAXDATA);
		g_iAdFilterOut[1] = (g_lADCBufSUM[1]/MAXDATA);
		g_iAdFilterOut[2] = (g_lADCBufSUM[2]/MAXDATA);
		g_iAdFilterOut[3] = (g_lADCBufSUM[3]/MAXDATA);
		g_iAdFilterOut[4] = (g_lADCBufSUM[4]/MAXDATA);
		g_iAdFilterOut[5] = (g_lADCBufSUM[5]/MAXDATA);
		 
		g_cStartSendFlag++;
	}
		
}

void SendResult()
{
	int lbuf[6],lbuf22,carCnt;
    unsigned char i;


 
	if(g_bSelBigRange != g_bRangeTransFlag){ //切换时前MAXDATA次数据不要，确保量程正确
		if(g_URangeTransCount > 10){
			g_URangeTransCount = 0;
			g_bRangeTransFlag = g_bSelBigRange;
		}
		else{
			g_URangeTransCount++;
//			SEGGER_RTT_printf(0,"  %d\r\n",g_URangeTransCount);	
			return;
		}
	}
	if(g_cStartSendFlag<MAXDATA)  //取样数少于滤波最小数据，不上传
	{	
		g_iAdc1buf[g_cAdcPutOffset]=g_iad1_dat;
		g_iAdc2buf[g_cAdcPutOffset]=g_iad2_dat;	
		g_iAdc3buf[g_cAdcPutOffset]=g_iad3_dat;
		g_iAdc4buf[g_cAdcPutOffset]=g_iad4_dat;
		g_iAdc5buf[g_cAdcPutOffset]=g_iad5_dat;
		g_iAdc6buf[g_cAdcPutOffset]=g_iad6_dat;
		
		g_cAdcPutOffset ++;
		if(g_cAdcPutOffset >= MAXDATA){
			g_cAdcPutOffset = 0;	
		}		
		g_lADCBufSUM[0] +=g_iad1_dat;
		g_lADCBufSUM[1] +=g_iad2_dat;
		g_lADCBufSUM[2] +=g_iad3_dat;
		g_lADCBufSUM[3] +=g_iad4_dat;
		g_lADCBufSUM[4] +=g_iad5_dat;
		g_lADCBufSUM[5] +=g_iad6_dat;
		
		g_cStartSendFlag++;
		if(g_cStartSendFlag == MAXDATA){
			MaoCiFilter();
		}
		else{
//			SEGGER_RTT_printf(0,"  %d\r\n",g_cStartSendFlag);
			return;
		}
	}
	else{
		g_lADCBufSUM[0] -=g_iAdc1buf[g_cAdcPutOffset];
		g_lADCBufSUM[1] -=g_iAdc2buf[g_cAdcPutOffset];
		g_lADCBufSUM[2] -=g_iAdc3buf[g_cAdcPutOffset];
		g_lADCBufSUM[3] -=g_iAdc4buf[g_cAdcPutOffset];
		g_lADCBufSUM[4] -=g_iAdc5buf[g_cAdcPutOffset];
		g_lADCBufSUM[5] -=g_iAdc6buf[g_cAdcPutOffset];
		
		g_lADCBufSUM[0] +=g_iad1_dat;
		g_lADCBufSUM[1] +=g_iad2_dat;
		g_lADCBufSUM[2] +=g_iad3_dat;
		g_lADCBufSUM[3] +=g_iad4_dat;
		g_lADCBufSUM[4] +=g_iad5_dat;
		g_lADCBufSUM[5] +=g_iad6_dat;
		
		g_iAdc1buf[g_cAdcPutOffset]=g_iad1_dat;
		g_iAdc2buf[g_cAdcPutOffset]=g_iad2_dat;	
		g_iAdc3buf[g_cAdcPutOffset]=g_iad3_dat;
		g_iAdc4buf[g_cAdcPutOffset]=g_iad4_dat;	
		g_iAdc5buf[g_cAdcPutOffset]=g_iad5_dat;
		g_iAdc6buf[g_cAdcPutOffset]=g_iad6_dat;	
		
		g_cAdcPutOffset ++;
		if(g_cAdcPutOffset >= MAXDATA){
			g_cAdcPutOffset = 0;	
		}
		MaoCiFilter();
	}
//	SEGGER_RTT_printf(0,"   %d: %d   %d\r\n", g_bSelBigRange,g_iAd1FilterOut,g_iAd2FilterOut);
	lbuf[0] = g_iAdFilterOut[0] - g_iAdcZeroBuf[0];
	lbuf[1] = g_iAdFilterOut[1] - g_iAdcZeroBuf[1];
	lbuf[2] = g_iAdFilterOut[2] - g_iAdcZeroBuf[2];
	lbuf[3] = g_iAdFilterOut[3] - g_iAdcZeroBuf[3];
	lbuf[4] = g_iAdFilterOut[4] - g_iAdcZeroBuf[4];
	lbuf[5] = g_iAdFilterOut[5] - g_iAdcZeroBuf[5];
 
//	g_uCount++;
////	if(g_uCount>=15){
//	if(g_iAd1FilterOut>450||g_iAd2FilterOut>450){
// 		g_uCount = 0;
//	}
	for(i=0;i<6;i++){
		if(lbuf[i] > 0x1ffff)
			lbuf[i] = 0;
	}
	  
	carCnt = g_iCarCount;
	g_ucUart2SendBuf[0] = 0xA1; //
	
	g_ucUart2SendBuf[1] = (unsigned char)(lbuf[0] >> 16);
	g_ucUart2SendBuf[2] = (unsigned char)(lbuf[0] >> 8);
	g_ucUart2SendBuf[3] = (unsigned char)lbuf[0];
	 
	g_ucUart2SendBuf[4] = (unsigned char)(lbuf[1] >> 16);
	g_ucUart2SendBuf[5] = (unsigned char)(lbuf[1] >> 8);
	g_ucUart2SendBuf[6] = (unsigned char)lbuf[1];
	
	g_ucUart2SendBuf[7] = (unsigned char)(lbuf[2] >> 16);
	g_ucUart2SendBuf[8] = (unsigned char)(lbuf[2] >> 8);
	g_ucUart2SendBuf[9] = (unsigned char)lbuf[2];
	
	g_ucUart2SendBuf[10] = (unsigned char)(lbuf[3] >> 16);
	g_ucUart2SendBuf[11] = (unsigned char)(lbuf[3] >> 8);
	g_ucUart2SendBuf[12] = (unsigned char)lbuf[3];
	
	g_ucUart2SendBuf[13] = (unsigned char)(lbuf[4] >> 16);
	g_ucUart2SendBuf[14] = (unsigned char)(lbuf[4] >> 8);
	g_ucUart2SendBuf[15] = (unsigned char)lbuf[4];
	
	g_ucUart2SendBuf[16] = (unsigned char)(lbuf[5] >> 16);
	g_ucUart2SendBuf[17] = (unsigned char)(lbuf[5] >> 8);
	g_ucUart2SendBuf[18] = (unsigned char)lbuf[5];
	
	
	g_ucUart2SendBuf[20] = (unsigned char)carCnt;
	carCnt >>= 8;
	g_ucUart2SendBuf[19] = (unsigned char)carCnt;
	
    
	g_ucUart2SendBuf[21] = 0xA5; //
   
	MYDMA_Enable(DMA1_Channel7);//开始一次DMA传输！	
 
}
//////////////////////////////////////////////////////////////////////////////////////////
//void RxSwitch(unsigned char bSelBigRange)
//{  
//	if(bSelBigRange == 1)
//    {
//        
//        GPIO_ResetBits(GPIOB, GPIO_Pin_5);   
//	}
//	else
//    {
//		  GPIO_SetBits(GPIOB, GPIO_Pin_5);  
//	}
//}
void UnpackUart2()
{
	//if((g_usUart2RecCnt == 6)&&(g_ucUart2RecBuf[0] == 0xA0)&&(g_ucUart2RecBuf[5] == 0xA5)){
    switch(g_ucUart2RecBuf[1])
    {
        case 0x10:
//							g_iAdc1ZeroBuf = g_iAd1FilterOut;
//							g_iAdc2ZeroBuf = g_iAd2FilterOut;
							break;
        case 0x01:
//							g_iAdc1ZeroBuf = 0;
//							g_iAdc2ZeroBuf = 0;
							break;
		
		

////////        case 0x11: //小量程                        2021.04.14  WA 添加大量程小量程选择。
////////			if(g_bSelBigRange != 0){
////////				g_bSelBigRange = 0;
////////				RxSwitch(g_bSelBigRange);
////////				g_cStartSendFlag=0;
//////////				g_lADC1BufSUM=0;
//////////				g_lADC2BufSUM=0;
////////			}
////////            break;
////////        case 0x22: //大量程
////////			if(g_bSelBigRange != 1){
////////				g_bSelBigRange = 1;
////////				RxSwitch(g_bSelBigRange);
////////				g_cStartSendFlag=0;
//////////				g_lADC1BufSUM=0;
//////////				g_lADC2BufSUM=0;
////////			}
////////            break;
			
			
			
			
		
        case 0x55: //将编码器计数清零  
            TIM_Cmd(TIM4, ENABLE); 
            g_iCarCount = 0;
            TIM4->CNT = 0;
            g_bBmqStop = 0;
            break;
        case 0x66: //将编码计数器关闭  不计数
            // g_iCarCount = 0;
            //TIM4->CNT = 0;
            g_bBmqStop = 1;
            TIM_Cmd(TIM4, DISABLE); //停止计数器
            break;
        case 0x77: //将编码计数器打开  继续计数
            TIM_Cmd(TIM4, ENABLE);  
            g_bBmqStop = 0;
            TIM4->CNT = g_iCarCount;            
            break;
        
        default:
            break;			
    }		
	//}
	g_usUart2RecCnt = 0;
}
//unsigned char AverageAD()
//{
//    unsigned short min,max;
//	int lSum;
//	unsigned char i;
//	g_usAdcBuf[0] = g_usAdcBuf[1];
//	g_usAdcBuf[1] = g_usAdcBuf[2];
//	g_usAdcBuf[2] = g_usAdcBuf[3];
//	g_usAdcBuf[3] = g_usAdcBuf[4];
//	g_usAdcBuf[4] = g_usAdcBuf[5];
//	g_usAdcBuf[5] = g_usAdcBuf[6];
//	g_usAdcBuf[6] = g_usAdcBuf[7];
//	g_usAdcBuf[7] = g_usAdcBuf[8];
//	g_usAdcBuf[8] = g_usAdcBuf[9];
////	g_usAdcBuf[9] = g_usAdcVal;
//	
//	
//	if(g_ucAdcNum < 10){ //前十个数先不发
//		g_ucAdcNum += 1;	
//		return 0;
//	}		

//	min = g_usAdcBuf[0];
//	max = g_usAdcBuf[0];
//	lSum = 0;
//	for(i=0;i<10;i++)
//    {
//		if(g_usAdcBuf[i] < min){
//			min = g_usAdcBuf[i]; 
//		}
//		if(g_usAdcBuf[i] > max){
//			max = g_usAdcBuf[i]; 
//		}
//		lSum += g_usAdcBuf[i]; 
//	}	
//	g_usAdcAvg = ((lSum - max - min) >> 3);
//	//g_usAdcAvg = g_usAdcVal;
//	return 1;	
//}
void Para_Init(void)
{
	unsigned char i;
	
//	  g_iAdc1ZeroBuf = 0;
//    g_iAdc2ZeroBuf = 0;
	
	g_uCount = 0;
	
	for(i=0;i<6;i++){
			g_lADCBufSUM[i] = 0;		
			g_iAdFilterOut[i] = 0;						//ch1 AD毛刺滤波数据
	}
	
	 
	for(i=0;i<MAXDATA;i++){
		g_iAdc1buf[i]=0;						//ch1 AD采集数据
		g_iAdc2buf[i]=0;						//ch2 AD采集数据
		g_iAdc3buf[i]=0;
		g_iAdc4buf[i]=0;
		g_iAdc5buf[i]=0;
		g_iAdc6buf[i]=0;
	}
	g_cAdcPutOffset=0;						//当前AD采集值存储位置
	g_cStartSendFlag=0;						//开始发送数据标志，采集数据毛刺滤波需要开机时先采集10次数据，之后才可上传数据
	
	g_bSelBigRange = 0;
	g_bRangeTransFlag = 0;
	g_URangeTransCount = 0;
}

int g_i1msFlag = 0;
int main(void)
{ 
    //PORT_Init(); 
    //LEDB_Off(); 
	  DelayInit();
	  AdcPortInit();
//	  L_BIG_SW_H(); 
//	  delay_ms(10000);
	
	
//	添加以下几行代码，验证解决漂移问题暂无效果。	
//	L_SA_On();          //基线采样
//	M_SA_On();
//	R_SA_On();
//	L_SB_On();	        //信号采样
//	M_SB_On();
//	R_SB_On();
//for(int k = 0;k<33333;k++){      //10秒
//        L_BIG_SW_H();       //发射                          
//        delay_us(150);           
//        L_BIG_SW_L();          
//        delay_us(150);   
//    }	
	
	
	
	SysTickInit();    //此处 下一次试验  拿到 最上边，一上电就开始发射线圈   2021.03.12
    USART2_Config(); 
    USARTx_DMA_Config();	
    Para_Init();
	
	
	   
		
	
    AdcInit();
    //RxSwitch(0); 
    
    ENC_Init();
	
 
	
    while(1)
    {  
        if(!g_bBmqStop){
           BMQRead();
        }
				if(g_uc1msTick == 1){   //&& g_bSending == 0
					  g_uc1msTick = 0; 
					  //g_i1msFlag++;
//						if(g_i1msFlag >= 1000){  //2个通道读出来就发送  
//							  g_i1msFlag = 0;
					    if(g_ucAdcResultFlag == 0x0F){ 	//0x3F
		            if(DMA_GetCurrDataCounter(DMA1_Channel7) == 0){	
		                g_ucAdcResultFlag = 0;
		                SendResult();					
		            }
						  
						} 
				} 
				
//        if(g_Uart2RecValid)//处理接收的串口数据
//        {
//            g_Uart2RecValid = 0;
//            UnpackUart2();
//        }
          
    }
		
		
}
/*********************************************END OF FILE**********************/
