#include "stm32f10x.h"
 #include "antilib_gpio.h"
 //=============================================================================
 // Defines
 //=============================================================================
 #define USART_RX_GPIO GPIOA
 #define USART_RX_PIN  10

#define USART_TX_GPIO GPIOA
 #define USART_TX_PIN  9

uint8_t text [] = "STM32VLDISCOVERY tutorial\n\rhttp://en.radzio.dxp.pl/stm32vldiscovery/\n\r\n\r";
 //=============================================================================
 //
 //=============================================================================
 void USART_PutChar(uint8_t ch)
 {
 while(!(USART1->SR & USART_SR_TXE));
 USART1->DR = ch;
 }
 //=============================================================================
 //
 //=============================================================================
 void USART_PutString(uint8_t * str)
 {
 while(*str != 0)
 {
 USART_PutChar(*str);
 str++;
 }
 }
 //=============================================================================
 // main function
 //=============================================================================
 int main(void)
 {
 vu32 dly;

RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_USART1EN;

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

USART1->CR1 = USART_CR1_UE | USART_CR1_TE;
 USART1->BRR = (SystemCoreClock / 115200);

while (1) 
   {
   USART_PutString(text);
   for(dly = 0; dly < 1000000; dly++);
   }
 }
 //=============================================================================
 // End of file
 //=============================================================================
