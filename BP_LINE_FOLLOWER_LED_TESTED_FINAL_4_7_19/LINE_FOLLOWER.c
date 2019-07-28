#include<stm32f10x.h>

// PA0,PA1,PA2,PA3 -> motors
// PA4,PA5 ->IR input
// PB1->PWM output

#define LS  1<<4      // left sensor
#define RS  1<<5      // right sensor
#define LM1 1<<0      // left motor  +ve
#define LM2 1<<1      // left motor  -ve
#define RM1 1<<2      // right motor +ve
#define RM2 1<<3      // right motor -ve

void move_forward();
void move_left();
void move_right();
void check();
void stop_moving();
int PWM_init();
int pwm(int value);
void PWM_3(int value);

// Timer3 ccr3 o/p-PB0 
// ADC input PA0

int PWM_init()
{
   RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPBEN;
   RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

	 GPIOB->CRL=0x000000A0;
	 
   AFIO->MAPR = (AFIO_MAPR_TIM3_REMAP_1);             // Full TIM3 remap

   TIM3->PSC = 23;                                    // Set prescaler to 24 (PSC + 1)
   TIM3->ARR = 4096;                                  // Auto reload value 4096 (PWM resolution 12-bits)
   TIM3->CCR4 = 0;                                    // Start value channel 3

	 TIM3->CCMR2 = TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1;

   TIM3->CCER = TIM_CCER_CC4E;                        // capture compare 3 output enable
   TIM3->CR1 = TIM_CR1_CEN;                           // Enable timer
 }

int PWM(int value)
{
	 TIM3->CCR4 = value;
}

void set()
{
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
  GPIOA->CRL    = 0x22882222;                   // PC0,PC1,PC2,PC3=output  PC4,PC5=input PC6,PC7=output
	GPIOA->ODR   &= ~(LS|RS);                     // PULL down for pin PA4 and PA5
}

int main()
{
	PWM_init();
	set();
	while(1)
	{
		//PWM(10000);  // trial only
    check();
  }
}

void check()
{
	  if(!(GPIOA->IDR & LS) && !(GPIOA->IDR & RS))     // Move Forward
    {
			PWM(1200);  // real run
      move_forward();
    }
  
    if((GPIOA->IDR & LS) && !(GPIOA->IDR & RS))     // Turn right
    {
			PWM(1200);
      move_right();
    }
  
    if(!(GPIOA->IDR & LS) && (GPIOA->IDR & RS))     // turn left
    {
      PWM(1200);
      move_left();
    }
  
    if((GPIOA->IDR & LS) && (GPIOA->IDR & RS))     // stop
    {
      stop_moving();
    }
}

void move_forward()
{
   GPIOA->BSRR =  LM1;
   GPIOA->BRR  =  LM2;
   GPIOA->BSRR =  RM1;
   GPIOA->BRR  =  RM2;
   return;
}

void move_left()
{
	 GPIOA->BSRR =  LM1;
   GPIOA->BRR  =  LM2;
   GPIOA->BRR  =  RM1;
   GPIOA->BRR  =  RM2;
   return;
}

void move_right()
{
	 GPIOA->BRR  = LM1;
   GPIOA->BRR  = LM2;
   GPIOA->BSRR = RM1;
   GPIOA->BRR  = RM2;
   return;
}

void stop_moving()
{
	 GPIOA->BRR = LM1;
   GPIOA->BRR = LM2;
   GPIOA->BRR = RM1;
   GPIOA->BRR = RM2;
   return;
}



