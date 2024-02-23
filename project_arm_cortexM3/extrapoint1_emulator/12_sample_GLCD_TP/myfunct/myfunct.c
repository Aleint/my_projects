#include "lpc17xx.h"
#include "funct.h"
#include "../RIT/RIT.h"
#include "../GLCD/GLCD.h" 
#include "../timer/timer.h"
#include <string.h>

#define BOARD_GAME_SIZE 13
#define EMPTY_CELL 0
#define PLAYER_1_CELL 1
#define PLAYER_2_CELL 2
#define WALL_CELL_V 3
#define WALL_CELL_H 4
#define boxY 0x0109
#define box1X 10
#define box2X  85
#define box3X  165
#define SQUARE_SIZE 30

struct Move {
    uint32_t playerID : 8;           // 8 bits for PlayerID
    uint32_t playerMove : 4;         // 4 bits for PlayerMove (0: move, 1: wall placement)
    uint32_t dir : 4;           // 4 bits for Vertical (0: vertical wall, 1: horizontal wall)
    uint32_t y : 8;                  // 8 bits for Y coordinate
    uint32_t x : 8;                  // 8 bits for X coordinate
}move;

static struct Move moves[100];
static int numMove=0;


 struct TokenPos {
    int x;
    int y;
};
volatile struct TokenPos lastTokenPos[2];
volatile struct TokenPos previousTokenPos[2];

struct WallPos {
    int x;
    int y;
};

static struct WallPos wallPos = {7, 5};

static int walls[2]={8, 8};

extern volatile int dirWall ;
extern volatile int disable; //defined in  irq_timer
extern volatile int placeWall; //defined in irq_button

extern volatile uint16_t player;  //defined in irq_timer.c
volatile int endTurn=0 ;

volatile int board[BOARD_GAME_SIZE][BOARD_GAME_SIZE];//walls in colums and rows 1 3 5 7 9 11 and tokens in 0 2 4 6 8 10 12 
volatile int board2[BOARD_GAME_SIZE][BOARD_GAME_SIZE];
volatile int wallx=0;
volatile int wally=0;
volatile int tokenx=0;
volatile int tokeny=0;
	
void initBoard(void){
int i = 0;
int j = 0;

	
	 for (i = 0; i < BOARD_GAME_SIZE; ++i) {
        for (j = 0; j < BOARD_GAME_SIZE; ++j) {
            board[i][j]=0;
        }
}
	  previousTokenPos[0].x=6;
		lastTokenPos[0].x=6;
		previousTokenPos[0].y=0;
		lastTokenPos[0].y=0;
		
		previousTokenPos[1].x=6;
		lastTokenPos[1].x=6;
		previousTokenPos[1].y=12;
		lastTokenPos[1].y=12;

		board[0][6]=PLAYER_1_CELL;
		board[12][6]=PLAYER_2_CELL;
	 }

int availableWall(){
	if (walls[player] > 0){
	
			return 1;
	}
	else 
		return 0;
		
} 

void updateBoard(int y,int x,int value){
	
	 board[y][x] = value;

}

