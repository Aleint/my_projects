#ifndef __MYFUNCT_H
#define __MYFUNCT_H


void initBoard(void);

int availableWall(void);
void updateBoard(int y,int x,int value);
enum cmdJoy {SEL ,L,R,U,D,ROT};
void moveToken(enum cmdJoy cmd);
void moveWall(enum cmdJoy cmd );
void restore(void);
void redrawWalls(void);
void checkWin(void);
int isPathPossible(int p);
int canMoveToDestination(int x, int y, int destinationY);
int isWithinBounds(int x, int y);
void drawInitWall(int disable);
void redrawTokens(void);
void displayMoves(int update);
#endif
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
