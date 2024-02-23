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
volatile int playing=0;
volatile int mode=1;//single board=1 two boards=2
volatile int human=1;//human=1 NPC=0
extern volatile uint16_t placeWall;
extern int player;
extern int *walls;

extern volatile int wallx ;//defined in myfunct.c
extern volatile int wally ;//defined in myfunct.c
volatile int dirWall ;
volatile int disable =0;
#define BOARD_SIZE 8
#define boxY 0x0109
#define box1X 10
volatile int down_0 = 0;
volatile int down_1 = 0;
volatile int down_2 = 0;
//defined in myfunct
extern volatile int selMode;
extern volatile int connect;

void RIT_IRQHandler (void)
{		
	//buttons
	if(down_0!=0){  
			down_0 ++;  
		if((LPC_GPIO2->FIOPIN & (1<<10)) == 0){

			switch(down_0){
			case 2:
				if (mode==2 && connect==1){
					enable_timer(1);
					send_handshake();
				}else{
				LCD_Clear(Blue2);
				gameMode();
				placeWall=0;}
				break;
			default:
				break;
		}
	}
	else {	/* button released */
		down_0=0;			
		NVIC_EnableIRQ(EINT0_IRQn);							 /* enable Button interrupts			*/
		LPC_PINCON->PINSEL4    |= (1 << 20);     /* External interrupt 0 pin selection */
	}

}	

	if(down_1!=0){  
			down_1 ++;  
		if((LPC_GPIO2->FIOPIN & (1<<11)) == 0){

			switch(down_1){
			case 2:
							if(disable){
						drawInitWall(disable);	 //delete the wall		
						LPC_SC->EXTINT &= (1 << 2);     /* clear pending interrupt         */  
						NVIC_DisableIRQ(EINT2_IRQn);		/* disable Button interrupts			 */
						disable=0;
						placeWall = 0;//return game mode
						redrawWalls();
						}	
				else{
				
					if (availableWall()) {
						// Draw a initial yellow wall 
						drawInitWall(disable);			
						placeWall = 1;
					disable=1;
					NVIC_EnableIRQ(EINT2_IRQn);
					}
					else
					{
					GUI_Text(box1X-7, boxY-35,(uint8_t *)"No walls available,move the   token", Black,player? Red:White);
						delay_s(1,box1X-7, boxY-35,"No walls available,move the   token"); 
				
					}
				}
				break;
	default:
				break;
}
	}
	else {	/* button released */
		down_1=0;			
		NVIC_EnableIRQ(EINT1_IRQn);							 /* enable Button interrupts			*/
		LPC_PINCON->PINSEL4    |= (1 << 22);     /* External interrupt 1 pin selection */
	}
	}
	
		if(down_2!=0){  
			down_2 ++;  
		if((LPC_GPIO2->FIOPIN & (1<<12)) == 0){
			switch(down_2){
			case 2:
				drawWall(wallx, wally, dirWall,Black);//delete previous wall
				dirWall++;
				dirWall=dirWall%2;
				moveWall(ROT); 
				break;
			default:
				break;
		}
	}
		else {	/* button released */
		down_2=0;			
		NVIC_EnableIRQ(EINT2_IRQn); 							 /* enable Button interrupts			*/
		LPC_PINCON->PINSEL4    |= (1 << 24);     /* External interrupt 2 pin selection */
	}
	}
	
	
//joystick
	if((LPC_GPIO1->FIOPIN & (1<<25)) == 0){	
		/* Joytick Select pressed p1.25*/
		select++;
		switch(select){
			case 1:
				if(playing){
					if(placeWall){
				moveWall(SEL);               
            
				}
				else
				{
					moveToken(SEL);
				}	
				}
				else
					changeMode(SEL);
								  			
				
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
				if(playing){
						if(placeWall){
				//move wall
					moveWall(L);
				}
				else //move token
				{
				moveToken(L);
				}
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
				if(playing){
						if(placeWall){
				//move wall
					moveWall(R);
				}
				else //move token
				{
				moveToken(R);
				}
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
				if(playing){
							if(placeWall){
				//move wall
					moveWall(D);
				}
				else //move token
				{
				moveToken(D);
				}
				
			}else
				changeMode(D);
		
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
					if(playing){
							if(placeWall){
				//move wall
					moveWall(U);
				}
				else //move token
				{
				moveToken(U);
				}
			}
					else
						changeMode(U);
		
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
