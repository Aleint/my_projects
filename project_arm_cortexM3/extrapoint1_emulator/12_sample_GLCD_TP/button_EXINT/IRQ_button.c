#include "button.h"
#include "lpc17xx.h"
#include "../TouchPanel/TouchPanel.h"
#include "../GLCD/GLCD.h"
#include "../myfunct/funct.h"

extern int player;
extern int *walls;
volatile int placeWall; 
extern volatile int wallx ;//defined in myfunct.c
extern volatile int wally ;//defined in myfunct.c
volatile int dirWall ;
volatile int disable =0;
#define BOARD_SIZE 8
#define boxY 0x0109
#define box1X 10

void EINT0_IRQHandler (void)	  	/* INT0														 */
{		
	LCD_Clear(Blue2);
	drawBoardAndBoxes();
	placeWall=0;
	LPC_SC->EXTINT &= (1 << 0);     /* clear pending interrupt         */
	NVIC_EnableIRQ(EINT1_IRQn);
	NVIC_DisableIRQ(EINT0_IRQn);
}


void EINT1_IRQHandler (void)	  	/* KEY1														 */
{
	
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
		GUI_Text(box1X-7, boxY-35,(uint8_t *)"No walls available,move the   token", Blue2, Blue2);
		}
	}
	

	LPC_SC->EXTINT &= (1 << 1);     /* clear pending interrupt         */
}

void EINT2_IRQHandler (void)	  	/* KEY2														 */
{
	drawWall(wallx, wally, dirWall,Black);//delete previous wall
	dirWall++;
	dirWall=dirWall%2;
	moveWall(ROT); 

  LPC_SC->EXTINT &= (1 << 2);     /* clear pending interrupt         */    
}


