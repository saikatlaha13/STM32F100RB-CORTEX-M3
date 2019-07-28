#include <stm32f10x.h>
#include <STRING.h>
#include "lcd.h"
#include <stdio.h>

 // GSM module connected to UART3 PB10(TX),PB11(RX)
 // PA10=RX receive PA9=TX transmit USART1
 // pin beside 3.3volt gps is TX(ttl) should be connected to PA10(UART1 RX)
 // LCD module D0-D7 (PA0-PA7)
 // EN->PC2 RW->PC1 RS->PC0 

void gsm_init();
unsigned char cmd_AT[] = "AT";
unsigned char cmd_CMGF[] = "AT+CMGF=1";
unsigned char cmd_CMGS[] = "AT+CMGS=";
unsigned char cmd_CNMI[] = "AT+CNMI=2,2,0,0,0";
unsigned char mob_num[]="9845615462";
unsigned char CTRLZ = 0x1A;
unsigned char ENTER =0x0D;	
unsigned char *str="$GPGGA,062516.000,1254.6273,N,07736.5107,E,1,3,3.28,238.5,M,-88.5,M,,*70\n";
unsigned char lat1[100];
unsigned char lon[100];
unsigned char total[100];
int flag_message=0;
void gpio_init_lcd();

unsigned char rxdat,rxdat1;
void send_msg(unsigned char *ptr4);
void  Uart1PutS(unsigned char *str)	;
void  Uart3PutS(unsigned char *str);
void Uart1PutCh (unsigned char ch) ;
void Uart3PutCh (unsigned char ch) ;
void Uart3PutS(unsigned char *str) ;
void send_msg(unsigned char *ptr4);
void Delay_Ms(unsigned int count);
unsigned char ch,lng[4]="LON:",lat[4]="LAT:",init[9]="0000.0000",init1[13]="LAT:0000.0000",init2[13]="LON:0000.0000";
unsigned int i=0,j,flag1,flag2;
char message[100]={0};
unsigned char *ptr[20];
int str_len,m=0,n=0,k=0;
char del=',';
char *delim=&del;
char buffer[30];
unsigned char *dec1[3];
unsigned char *dec2[3];
int p=0,q=0;
unsigned char delim2='.';
unsigned char *add=&delim2;
float lat_decimal_value, lat_degrees_value;
unsigned int lat_degrees;
float long_decimal_value, long_degrees_value;
unsigned int long_degrees;
char lat_buf[100];
char lon_buf[100];
char message2[100];
int flag3=0,flag4=0,flag5=0;
void recv_msg_cmd();
int flag=0;

void USART3_IRQHandler(void)
{
  static int count;
  rxdat = USART3->DR;

	if(rxdat=='t' && flag==0)
	{
		Uart1PutS("hello");
		gsm_init();
	  send_msg(total);
		flag=1;
	}	
}

double convert_to_degrees(float NMEA_lat_long)
{	
	double minutes, dec_deg, decimal;
	int degrees;
	double position;

	degrees = (int)(NMEA_lat_long/100.00);
	minutes = NMEA_lat_long - degrees*100.00;
	dec_deg = minutes / 60.00;
	decimal = degrees + dec_deg;
	return decimal;
}

void USART1_IRQHandler(void)
{        	 
	static int count;
  rxdat1 = USART1->DR;
	if(rxdat1=='$')
	{
			flag1=1;
			count=0;
	}
	if(flag1==1)
	{
			message[count++]=rxdat1;
			if(rxdat1=='*')
			{
				count++;
				flag1=0;
				flag2=1;
				message[count]='\0';

				if(strncmp(message,"$GPGGA",6)==0)
				{
				    ptr[m]=strtok(message,delim);
				    while(ptr[m] != NULL)
				    {
					     m++;
					     ptr[m]=strtok(NULL,delim);
				    }
						
						lat_decimal_value = atol(ptr[2]);	/* Latitude in ddmm.mmmm */						
						lat_degrees_value=convert_to_degrees(lat_decimal_value);						
						sprintf(lat_buf, "%f", lat_degrees_value);
						
						LcdGoToXY(0,0);
						LcdString("LAT:");
		        LcdString(lat_buf);
						LcdString(ptr[3]);
							
						long_decimal_value = atol(ptr[4]);	/* Longitude in dddmm.mmmm */
						long_degrees_value=convert_to_degrees(long_decimal_value);
						sprintf(lon_buf, "%f", long_degrees_value);
						
						LcdGoToXY(1,0);
						LcdString("LON:");
		        LcdString(lon_buf);
						LcdString(ptr[5]);
						
						strcat(total,"LAT:");
						strcat(total,lat_buf);
						strcat(total,ptr[3]);
						strcat(total,"LON:");
						strcat(total,lon_buf);
						strcat(total,ptr[5]);	
						
						gsm_init();
						recv_msg_cmd();
			  }
		  }
		}
}
	
