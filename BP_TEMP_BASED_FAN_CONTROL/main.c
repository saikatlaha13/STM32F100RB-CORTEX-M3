#include "stm32f10x.h"
#include "adc.h"
#include<stdio.h>
#include<STRING.h>

// Timer3 ccr4 o/p-PB1 
// ADC input PA0 -> temperature sensor output connected 

unsigned int adc_val;
float temp;
void fan_speed_control();
void Uart1PutS(unsigned char *str);
char check[30];
void Init_UART1();

int main(void)
{
	 Init_UART1();
   RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
   RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
		
	 GPIOA->CRL=0X00000000;                             // PORT A PIN 0 IS ANALOG INPUT....OO(MODE).00(CONFIG)
	 GPIOB->CRL=0x000000AA;
	 
   AFIO->MAPR = (AFIO_MAPR_TIM3_REMAP_1);                 // Full TIM3 remap

   TIM3->PSC = 23;                                    // Set prescaler to 24 (PSC + 1)
   TIM3->ARR = 4096;                                  // Auto reload value 4096 (PWM resolution 12-bits
	 TIM3->CCR4 = 0;

	 TIM3->CCMR2 = TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1;
	 TIM3->CCER = TIM_CCER_CC4E;
   TIM3->CR1 = TIM_CR1_CEN;                           // Enable timer

   ADC1->CR2 = ADC_CR2_ADON | ADC_CR2_CONT;           // Turn on ADC, enable continuos mode
   ADC1->SQR1 = ADC_SEQUENCE_LENGTH(0);               // One channel in sequence
   ADC1->SQR3 = ADC_SEQ1(0);                          // ADC channel 0 is first in sequence
   ADC1->SMPR2 = ADC_SAMPLE_TIME0(SAMPLE_TIME_239_5); // sample time for first channel

   ADC1->CR1 = ADC_CR1_EOCIE;                         // Enable interrupt form End Of Conversion
   NVIC_EnableIRQ(ADC1_IRQn);                         // Enable interrupt form ACD1 peripheral

   ADC1->CR2 |= ADC_CR2_ADON;                         // turn adc peripheral on

   while (1) 
	 {
		 fan_speed_control();
	 }
}

void ADC1_IRQHandler (void)
{
   if(ADC1->SR & ADC_SR_EOC)
   {
		 adc_val = ADC1->DR;
   }
	 temp=adc_val/10;
	 sprintf(check,"temp=%f\n",temp);
	 Uart1PutS(&check[0]);
	 fan_speed_control();
}

int PWM(int value)
{
	 TIM3->CCR4 = value;
}

void fan_speed_control()
{
	if(temp > 29 && temp < 33)
	{
		PWM(500);
		Uart1PutS("fan speed 0\n");
	}
	else if(temp >35 && temp <40)
	{
		PWM(1000);
		Uart1PutS("fan speed 1\n");
	}
	else if(temp >40 && temp <45)
	{
		PWM(2000);
		Uart1PutS("fan speed 2\n");
	}
	else if(temp>50)
	{
		PWM(3000);
		Uart1PutS("fan speed 3\n");
	}
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

