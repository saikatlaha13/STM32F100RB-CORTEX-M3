#include <stm32f10x.h>

void timer1_config()
{
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;                       // enable tim1 clock and alternate functions
	TIM1->CR1 = 0x00;                                         // reset all the values in the register
	TIM1->SMCR &= ~7;                                         // slave mode disabled
	TIM1->PSC = 15998;                                         // 1 ms since system core clock is 8MHZ so psc=8mhz/7999
	TIM1->CR1 |= TIM_CR1_CEN;                                 // counter enable
}

void gpio_config()
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN;  // enabling clock to port b
	GPIOB->CRL = 0x22222222;                                  // all pins are output
}

void Wait_ms(int t) 
{ 
	TIM1->EGR = TIM_EGR_UG;                                   // update timer event (Resets counter/prescaler)
	while (TIM1->CNT  < t);                                   // it acts as a delay each 7999 clock cycles increments cnt
}

int main()
{
	gpio_config();
	timer1_config();
	for(;;)
	{
		GPIOB->ODR ^=0x00ff;                                    // toggling
		Wait_ms(2000);                                          // like delay
	}
}