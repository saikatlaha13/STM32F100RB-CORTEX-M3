 #include "stm32f10x.h"
 #include "stm32f10x.h"
 #include <stdio.h>
 #include <STRING.h>
 //#include "jyotilib_gpio.h"

// send "ON*" for glowing LED send "OFF*" for switching off led
// PA10->UART1(RX) --> connected to bluetooth(TX) 
// PA9->UART1(TX)

 //=============================================================================
 // Defines
 //=============================================================================
 
 #define led1  (1<<8)
 #define led2  (1<<9)  //status led
 
 void Delay_Ms(unsigned int count); 
 void dataread(void);
 void datacompare(void);
 void Uart1PutS(char *str);
 void Uart1PutCh (char ch) ;

 char inChar[10];  
 int x=0;
 char ch1;
 
 //=============================================================================
 // main function
 //=============================================================================
int main(void)
{
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN| RCC_APB2ENR_USART1EN | RCC_APB2ENR_AFIOEN|RCC_APB2ENR_IOPCEN ;
  RCC->APB1ENR |= RCC_APB1ENR_TIM3EN|RCC_APB1ENR_USART2EN;
  GPIOC->CRH = 0x22222222;
	GPIOA->CRH = 0x000008b0;   // PA10 input pull up/ pull down

  USART1->CR1 = USART_CR1_UE | USART_CR1_RE | USART_CR1_TE;
  USART1->BRR = (SystemCoreClock / 9600); 
  GPIOC->BRR = led2;
	 //Uart1PutS("hello");
	 
  while(1)
  {
	   GPIOC->BRR = led2;
	   dataread();
	   datacompare();
  }
}

void dataread(void)
{
	int index=0;
	 l1: 
  while(!(USART1->SR & USART_SR_RXNE));

  inChar[index] = USART1->DR;

  if(inChar[index]!='*')
  {
	  index++;
    goto l1;
  }
 
  inChar[++index]='\0';
  Uart1PutS(inChar);             // for debugging purpose
}


void datacompare(void)
{
 Delay_Ms(1000);
 x=0;
 if(inChar[0] =='O' && inChar[1] == 'N' && inChar[2] == '*')
 {
    GPIOC->BSRR = led1;
 }
 if(inChar[0] =='O' && inChar[1]=='F' && inChar[2]=='F' && inChar[3] == '*')
 {
   GPIOC->BRR = led1;
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
