/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            The GLCD application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-11-7
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             Paolo Bernardi
** Modified date:           03/01/2020
** Version:                 v2.0
** Descriptions:            basic program for LCD and Touch Panel teaching
**
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "LPC17xx.h"
#include "GLCD/GLCD.h" 
#include "TouchPanel/TouchPanel.h"
#include "timer/timer.h"
#include "button_EXINT/button.h"
#include "RIT/RIT.h"
#include "joystick/joystick.h"
#include "myfunct/funct.h"

#define SIMULATOR 1

#ifdef SIMULATOR
extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif




int main(void)
{		
	
  SystemInit();  												/* System Initialization (i.e., PLL)  */
	
  LCD_Initialization();
	BUTTON_init();												/* BUTTON Initialization              */
	NVIC_DisableIRQ(EINT1_IRQn);		/* disable Button interrupts			 */
  NVIC_DisableIRQ(EINT2_IRQn);		/* disable Button interrupts			 */
	//TP_Init();
	//TouchPanel_Calibrate();
	
	LCD_Clear(Blue2);	
	initBoard();//inizializzazione matrice
	
	initGame((uint8_t*)"Quoridor", Black, Blue2);
	joystick_init();							/* Joystick Initialization  */

	
	
	init_timer(0,0x3D090);               /*1 * 10^-2* s*/

	init_RIT(0x004C4B40);									/* RIT Initialization 50 msec * 100 MHz (il Rit usa il timer del processore, quindi 100 MHz)	*/  //IL RIT SI OCCUPERA DEL JOYSTICK (50 ms)
	
		enable_RIT();													/* RIT enabled			*/

	
	LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	LPC_SC->PCON &= ~(0x2);						
	

	
	
  while (1)	
  {
		__ASM("wfi");
  }
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
