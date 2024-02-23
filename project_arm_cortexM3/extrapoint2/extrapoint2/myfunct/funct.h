#ifndef __MYFUNCT_H
#define __MYFUNCT_H
#include "stdint.h"

void initBoard(void);
int availableWall(void);
void updateBoard(int y,int x,int value);
enum cmdJoy {SEL ,L,R,U,D,ROT};
void moveToken(enum cmdJoy cmd);
int moveWall(enum cmdJoy cmd );
void restore(void);
void redrawWalls(void);
void checkWin(void);
int isPathPossible(int p);
int canMoveToDestination(int x, int y, int destinationY);
int isWithinBounds(int x, int y);
void drawInitWall(int disable);
void redrawTokens(void);
void displayMoves(int update);
void delay_s(int s,int strx,int stry,char *str );
void resetOver(void);
void changeMode(enum cmdJoy cmd );
void NPC(void);
void connectBoards(void);
void send_handshake(void);
void send_response(void);
#endif
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