void Init_UART3(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN  | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_IOPBEN;
	RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
	RCC->APB2ENR |=  RCC_APB2ENR_AFIOEN ;
	GPIOB->CRH    = 0x00008b00;                                       // PA3=RX receive PA2=TX transmit
	USART3->CR1  |= USART_CR1_UE ;                      // USART2 enable and TE transmitter enable
	USART3->CR2  &= ~USART_CR2_STOP_0 | ~USART_CR2_STOP_1;
	USART3->CR1  &= ~USART_CR1_M;
	USART3->BRR   = (SystemCoreClock / 9600);                         // baud rate
	USART3->CR1  |= USART_CR1_RE | USART_CR1_TE;
	USART3->CR1  |= USART_CR1_RXNEIE;
	NVIC_EnableIRQ(USART3_IRQn);
}

void Init_UART1(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN  | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_IOPBEN;
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN | RCC_APB2ENR_AFIOEN ;
	GPIOA->CRH    = 0x000008b0;                                          // PA9=transmit PA10=receive
	USART1->CR1  |= USART_CR1_UE;                                        // usart1 enable and receiver enable
  USART1->CR2  &= ~USART_CR2_STOP_0 | ~USART_CR2_STOP_1;
	USART1->CR1  &= ~USART_CR1_M;
	USART1->BRR   = (SystemCoreClock / 9600);                           // usart1 baud rate register setting	
	USART1->CR1  |= USART_CR1_RE | USART_CR1_TE ;
	USART1->CR1  |= USART_CR1_RXNEIE;
	NVIC_EnableIRQ(USART1_IRQn);
}

void Uart3PutCh (unsigned char ch)   // Write character to Serial Port   
{                  		
    while(!(USART3->SR & USART_SR_TXE));             // check for usart2 transmit register empty
    USART3->DR = ch;
}

void  Uart3PutS(unsigned char *str)	 //A function to send a string on UART0
{  
   while(*str)
   {  
      Uart3PutCh(*str++);	    
   }
}

void Uart1PutCh (unsigned char ch)   // Write character to Serial Port   
{                  		
    while(!(USART1->SR & USART_SR_TXE));
	  USART1->DR=ch;
}

void  Uart1PutS(unsigned char *str)	 //A function to send a string on UART1
{  
   while(*str)
   {  
      Uart1PutCh(*str++);	    
   }
}

void Delay_Ms(unsigned int count)
{
    volatile unsigned int j,k;
    for (j=0;j<count;j++)
	  {
        for (k=0;k<6000;k++)
		    {
            __asm
			      {
                nop;
                nop;
            }
        }
    }
}

void AT_cmd(unsigned char *ptr3)
{	 						  	 
	 while(*ptr3!='\0')
	 {
	 	Uart3PutCh (*ptr3++);
	 }	
	 return;
}

void gsm_init()
{	
	AT_cmd(cmd_AT);
	Uart3PutCh (ENTER);
	Delay_Ms(500);
	AT_cmd(cmd_CMGF);
	Uart3PutCh (ENTER);
	Delay_Ms(500);
	
	return;
}

void send_msg(unsigned char *ptr4)
{
	 AT_cmd(cmd_CMGS); 
	 Uart3PutCh (0x22);
	 AT_cmd(mob_num);
	 Uart3PutCh (0x22);
	 Uart3PutCh (ENTER);
	 Delay_Ms(500);		  	 
	 while(*ptr4!='\0')
	 {
	 	Uart3PutCh (*ptr4++);
	 }
	 Uart3PutCh(CTRLZ);

   return;	 
}

void recv_msg_cmd()
{	 		
	AT_cmd(cmd_CNMI); 
	Uart3PutCh (ENTER);
	
	return;
}

int main(void)
{
    Init_UART3();	
		Delay_Ms(100);	
		Init_UART1();
    Delay_Ms(100);
	  gpio_init_lcd();
	  LcdInit();
	  LcdClr();
	  while(1)
	  {
	  }
}

void gpio_init_lcd()
{
	GPIOA->CRL=0x22222222;
	GPIOC->CRL=0x00000222;
}