void moveToken(enum cmdJoy cmd){
tokenx=10+(lastTokenPos[player].x/2)*30+15;
	tokeny=10+(lastTokenPos[player].y/2)*30+15;
if(!endTurn ){
displayMoves(1);
	switch (cmd) {
	 
        case SEL:
        			
						// Clear the previous position of the player token 
            board[previousTokenPos[player].y][previousTokenPos[player].x] = EMPTY_CELL;
						board[lastTokenPos[player].y][lastTokenPos[player].x] = player ? PLAYER_2_CELL: PLAYER_1_CELL;  
				
								previousTokenPos[player]=lastTokenPos[player];
								moves[numMove].playerID=player;
								moves[numMove].playerMove=0;
								moves[numMove].dir=0;
								moves[numMove].x=lastTokenPos[player].x;
								moves[numMove].y=lastTokenPos[player].y;
								numMove++;
           endTurn=1;
				
				break;
        case L:
					if(lastTokenPos[player].y== previousTokenPos[player].y )//cannot move diagonal
            if(lastTokenPos[player].x-2>=0 && board[lastTokenPos[player].y][lastTokenPos[player].x-1]==EMPTY_CELL ){//no walls						
							if( board[lastTokenPos[player].y][lastTokenPos[player].x - 2] == EMPTY_CELL || board[lastTokenPos[player].y][lastTokenPos[player].x - 2] == (player ? PLAYER_2_CELL : PLAYER_1_CELL)){
							if((lastTokenPos[player].x-2== previousTokenPos[player].x-2  || lastTokenPos[player].x-2== previousTokenPos[player].x )&& lastTokenPos[player].y== previousTokenPos[player].y ){//only 1 step from start pos
							drawToken(tokenx,tokeny,Blue2);//delete token
							lastTokenPos[player].x-=2;
							tokenx=10+((lastTokenPos[player].x/2)*30)+15;
							drawToken(tokenx,tokeny,player ?Red: White);
							displayMoves(0);
							}
							
						}else{//if there is a token of the other player
							if( lastTokenPos[player].x-4>=0 && (  board[lastTokenPos[player].y][lastTokenPos[player].x - 2] == (player ? PLAYER_1_CELL:PLAYER_2_CELL)))
							if(lastTokenPos[player].x-4== previousTokenPos[player].x-4  || lastTokenPos[player].x-4== previousTokenPos[player].x )
							if( board[lastTokenPos[player].y][lastTokenPos[player].x-3]==EMPTY_CELL){
										drawToken(tokenx,tokeny,Blue2);//delete token
										lastTokenPos[player].x-=4;
										tokenx=10+((lastTokenPos[player].x/2)*30)+15;
										drawToken(tokenx,tokeny,player ?Red: White);
								}
							displayMoves(0);
						}
						}
							
            break;
        case R:
							if(lastTokenPos[player].y== previousTokenPos[player].y )//cannot move diagonal
             if(lastTokenPos[player].x+2<13 && board[lastTokenPos[player].y][lastTokenPos[player].x+1]==EMPTY_CELL  ){
					
								if( board[lastTokenPos[player].y][lastTokenPos[player].x+2]==EMPTY_CELL || board[lastTokenPos[player].y][lastTokenPos[player].x+2]==(player ? PLAYER_2_CELL : PLAYER_1_CELL)){
							if((lastTokenPos[player].x+2== previousTokenPos[player].x+2  || lastTokenPos[player].x+2== previousTokenPos[player].x)&& lastTokenPos[player].y== previousTokenPos[player].y  ){//only 1 step from start pos
							 drawToken(tokenx,tokeny,Blue2);//delete token
							lastTokenPos[player].x+=2;
							tokenx=10+((lastTokenPos[player].x/2)*30)+15;
							drawToken(tokenx,tokeny,player ?Red: White);
							displayMoves(0);
							}
						 }
							else{//if there is a token of the other player
							if( lastTokenPos[player].x+4<13 &&( board[lastTokenPos[player].y][lastTokenPos[player].x+2]== (player ? PLAYER_1_CELL:PLAYER_2_CELL) ))
							if(lastTokenPos[player].x+4== previousTokenPos[player].x+4  || lastTokenPos[player].x+4== previousTokenPos[player].x)							
							if( board[lastTokenPos[player].y][lastTokenPos[player].x+3]==EMPTY_CELL){
										drawToken(tokenx,tokeny,Blue2);//delete token
										lastTokenPos[player].x+=4;
										tokenx=10+((lastTokenPos[player].x/2)*30)+15;
										drawToken(tokenx,tokeny,player ?Red: White);
										displayMoves(0);
								}
						}
							
						}
							
            break;
        case U:
						if(lastTokenPos[player].x== previousTokenPos[player].x )//cannot move diagonal
            if(lastTokenPos[player].y-2>=0 && board[lastTokenPos[player].y-1][lastTokenPos[player].x]==EMPTY_CELL){//no wall
								if( board[lastTokenPos[player].y-2][lastTokenPos[player].x]==EMPTY_CELL || board[lastTokenPos[player].y-2][lastTokenPos[player].x]==(player ? PLAYER_2_CELL : PLAYER_1_CELL)){
							if((lastTokenPos[player].y-2== previousTokenPos[player].y-2  || lastTokenPos[player].y-2== previousTokenPos[player].y)&& lastTokenPos[player].x== previousTokenPos[player].x  ){//only 1 step from start pos
							drawToken(tokenx,tokeny,Blue2);//delete token
							lastTokenPos[player].y-=2;
							tokeny=10+((lastTokenPos[player].y/2)*30)+15;
							drawToken(tokenx,tokeny,player ?Red: White);	
							displayMoves(0);
						}
					}
								else{//if there is a token of the other player
							if(board[lastTokenPos[player].y - 2][lastTokenPos[player].x]  == (player ? PLAYER_1_CELL:PLAYER_2_CELL))
								if(lastTokenPos[player].y-3>0) 
									if(lastTokenPos[player].y-4== previousTokenPos[player].y-4  || lastTokenPos[player].y-4== previousTokenPos[player].y)
									
								if( board[lastTokenPos[player].y-3][lastTokenPos[player].x]==EMPTY_CELL){
										drawToken(tokenx,tokeny,Blue2);//delete token
										lastTokenPos[player].y-=4;
										tokeny=10+((lastTokenPos[player].y/2)*30)+15;
										drawToken(tokenx,tokeny,player ?Red: White);
										displayMoves(0);
								}
						}
									checkWin();
						}
					
            break;
        case D:
							if(lastTokenPos[player].x== previousTokenPos[player].x )//cannot move diagonal
            if(lastTokenPos[player].y+2<13 && board[lastTokenPos[player].y+1][lastTokenPos[player].x]==EMPTY_CELL ){
							if( board[lastTokenPos[player].y+2][lastTokenPos[player].x]==EMPTY_CELL ||board[lastTokenPos[player].y+2][lastTokenPos[player].x]==(player ? PLAYER_2_CELL : PLAYER_1_CELL)){
							if((lastTokenPos[player].y+2== previousTokenPos[player].y+2  || lastTokenPos[player].y+2== previousTokenPos[player].y)&& lastTokenPos[player].x== previousTokenPos[player].x){//only 1 step from start pos
							drawToken(tokenx,tokeny,Blue2);//delete token
							lastTokenPos[player].y+=2;
							tokeny=10+((lastTokenPos[player].y/2)*30)+15;
							drawToken(tokenx,tokeny,player ?Red: White);
							displayMoves(0);
						}
					}
							else{//if there is a token of the other player
							if( lastTokenPos[player].y+4<13 && (board[lastTokenPos[player].y+2][lastTokenPos[player].x] == (player ? PLAYER_1_CELL:PLAYER_2_CELL) ))
								if(lastTokenPos[player].y+4== previousTokenPos[player].y+4  || lastTokenPos[player].y+4== previousTokenPos[player].y)
							if( board[lastTokenPos[player].y+3][lastTokenPos[player].x]==EMPTY_CELL){
										drawToken(tokenx,tokeny,Blue2);//delete token
										lastTokenPos[player].y+=4;
										tokeny=10+((lastTokenPos[player].y/2)*30)+15;
										drawToken(tokenx,tokeny,player ?Red: White);
										displayMoves(0);
								}
						}
								checkWin();
						}
						
            break;
        default:
            break;
			}
}
 
	
}
void moveWall(enum cmdJoy cmd ){
	
	char wall_in_char[5] = "";
	int i,overlap=0,path=0;

	if(!endTurn){	
 switch (cmd) {
        case SEL: //confirm wall    
						LPC_SC->EXTINT &= (1 << 2);     /* clear pending interrupt         */    
						NVIC_DisableIRQ(EINT2_IRQn);
						disable=0; //disable wall mode
						placeWall = 0;//return game mode
						wallx = 10 + ((wallPos.x+1) / 2) * 30;
						wally = 10 + ((wallPos.y+1)/ 2) * 30;						
                    // Insert wall into the board 
				if(dirWall){ //horizontal
					for (i=wallPos.x-1;i<wallPos.x+2;i++){
							if(board[wallPos.y][i]!=0){
								GUI_Text(box1X, boxY-20,(uint8_t *)"wall overlap", Black, player? Red:White);
								GUI_Text(box1X, boxY-20,(uint8_t *)"wall overlap", Blue2, Blue2);
								overlap=1;
								break;
							}
						}
					if(!overlap){
						displayMoves(1);
						for (i=wallPos.x-1;i<wallPos.x+2;i++)									
							updateBoard(wallPos.y,i,WALL_CELL_H);
						
								if(isPathPossible(0) &&isPathPossible(1))
								{
									path=1;
									drawWall(wallx, wally, dirWall,Green);
								moves[numMove].playerID=player;
								moves[numMove].playerMove=1;
								moves[numMove].dir=1;
								moves[numMove].x=wallPos.x;
								moves[numMove].y=wallPos.y;
								numMove++;
								}
								else{
									GUI_Text(box1X, boxY-20,(uint8_t *)"The wall traps the token.", Black, player? Red:White);
									GUI_Text(box1X, boxY-20,(uint8_t *)"The wall traps the token.", Blue2, Blue2);
										for (i=wallPos.x-1;i<wallPos.x+2;i++)									
											updateBoard(wallPos.y,i,EMPTY_CELL);
								}
					
					}
					}else{//vertical
						for (i=wallPos.y-1;i<wallPos.y+2;i++){
							if(board[i][wallPos.x]!=0){
								GUI_Text(box1X, boxY-20,(uint8_t *)"wall overlap", Black, player? Red:White);
								GUI_Text(box1X, boxY-20,(uint8_t *)"wall overlap", Blue2, Blue2);
								overlap=1;
								break;
							}
						}
					if(!overlap){
						displayMoves(1);
						for (i=wallPos.y-1;i<wallPos.y+2;i++)									
							updateBoard(i,wallPos.x,WALL_CELL_V);
						
						if(isPathPossible(0) && isPathPossible(1)){
								path=1;
								drawWall(wallx, wally, dirWall,Green);
								moves[numMove].playerID=player;
								moves[numMove].playerMove=1;
								moves[numMove].dir=0;
								moves[numMove].x=wallPos.x;
								moves[numMove].y=wallPos.y;
								numMove++;
					}
						else{
									GUI_Text(box1X, boxY-20,(uint8_t *)"The wall traps the token.", Black, player? Red:White);
									GUI_Text(box1X, boxY-20,(uint8_t *)"The wall traps the token.", Blue2, Blue2);
									for (i=wallPos.y-1;i<wallPos.y+2;i++)									
										updateBoard(i,wallPos.x,EMPTY_CELL);
						}
						}
					}
					
						if(!overlap && path){
							walls[player]--;
						sprintf(wall_in_char,"%4d",walls[player]);
						if(player){
						GUI_Text(box3X + 18, boxY + 30, (uint8_t*)wall_in_char, Black, Blue2);
            }
						else
							GUI_Text(box1X + 18, boxY + 30, (uint8_t*)wall_in_char, Black, Blue2);						
						redrawWalls();
						redrawTokens();
						//reset walls pos
						wallPos.x=7;
						wallPos.y=5;
						displayMoves(1);
						endTurn=1;
					}
						else {
							displayMoves(0);
							drawWall(wallx, wally, dirWall,Black);
							redrawWalls();	
							//reset walls pos
							wallPos.x=7;
							wallPos.y=5;
						}
						
						break;
						
        case L:
					if(wallPos.x-2>0){
						drawWall(wallx, wally, dirWall,Black);//delete previous wall
            wallPos.x-=2;
						wallx = 10 + ((wallPos.x+1) / 2) * 30;
						wally = 10 + (((wallPos.y+1)/ 2)) * 30;
						drawWall(wallx, wally, dirWall,Yellow);
						redrawWalls();
						}
            break;
        case R:
            if(wallPos.x+2<12){
						drawWall(wallx, wally, dirWall,Black);//delete previous wall
            wallPos.x+=2;
						wallx = 10 + ((wallPos.x+1) / 2) * 30;
						wally = 10 + (((wallPos.y+1)/ 2)) * 30;
						drawWall(wallx, wally, dirWall,Yellow);
						redrawWalls();
						}
            break;
        case U:
           	if(wallPos.y-2>0){
						drawWall(wallx, wally, dirWall,Black);//delete previous wall
            wallPos.y-=2;
						wallx = 10 + ((wallPos.x+1) / 2) * 30;
						wally = 10 + (((wallPos.y+1)/ 2)) * 30;
						drawWall(wallx, wally, dirWall,Yellow);
						redrawWalls();
						}
            break;
        case D:
            if(wallPos.y+2<12){
						drawWall(wallx, wally, dirWall,Black);//delete previous wall
            wallPos.y+=2;
						wallx = 10 + ((wallPos.x+1) / 2) * 30;
						wally = 10 + (((wallPos.y+1)/ 2)) * 30;
						drawWall(wallx, wally, dirWall,Yellow);
						redrawWalls();
						}
            break;
				case ROT:					
						wallx = 10 + ((wallPos.x+1) / 2) * 30;
						wally = 10 + (((wallPos.y+1)/ 2)) * 30;				
            drawWall(wallx, wally, dirWall,Yellow);
						redrawWalls();
            break;
        default:
            break;
    }
	
	}

}
void drawInitWall(int disable)	{
	wallx = 10 + ((wallPos.x+1) / 2) * 30;
	wally = 10 + (((wallPos.y+1)/ 2)) * 30;
	
	drawWall(wallx, wally, dirWall,disable?Black:Yellow);

} 

