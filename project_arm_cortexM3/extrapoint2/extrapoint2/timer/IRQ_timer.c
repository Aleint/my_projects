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
extern volatile int human;

void TIMER0_IRQHandler (void)
{
	extern volatile int endTurn;
	static int time_turn = 20;
	static int count = 0;
	char time_in_char[5] = "";
					
				count++;
				if(count%100==0){//ogni 100*10^-2 s
					time_turn --;
					sprintf(time_in_char,"%4d",time_turn);
					GUI_Text(box2X + 18, boxY + 30, (uint8_t *) time_in_char, Black, Blue2);
					}
					if(count == 2000 || endTurn){	/* 2000* 0.01 =20 s  seconds */		
						
						time_turn = 20;

						if(!endTurn){
							GUI_Text(box2X + 5, boxY + 30,(uint8_t *)"timeout", Black, Blue2);
								
							//end turn and delete all unconfirmed actions	
							restore();
							
								delay_s(1,box2X + 5, boxY + 30,(uint8_t *)"timeout");
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
						if(player==1 && human==0){
						NPC();
						}
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
	extern volatile int delay;
	extern volatile int stringx;
	extern volatile int stringy;
	extern volatile int strLen;
	static int count = 0;
	int i;
	extern volatile int connect;
	extern volatile int playing;
	count++;
	
	if(connect){
		if(count%500==0){// 50*10^-2 s			
			connect=1;
			connectBoards();}			
			
	}else{
				if(count%100==0){// 100*10^-1 s
					delay --;
					if(delay==0){
						for(i=0;i<strLen*8;i+=8){//i=E8
							if(i<=0xE8){
						GUI_Text(stringx+i, stringy,(uint8_t *)" ", Blue2, Blue2);
							}else{
							GUI_Text((i)%0xE8, stringy+16,(uint8_t *)" ", Blue2, Blue2);
							}
						}
						count =0;
						disable_timer(1);
					}
						
				}
			}
	
	
  LPC_TIM1->IR = 1;			/* clear interrupt flag */
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
