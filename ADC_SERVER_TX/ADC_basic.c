#include "stm32f10x.h"
 #include "jyotilib_gpio.h"
 #include "adc.h"
 //=============================================================================
 // Defines
 //=============================================================================
 #define LED_BLUE_GPIO GPIOC
 #define LED_BLUE_PIN 8 

#define led2  (1<<9)//status led

#define AIN0_GPIO GPIOA
 #define AIN0_PIN 0 //adc pin PORT(PA0)
 
 #define USART_RX_GPIO GPIOA
 #define USART_RX_PIN  3

#define USART_TX_GPIO GPIOA
 #define USART_TX_PIN  2
 
 void Delay_Ms(unsigned int count);
 unsigned char cmd_AT[] = "AT";

unsigned char cmd_RST[] = "AT+RST";

unsigned char cmd_CWMODE[] = "AT+CWMODE=3";

unsigned char cmd_CIPSTART[] = "AT+CIPSTART=\"TCP\",\"192.168.43.177\",80";

unsigned char cmd_CIPSEND[] = "AT+CIPSEND=85";

unsigned char ENTER = 0x0D;	

unsigned char ENTER1 = 0x0A;	
 
 void AT_cmd(unsigned char *ptr3);
 
 void Uart2PutCh (unsigned char ch);
 
 void Uart2PutS(unsigned char *str)	;
 
  unsigned int value=0;
 
 void ADC_Init(void);
 
 void ADC(int);
 
 char ADC_DATA[4];
 
 void ESP_init();
 //=============================================================================
 // main function
 //=============================================================================
 int main(void)
 {
 RCC->APB2ENR |= RCC_APB2ENR_IOPAEN  | RCC_APB2ENR_ADC1EN | RCC_APB2ENR_IOPCEN;
 RCC->APB1ENR |= RCC_APB1ENR_TIM3EN|RCC_APB1ENR_USART2EN;


	 
	 GPIOC->CRH=0x22222222;





ADC1->CR2 = ADC_CR2_ADON | ADC_CR2_CONT; // Turn on ADC, enable continuos mode
 ADC1->SQR1 = ADC_SEQUENCE_LENGTH(0); // One channel in sequence
 ADC1->SQR3 = ADC_SEQ1(0); // ADC channel 0 is first in sequence
 ADC1->SMPR2 = ADC_SAMPLE_TIME0(SAMPLE_TIME_239_5); // sample time for first channel

ADC1->CR1 = ADC_CR1_EOCIE; // Enable interrupt form End Of Conversion
 NVIC_EnableIRQ(ADC1_IRQn); // Enable interrupt form ACD1 peripheral

ADC1->CR2 |= ADC_CR2_ADON; // Turn on conversion

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

 GPIOC->BRR = led2;

ESP_init();



//while (1) {}
 }
 //=============================================================================
 // ADC1 Interrupt handler
 //=============================================================================
 void ADC1_IRQHandler (void)
 {
 if(ADC1->SR & ADC_SR_EOC)
 {
 value = ADC1->DR;//adc pin PORT(PA0)
	 ADC(value);
 }
 }
 void ADC(int value)
{	 
	unsigned long adc_data;
    unsigned char ones,tens,hundreds,thousands;
	  unsigned long temp;
	
	  temp = value;
		ones = temp % 10;	
		temp = temp / 10;
		tens = temp % 10;
		temp = temp / 10;
		hundreds = temp % 10;
		temp = temp / 10;
		thousands = temp % 10;
	
	  ones |= 0x30;
		tens |= 0x30;
		hundreds |= 0x30;
		thousands |= 0x30;
	
	  ADC_DATA[0]=thousands;
	  ADC_DATA[1]=hundreds;
    ADC_DATA[2]=tens;
		ADC_DATA[3]=ones;
	
	
	
	
	
	
	
}
 //=============================================================================
 // End of file
 void ESP_init()
{
		int i;
	AT_cmd(cmd_AT);
		Uart2PutCh (ENTER);
		Uart2PutCh (ENTER1);
	  Delay_Ms(500);
   
	AT_cmd(cmd_RST);
	  Uart2PutCh (ENTER);
	  Uart2PutCh (ENTER1);
		Delay_Ms(3000);

	AT_cmd(cmd_CWMODE);
	     Uart2PutCh (ENTER);
	     Uart2PutCh (ENTER1);
	Delay_Ms(3000);
	AT_cmd(cmd_CIPSTART);
	     Uart2PutCh (ENTER);
	     Uart2PutCh (ENTER1);
	    
			Delay_Ms(2000);
	AT_cmd(cmd_CIPSEND);
	Uart2PutCh (ENTER);
	Uart2PutCh (ENTER1);
		Delay_Ms(2000);
		Uart2PutS("GET /dashboard/iot/api/data.php?data=");	
		
		for(i=0;i<4;i++)
  {
		Uart2PutCh(ADC_DATA[i]);
	}
		Uart2PutS("&val=adc HTTP/1.1");
		Uart2PutCh (ENTER);
		Uart2PutCh (ENTER1);
    Uart2PutS("HOST:192.168.43.197");
		 
Uart2PutCh (ENTER);
Uart2PutCh (ENTER1);
Uart2PutCh (ENTER);
Uart2PutCh (ENTER1);
Uart2PutCh (ENTER);
Uart2PutCh (ENTER1);
	
GPIOC->BSRR = led2;
	Delay_Ms(10);
	return;
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
