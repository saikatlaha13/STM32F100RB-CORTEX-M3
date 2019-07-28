#include "stm32f10x.h"
#include "jyotilib_gpio.h"

int main(void)
{
	volatile uint32_t dly;
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN;
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;       // enable clock for Alternate Function
	GPIOA->CRL =0X22222222;                   // ALTERNATE FUNC PUSH PULL=A, ALTERNATE FUCT OPEN DRAIN=E,NO DIFFERENCE
	GPIOB->CRL =0x222222A2;                   // PB1 as interrupt pin

	NVIC_EnableIRQ(EXTI1_IRQn);               // Enable interrupt (NVIC level)
  
	AFIO->EXTICR[0] |= 0xFF1F;                // PORTB PB1 PIN IS INTERRUPT writtn as 1 for B port
	
  EXTI->IMR       |= (1 << 1) ;             // unmask interrupt
  EXTI->EMR       |= (1 << 1) ;             // unmask event
  EXTI->RTSR      |= (1 << 1) ;             // set rising edge
  EXTI->FTSR      |= (1 << 1) ;             // set falling edge O=ONE TIME,1=TWO TIME
  NVIC_SetPriority(1,1);                    // pin2 priority 1 
	
  while (1) 
	{
	   GPIOA -> ODR = 0X000F ;
     for(dly = 0; dly < 300000; dly++);
	   GPIOA -> ODR = 0X0000 ; 
	   for(dly = 0; dly < 300000; dly++);
	}
}

void EXTI1_IRQHandler(void)
{
   volatile uint32_t dly;
	 EXTI->PR |= EXTI_PR_PR1 ;                // clearing interrupt pending register
			
	 GPIOA -> ODR = 0X00F0 ;
   for(dly = 0; dly < 300000; dly++);
	 GPIOA -> ODR = 0X0000 ; 
	 for(dly = 0; dly < 300000; dly++);       // toggle LED state
}