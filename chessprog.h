// Oct 05 22:07

#ifndef CHESS_
#define CHESS_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#define WHITE_P 'm'
#define WHITE_B 'b'
#define WHITE_N 'n'
#define WHITE_R 'r'
#define WHITE_Q 'q'
#define WHITE_K 'k'

#define BLACK_P 'M'
#define BLACK_B 'B'
#define BLACK_N 'N'
#define BLACK_R 'R'
#define BLACK_Q 'Q'
#define BLACK_K 'K'


#define EMPTY ' '

#define BOARD_SIZE 8
#define WHITE 'w'
#define BLACK 'b'

#define COMP_COLOR ((userColor==WHITE) ? (BLACK) : (WHITE))
#define OPPOSITE_COLOR(color) ((color==WHITE) ? (BLACK) : (WHITE))
#define colorOf(player) ((player>='a' && player<='z') ? WHITE : BLACK)
#define SAME_COLOR(a,b) (a!=EMPTY && b!=EMPTY && colorOf(a)==colorOf(b))
#define IS_EMPTY(c,r) (playerType(c,r)==EMPTY)

// board initialization
#define ENTER_SETTINGS "Enter game settings:\n" 
#define WRONG_GAME_MODE "Wrong game mode\n"
#define TWO_PLAYERS_GAME_MODE "Running game in 2 players mode\n"
#define PLAYER_VS_AI_GAME_MODE "Running game in player vs. AI mode\n"
#define WRONG_MINIMAX_DEPTH "Wrong value for minimax depth. The value should be between 1 to 4\n"
#define WRONG_FILE_NAME "Wrong file name\n"
#define WRONG_POSITION "Invalid position on the board\n"
#define WRONG_PEACE "The specified position does not contain your piece\n" 
#define NO_PIECE "Setting this piece creates an invalid board\n"  
#define WROND_BOARD_INITIALIZATION "Wrong board initialization\n"

#define ILLEGAL_COMMAND "Illegal command, please try again\n"
#define ILLEGAL_MOVE "Illegal move\n"
 
#define WRONG_ROOK_POSITION "Wrong position for a rook\n" 
#define ILLEGAL_CALTLING_MOVE "Illegal castling move\n"  

#define TIE "The game ends in a tie\n"
 
#define perror_message(func_name) (perror("Error: standard function %s has failed", func_name));
#define print_message(message) (printf("%s", message));


// the mode of the game - two players mode by default - represented by 1, player vs computer represented by 2
int gameMode;

// the color of user's pieces - it is white by default
char userColor; //white by default


// the player who plays first in the case of 2 players, white by default
char firstPlayer;

// we keep in these arrays the number of the pieces on the board until the start
// 1st cell for pawns, 2nd for bishops, 3rd for rooks, 4th for knights, 5th for queen and 6st for the king
int whites[6];
int blacks[6];

//the depth of minimax algorithm. we present it by an interger which value is between 1 and 4
//the default value is 1
int miniMaxDepth;

// a constant that represent the state of minimax - 0 for depth from 1 to 4, 1 for best
int BEST;

/* Declaration of structs that we'll work with */
typedef struct Location{
    //the specific location of a checker
    char column;
    int row; 
    // representive of the player in this checker,
	char player;  // 'm' for white Pawn, 'M' for black Pawn,
                  // 'b' for white Bishop, 'B' for black Bishop
                  // 'r' for white Rook, 'R' for black Rook    
                  // 'n' for white Knight, 'N' for black Knight
                  // 'q' for white Queen, 'Q' for black Queen
                  // 'k' for white King, 'K' for black King
                  // ' ' if player is empty
} Location; /* represent the location in the board */

typedef struct Move{
    // each move will be represnted by it's "from"(source) and "to"(target) adrresses
    Location from; // source adress of the move
    Location to; // target adress of the move
    char promotion; // this variable represents the name of the promotion in the case if there is a promotion, otherwise will be NULL;
    int score; // will be useful during minimax calculations, for bubbling up
	// the tree.
    struct Move* castle; // 2 points Bonus. represents the additional (*) Move
    // assosiated with castling.
    // * by 'additional' we mean:
    // additional position relocation of same-color player
   	struct Move* next; // totally different Move, will be used for the list of
	// offered moves, returned by possibleMoves(char color). 
	int alpha, beta; // used for Alpha-Beta prunning
 
} Move; //represent a single move


