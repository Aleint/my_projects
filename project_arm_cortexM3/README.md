# Quoridor for LandTiger Board

This project involves the implementation and enhancement of the abstract board game "Quoridor" on the LPC1768 SoC, using the Keil IDE and the LandTiger development board. The project is divided into two main parts: the first focuses on creating the core game mechanics, while the second adds advanced features like multiplayer support and an AI opponent.

## About The Game: Quoridor

Quoridor is a 2-player abstract strategy game. The objective is to be the first to move your pawn to the opposite side of the board.

### Game Rules
* The game is played on a 7x7 board. 
* Each player starts with a pawn in the center of their respective starting edge and is given eight barriers (walls). 
* On each turn, a player can either move their pawn one space horizontally or vertically, or place one of their walls on the board. 
* Walls block movement and must be navigated around.  It is forbidden to place a wall that completely blocks an opponent from reaching their goal. 
* Special "face-to-face" jump moves are allowed if two pawns are adjacent and not separated by a wall. 
* Each player has 20 seconds to make a move, or they forfeit their turn. 

---

## Part 1: Core Game Implementation

This part covers the implementation of the core Quoridor game mechanics for a single board, developed using the Keil µVision environment for either the LandTiger emulator or the physical board. 

### Key Features
* **Game Board**: The 7x7 game board is drawn on the LCD display.  Player 1's pawn is white, and Player 2's is red. 
* **Pawn Movement**: Players use the **JOYSTICK** to select a move (up, down, left, or right) from highlighted adjacent squares.  The move is confirmed with **JOY_SELECT**. 
* **Wall Placement**:
    * Pressing **KEY1** enters "wall placement" mode.  A wall appears on the board, which can be positioned using the **JOYSTICK**. 
    * Pressing **KEY2** rotates the selected wall 90 degrees. 
    * **JOY_SELECT** confirms the wall's position, ending the player's turn. 
    * Pressing **KEY1** again cancels wall placement and returns to pawn movement mode. 
* **Game State**: The game begins when the **INT0** button is pressed.  Each player has 8 walls, and a message appears if they run out. 
* **Turn Timer**: A 20-second timer is implemented for each turn.  If the timer expires, the player loses their turn. 

### Move Data Structure
Every move is stored in a 32-bit `unsigned int` variable with the following format: 

| Field | Bits | Description |
| :--- | :--- | :--- |
| **PlayerID** | 8 bits | Identifies the player (0 or 1).  |
| **PlayerMove/WallPlacement** | 4 bits | `0` for moving a pawn, non-zero for placing a wall.  |
| **Vertical/Horizontal** | 4 bits | `0` for a vertical wall, non-zero for horizontal.  Defaults to `0` for pawn moves.  |
| **Y** | 8 bits | The Y-coordinate of the pawn's new position or the wall's center.  |
| **X** | 8 bits | The X-coordinate of the pawn's new position or the wall's center.  |

---

## Part 2: Enhancements and Multiplayer

This part enhances the core game with a menu system, a non-player character (NPC) opponent, and a 1 vs. 1 multiplayer mode using a CAN bus connection. 

### Key Features
* **Game Menu**:
    * A startup menu is implemented, appearing after the **INT0** button is pressed. 
    * Players use the **JOYSTICK** to navigate and **JOY_SEL** to confirm choices. 
    * **Mode Selection**: The first screen allows choosing between "Single Board" or "Two Boards". 
    * **Opponent Selection**: In "Single Board" mode, the player can choose to play against another "Human" or the "NPC".  In "Two Boards" mode, players can select their own player type (Human or NPC). 
* **Non-Player Character (NPC)**:
    * An "Artificial Intelligence" is developed for the opponent. 
    * The NPC makes its move immediately after the human player. 
    * The implementation must be more advanced than a simple random choice (e.g., using a decision tree). 
    * The NPC **cannot cheat** and must adhere to all game rules, including having only 8 walls and moving one square per turn. 
* **Multiplayer Mode**:
    * A 1 vs. 1 mode is implemented by connecting two LandTiger boards via their **CAN bus**. 
    * This feature must be developed on the **physical boards**, not the emulator. 
    * The CAN controllers on both boards must be configured with the same Baud Rate and appropriate ID filters. 
    * A handshake protocol is required to establish a connection.  If the handshake fails, an error is shown. 
    * Player moves are exchanged between boards using the 32-bit data structure from Part 1. 