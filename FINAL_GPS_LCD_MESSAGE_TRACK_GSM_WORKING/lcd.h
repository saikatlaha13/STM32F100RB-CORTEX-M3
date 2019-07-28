#ifndef LCD_LPC2129_H
#define LCD_LPC2129_H

void LcdInit(void);
void LcdClr(void);	
void LcdGoToLine1(void);	
void LcdGoToLine2(void);	 
void LcdString(unsigned char *Str);
void LcdScrollMsg(unsigned char *msg_ptr);
void LcdDisplayNumber(unsigned int num);
void LcdGoToXY(unsigned char row,unsigned char col);
void LcdDispRtcTime(unsigned hour, unsigned char min, unsigned char sec);
void LcdDispRtcDate(unsigned day, unsigned char month, unsigned char year);
void LcdCmd8Bit(unsigned char Cmd);
void LcdInit_8bit(void);
void LcdClr_8bit(void);
void LcdChar8Bit(unsigned char Chr);
void LcdScrollMsg_8bit(unsigned char *msg_ptr);

#endif