//here we define the board for the game, which is represented by a double array (of a constant size) of structs 
Location **chessBoard; // will be allocated and initialized later

// these variables represent the location of the kings on the board
Location wKing, bKing;

//brains -  macros and globals
#define INFINITY 9000
#define WIN_SCORE 7000
#define LOSE_SCORE -7000
#define INITIAL -6900
#define SMALL 0
#define BIG 1
#define UP 'u'
#define DOWN 'd'
#define LEFT 'l'
#define RIGHT 'r'
#define OFF 0
#define ON 1
#define CHECK_STATUS 0
#define MATE_STATUS 1
#define TIE_STATUS 2
#define NORMAL_STATUS 3
#define ABS(x) ((x>=0)?(x):(-1*(x)))
#define BUBBLE1 (depth%2 == 0 && (caller->score==INITIAL || caller->score > curr->score))
#define BUBBLE2 (depth%2 == 1 && (caller->score==INITIAL || caller->score < curr->score))
#define BUBBLE_SCORE_UP if (BUBBLE1 || BUBBLE2) caller->score = curr->score

Move* head; // head list returned by possibleMoves
Move* tail; // tail of that list

char castle_flags[4]; // the are 4 different casling moves
// possible. Index 0 is for WHITE_SMALL, index 1 is for WHITE_BIG, index 2
// is for BLACK_SMALL, index 3 is for BLACK_BIG. If either players related to
// any possible castle moved already in this game, their respective field is 
// switched to ON, so castling is forbidden.

// chessprog function declarations
void print_line();
void print_board();
void init_board();
void clearBoard();
char* readline();
void freeWordsArray();
void wordsArray();
int correctFormat(char* str);
char firstParameter(char* str);
int secondParameter(char* str);
void updateKings();
void updateCells(char type, int num);
int legalSet(char* type, char* color);
void forSetPos(Location loc, char* piece, char* color);
void setPosition(Location loc, char piece);
int wrongBoard();
int settingsState();
char checkerColor(char,int);
void MoveToPrint(Move move);
Move* stringToMove(char* src, char* dest, char prom);
int checkMove(char* src, char* dest, char prom, char color);
int fileExists(char *fname);
int saveState(char* fname, char color);
void loadState(char* fname);
char typeToMacro(char* str, char color);
int userTurn(char COLOR);
int compTurn(char COLOR);
int vsPlayerState();
int vsCompState();
char playerType(char column, int row);

// brains.c function declarations
int progressAllowed(char column, int row, char height, char side);
void increment(char*,int*,int,char,char);
void applyMove(Move);
void possibleMoves(char,int);
void checkDirection(char,int,char,char,int);
void printPossibleMoves();
Location** boardMirrorCopy();
void freeBoard(Location*** board);
Move* createMove(char,int,char,int);
void addKnightMoves(char, int, char);
void recursiveFree(Move** m);
void addMoveToList(Move *m);
Move* copyList(Move* head_);
Move* copyMove(Move* m);
void copyLocationFields(Move *A, Move *B);
int legalMove(Move);
void addPromotions(char,int,char,int);
void checkCastle(char,char);
int canCaptureKing(char,int,int);
int reportStatus(char);
Move* doesTheListContainThisMove(char,int,char,int,char,char);
int kingIsBeingThreatened(char);
int atLeastOneMoveIsPossibleFor(char);
void initializeMovesList();
void printSingleMove(Move* m);
void updateCastleFlags(char*,char,int);
void allPossibleMoves(char);
void removeFromList(Move*,Move*,int);
int sameMove(Move*,Move*);

// minimax.c functions:
int scoreBoard(char);
int berlingerScore(char);
void updateBestDepth(int*,char);
void minimaxCalculator(char,int,Move*,Move*);
void minimax(char,Move*);

#endif //CHESS_
