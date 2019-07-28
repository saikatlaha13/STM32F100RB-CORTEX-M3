#include "stm32f10x.h"

// LED PA.0 , PA.1 ,PA.2 , INTERRUPT PA.5 , PA.6  -- GROUP INTERRUPT
int main(void)
{
	volatile uint32_t dly;
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN;        // enable clock for port A
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;         // enable clock for Alternate Function

  GPIOA->CRL =0X22222222;                     // ALTERNATE FUNC ENABLING INTERRUPT %TH AND 6TH PIN	
	GPIOB->CRL =0x2AA22222;

  NVIC_EnableIRQ(EXTI9_5_IRQn);               // Enable interrupt 
	AFIO->EXTICR[1] |= 0xF11F;                  // PORTA 5TH and 6th PIN IS INTERRUPT EX 0000-PORTA OTH PIN
	
  EXTI->IMR       |= (1 << 5) | (1<<6);       // unmask interrupt
  EXTI->EMR       |= (1 << 5) | (1<<6);       // unmask event
  EXTI->RTSR      |= (1 << 5) | (1<<6);       // set rising edge
  EXTI->FTSR      |= (1 << 5) | (1<<6);       // set falling edge O=ONE TIME,1=TWO TIM
	
  NVIC_SetPriority(5,1);                      //  5    alternatively NVIC_SetPriority(5,1)
  NVIC_SetPriority(6,2);                      //	 6    making 6th pin interrupt as second priority
  NVIC_EnableIRQ(EXTI9_5_IRQn);               //   Enable interrupt from interrupt 5 to 9 (NVIC level)	
	
  while (1)
	{
	  GPIOA -> ODR = 0X0001 ;                   // glowing first led
 	  for(dly = 0; dly < 300000; dly++);        // delay
	  GPIOA -> ODR = 0X0000 ;                   // switching off all the leds
	  for(dly = 0; dly < 300000; dly++);        // delay
  }
}

void EXTI9_5_IRQHandler(void)
{
   volatile uint32_t dly;		
	 if(EXTI->PR & (1<<5))                      // checking if the interrupt occured in 5th pin
	 {
			EXTI->PR |= EXTI_PR_PR5;                // clearing the 5th interrupt pending register
      GPIOA -> ODR = 0X0020 ;                 // glowing second led for 5th interrupt
      for(dly = 0; dly < 300000; dly++);
   
	    GPIOA -> ODR = 0X0000 ;                 // switching off all the led
	    for(dly = 0; dly < 300000; dly++);      // toggle LED state
	 } 
	 else if(EXTI->PR & (1<<6))  
	 {
			EXTI->PR |= EXTI_PR_PR6;                // clearing 6th pin interrupt pending register 
  		GPIOA -> ODR = 0X0040 ;                 // glowing 6th led if its the interrupt from 6th pin
      for(dly = 0; dly < 300000; dly++);
  
	    GPIOA -> ODR = 0X0000 ;                 // switching off all the leds
	    for(dly = 0; dly < 300000; dly++);      // toggle LED state
	 }
}
	
			
