#include "stm32f10x.h"
#include "jyotilib_gpio.h"

// PIN PA0-inteerupt pin
// output pin PA4-PA7 output pin in ISR

int main(void)
{
	 volatile uint32_t dly;
   RCC->APB2ENR |= RCC_APB2ENR_IOPAEN ;
   RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;     // enable clock for Alternate Function
   GPIOA->CRL =0X2222222A;                 // ALTERNATE FUNC PUSH PULL=A, ALTERNATE FUCT OPEN DRAIN=E,NO DIFFERENCE
  
	 AFIO->EXTICR[0] |= 0xFFF0;              // PORTA 0TH PIN IS INTERRUPT EX 0000-PORTA OTH PIN
	
   EXTI->IMR       |= (1 << 0) ;           // unmask interrupt
   EXTI->EMR       |= (1 << 0) ;           // unmask event
   EXTI->RTSR      |= (1 << 0) ;           // set rising edge
   EXTI->FTSR      |= (1 << 0) ;           // set falling edge O=ONE TIME,1=TWO TIME
	
   NVIC_SetPriority(0,1);                  // PIN 0 priority 1

   NVIC_EnableIRQ(EXTI0_IRQn);             // Enable interrupt for EXTI0 (NVIC level)	
	
   while (1) 
   {
 	   GPIOA -> ODR = 0X000F ;               // PA0-PA3 will keep toggeling
     for(dly = 0; dly < 300000; dly++);
		 
	   GPIOA -> ODR = 0X0000 ; 
	   for(dly = 0; dly < 300000; dly++);
   }
}

void EXTI0_IRQHandler(void)
{
   volatile uint32_t dly;
	 EXTI->PR |= EXTI_PR_PR0 ;	
   
	 GPIOA -> ODR = 0X00F0 ;                 // PA4 - PA7
	 for(dly = 0; dly < 300000; dly++);
   GPIOA -> ODR = 0X0000 ; 
	 for(dly = 0; dly < 300000; dly++);      // toggle LED state
}