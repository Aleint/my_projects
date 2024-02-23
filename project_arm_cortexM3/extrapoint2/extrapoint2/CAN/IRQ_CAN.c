/*----------------------------------------------------------------------------
 * Name:    Can.c
 * Purpose: CAN interface for for LPC17xx with MCB1700
 * Note(s): see also http://www.port.de/engl/canprod/sv_req_form.html
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2009 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#include <lpc17xx.h>                  /* LPC17xx definitions */
#include "CAN.h"                      /* LPC17xx CAN adaption layer */
#include "../GLCD/GLCD.h"
#include "../myfunct/funct.h"
#include "../timer/timer.h"
extern uint8_t icr ; 										//icr and result must be global in order to work with both real and simulated landtiger.
extern uint32_t result;
extern CAN_msg       CAN_TxMsg;    /* CAN message for sending */
extern CAN_msg       CAN_RxMsg;    /* CAN message for receiving */                                


volatile int handshake = 0; 
extern volatile int connect;        
extern volatile int playing;
/*----------------------------------------------------------------------------
  CAN interrupt handler
 *----------------------------------------------------------------------------*/
void CAN_IRQHandler (void)  {

  /* check CAN controller 1 */
	icr = 0;
  icr = (LPC_CAN1->ICR | icr) & 0xFF;               /* clear interrupts */
	
  if (icr & (1 << 0)) {                          		/* CAN Controller #1 meassage is received */
		CAN_rdMsg (1, &CAN_RxMsg);	                		/* Read the message */
    LPC_CAN1->CMR = (1 << 2);                    		/* Release receive buffer */
		
		handshake = (CAN_RxMsg.data[0])  ;
		if(handshake== 0xff){
		send_response();
		}else
		if(handshake== 0xf1){//response ok
			disable_timer(1);
			connect=0;
			LCD_Clear(Blue2);
			playing=1;
			drawBoardAndBoxes();
			NVIC_EnableIRQ(EINT1_IRQn);
			
		}
	
  }
	else
		if (icr & (1 << 1)) {                         /* CAN Controller #1 meassage is transmitted */
			// do nothing in this example
			
		}
		
	
}
