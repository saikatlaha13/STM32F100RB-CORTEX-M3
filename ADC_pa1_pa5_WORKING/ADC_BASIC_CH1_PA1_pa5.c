//A-G = PC->8 = PC->15
//SEG1 - SEG4 = SEG1->PC9 , SEG2->PC8 , SEG3->PC7 , SEG3->PC6
//POTENTIOMETER INPUT=PA0 
 
 #include "stm32f10x.h"
 #include "jyotilib_gpio.h"
 #include "adc.h"
 #include <stdio.h>
 #include <string.h>
 //=============================================================================
 // Defines
 //=============================================================================
 
 #define segment1 1<<6;
 #define segment2 1<<7;
 #define segment3 1<<8;
 #define segment4 1<<9;
 
 void Uart1PutS(char *str);
  void Uart1PutCh (char ch) ;
 void display(unsigned int i);
 void view(unsigned int );
 void uart1_config();
 unsigned int b[16]={ 0x3F, 0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};
 char str[100];
 //=============================================================================
 // main function
 //=============================================================================
 int main(void)
 {
	 int i;
	 uart1_config();
	 
   RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN |  RCC_APB2ENR_ADC1EN | RCC_APB2ENR_IOPBEN;

   //GPIOB->CRH=0x22222222;// 7 SEG DATA 
	 //GPIOC->CRL=0X22000000;//ALTERNATE ANALOG INPUT PC1 CH11 + SEG SEL
	 //GPIOC->CRH=0X00000022;// SEG SEL
   //GPIOB->CRL=0X00000000;	 
	 //GPIOA->CRL=0X00000000;//PORT A PIN 0 IS ANALOG INPUT....OO(MODE).00(CONF

   ADC1->CR2 = ADC_CR2_ADON | ADC_CR2_CONT; // Turn on ADC, enable continuos mode
   //ADC1->SQR1 = ADC_SEQUENCE_LENGTH(0); // One channel in sequence
   //ADC1->SQR3 = ADC_SEQ1(11); // ADC channel 0 is first in sequence
	 
	 //ADC1->SQR3 = 0X000B; // CHANEL NO 11
	 ADC1->SQR3 |=  ADC_SQR3_SQ1_0;  // CHANNEL 1,PA1
   ADC1->SMPR2 = ADC_SAMPLE_TIME1(SAMPLE_TIME_239_5); // sample time for first channel
	 //ADC1->SMPR2= (ADC_SMPR2_SMP0_0 | ADC_SMPR2_SMP0_1 | ADC_SMPR2_SMP0_2); 2ND WAY FOR PREVIOUS LINE

   ADC1->CR2 |= ADC_CR2_ADON; // Turn on conversion

   while (1) {
		 for(i=0 ; i<1000000 ; i++);
	 if(ADC1->SR & ADC_SR_EOC)
   {
		 display(ADC1->DR & 0x00000fff);
   }
	}
 }
 
 void uart1_config()
{	
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN  | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_IOPBEN;
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN | RCC_APB2ENR_AFIOEN ;
  RCC->APB1ENR |= RCC_APB1ENR_TIM3EN ; 
	GPIOA->CRH = 0x000008b0;                                          // PA9=transmit PA10=receive
  USART1->CR1 = USART_CR1_UE | USART_CR1_TE;                        // usart1 enable and receiver enable
	USART1->BRR = (SystemCoreClock / 9600);                           // usart1 baud rate register setting	
}

void display(unsigned int num)
 {
		sprintf((char*)str,"moisture_channel_PA1=%d\n",num);
  Uart1PutS(str);
	memset(str,'\0',44); 
 }

 void Uart1PutCh (char ch) 
{
  while(!(USART1->SR & USART_SR_TXE));             // check for Transmit register empty
  USART1->DR = ch;
}
 
void Uart1PutS(char *str)
{
  while(*str != 0)
  {
    Uart1PutCh(*str);                              // put into uart register
    str++;
  }
}

 
	  
 //=============================================================================
 // End of file
 
