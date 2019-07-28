#include "stm32f10x.h"
 #include "jyotilib_gpio.h"

 //=============================================================================
 // Defines
 //=============================================================================
 
 
 #define USART_RX_GPIO GPIOA
 #define USART_RX_PIN  3                    

 #define USART_TX_GPIO GPIOA
 #define USART_TX_PIN  2
 
 #define led1  (1<<8)
 
 #define led2  (1<<9)//status led
 
 void Delay_Ms(unsigned int count);
 unsigned char cmd_AT[] = "AT";

unsigned char cmd_RST[] = "AT+RST";


unsigned char cmd_CWJAP[] ="AT+CWJAP=\"jyotielabs\",\"9845615462\"";

unsigned char cmd_CWMODE[] = "AT+CWMODE=3";

unsigned char cmd_CIPMUX[] = "AT+CIPMUX=1";

unsigned char cmd_CIPSERVER[] = "AT+CIPSERVER=1,80";
unsigned char cmd_CIFSR[] = "AT+CIFSR";

unsigned char ENTER = 0x0D;	

unsigned char ENTER1 = 0x0A;	
 
 void AT_cmd(unsigned char *ptr3);
 
 void Uart2PutCh (unsigned char ch);
 
 void Uart2PutS(unsigned char *str)	;
 
 void dataread(void);

void datacompare(void);

char inData[400]; 
char inChar; 
int index = 0; 

int x=0;
 
 
 
 void ESP_init();
 void ESP_SEND();
 char ch1;
 //=============================================================================
 // main function
 //=============================================================================
 int main(void)
 {
 RCC->APB2ENR |= RCC_APB2ENR_IOPAEN| RCC_APB2ENR_USART1EN | RCC_APB2ENR_AFIOEN|RCC_APB2ENR_IOPCEN ;
 RCC->APB1ENR |= RCC_APB1ENR_TIM3EN|RCC_APB1ENR_USART2EN;


	 
GPIOC->CRH = 0x22222222;



#if (USART_RX_PIN > 7)
   USART_RX_GPIO->CRH = (USART_RX_GPIO->CRH & CONFMASKH(USART_RX_PIN)) | GPIOPINCONFH(USART_RX_PIN,   GPIOCONF(GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULLUPDOWN));
 #else
   USART_RX_GPIO->CRL = (USART_RX_GPIO->CRL & CONFMASKL(USART_RX_PIN)) | GPIOPINCONFL(USART_RX_PIN,   GPIOCONF(GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULLUPDOWN));
 #endif

#if (USART_TX_PIN > 7)
   USART_TX_GPIO->CRH = (USART_TX_GPIO->CRH & CONFMASKH(USART_TX_PIN)) | GPIOPINCONFH(USART_TX_PIN,   GPIOCONF(GPIO_MODE_OUTPUT2MHz, GPIO_CNF_AFIO_PUSHPULL));
 #else
   USART_TX_GPIO->CRL = (USART_TX_GPIO->CRL & CONFMASKL(USART_TX_PIN)) | GPIOPINCONFL(USART_TX_PIN,   GPIOCONF(GPIO_MODE_OUTPUT2MHz, GPIO_CNF_AFIO_PUSHPULL));
 #endif

USART2->CR1 = USART_CR1_UE | USART_CR1_TE; // USART enable and TE transmitter enable
//USART1->CR1 = USART_CR1_UE | USART_CR1_RE;
	USART2->BRR = (SystemCoreClock / 9600);

USART1->CR1 = USART_CR1_UE | USART_CR1_RE;
	 USART1->BRR = (SystemCoreClock / 9600);
	 
	GPIOC->BRR = led2;
	 
while(1)
{
	
	 //GPIOC->BSRR = led1;
	GPIOC->BRR = led2;
	
	
	ESP_init();
	
	dataread();
	  datacompare();
}




}
 void ESP_init()
{
	
	AT_cmd(cmd_AT);
		Uart2PutCh (ENTER);
		Uart2PutCh (ENTER1);
	  Delay_Ms(500);
   
	AT_cmd(cmd_RST);
	  Uart2PutCh (ENTER);
	  Uart2PutCh (ENTER1);
		Delay_Ms(1000);
	
	AT_cmd(cmd_CWJAP);
	  Uart2PutCh (ENTER);
	  Uart2PutCh (ENTER1);
		Delay_Ms(6000);

	AT_cmd(cmd_CWMODE);
	     Uart2PutCh (ENTER);
	     Uart2PutCh (ENTER1);
	Delay_Ms(1000);
	AT_cmd(cmd_CIPMUX);
	     Uart2PutCh (ENTER);
	     Uart2PutCh (ENTER1);
			Delay_Ms(900);
	AT_cmd(cmd_CIPSERVER);
	Uart2PutCh (ENTER);
	Uart2PutCh (ENTER1);
	Delay_Ms(900);
	AT_cmd(cmd_CIFSR);
	Uart2PutCh (ENTER);
	Uart2PutCh (ENTER1);
		Delay_Ms(1000);
		GPIOC->BSRR = led2;
		Delay_Ms(10);
	return;
}
void dataread(void)
{

index=0;  
 l1: 
while(!(USART1->SR & USART_SR_RXNE));
{
	
inChar = USART1->DR;;

inData[index] = inChar;
index++;
	//IO1SET=0xFFFFFFFF;
}
if(inChar !='H')

	
goto l1;



}


void datacompare(void)
{


 Delay_Ms(1000);
 x=0;


 
   if(inData[34] =='1')
   {
    GPIOC->BSRR = led1;
   }
	 
   if(inData[34] =='0')
   {
   GPIOC->BRR = led1;
   }
 }
 void AT_cmd(unsigned char *ptr3)
{	 						  	 
	 while(*ptr3!='\0')
	 {
	 	Uart2PutCh (*ptr3++);
	 }	
	 return;
	 
}
 
 void Uart2PutCh (unsigned char ch) 
 {
 while(!(USART2->SR & USART_SR_TXE));
 USART2->DR = ch;
 }
 
 void  Uart2PutS(unsigned char *str)
 {
 while(*str != 0)
 {
   Uart2PutCh(*str);
  str++;
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
