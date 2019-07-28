#include "stm32f10x.h"
#include "jyotilib_gpio.h"
#include <STRING.h>
#include <stdarg.h>

unsigned char a[10]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};  

void Init( ) 
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN;     // enabling clock for PORTB
  RCC->APB1ENR |= RCC_APB1ENR_I2C1EN ;                         // enabling I2c Clock
	
	// alternate function remapping
  AFIO->MAPR &= ~AFIO_MAPR_I2C1_REMAP;                         // pb6=SCL  Pb7=SDA
	GPIOB->CRL = 0xFF000000;                                     // enabling alternate function on pins with max speed  MHZ
	I2C1->CR1 &= ~I2C_CR1_SMBUS;                                 // setting the bus ti I2C mode
  I2C1->OAR1 &= ~I2C_OAR1_ADDMODE;                             // 7 bit addressing mode
	I2C1->CCR |= 1<<14;                                          // I2C_CCR Duty cycle mode 2
  I2C1->OAR1 |= 1<<14;                                         // I2C_AcknowledgedAddress_7bit	

	//Configure the I2C
	I2C1->CR2     = 0x0002;                                      //2 MHz HSI peripheral clock
	I2C1->CCR     = 0x0050;                                      //100kHz Bit rate 
	I2C1->TRISE   = 0x0009;                                      //1000 ns / 62.5 ns = 16 + 1
	I2C1->CR1 |= I2C_CR1_ACK | I2C_CR1_PE ;                      // enable acknowledgement and I2c peripheral
}

unsigned int operation(unsigned char value)
{
  unsigned char mask,extracted_value,final_value;
  mask=(~0<<3);
  extracted_value=value & mask;
  value=value ^ extracted_value;
  final_value=value | (extracted_value<<1);
  
  return final_value;
}

void I2C_Start() 
{
  I2C1->CR1 |= I2C_CR1_START;                                  // send start condition to slave
    while(!(I2C1->SR1 & I2C_SR1_SB));                          // check that start bit has been set or not  

}//subsequent access of data register will clear start bit

void I2C_Stop (void) 
{
  I2C1->CR1 |= I2C_CR1_STOP;
  while (I2C1->SR2 & I2C_SR2_BUSY);   //wait for BUSY set  
};

void I2C_Write (unsigned char c) 
{	
	I2C1->DR = c;
  while(!(I2C1->SR1 & I2C_SR1_TXE));                            // Wait TxE bit set - <A HREF="#linkSR">see status register</A>
	  
 };

unsigned char I2C_Read (int ack) 
{
	  if (ack) 
		{	
		  I2C1->CR1 |=  I2C_CR1_POS;	                              //multiple bytes - set acknowledge bit in<A HREF="#linkcontrol"> I2C_CR1</A>
  	}
	  else
		{			
			I2C1->CR1 &= ~I2C_CR1_POS;	//single or last byte clear ack bit
		}
  	while (!(I2C1->SR1 & I2C_SR1_RXNE));    // Wait until RxNE bit set - <A HREF="#linkSR">see status register</A>
	
	return (I2C1->DR);
}

void I2C_Address (unsigned char adr) 
{
  char res;
	
	I2C1->DR = 0X4c | 0;    			                //Write to I2C Address register
  while(!(I2C1->SR1 & I2C_SR1_ADDR));           // wait till address matched and acknowlegement received  
	res = (I2C1->SR2);                            //dummy read to clear 

}
	
void Delay(int m) 
{
	int i,j;
	for (i=0; i<m; i++)
	{
		for(j=0 ; j<45000 ; j++);
	}
}

int main () 
{
	int i;
	char res;
	Init();
	i=0;
	I2C_Start();
  I2C_Address(0x4C);
	
	while (1)
	{
		   Delay(18);
		
			 I2C_Write(operation(a[i]));
		   i++;
		
       //res = I2C_Read(1);
			 Delay(18);					
			 if(i==10)
			 {
						i=0;
			 }
			 	
	}
	I2C_Stop();
	Delay(10);	
}
