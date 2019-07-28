#include <stm32f10x.h> 

//void  SystemInit ( ) { };
//CH3=PC8 CH4=PC9 PC8 and PC9 are the output

void  GPIO_Init()
{
	  RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;       //clock for GPIOC
   	GPIOC->CRH   &= ~0xff;  
   	GPIOC->CRH   |= 0xBB;                     //PC8,PC9 AF push pull   
} 
void InitPWM( ) 
{
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;       //AF clock - see sequence following figure 196
  	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;       //TIM3 clock enable
    AFIO->MAPR   |= AFIO_MAPR_TIM3_REMAP_0 | AFIO_MAPR_TIM3_REMAP_1;  // full remap=11 PC8 PC9

   	TIM3->CR1 = 0x0000;                       //default
	  TIM3->PSC = 0x0001;                       //set pre-scaler
	
	  // changing the values of auto reload register will change the result
	  //TIM3->ARR = 0xFF;                       //set reload count
	  TIM3->ARR=0xffff;                         // counter counts till auto reload value
	  TIM3->CCMR2 = 0x6868;                     //Set PWM mode 1
	  
	  // change the below register values and the intensity of PC8 led light changes
	  TIM3->CCR3=0x02ff;
	  // change the below regitsre value and the intensity of PC9 changes
	  TIM3->CCR4 = 0x1f4F;
	
	  TIM3->CCER = TIM_CCER_CC3E|TIM_CCER_CC4E; //Enable ch 3 and ch 4 output
	  TIM3->CR1 |= TIM_CR1_CEN;                 //enable timer
}
int main ( ) 
{
	  InitPWM( );
    GPIO_Init();                   
	  while(1)
    {      	
	  }
}	
