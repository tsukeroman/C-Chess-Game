// Oct 04 19:00

#ifndef GRAPHICS_
#define GRAPHICS_

#include <SDL.h>
#include <SDL_video.h>
#include <SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SCREEN_W 800
#define SCREEN_H 640
#define	MENU_BUTTON_X 307	
#define	MENU_BUTTON_Y 150
#define	MENU_BUTTON_W 185
#define	MENU_BUTTON_H 50
#define	MENU2_BUTTON_X 280
#define	MENU2_BUTTON_Y 148
#define	MENU2_BUTTON_W 238
#define	MENU2_BUTTON_H 55

#define slotsNum 7

#define WIN_W 640
#define WIN_H 480
#define IMG_W 240
#define IMG_H 296


/* Declaration of the struct that we'll work with */

typedef struct Control{

	char Type; // this field is the type of the control: 'b' = button, 'i' = image, 'p' = panel;
	int size; // this field is the number of the children the control has in the ui tree
	struct Control* children[100]; // this field is an array of pointers to the control's children
	SDL_Surface *surface; // this is the surface the control will be drawn on
	SDL_Rect rect; // this field represnts the rect which will present control's size and offset

} Control; //represents any graphical component that is drawn (or assists in drawing) to our screen


// the surface for the main screen of the game
SDL_Surface* screen;

// this SDL_Rect represents the offset of the main screen
SDL_Rect screenOffset;

// some controls we'll use
Control* menuPanel;
Control* gameWindow;

// main event handling structure
SDL_Event e; 

// this variable is used for quiting the game
int quit;

// this variable is used for quiting a specific panel but not all the game
int cancel;

// this variable tells us whether the user wants to change the board setting ; 0 = no , 1 = yes
int toSet;

// this variable tells us whether the game is loaded or it's a new game; 0 = new game, 1 = loaded
int loaded;

// this variable is used for representing the current color
char colorGui;

// this variables is user for freeing the memory in order to avoid double-freeing of some component
int gameStarted;

// function declarations

Control* createButton(SDL_Rect rect, char* file);
Control* createImage (SDL_Rect rect, char* file);
Control* createPanel(SDL_Rect rect);
SDL_Rect geneRect(int x, int y, int w, int h);
int clickedButton(Control* button, SDL_Rect rect);
char klicationCol(SDL_Rect clickRect);
int klicationRow(SDL_Rect clickRect);
void addChild(Control* panel, Control* child);
void removeChild(Control* panel);
void freePanel(Control* panel);
void drawPanel(Control* panel, SDL_Surface* screen);
SDL_Surface* loadImage(char* file);
void applySurface(SDL_Rect rect,SDL_Surface* source, SDL_Surface* destination);
int firstPlayerGui();
int toSetBoard();
int setBoardGui();
char setOnePieceGui();
void drawBoard();
int settingsStateGui();
int playStateGui();
int pvscSettings();
int userTurnGui(char COLOR);
int compTurnGui(char COLOR);
char promotionGui(char COLOR);
int bestMoveGui(char COLOR);
int vsPlayerStateGui();
int vsCompStateGui();
int saveStateGui(char color);
void loadStateGui();
int resultStateGui(char COLOR);
void freeGame();
int mainGraphics();


#endif //GRAPHICS_