//time exipired
void restore(){
	drawWall(wallx, wally, dirWall,Black);//delete previous wall
	//reset walls pos
	wallPos.x=7;
	wallPos.y=5;
	redrawTokens();
	displayMoves(1);
	disable=0; //disable wall mode
	placeWall = 0;//return game mode
	redrawWalls();
	// move out of time
		moves[numMove].playerID=player;
		moves[numMove].playerMove=0;
		moves[numMove].dir=1;
	  moves[numMove].x=0;
		moves[numMove].y=0;
		numMove++;
}

void redrawWalls() {
	uint16_t j,i,x,y;
    for (i = 1; i < BOARD_GAME_SIZE-1; i ++) {
        for (j = 1; j < BOARD_GAME_SIZE-1; j ++) {
             x = 10 + ((j+1) / 2) * 30; // Calculate x-coordinate based on column index
             y = 10+((i+1) / 2) * 30;  // Calculate y-coordinate based on row index

            // Check adjacent cells to determine wall direction
            if (j - 1 >= 0 && j+1<BOARD_GAME_SIZE-1 && board[i][j] == WALL_CELL_H && board[i][j - 1] == WALL_CELL_H && board[i][j+1] == WALL_CELL_H) {
                // Draw a horizontal wall
                LCD_DrawLine(x- SQUARE_SIZE , y, x + SQUARE_SIZE, y, Green);
            }

       
            if (i - 1 >= 0 && i+1<BOARD_GAME_SIZE-1 && board[i][j] == WALL_CELL_V && board[i - 1][j] == WALL_CELL_V && board[i+1][j] == WALL_CELL_V) {                
							// Draw a vertical wall
                LCD_DrawLine(x, y -SQUARE_SIZE, x, y + SQUARE_SIZE, Green);
            }
       
        }
    }
}
//reset tokens
void redrawTokens(){

	tokenx=10+(lastTokenPos[player].x/2)*30+15;
	tokeny=10+(lastTokenPos[player].y/2)*30+15;
	drawToken(tokenx,tokeny,Blue2);//delete token	
		
	lastTokenPos[player]=previousTokenPos[player];//restore token
	tokenx=10+(lastTokenPos[player].x/2)*30+15;
	tokeny=10+(lastTokenPos[player].y/2)*30+15;
	drawToken(tokenx,tokeny,player ?Red: White)	;
}
void checkWin(){
char win[10]="";
	if(player){//player 2
			if(lastTokenPos[player].y==0){
				sprintf(win,"player %d win",player+1);
				GUI_Text(box1X, boxY-20, (uint8_t *) win, Green, Blue2);
				disable_timer(0);
			}
	}
	else{
			if(lastTokenPos[player].y==12){
				sprintf(win,"player %d win",player+1);
				GUI_Text(box1X, boxY-20, (uint8_t *) win, Green, Blue2);
				disable_timer(0);
			}
	}

}

