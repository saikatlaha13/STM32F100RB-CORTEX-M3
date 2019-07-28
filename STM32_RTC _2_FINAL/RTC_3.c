#include "stm32f10x.h"

void delay_ms(unsigned int i);
void RTC_init_1();
void stm32_custom(void);

unsigned int ledPosSec   = 0;                     // led position for second
unsigned int ledPosAlarm = 2;                     // led position for Alarm
unsigned int ledRtcSec   = 0;
unsigned int flag=0;

void RTC_IRQHandler(void)
{    
		if(flag==0)
		{ 
      GPIOB->BSRR = (1<<5 | 1<<6 | 1<<7);           // Second LED on
			delay_ms(400);
			flag=1;
		}
		
		if(flag==1 )
		{
			GPIOB->BRR = (1<<7 | 1<<5 | 1<<6);
			delay_ms(400);
			flag=0;
		}
			
		while(!(RTC->CRL & RTC_CRL_RTOFF));
		RTC->CRL |= RTC_CRL_CNF; 
		RTC->CRL &= ~(1<<RTC_CRL_SECF);                // clear second flag
		RTC->CRL &= ~RTC_CRL_CNF;
} 

int main (void) 
{
	RTC_init_1();
  while (1) 
	{                                               // Loop forever
		
  } 
} 

void RTC_init_1()
{
    unsigned char timeout = 0;
    
	      RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;                     // providing clock to B port
	      GPIOB->CRH = 0x22222222;                               // output configuration for B port
	      GPIOB->CRL = 0x22222222;
	
        RCC->APB1ENR |= RCC_APB1ENR_PWREN ;                     // power interface clock enable backup interface clock enable
        RCC->APB1ENR |= RCC_APB1ENR_BKPEN;                      // enable backup
        
        PWR->CR |= PWR_CR_DBP;                                  // disable backup write protection
        RCC->BDCR |= RCC_BDCR_BDRST;                            // enable bacup domain software reset
        RCC->BDCR &= ~RCC_BDCR_BDRST;                           // disable backup domain software reset
        
        RCC->BDCR &= ~RCC_BDCR_LSEBYP;                          // no bypass of external clock
        RCC->BDCR |= RCC_BDCR_LSEON;                            // lse clock switched on
        
			  while(!(RCC->BDCR & RCC_BDCR_LSERDY));
      
        delay_ms(100);
 
        RCC->BDCR |= (RCC_BDCR_RTCSEL_0);// hse clock divided by 128 is selected
        RCC->BDCR |= RCC_BDCR_RTCEN;                            // enable RTC clock
 
        while(!(RTC->CRL & RTC_CRL_RTOFF));                    // write on th rtc registers are still ongoing
        while(!(RTC->CRL & RTC_CRL_RSF));                      // register synchronised flag
        RTC->CRH |= RTC_CRH_SECIE;                              // enable rtc second interrupt
        while(!(RTC->CRL & RTC_CRL_RTOFF));                    // write on th rtc registers are still ongoing
        
        RTC->CRL |= RTC_CRL_CNF;                                // enabling cnf to configure rtc parameters
        RTC->PRLL = 32767;
				//RTC->DIVL = 32767;
				while(!(RTC->CRL & RTC_CRL_RTOFF));                    // write on th rtc registers are still ongoing
				
        RTC->CRL &= ~RTC_CRL_CNF;                                // disabling cnf to configure rtc parameters
 
        while(!(RTC->CRL & RTC_CRL_RTOFF));                    // write on th rtc registers are still ongoing
        //BKP_DR1 = rtc_access_code;
        PWR->CR &= ~PWR_CR_DBP;                                 // enable backup write protection 	
        
				EXTI->IMR |= EXTI_IMR_MR17;                             // interrupt on line 17 in not masked
	      EXTI->EMR |= EXTI_EMR_MR17;                             // eevnt on line 17 is not masked
	      EXTI->RTSR |= EXTI_RTSR_TR17;
	
	      NVIC_EnableIRQ(RTC_IRQn);   
}

void delay_ms(unsigned int i)
{
	unsigned int j,k;
	for(k=0 ; k<i ; k++)
	{
		for(j=0 ; j<6000 ; j++);
	}
}


