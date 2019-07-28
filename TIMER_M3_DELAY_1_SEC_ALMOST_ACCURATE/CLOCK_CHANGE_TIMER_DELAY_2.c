#include <stm32f10x.h>
void  System( ) ;

void timer1_config()
{
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN ;                      // enable tim1 clock and alternate functions
	TIM1->CR1 = 0x00;                                         // reset all the values in the register
	TIM1->SMCR &= ~7;                                         // slave mode disabled
	TIM1->PSC = 12000;                                        // 1 ms since system core clock is 8MHZ so psc=8mhz/7999
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
	System();
	gpio_config();
	timer1_config();
	for(;;)
	{
		GPIOB->ODR ^=0x00ff;                                    // toggling
		Wait_ms(5000);                                          // like delay in millisecond
	}
}

void  System() 
{   
   RCC->CR |= RCC_CR_HSION;                         //4MHZ clock  //PLLSRC;   
	 while(!(RCC->CR & RCC_CR_HSIRDY));
	 
	 RCC->CR &= ~RCC_CR_PLLON;      //   //PLLoff
	 RCC->CFGR &= ~RCC_CFGR_PLLSRC;                   //HSI clock/2 is selected as PLL input clock 
	 RCC->CFGR |= 1<<19;                              //4 * 4 = 16MHZ
	
	 //clock=2*4 if PLLMUL=0 8mhz

	 RCC->CFGR &= ~RCC_CFGR_SW_0 | RCC_CFGR_SW_1;     //PLL is selected as system clock
	 RCC->CR |= RCC_CR_PLLON;                         //PLLON
	 while (!(RCC->CR & (1<<25)));	  //  
	 RCC->CFGR |= 1<<7;                               //sysclock/2 AHB prescaler    8 MHz &  also if APB2=1 then multiply with 1 or else with 2 , here else case is taken so timer clock=16MHZ
} 
