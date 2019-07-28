#include<stm32f10x.h>

void delayMs(int);

int main()
{
	RCC->APB2ENR= RCC_APB2ENR_IOPAEN;
	GPIOA->CRH = 0x00002222;
	GPIOA->ODR = 0x00000000;
	while(1)
	{		
    GPIOA->BSRR=(1<<9|1<<10);
    delayMs(200);
    GPIOA->BRR=(1<<9|1<<10);
		
    GPIOA->BSRR=(1<<10|1<<11);
    delayMs(200);
    GPIOA->BRR=(1<<10|1<<11);
			
    GPIOA->BSRR=(1<<8|1<<11);
    delayMs(200);
    GPIOA->BRR=(1<<8|1<<11);
				
    GPIOA->BSRR=(1<<8|1<<9);
    delayMs(200);
    GPIOA->BRR=(1<<8|1<<9);
	}
}

void delayMs(int c)
{
	int j,k;
	for(j=0;j<c;j++)
	{
		for(k=0;k<6000;k++)
		{
			
			int a;
			a=0;
		}
	}
}