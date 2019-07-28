#include "stm32f10x.h"
 #include "jyotilib_gpio.h"
 #include "adc.h"
 //=============================================================================
 // Defines
 //=============================================================================
 #define LED_BLUE_GPIO GPIOC
 #define LED_BLUE_PIN 8 

#define AIN0_GPIO GPIOA
 #define AIN0_PIN 0 
 //=============================================================================
 // main function
 //=============================================================================
 int main(void)
 {
 RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_AFIOEN | RCC_APB2ENR_ADC1EN;
 RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

#if (LED_BLUE_PIN > 7)
 LED_BLUE_GPIO->CRH = (LED_BLUE_GPIO->CRH & CONFMASKH(LED_BLUE_PIN)) | GPIOPINCONFH(LED_BLUE_PIN, GPIOCONF(GPIO_MODE_OUTPUT2MHz, GPIO_CNF_AFIO_PUSHPULL));
 #else
 LED_BLUE_GPIO->CRL = (LED_BLUE_GPIO->CRL & CONFMASKL(LED_BLUE_PIN)) | GPIOPINCONFL(LED_BLUE_PIN, GPIOCONF(GPIO_MODE_OUTPUT2MHz, GPIO_CNF_AFIO_PUSHPULL));
 #endif

#if (AIN0_PIN > 7)
 AIN0_GPIO->CRH = (AIN0_GPIO->CRH & CONFMASKH(AIN0_PIN)) | GPIOPINCONFH(AIN0_PIN, GPIOCONF(GPIO_MODE_INPUT, GPIO_CNF_INPUT_ANALOG));
 #else
 AIN0_GPIO->CRL = (AIN0_GPIO->CRL & CONFMASKL(AIN0_PIN)) | GPIOPINCONFL(AIN0_PIN, GPIOCONF(GPIO_MODE_INPUT, GPIO_CNF_INPUT_ANALOG));
 #endif

AFIO->MAPR = AFIO_MAPR_TIM3_REMAP; // Full TIM3 remap

TIM3->PSC = 23; // Set prescaler to 24 (PSC + 1)
 TIM3->ARR = 4096; // Auto reload value 4096 (PWM resolution 12-bits)
 TIM3->CCR3 = 0; // Start value channel 3

TIM3->CCMR2 = TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1; // PWM mode on channel 3

TIM3->CCER = TIM_CCER_CC3E; // Enable compare on channel 3
 TIM3->CR1 = TIM_CR1_CEN; // Enable timer

ADC1->CR2 = ADC_CR2_ADON | ADC_CR2_CONT; // Turn on ADC, enable continuos mode
 ADC1->SQR1 = ADC_SEQUENCE_LENGTH(0); // One channel in sequence
 ADC1->SQR3 = ADC_SEQ1(0); // ADC channel 0 is first in sequence
 ADC1->SMPR2 = ADC_SAMPLE_TIME0(SAMPLE_TIME_239_5); // sample time for first channel

ADC1->CR1 = ADC_CR1_EOCIE; // Enable interrupt form End Of Conversion
 NVIC_EnableIRQ(ADC1_IRQn); // Enable interrupt form ACD1 peripheral

ADC1->CR2 |= ADC_CR2_ADON; // Turn on conversion

while (1) {}
 }
 //=============================================================================
 // ADC1 Interrupt handler
 //=============================================================================
 void ADC1_IRQHandler (void)
 {
 if(ADC1->SR & ADC_SR_EOC)
 {
 TIM3->CCR3 = ADC1->DR;
 }
 }
 
 //=============================================================================
 // End of file
 
