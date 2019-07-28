/*----------------------------------------------------------------------------
 * Name:    Timer.c
 * Purpose: Timer usage for STM32
 * Version: V1.00
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * Copyright (c) 2005-2007 Keil Software. All rights reserved.
 *----------------------------------------------------------------------------*/

#include <stm32f10x_lib.h>                        // STM32F10x Library Definitions
#include "STM32_Init.h"                           // STM32 Initialization


unsigned int ledPosSec   = 0;                     // led position for second
unsigned int ledPosAlarm = 2;                     // led position for Alarm
unsigned int ledRtcSec   = 0;

/*------------------------------------------------------------------------------
  RTC Interrupt Handler
 *------------------------------------------------------------------------------*/
void RTC_IRQHandler(void)
{
  if (RTC->CRL & (1<<0) ) {                       // check second flag
    RTC->CRL &= ~(1<<0);                          // clear second flag
    if ((ledRtcSec ^=1) == 0) 
      GPIOB->BSRR = 1 << (ledPosSec+8);           // Second LED on
    else
      GPIOB->BRR  = 1 << (ledPosSec+8);           // Second LED off
    }

  if (RTC->CRL & (1<<1) ) {                       // check alarm flag
    RTC->CRL &= ~(1<<1);                          // clear alarm flag
    GPIOB->BSRR = 1 << (ledPosAlarm+8);           // Alarm LED on
    }

} // end RTC_IRQHandler


/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {

  stm32_Init ();                                  // STM32 setup

  while (1) {                                     // Loop forever
    ;
  } // end while
} // end main
