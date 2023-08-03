//******************************************************************************//
//Project:		ѹ������� (LcdDriver.c��ͷ�ļ�)
//Date:			2008.09.23
//Version:		1.0
//Author:		akangquan	 
//******************************************************************************//
#ifndef ADCCS1237_H
#define ADCCS1237_H

//#define RangeSw_Big()						( GPIOC->ODR &= ~(1<<13) )	
//#define RangeSw_Small()					( GPIOC->ODR |=  (1<<13) )

// #define Adc1Sclk_H()					  ( GPIOA->ODR |=  (1<<12) )
// #define Adc1Sclk_L()						( GPIOA->ODR &= ~(1<<12) )

// #define Adc2Sclk_H()					  ( GPIOA->ODR |= (1<<10)  )
// #define Adc2Sclk_L()					  ( GPIOA->ODR &= ~(1<<10) )

// #define Adc3Sclk_H()					  ( GPIOB->ODR |= (1<<9)  )
// #define Adc3Sclk_L()					  ( GPIOB->ODR &= ~(1<<9) )

// #define Adc4Sclk_H()						( GPIOB->ODR |= (1<<5)   )
// #define Adc4Sclk_L()					  ( GPIOB->ODR &= ~(1<<5)  )

// #define Adc5Sclk_H()						( GPIOB->ODR |= (1<<10)   )
// #define Adc5Sclk_L()					  ( GPIOB->ODR &= ~(1<<10)  )

// #define Adc6Sclk_H()						( GPIOB->ODR |= (1<<1)  )
// #define Adc6Sclk_L()					  ( GPIOB->ODR &= ~(1<<1) )

#define Adc1Sclk_H()					  ( GPIOB->ODR |=  (1<<14) )
#define Adc1Sclk_L()						( GPIOB->ODR &= ~(1<<14) )

#define Adc2Sclk_H()					  ( GPIOB->ODR |= (1<<11)  )
#define Adc2Sclk_L()					  ( GPIOB->ODR &= ~(1<<11) )

#define Adc3Sclk_H()					  ( GPIOB->ODR |= (1<<9)  )
#define Adc3Sclk_L()					  ( GPIOB->ODR &= ~(1<<9) )

#define Adc4Sclk_H()						( GPIOB->ODR |= (1<<3)   )
#define Adc4Sclk_L()					  ( GPIOB->ODR &= ~(1<<3)  )


//#define Adc1Dat_H()							( GPIOA->ODR |= (1<<8)  )
//#define Adc1Dat_L()					    ( GPIOA->ODR &= ~(1<<8) )

//#define Adc2Dat_H()							( GPIOA->ODR |= (1<<9)  )
//#define Adc2Dat_L()					    ( GPIOA->ODR &= ~(1<<9) )

//#define Adc3Dat_H()							( GPIOB->ODR |= (1<<4)  )
//#define Adc3Dat_L()					    ( GPIOB->ODR &= ~(1<<4) )

//#define Adc4Dat_H()							( GPIOB->ODR |= (1<<8)   )
//#define Adc4Dat_L()					    ( GPIOB->ODR &= ~(1<<8)  )

//#define Adc5Dat_H()							( GPIOA->ODR |= (1<<1)   )
//#define Adc5Dat_L()					    ( GPIOA->ODR &= ~(1<<1)  )

//#define Adc6Dat_H()							( GPIOA->ODR |= (1<<5)  )
//#define Adc6Dat_L()					    ( GPIOA->ODR &= ~(1<<5) )

// #define GetAdc1Dat()    				( GPIOA->IDR & (1<<11)   	)
// #define GetAdc2Dat()    				( GPIOA->IDR & (1<<9)   	)
// #define GetAdc3Dat()    				( GPIOB->IDR & (1<<8)   	)
// #define GetAdc4Dat()    				( GPIOB->IDR & (1<<4)   	)
// #define GetAdc5Dat()    				( GPIOB->IDR & (1<<11)  	)
// #define GetAdc6Dat()    				( GPIOB->IDR & (1<<2)   	)

#define GetAdc1Dat()    				( GPIOB->IDR & (1<<13)   	)
#define GetAdc2Dat()    				( GPIOB->IDR & (1<<10)   	)
#define GetAdc3Dat()    				( GPIOB->IDR & (1<<8)   	)
#define GetAdc4Dat()    				( GPIOA->IDR & (1<<15)   	)

//#define L1_SW_H()								( GPIOA->ODR |= (1<<9)   )	
//#define L1_SW_L()					  		( GPIOA->ODR &= ~(1<<9)  )

//#define L2_SW_H()								( GPIOA->ODR |= (1<<10)  )	
//#define L2_SW_L()					  		( GPIOA->ODR &= ~(1<<10) )

//#define L3_SW_H()								( GPIOA->ODR |= (1<<11)  )	
//#define L3_SW_L()					  		( GPIOA->ODR &= ~(1<<11) )

#define L_BIG_SW_H()					  ( GPIOA->ODR |= (1<<4)  )
#define L_BIG_SW_L()					  ( GPIOA->ODR &= ~(1<<4) )

#define L_SA_On()								( GPIOA->ODR |= (1<<9)   )   //����    
#define L_SA_Off()							    ( GPIOA->ODR &= ~(1<<9)  )   

#define L_SB_On()								( GPIOA->ODR |= (1<<8)  )   //�ź�
#define L_SB_Off()							    ( GPIOA->ODR &= ~(1<<8) )

#define M_SA_On()								( GPIOB->ODR |= (1<<5)   )
#define M_SA_Off()							    ( GPIOB->ODR &= ~(1<<5)  )

#define M_SB_On()								( GPIOB->ODR |= (1<<4)   )
#define M_SB_Off()							    ( GPIOB->ODR &= ~(1<<4)  )


// #define R_SA_On()								( GPIOB->ODR |= (1<<13)   )
// #define R_SA_Off()							    ( GPIOB->ODR &= ~(1<<13)  )

// #define R_SB_On()								( GPIOB->ODR |= (1<<14)   )
// #define R_SB_Off()							    ( GPIOB->ODR &= ~(1<<14)  )
 


//#define LED1_On()								( GPIOA->ODR &= ~(1<<0)  )
//#define LED1_Off()              ( GPIOA->ODR |= (1<<0)   )

//#define LED2_On()								( GPIOA->ODR &= ~(1<<1)  )
//#define LED2_Off()              ( GPIOA->ODR |= (1<<1)   )



extern void AdcPortInit(void);
extern void AdcInit(void);

extern void delay_us(unsigned int us);


extern void delay_ms(unsigned int ms);
extern unsigned char ReadAdc1();
extern unsigned char ReadAdc2();
extern unsigned char ReadAdc3();
extern unsigned char ReadAdc4();
// extern unsigned char ReadAdc5();
// extern unsigned char ReadAdc6();


//void InitAdcCS1237A(void);
//unsigned char ReadAdcCS1237A(void); //int Ϊ���ֽڳ���
//unsigned char ConfigCS1237A(void);

//void InitAdcCS1237B(void);
//unsigned char ReadAdcCS1237B(void);
//unsigned char ConfigCS1237B(void);
//unsigned char RedConfigCS1237B(void);
//void AdcCs1237Init(void);
#endif
