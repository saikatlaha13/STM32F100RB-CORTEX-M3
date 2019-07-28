#include<stm32f10x.h>
#include<stdio.h>
#include<STRING.h>

// vlotage=+5volt to ultrasound
// PA0,PA1,PA2,PA3 -> motors
// PA4,PA5 ->IR input
// PB1->PWM output
// PA6->trigger
// PA7->Echo
// PB1->pwm output

#define LS  1<<4      // left sensor
#define RS  1<<5      // right sensor
#define LM1 1<<0      // left motor  +ve
#define LM2 1<<1      // left motor  -ve
#define RM1 1<<2      // right motor +ve
#define RM2 1<<3      // right motor -ve
#define TRIG 1<<6     // trigger pin
#define ECHO 1<<7     // echo pin

void move_forward();
void Uart1PutS(unsigned char *str);
void move_left();
void move_right();
void check();
void stop_moving();
int PWM_init();
int pwm(int value);
void PWM_3(int value);
int distance_ultrasound();
int echo_time;
int distance,i;
char check_string[40];
void Uart1PutCh (unsigned char ch);

void timer1_config()
{
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN ;                      // enable tim1 clock and alternate functions
	TIM1->CR1 = 0x00;                                         // reset all the values in the register
	TIM1->SMCR &= ~7;                                         // slave mode disabled
	TIM1->PSC = 12;                                        // 1 ms since system core clock is 8MHZ so psc=8mhz/7999
	TIM1->CR1 |= TIM_CR1_CEN;                                 // counter enable
}

void set()
{
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
  GPIOA->CRL    = 0x82882222;                   // PC0,PC1,PC2,PC3=output  PC4,PC5=input PC6,PC7=output
	GPIOA->ODR   &= ~(LS|RS);                     // PULL down for pin PA4 and PA5
	GPIOA->ODR   &= ~ECHO;
}

void delay(int t) 
{ 
	TIM1->EGR = TIM_EGR_UG;                                   // update timer event (Resets counter/prescaler)
	while (TIM1->CNT  < t);                                   // it acts as a delay each 7999 clock cycles increments cnt
}

void time()
{
	TIM1->EGR = TIM_EGR_UG;
}

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

void check(int dist)
{
	  if(dist > 35)     // Move Forward
    {  
			PWM(1000);
      move_forward();
    }
  
    if(dist < 35)     // Turn right
    {
			PWM(1000);
      move_left();
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

int distance_ultrasound()
{
		//Output 10us HIGH on TRIG pin
		GPIOA->BSRR |= TRIG;
		delay(10);
		GPIOA->BRR |= TRIG;

		while(!(GPIOA->IDR & ECHO)); //Wait for a HIGH on ECHO pin
		time(); //Start counting
		while(GPIOA->IDR & ECHO); //Wait for a LOW on ECHO pin
		echo_time = TIM1->CNT; //Stop counting and save value(us) in echoTime

		distance = (0.0343 * echo_time)/2; //Find the distance
	
	  return distance;
}

void Init_UART1(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN  | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_IOPBEN;
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN | RCC_APB2ENR_AFIOEN ;
	GPIOA->CRH    = 0x000008b0;                                          // PA9=transmit PA10=receive
	USART1->CR1  |= USART_CR1_UE;                                        // usart1 enable and receiver enable
  USART1->CR2  &= ~USART_CR2_STOP_0 | ~USART_CR2_STOP_1;
	USART1->CR1  &= ~USART_CR1_M;
	USART1->BRR   = (SystemCoreClock / 9600);                           // usart1 baud rate register setting	
	USART1->CR1  |= USART_CR1_RE | USART_CR1_TE ;
}

void Uart1PutCh (unsigned char ch)   // Write character to Serial Port   
{                  		
    while(!(USART1->SR & USART_SR_TXE));
	  USART1->DR=ch;
}

void Uart1PutS(unsigned char *str)	 //A function to send a string on UART1
{  
   while(*str)
   {  
      Uart1PutCh(*str++);	    
   }
}

int main()
{
	int dist;
	set();
	timer1_config();
	PWM_init();
	Init_UART1();

	while(1)
	{
		dist=distance_ultrasound();
    check(dist);
		sprintf(check_string,"dist=%d\n",dist);
		Uart1PutS(&check_string[0]);
  }
}






