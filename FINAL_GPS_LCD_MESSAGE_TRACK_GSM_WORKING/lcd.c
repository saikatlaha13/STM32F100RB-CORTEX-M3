#include "stm32f10x.h"
#include "jyotilib_gpio.h"
#include "lcd.h"




/**********************************************************/
/*					DEFINATION FOR LCD CONTROL PINS		  */
/*					RS=P0.19,RW=P0.18, EN=P0.17		   	  */
/*					LcdDataPort=P1.20-P1.23	to D4...D7	  */
/**********************************************************/
#define RS (1<<0)// PORT C
#define RW (1<<1)// PORT C
#define EN (1<<2)// PORT C
#define LCDMaxLines 2
#define LCDMaxChars 16
#define LineOne 0x80
#define LineTwo 0xC0
#define BlankSpace ' '
#define LcdDataPort GPIOA

int const count=900;

/**********************************************************/
/*		LCD COMMAND FUNCTION						      */
/**********************************************************/

void LcdCmd8Bit(unsigned char Cmd)
{
	volatile uint32_t dly;
	
	GPIOA->ODR = Cmd ;		//Send the commands to the LCD. #define LcdDataPort IOPIN1
	GPIOC->BRR=RS;						    //Set as Command or Instruction register to send the Command to the LCD.
	GPIOC->BRR=RW;							//Clear the Read/write=0.
	GPIOC->BSRR=EN;							//Send High to Low Pulse to the LCD to Latch the Data From
	for(dly = 0; dly < count; dly++);					//Controller to the LCD data pins.
	GPIOC->BRR=EN;
	//Lcd_Delay_ms(10);				   //wait for few millisec

}



/**********************************************************/
/*		LCD CHAR FUNCTION								  */
/**********************************************************/


void LcdChar8Bit(unsigned char Chr)
{
	volatile uint32_t dly;
	GPIOA->ODR=Chr;				//Send the commands to the LCD.
	GPIOC->BSRR=RS;							//Set as Data register to send the Data onto the LCD.
	GPIOC->BRR=RW;							//Clear the Read/write=0.
	GPIOC->BSRR=EN;							//Send High to Low Pulse to the LCD to Latch the Data From
	for(dly = 0; dly < count; dly++);				    //Controller to the LCD data pins.
	GPIOC->BRR=EN;

	
}
void LcdInit(void)
{
	volatile uint32_t dly;
	LcdCmd8Bit(0x03);    //cursor to home
		for(dly = 0; dly < count; dly++);	
	LcdCmd8Bit(0x02);
	for(dly = 0; dly < count; dly++);	
	LcdCmd8Bit(0x38);					   //Initialize the LCD by sending the Command 0x28
		for(dly = 0; dly < count; dly++);	
	LcdCmd8Bit(0x0C);					   //Dipslay ON and Cursor OFF
		for(dly = 0; dly < count; dly++);	
	LcdCmd8Bit(0x06);			//entry mode
		for(dly = 0; dly < count; dly++);	
	LcdCmd8Bit(0x01);					   //Clear The Display
		for(dly = 0; dly < count; dly++);	
}






/**********************************************************/
/*				LCD CLEAR FUNCTION						  */
/**********************************************************/

void LcdClr(void)						//LCD Clear Function
{
	LcdCmd8Bit(0x01);
	LcdCmd8Bit(LineOne);
}

void LcdClr_8bit(void)						//LCD Clear Function
{
	volatile uint32_t dly;
	LcdCmd8Bit(0x38);					   //Initialize the LCD by sending the Command 0x28
	for(dly = 0; dly < count; dly++);
	LcdCmd8Bit(0x01);
	LcdCmd8Bit(LineOne);
}



/**********************************************************/
/*			LCD LINE1 FUNCTION							  */
/**********************************************************/

void LcdGoToLine1(void)				   //Fucntion to move the cursor to first line
{
	LcdCmd8Bit(LineOne);
}

/**********************************************************/
/*			LCD LINE2 FUNCTION							  */
/**********************************************************/

void LcdGoToLine2(void)				   	//Fucntion to move the cursor to first line
{
	LcdCmd8Bit(LineTwo);
}







/**********************************************************/
/*			LCD STRING FUNCTION							  */
/**********************************************************/
void LcdString(unsigned char *Str)		// Function to Display the String
{
	while(*Str!='\0')
	{
		LcdChar8Bit(*Str++);
	}
}




/**********************************************************/
/*			LCD GOTO XY FUNCTION						  */
/**********************************************************/
void LcdGoToXY(unsigned char row,unsigned char col)	  // Function to set the cursor postion
{													  // according to user define by filling row and column
	char pos;
	if(row<LCDMaxLines)
	{
		pos=LineOne|(row<<6);
		if(col<LCDMaxChars)
		{
			pos=pos+col;
		}
		LcdCmd8Bit(pos);
	}
}

void LcdStr(unsigned char *Str)		// Function to Display the String
{
	while(*Str!='\0')
	{
		LcdChar8Bit(*Str++);
	}
}

void LcdScrollMsg_8bit(unsigned char *msg_ptr)			// Function to Scroll th display onto the LCD
{
	unsigned char i,j;
	//LcdCmd4Bit(0x0c);		//address of second line
	for(i=0;msg_ptr[i];i++)
	{
		//LcdCmd8Bit(0xC0);
		for(j=0;j<LCDMaxChars && msg_ptr[i+j];j++)
		{
			LcdChar8Bit(msg_ptr[i+j]);
		}
		for(j=j;j<LCDMaxChars;j++)
		{
			LcdChar8Bit(BlankSpace);
		}
		//Lcd_Delay_ms(300);
	}
}























