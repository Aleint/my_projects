/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    timer.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include <string.h>
#include "lpc17xx.h"
#include "timer.h"
#include "../GLCD/GLCD.h" 
#include "../TouchPanel/TouchPanel.h"
#include "../myfunct/funct.h"

/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
#define boxY 0x0109
#define box2X  85

volatile uint16_t player=0;

void TIMER0_IRQHandler (void)
{
	extern volatile int endTurn;
	static int time_turn = 20;
	static int count = 0;
	char time_in_char[5] = "";
					
				count++;
				if(count%10==0){//ogni 10*10^-2 s
					time_turn --;
					sprintf(time_in_char,"%4d",time_turn);
					GUI_Text(box2X + 18, boxY + 30, (uint8_t *) time_in_char, Black, Blue2);
					}
					if(count == 200 || endTurn){	/* 200* 0.01 =2 s  seconds */		
						
						time_turn = 20;

						if(!endTurn){
							GUI_Text(box2X + 5, boxY + 30,(uint8_t *)"timeout", Black, Blue2);
								
							//end turn and delete all unconfirmed actions	
							restore();
							GUI_Text(box2X + 5, boxY + 30,(uint8_t *)"timeout", Blue2, Blue2);
						}						
						
						
						if(endTurn){						
					
							endTurn=0;
							
						}										
						
						player++;
						player=player%2;
            displayMoves(0);
						sprintf(time_in_char,"%4d",time_turn);
						
						GUI_Text(box2X + 5, boxY + 5, (uint8_t*)"Time",Black,player ?Red: White);
						GUI_Text(box2X + 18, boxY + 30, (uint8_t *) time_in_char, Black, Blue2);
						count=0;
						
					}
	
  LPC_TIM0->IR = 1;			/* clear interrupt flag */
  return;

}

/******************************************************************************
** Function name:		Timer1_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TIMER1_IRQHandler (void)
{
  LPC_TIM1->IR = 1;			/* clear interrupt flag */
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