int isWithinBounds(int x, int y) {
    return x >= 0 && x < BOARD_GAME_SIZE && y >= 0 && y < BOARD_GAME_SIZE;
}

int search(int x, int y, int destinationY) {
		int result = 0;
    if (y == destinationY) {
        return 1;  // Arrivato alla destinazione
    }    

    // Segna la cella corrente come visitata
    board2[y][x] = -1;

    // Controlla le mosse possibili in tutte le direzioni
    if (isWithinBounds(x, y + 2) && board2[y + 1][x] != WALL_CELL_H && board2[y + 1][x] != WALL_CELL_V && board2[y + 2][x] != -1 && !result) {
        result |= search(x, y + 2, destinationY);
    }
    if (isWithinBounds(x, y - 2) && board2[y - 1][x] != WALL_CELL_H && board2[y - 1][x] != WALL_CELL_V && board2[y - 2][x] != -1 && !result) {
        result |= search(x, y - 2, destinationY);
    }
    if (isWithinBounds(x + 2, y) && board2[y][x + 1] != WALL_CELL_H && board2[y][x + 1] != WALL_CELL_V && board2[y][x + 2] != -1 && !result) {
        result |= search(x + 2, y, destinationY);
    }
    if (isWithinBounds(x - 2, y) && board2[y][x - 1] != WALL_CELL_H && board2[y][x - 1] != WALL_CELL_V && board2[y][x - 2] != -1 && !result) {
        result |= search(x - 2, y, destinationY);
    }

    return result;
}

