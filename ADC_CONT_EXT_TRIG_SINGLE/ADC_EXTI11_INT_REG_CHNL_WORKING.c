//A-G = PC->8 = PC->15
//SEG1 - SEG4 = SEG1->PC9 , SEG2->PC8 , SEG3->PC7 , SEG3->PC6
//POTENTIOMETER INPUT=PA0 
 
 // TO TRIGGER ADC CONVERSION THROUGH EXTI11 SO PA11 IS CHOSEN AS THE EXTERNAL TRIGGER SOURCE FOR CONVERSION
 
 #include "stm32f10x.h"
 #include "jyotilib_gpio.h"
 #include "adc.h"
 //=============================================================================
 // Defines
 //=============================================================================
 
 #define segment1 1<<6;
 #define segment2 1<<7;
 #define segment3 1<<8;
 #define segment4 1<<9;
 
 void display(unsigned int i);
 void view(unsigned int );
 unsigned int b[16]={ 0x3F, 0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};
 
 //=============================================================================
 // main function
 //=============================================================================
 int main(void)
 {
	 uint32_t i,CH11;
	 
   RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN |  RCC_APB2ENR_ADC1EN | RCC_APB2ENR_IOPBEN  ;

   GPIOB->CRH=0x22222222;// 7 SEG DATA 
	 GPIOC->CRL=0X22000000;// ALTERNATE ANALOG INPUT PC1 CH11 + SEG SEL
	 GPIOC->CRH=0X00000022;// SEG SEL         
	 //GPIOA->CRL=0X00000000;// PORT A PIN 0 IS ANALOG INPUT....OO(MODE).00(CONF
	 GPIOA->CRH=0X00008000;// PA11 AS SOURCE FOR EXTERNAL TRIGGER

   //ADC1->CR2 = ADC_CR2_ADON | ADC_CR2_CONT; // Turn on ADC, enable continuos mode
	 ADC1->CR2 |= ADC_CR2_CAL; // CALLIBRATION OF ADC CAPACITORS
	 
	 AFIO->EXTICR[2] &= 0X0FFF;     // PA11 IS CHOSEN AS EXTERNAL TRIGGER SOURCE FOR ADC CONVERSION
	 
	  EXTI->IMR       |= (1 << 11);              // unmask interrupt
    EXTI->EMR       |= (1 << 11);             // unmask event
    EXTI->RTSR      |= (1 << 11);           // set rising edge
    EXTI->FTSR      |= (1 << 11);        // set falling edge O=ONE TIME,1=TWO TIME
	
	 ADC1->CR2 |= ADC_CR2_EXTTRIG; // SELECTING EXTERNAL TRIGGER FOR REGULAR CHANNELS
	 ADC1->CR2 |= (~ADC_CR2_EXTSEL_0 | ADC_CR2_EXTSEL_1 | ADC_CR2_EXTSEL_2); // SELECTING TIMER 1 CC1 EVENT
	 
	 //PROVIDING THE NUMBER OF CHANNELS
   ADC1->SQR1 &= ~ADC_SQR1_L_3 | ~ADC_SQR1_L_2 | ~ADC_SQR1_L_1 | ~ADC_SQR1_L_0; 
	 
	 // SETTING THE SEQUENCE
	 ADC1->SQR3 |= ADC_SQR3_SQ1_0| ADC_SQR3_SQ1_1 | ADC_SQR3_SQ1_3; //CH11 , SEQUENCE 1
	 
	 // SETTING THE SAMPLE TIME
   ADC1->SMPR1 |= ADC_SAMPLE_TIME11(SAMPLE_TIME_239_5); // sample time for channel 11 , PC1  

   //ADC1->CR2 |= ADC_CR2_ADON; // Turn on conversion
	// NVIC_SetPriority(11,0);
	  //NVIC_EnableIRQ(EXTI0_IRQn);

   while (1) {
	if((ADC1->SR & ADC_SR_EOC))
	{
		 display(ADC1->DR & 0X00000FFF);
  }
 }
 }
 
 void display(unsigned int num)
 {
	  uint32_t m;
	  unsigned int ones,tens,hundred,thousand;
	 
	  ones= num % 10;
	  num = num / 10;
	 
	  tens= num % 10;
	  num = num /10;
	 
	  hundred=num%10;
	  num = num/10;
	
	  thousand=num%10;
	
	  GPIOC->ODR=segment1;
	  view(ones);
	  for(m=0 ; m<10000 ; m++);
	 
	  GPIOC->ODR=segment2;
	  view(tens);
	  for(m=0 ; m<10000 ; m++);
	 
	  GPIOC->ODR=segment3;
	  view(hundred);
	  for(m=0 ; m<10000 ; m++);
	 
	  GPIOC->ODR=segment4;
	  view(thousand);
	  for(m=0 ; m<10000 ; m++);
 }
 
 void view(unsigned int i)
 {
	  volatile uint32_t k;
	  GPIOB->ODR=(b[i]<<8);
	  for(k=0 ; k<10000 ; k++);
 }
  
 //=============================================================================
 // End of file
 
