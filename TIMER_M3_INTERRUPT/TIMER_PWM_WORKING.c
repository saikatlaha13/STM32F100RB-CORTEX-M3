#include <stm32f10x.h>

// PORTB pin PA4-PA7 output for update interrupt and PA0-PA3 for CC3 interrupt
// UIE is set by hardware when the register are updated(when CNT is reinitialized)

void init_interrupt();
void INIT_PWM()
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

	TIM3->CR1     = 0x00;                                             // reset all the register values
	TIM3->PSC     = 0x02ff;                                           // setting prescaler value
	TIM3->ARR     = 0xffff;
	
	TIM3->CCMR2  |= 0x0068;                                           // pwm mode on channel 3
	TIM3->CCR3    = 0x0fff;                                           // Set PWM start value if channel 3 
	TIM3->CCER    = TIM_CCER_CC3E;                                    // Enable capture compare on ch 3 
	TIM3->CR1    |= TIM_CR1_CEN;                                      // enable timer
}

void gpio_config()
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;                               // enabling clock to port b
	GPIOA->CRL = 0x22222222;                                          // all pins are output
}

void TIM3_IRQHandler()  
{
	unsigned int i;
  if (TIM3->SR & TIM_SR_UIF)       
	{  
		 //update interrupt flag
		 TIM3->SR &= ~TIM_SR_UIF;                                       // update interrupt flag
		 GPIOA->ODR ^= 0x00f0;
		 for(i=0 ; i<1000000 ; i++);
	}
 	if (TIM3->SR & TIM_SR_CC3IF)                                      // capture compare 3 interrupt flag 
	{ 
		 //capture compare 3 interrupt flag
		 TIM3->SR &= ~TIM_SR_CC3IF;    
     GPIOA->ODR ^= 0x000f;
		 for(i=0 ; i<1000000 ; i++);
	}
}

void init_interrupt()
{
	//additional code illustrating interrupts	
  TIM3->DIER =  TIM_DIER_UIE;                                 //UIE update interrupt enable
  TIM3->DIER |= TIM_DIER_CC3IE;                               //CCR3 capture compare 3 interrupt enable
  NVIC->ISER[0] |= 1<<29;                                     //TIM3 interrupt enable (INterrupt Set Enable Register)
}

int main () 
{
	INIT_PWM();
  gpio_config(); 
  init_interrupt();
	
	while(1)
	{
  }
}	

