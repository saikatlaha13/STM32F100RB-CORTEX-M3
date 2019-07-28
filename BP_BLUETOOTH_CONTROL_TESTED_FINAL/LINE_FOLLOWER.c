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
int PWM(int value);
void Init_UART1(void);
char rxdat;
char flag0=0,flag1=0,flag2=0,flag3=0,flag4=0;
void Uart1PutS(unsigned char *str);
void data_collect();
void Uart1PutCh (unsigned char ch);
void Delay_Ms(unsigned int count); 
void dataread(void);
void datacompare(void);
void set();
void decision();
void move_back();

char inChar;  
int x=0;
char rxdat1;
 
int main(void)
{
	PWM_init();
	set();
	Init_UART1();
	
	while(1)
  {
		if(flag0==1)     // Move Forward
    {
			Uart1PutS("forward");
      move_forward();
    }
  
    if(flag1==1)     // Turn right
    {
			Uart1PutS("left");
      move_left();
    }
  
    if(flag2==1)     // turn left
    {
			Uart1PutS("right");
      move_right();
    }
  
    if(flag3==1)     // stop
    {
			Uart1PutS("stop");
      stop_moving();
    }
		
		if(flag4==1)
		{
			Uart1PutS("back");
			move_back();
		}
  }
}

void set()
{
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
  GPIOA->CRL    = 0x22882222;                   // PC0,PC1,PC2,PC3=output  PC4,PC5=input PC6,PC7=output
	GPIOA->ODR   &= ~(LS|RS);                     // PULL down for pin PA4 and PA5
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
	USART1->CR1  |= USART_CR1_RXNEIE;
	NVIC_EnableIRQ(USART1_IRQn);
}

void USART1_IRQHandler(void)
{
	flag0=0,flag1=0,flag2=0,flag3=0,flag4=0;
	//Uart1PutS("entered");
	while(!(USART1->SR & USART_SR_RXNE) );
     rxdat1 = USART1->DR;
		//Uart1PutCh(rxdat1);
	if(rxdat1=='f')
	{
		flag0=1;
		//Uart1PutS("entered flag0=1");
	}
	else if(rxdat1=='l')
	{
		flag1=1;
		//Uart1PutS("entered flag1=1");
	}
	else if(rxdat1=='r')
	{
		flag2=1;
		//Uart1PutS("entered flag2=1");
	}
	else if(rxdat1=='s')
	{
		flag3=1;
		//Uart1PutS("entered flag3=1");
	}
	else if(rxdat1=='b')
	{
		flag4=1;
	}
}
 
void Delay_Ms(unsigned int count)
{
    volatile unsigned int j,k;
    for (j=0;j<count;j++)
	  {
        for (k=0;k<6000;k++)
		    {
        
        }
    }
}


void move_forward()
{
	 PWM(1000); 
   
	 GPIOA->BSRR =  LM1;
   GPIOA->BRR  =  LM2;
   GPIOA->BSRR =  RM1;
   GPIOA->BRR  =  RM2;
   return;
}

void move_left()
{
	 PWM(1000); 
	 
	 GPIOA->BSRR =  LM1;
   GPIOA->BRR  =  LM2;
   GPIOA->BRR  =  RM1;
   GPIOA->BRR  =  RM2;
   return;
}

void move_right()
{
	 PWM(1000); 
	 
	 GPIOA->BRR  = LM1;
   GPIOA->BRR  = LM2;
   GPIOA->BSRR = RM1;
   GPIOA->BRR  = RM2;
   return;
}

void stop_moving()
{
	 PWM(1000); 
	 
	 GPIOA->BRR = LM1;
   GPIOA->BRR = LM2;
   GPIOA->BRR = RM1;
   GPIOA->BRR = RM2;
   return;
}

void move_back()
{
	 PWM(1000); 
   
	 GPIOA->BRR  =  LM1;
   GPIOA->BSRR =  LM2;
   GPIOA->BRR  =  RM1;
   GPIOA->BSRR =  RM2;
   return;
}

void Uart1PutCh (unsigned char ch)   // Write character to Serial Port   
{                  		
    while(!(USART1->SR & USART_SR_TXE));             // check for usart2 transmit register empty
    USART1->DR = ch;
}

void Uart1PutS(unsigned char *str)	 //A function to send a string on UART0
{  
   while(*str)
   {  
      Uart1PutCh(*str++);	    
   }
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

void decision()
{
	  //Delay_Ms(1000);
	 
}