int isPathPossible(int p) {
    
    int i, j;
    for (i = 0; i < BOARD_GAME_SIZE; ++i) {
        for (j = 0; j < BOARD_GAME_SIZE; ++j) {
          
               board2[i][j]= board[i][j] ;
            
        }
    }

    return search(lastTokenPos[p].x, lastTokenPos[p].y, p ? 0 : BOARD_GAME_SIZE - 1);
}

void displayMoves(int update){
int x,y;
	tokenx=10+(lastTokenPos[player].x/2)*30+15;
	tokeny=10+(lastTokenPos[player].y/2)*30+15;

//case L:
					if(lastTokenPos[player].y== previousTokenPos[player].y )//cannot move diagonal
            if(lastTokenPos[player].x-2>=0 && board[lastTokenPos[player].y][lastTokenPos[player].x-1]==EMPTY_CELL ){//no walls						
							if( board[lastTokenPos[player].y][lastTokenPos[player].x - 2] == EMPTY_CELL || board[lastTokenPos[player].y][lastTokenPos[player].x - 2] == (player ? PLAYER_2_CELL : PLAYER_1_CELL)){
							if((lastTokenPos[player].x-2== previousTokenPos[player].x-2  || lastTokenPos[player].x-2== previousTokenPos[player].x )&& lastTokenPos[player].y== previousTokenPos[player].y ){//only 1 step from start pos
							x=lastTokenPos[player].x-2;
							x=10+((x/2)*30)+15;
							if(update){
								drawToken(x,tokeny, Blue2);//delete previous possible position
							}else
							drawToken(x,tokeny, Black);						
							}
							
						}else{//if there is a token of the other player
							if( lastTokenPos[player].x-4>=0 && (  board[lastTokenPos[player].y][lastTokenPos[player].x - 2] == (player ? PLAYER_1_CELL:PLAYER_2_CELL)))
								if(lastTokenPos[player].x-4== previousTokenPos[player].x-4  || lastTokenPos[player].x-4== previousTokenPos[player].x )
							if( board[lastTokenPos[player].y][lastTokenPos[player].x-3]==EMPTY_CELL){										
										x=lastTokenPos[player].x-4;
										x=10+((x/2)*30)+15;
										if(update){
								drawToken(x,tokeny, Blue2);//delete previous possible position
							}else
							drawToken(x,tokeny, Black);
								}
						}
						}
							
            
      //  case R:
							if(lastTokenPos[player].y== previousTokenPos[player].y )//cannot move diagonal
             if(lastTokenPos[player].x+2<13 && board[lastTokenPos[player].y][lastTokenPos[player].x+1]==EMPTY_CELL  ){
					
								if( board[lastTokenPos[player].y][lastTokenPos[player].x+2]==EMPTY_CELL || board[lastTokenPos[player].y][lastTokenPos[player].x+2]==(player ? PLAYER_2_CELL : PLAYER_1_CELL)){
							if((lastTokenPos[player].x+2== previousTokenPos[player].x+2  || lastTokenPos[player].x+2== previousTokenPos[player].x)&& lastTokenPos[player].y== previousTokenPos[player].y  ){//only 1 step from start pos
							 
							x=lastTokenPos[player].x+2;
							x=10+((x/2)*30)+15;
								if(update){
								drawToken(x,tokeny, Blue2);//delete previous possible position
							}else
							drawToken(x,tokeny, Black);
								
							}
							
						 }
							else{//if there is a token of the other player
								if( lastTokenPos[player].x+4<13 &&( board[lastTokenPos[player].y][lastTokenPos[player].x+2]== (player ? PLAYER_1_CELL:PLAYER_2_CELL) ))
									if(lastTokenPos[player].x+4== previousTokenPos[player].x+4  || lastTokenPos[player].x+4== previousTokenPos[player].x)							
							if( board[lastTokenPos[player].y][lastTokenPos[player].x+3]==EMPTY_CELL){
								
										x=lastTokenPos[player].x+4;
										x=10+((x/2)*30)+15;
										if(update){
								drawToken(x,tokeny, Blue2);//delete previous possible position
							}else
							drawToken(x,tokeny, Black);
								}
						}
							
						}
							
            
       // case U:
						if(lastTokenPos[player].x== previousTokenPos[player].x )//cannot move diagonal
            if(lastTokenPos[player].y-2>=0 && board[lastTokenPos[player].y-1][lastTokenPos[player].x]==EMPTY_CELL){//no wall
								if( board[lastTokenPos[player].y-2][lastTokenPos[player].x]==EMPTY_CELL || board[lastTokenPos[player].y-2][lastTokenPos[player].x]==(player ? PLAYER_2_CELL : PLAYER_1_CELL)){
							if((lastTokenPos[player].y-2== previousTokenPos[player].y-2  || lastTokenPos[player].y-2== previousTokenPos[player].y)&& lastTokenPos[player].x== previousTokenPos[player].x  ){//only 1 step from start pos
							
							y=lastTokenPos[player].y-2;
							y=10+((y/2)*30)+15;
							if(update){
							drawToken(tokenx,y,Blue2);	
							}
							else
								drawToken(tokenx,y,Black);			
						}
					}
								else{//if there is a token of the other player
							if(board[lastTokenPos[player].y - 2][lastTokenPos[player].x] == PLAYER_1_CELL || board[lastTokenPos[player].y - 2][lastTokenPos[player].x] == PLAYER_2_CELL)
								if(lastTokenPos[player].y-3>0) 
								if(board[lastTokenPos[player].y - 2][lastTokenPos[player].x]  == (player ? PLAYER_1_CELL:PLAYER_2_CELL))									
								if( board[lastTokenPos[player].y-3][lastTokenPos[player].x]==EMPTY_CELL){
										
										y=lastTokenPos[player].y-4;
										y=10+((y/2)*30)+15;
										if(update){
							drawToken(tokenx,y,Blue2);	
							}
							else
								drawToken(tokenx,y,Black);
								}
						}
									
						}
					
           
     //   case D:
							if(lastTokenPos[player].x== previousTokenPos[player].x )//cannot move diagonal
            if(lastTokenPos[player].y+2<13 && board[lastTokenPos[player].y+1][lastTokenPos[player].x]==EMPTY_CELL ){
							if( board[lastTokenPos[player].y+2][lastTokenPos[player].x]==EMPTY_CELL ||board[lastTokenPos[player].y+2][lastTokenPos[player].x]==(player ? PLAYER_2_CELL : PLAYER_1_CELL)){
							if((lastTokenPos[player].y+2== previousTokenPos[player].y+2  || lastTokenPos[player].y+2== previousTokenPos[player].y)&& lastTokenPos[player].x== previousTokenPos[player].x){//only 1 step from start pos
							
							y=lastTokenPos[player].y+2;
							y=10+((y/2)*30)+15;
							if(update){
							drawToken(tokenx,y,Blue2);	
							}
							else
								drawToken(tokenx,y,Black);
						}
					}
							else{//if there is a token of the other player
							if( lastTokenPos[player].y+4<13 && (board[lastTokenPos[player].y+2][lastTokenPos[player].x] == (player ? PLAYER_1_CELL:PLAYER_2_CELL) ))
								if(lastTokenPos[player].y+4== previousTokenPos[player].y+4  || lastTokenPos[player].y+4== previousTokenPos[player].y)
							if( board[lastTokenPos[player].y+3][lastTokenPos[player].x]==EMPTY_CELL){
										
										y=lastTokenPos[player].y+4;
										y=10+((y/2)*30)+15;
										if(update){
							drawToken(tokenx,y,Blue2);	
							}
							else
								drawToken(tokenx,y,Black);
								}
						}
					}
						
						


}
