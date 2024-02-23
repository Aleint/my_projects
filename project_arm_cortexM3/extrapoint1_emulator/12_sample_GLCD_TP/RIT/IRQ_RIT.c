/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "lpc17xx.h"
#include "RIT.h"
#include "../GLCD/GLCD.h" 
#include "../myfunct/funct.h"

/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/




volatile int select=0;
volatile int left=0;
volatile int right=0;
volatile int up=0;
volatile int down=0;

extern volatile uint16_t placeWall;





void RIT_IRQHandler (void)
{					
	

	if((LPC_GPIO1->FIOPIN & (1<<25)) == 0){	
		/* Joytick Select pressed p1.25*/
		select++;
		switch(select){
			case 1:
					if(placeWall){
				moveWall(SEL);               
            
				}
				else
				{
					moveToken(SEL);
				}				  			
				
			break;
			default:
				break;
		}		
		
	}
	else{
	select=0;
	}
	
	
		if((LPC_GPIO1->FIOPIN & (1<<27)) == 0){	
		/* Joytick Left pressed p1.27*/
	left++;
		switch(left){
			case 1:
						if(placeWall){
				//move wall
					moveWall(L);
				}
				else //move token
				{
				moveToken(L);
				}
			break;			
			default:
				break;
		}		
		
		}
			else{
	left=0;
	}
			
		if((LPC_GPIO1->FIOPIN & (1<<28)) == 0){	
		/* Joytick Right pressed p1.28*/
			right++;
			switch(right){
			case 1:
						if(placeWall){
				//move wall
					moveWall(R);
				}
				else //move token
				{
				moveToken(R);
				}
			break;
			default:
				break;
		}			
		}	else{
		right=0;
		}
		
		
		if((LPC_GPIO1->FIOPIN & (1<<26)) == 0){	
		/* Joytick Down pressed p1.26*/		
			down++;
			switch(down){
			case 1:
							if(placeWall){
				//move wall
					moveWall(D);
				}
				else //move token
				{
				moveToken(D);
				}
		
			break;
			default:
				break;
		}		
		
		}	else{
		down=0;
		}
			
		if((LPC_GPIO1->FIOPIN & (1<<29)) == 0){	
		/* Joytick Up pressed p1.29*/
		up++;
			switch(up){
			case 1:
							if(placeWall){
				//move wall
					moveWall(U);
				}
				else //move token
				{
				moveToken(U);
				}
		
			break;
			default:
				break;
		}		
		
		}
		else{
		up=0;
		}
	/* button management */

			//NVIC_EnableIRQ(EINT1_IRQn);							 /* enable Button interrupts			*/
			//LPC_PINCON->PINSEL4    |= (1 << 22);     /* External interrupt 0 pin selection */
		
	
	
  LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
	
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
