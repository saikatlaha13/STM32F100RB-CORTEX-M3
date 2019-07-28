#include "stm32f10x.h"
#include "jyotilib_gpio.h"

int main(void)
{
	volatile uint32_t dly;
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN ;
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;       // enable clock for Alternate Function
	GPIOA->CRL =0X22222A22;                   // ALTERNATE FUNC PUSH PULL=A, ALTERNATE FUCT OPEN DRAIN=E,NO DIFFERENCE

	NVIC_EnableIRQ(EXTI2_IRQn);               // Enable interrupt (NVIC level)
  
	AFIO->EXTICR[0] |= 0xF0FF;                // PORTA PA2 PIN IS INTERRUPT
	
  EXTI->IMR       |= (1 << 2) ;             // unmask interrupt
  EXTI->EMR       |= (1 << 2) ;             // unmask event
  EXTI->RTSR      |= (1 << 2) ;             // set rising edge
  EXTI->FTSR      |= (1 << 2) ;             // set falling edge O=ONE TIME,1=TWO TIME
  NVIC_SetPriority(2,1);                    // pin2 priority 1 
	
  while (1) 
	{
	   GPIOA -> ODR = 0X000F ;
     for(dly = 0; dly < 300000; dly++);
	   GPIOA -> ODR = 0X0000 ; 
	   for(dly = 0; dly < 300000; dly++);
	}
}

void EXTI2_IRQHandler(void)
{
   volatile uint32_t dly;
	 EXTI->PR |= EXTI_PR_PR2 ;                // clearing interrupt pending register
			
	 GPIOA -> ODR = 0X00F0 ;
   for(dly = 0; dly < 300000; dly++);
	 GPIOA -> ODR = 0X0000 ; 
	 for(dly = 0; dly < 300000; dly++);       // toggle LED state
}