// Oct 04 19:00

#include "graphics.h"
#include "chessprog.h"



Control* createButton(SDL_Rect rect, char* file){
	// this function creates a button and returns a pointer to the Control component which represents the button
	// if malloc fails , the function returns NULL in order the caller function would terminate the game
	int i;
	Control* button = (Control*) malloc(sizeof(Control));
	if (button == NULL){
		printf("Error: standard function malloc has failed");
		return NULL;
	}
	button->Type = 'b';
	button->size = 0;
	for(i = 0 ; i < 100 ; i++)
		(button->children)[i] = NULL;
	button->surface = loadImage(file);
	button->rect.x = rect.x;
	button->rect.y = rect.y;
	button->rect.w = rect.w;
	button->rect.h = rect.h;
	return button;
}

Control* createImage (SDL_Rect rect, char* file){
	// this function creates an image and returns a pointer to the Control component which represents the image
	// if malloc fails , the function returns NULL in order the caller function would terminate the game
	int i;
	Control* image = (Control*) malloc(sizeof(Control));
	if (image == NULL){
		printf("Error: standard function malloc has failed");
		return NULL;
	}
	image->Type = 'i';
	image->size = 0;
	for(i = 0 ; i < 100 ; i++)
		(image->children)[i] = NULL;
	image->surface = loadImage(file);
	image->rect.x = rect.x;
	image->rect.y = rect.y;
	image->rect.w = rect.w;
	image->rect.h = rect.h;
	return image;
}

Control* createPanel(SDL_Rect rect){
	// this function creates a panel and returns a pointer to the Control component which represents the panel
	// if malloc fails , the function returns NULL in order the caller function would terminate the game
	int i;
	Control* panel = (Control*) malloc(sizeof(Control));
	if (panel == NULL){
		printf("Error: standard function malloc has failed");
		return NULL;
	}
	panel->Type = 'p';
	panel->size = 0;
	for(i = 0 ; i < 100 ; i++)
		(panel->children)[i] = NULL;
	panel->surface = NULL;
	panel->rect.x = rect.x;
	panel->rect.y = rect.y;
	panel->rect.w = rect.w;
	panel->rect.h = rect.h;
	return panel;
}

SDL_Rect geneRect(int x, int y, int w, int h){
	// this function generates and returns a SDL_Rect object of the size and the offset we need
	SDL_Rect generect;
	generect.x = x;
	generect.y = y;
	generect.w = w;
	generect.h = h;
	return generect;
}


int clickedButton(Control* button, SDL_Rect rect){
	// auxiliary function which determines if the mouse event took place in the bounds of the button or not
	// returns 1 if positive , otherwise returns 0
	SDL_Rect but;
	but.x = (button->rect).x;
	but.y = (button->rect).y;
	but.w = (button->rect).w;
	but.h = (button->rect).h;
	if(rect.x > but.x && rect.x < but.x + but.w)
		if(rect.y > but.y && rect.y < but.y + but.h)
			return 1;
	return 0;
}

char klickationCol(SDL_Rect clickRect){
	// this function determines where on the board the user clicked with the mouse
	// and returns a touple which is a string that consists of the column and the row, as like as in the console part implementation
	// RETURNS ROW FROM 1 TO 8 !!! to add (-1) if we want to see the location with respect to the board
	char column;
	column = 'a' + (clickRect.x / 80);
	return column;
}

int klickationRow(SDL_Rect clickRect){
	// this function determines where on the board the user clicked with the mouse
	// and returns a touple which is a string that consists of the column and the row, as like as in the console part implementation
	// RETURNS ROW FROM 1 TO 8 !!! to add (-1) if we want to see the location with respect to the board
	char row;
	row = 8 - (clickRect.y / 80);
	return row;
}

void addChild(Control* panel, Control* child){
	// this function adds a child to the panel , it's an important part of the ui tree creating
	int Size = panel->size;
	panel->children[Size] = child;
	panel->size = Size + 1;
	return;
}

void removeChild(Control* panel){
	// this function removes a child the lastest child of this panel , it's an important part of the ui tree maintenance
	int Size = panel->size;
	freePanel(panel->children[Size-1]);
	panel->size = Size - 1;
	return;
}

void freePanel(Control* panel){
	// this function frees the memory that was allocated for the panel, including all his children
	// it uses a recursion for this porpuse
	int i;
	for(i = 0 ; i < panel->size ; i++)
		freePanel(panel->children[i]);
	SDL_FreeSurface(panel->surface);
	free(panel);
	return;
}

void drawPanel(Control* panel, SDL_Surface* screen){
	// this function draws a window by scanning the UI tree of the panel with DFS (using recursion) and drawing each control in that order
	int i = 0;
	int Size = panel->size;
	char Type = panel->Type;
	if(Type == 'i' || Type == 'b')
		applySurface(panel->rect,panel->surface, screen);
	for(i = 0 ; i < Size ; i++)
		drawPanel(panel->children[i], screen);
	return;
}

SDL_Surface* loadImage(char* filename){
	// this function loads an image, creates an optimized one, and also eliminates the background
	// we chose to work with PNG images because IMG_Load() automatically handles transparency
	// this functions returnes a pointer to the surface where the new image is drawn
	SDL_Surface* loadedImage = NULL; 
	SDL_Surface* optimizedImage = NULL;
	loadedImage = IMG_Load(filename);
	if (loadedImage != NULL){
		optimizedImage = SDL_DisplayFormatAlpha(loadedImage);
		SDL_FreeSurface(loadedImage);
	}
	return optimizedImage;
}


void applySurface(SDL_Rect offset,SDL_Surface* source, SDL_Surface* destination){
	// this is a surface blitting function
	// it takes in the coordinates of where we want to blit the surface, the surface we're going to blit and the surface we're going to blit it to
	SDL_BlitSurface( source, NULL, destination, &offset );
	return;
}


int firstPlayerGui(){
	// Auxiliary function for settingsStateGui
	// this function allows us to set the "Next Player" in the start of the game - white by default
	// returns 0 if the programd doesn't succes to allocate some component , otherwise returns 1;
	int close = 0;
	Control *firstGame, *chooseGame, *whiteGame, *blackGame;
	SDL_Rect firstGameRect, chooseGameRect, whiteGameRect, blackGameRect, clickRect;
	
	firstGameRect = geneRect(230, 100, 450, 315);
	firstGame = (Control*) malloc(sizeof(Control));
	if(firstGame == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	} 
	firstGame = createPanel(firstGameRect);

	chooseGameRect = geneRect(180, 120, 300, 40);
	chooseGame = (Control*) malloc(sizeof(Control));
	if(chooseGame == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	chooseGame = createImage(chooseGameRect, "GUI_components/chooseFirst.png");	
	addChild(firstGame, chooseGame);

	whiteGameRect = geneRect(240, 175, 81, 38);
	whiteGame = (Control*) malloc(sizeof(Control));
	if(whiteGame == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	whiteGame = createImage(whiteGameRect, "GUI_components/whiteGame.png");	
	addChild(firstGame, whiteGame);

	blackGameRect = geneRect(320, 175, 82, 38);
	blackGame = (Control*) malloc(sizeof(Control));
	if(blackGame == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	blackGame = createImage(blackGameRect, "GUI_components/blackGame.png");	
	addChild(firstGame, blackGame);

	addChild(gameWindow->children[0], firstGame);
	drawPanel(firstGame, screen);
	SDL_Flip(screen);
	//SDL_Delay(3000);
	while(close == 0 && quit == 0){
		while (SDL_PollEvent(&e) != 0){
			switch (e.type) {
				case (SDL_QUIT):
					quit = 1;
					break;
				case (SDL_MOUSEBUTTONDOWN):
					clickRect = geneRect(e.button.x ,e.button.y, 0, 0);	

					if(clickedButton(whiteGame, clickRect)){
						firstPlayer = WHITE;
					}

					else if(clickedButton(blackGame, clickRect)){
						firstPlayer = BLACK;
					}
					else
						continue;
					close = 1;
					removeChild(gameWindow->children[0]);
					return 1;	
			}
		}
	}
	return 1;
}


int toSetBoard(){

	// Auxiliary function for settingsStateGui
	// this function gives the user the *OPTIONAL* ability - to set the board
	// returns 0 if the programd doesn't succes to allocate some component , otherwise returns 1;

	int close = 0;
	Control *setGame, *setPageGame, *chooseGame, *okGame, *cancelGame;
	SDL_Rect setGameRect, chooseGameRect, okGameRect, cancelGameRect, clickRect;
	
	setGameRect = geneRect(82, 82, 477, 238);
	setGame = (Control*) malloc(sizeof(Control));
	if(setGame == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	} 
	setGame = createPanel(setGameRect);

	setPageGame = (Control*) malloc(sizeof(Control));
	if(setPageGame == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	} 
	setPageGame = createImage(setGameRect, "GUI_components/colorCh.png");	
	addChild(setGame, setPageGame);

	chooseGameRect = geneRect(180, 120, 324, 30);
	chooseGame = (Control*) malloc(sizeof(Control));
	if(chooseGame == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	chooseGame = createImage(chooseGameRect, "GUI_components/toSet.png");	
	addChild(setGame, chooseGame);

	okGameRect = geneRect(180, 175, 165, 55);
	okGame = (Control*) malloc(sizeof(Control));
	if(okGame == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	okGame = createImage(okGameRect, "GUI_components/sureWhyNot.png");	
	addChild(setGame, okGame);

	cancelGameRect = geneRect(360, 175, 116, 55);
	cancelGame = (Control*) malloc(sizeof(Control));
	if(cancelGame == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	cancelGame = createImage(cancelGameRect, "GUI_components/noWay.png");	
	addChild(setGame, cancelGame);

	addChild(gameWindow->children[0], setGame);
	drawPanel(setGame, screen);
	SDL_Flip(screen);
	//SDL_Delay(3000);
	while(close == 0 && quit == 0){
		while (SDL_PollEvent(&e) != 0){
			switch (e.type) {
				case (SDL_QUIT):
					quit = 1;
					break;
				case (SDL_MOUSEBUTTONDOWN):
					clickRect = geneRect(e.button.x ,e.button.y, 0, 0);	

					if(clickedButton(okGame, clickRect)){
						toSet = 1;
					}

					else if(clickedButton(cancelGame, clickRect)){
						toSet = 0;
					}
					else
						continue;
					close = 1;
					removeChild(gameWindow->children[0]);
					return 1;	
			}
		}
	}

	return 1;
}




int setBoardGui(){
	// Auxiliary function for settingsStateGui
	// this function gives the player the option to set the pieces on the board as like as he wants
	// the setting MUST BE LEGAL
	// returns 1 if legal , otherwise returns 0

	int row, flag = 0;
	char column, back, temp;
	Control *startGame, *wrongBoard2;
	Control *mainMenuGame = (gameWindow->children[1])->children[2];
	Control *quitGame = (gameWindow->children[1])->children[3];
	Control *boardd = gameWindow->children[0];

	SDL_Rect clickRect, startGameRect, wrongBoard2Rect;
	
	startGameRect = geneRect(640, 470, 160, 68);
	startGame = (Control*) malloc(sizeof(Control));
	if(startGame == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	startGame = createImage(startGameRect, "GUI_components/startGame.png");
	addChild(gameWindow->children[1],startGame);
	drawPanel(gameWindow->children[1], screen);
	SDL_Flip(screen);

	drawBoard();

	while(cancel == 0 && quit == 0){
		while (SDL_PollEvent(&e) != 0){
			switch (e.type) {
				case (SDL_QUIT):
					quit = 1;
					break;
				case (SDL_MOUSEBUTTONDOWN):
					if(flag == 1){
						flag = 0;
						drawBoard();
						continue;
					}
					clickRect = geneRect(e.button.x, e.button.y, 0, 0);
					if(clickedButton(mainMenuGame, clickRect)){
						init_board(); 
						gameMode = 1;
						userColor = WHITE;
						firstPlayer = WHITE;
						miniMaxDepth = 1;
						BEST = 0;
						cancel = 1;
					}
					else if(clickedButton(quitGame, clickRect)){
						quit = 1;
					}
					else if(clickedButton(boardd, clickRect)){
						//while(boardd->size != boardsize)
						//	removeChild(boardd);
						drawBoard();
						column = klickationCol(clickRect);
						row = klickationRow(clickRect);
						if(column >= 'a' && column <= 'h' && row >= 1 && row <= 8){
							back = setOnePieceGui();
							temp = chessBoard[column - 'a'][row - 1].player;
							chessBoard[column - 'a'][row - 1].player = back;
							if(temp != EMPTY)
								updateCells(temp, -1); 
							if(back != EMPTY)
								updateCells(back, 1); 
							drawBoard();
						}
					}
					else if(clickedButton(startGame, clickRect)){
						if(wrongBoard() == 1){
							wrongBoard2Rect = geneRect(42, 195, 576, 251);
							wrongBoard2 = (Control*) malloc(sizeof(Control));
							if(wrongBoard2 == NULL){
								perror("Error: standard function malloc has failed");
								return 0;
							}
							wrongBoard2 = createImage(wrongBoard2Rect, "GUI_components/wrongBoard.png");	
							addChild(boardd, wrongBoard2);	
							drawPanel(boardd, screen);
							SDL_Flip(screen);		
							flag = 1;
							continue;
						}
						updateKings();
						return 1;
					}
			}
		}
	}
	return 1;
}

char setOnePieceGui(){
	// Auxiliary function for userTurnGui()
	// this functions opens a dialog in a case of a promotion, and the user can choose a kind of piece that the pawn will be promoted to

	Control *setGame, *setGamePage, *removeGame, *pawnGameW, *knightGameW, *bishopGameW, *rookGameW, *queenGameW, *kingGameW;
	Control *pawnGameB, *knightGameB, *bishopGameB, *rookGameB, *queenGameB, *kingGameB;
	SDL_Rect setGameRect, removeGameRect, pawnGameWRect, knightGameWRect, bishopGameWRect, rookGameWRect, queenGameWRect, kingGameWRect, clickRect;
	SDL_Rect pawnGameBRect, knightGameBRect, bishopGameBRect, rookGameBRect, queenGameBRect, kingGameBRect;

	setGame = (Control*) malloc(sizeof(Control));
	if(setGame == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	setGameRect = geneRect(85, 130, 550, 380);
	setGame = createPanel(setGameRect);

	setGamePage = (Control*) malloc(sizeof(Control));
	if(setGamePage == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	setGamePage = createImage(setGameRect, "GUI_components/setPiece.png");	
	addChild(setGame, setGamePage);


	removeGame = (Control*) malloc(sizeof(Control));
	if(removeGame == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	removeGameRect = geneRect(300, 450, 150, 39);
	removeGame = createImage(removeGameRect, "GUI_components/emptyRemove.png");	
	addChild(setGame, removeGame);


	pawnGameW = (Control*) malloc(sizeof(Control));
	if(pawnGameW == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	pawnGameWRect = geneRect(100, 230, 80, 80);
	pawnGameW = createImage(pawnGameWRect, "GUI_components/WhitePawn.png");	
	addChild(setGame, pawnGameW);

	pawnGameB = (Control*) malloc(sizeof(Control));
	if(pawnGameB == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	pawnGameBRect = geneRect(100 , 320, 80, 80);
	pawnGameB = createImage(pawnGameBRect, "GUI_components/BlackPawn.png");	
	addChild(setGame, pawnGameB);


	knightGameW = (Control*) malloc(sizeof(Control));
	if(knightGameW == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	knightGameWRect = geneRect(190, 230, 80, 80);
	knightGameW = createImage(knightGameWRect, "GUI_components/WhiteKnight.png");	
	addChild(setGame, knightGameW);

	knightGameB = (Control*) malloc(sizeof(Control));
	if(knightGameB == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	knightGameBRect = geneRect(190, 320, 80, 80);
	knightGameB = createImage(knightGameBRect, "GUI_components/BlackKnight.png");	
	addChild(setGame, knightGameB);

	bishopGameW = (Control*) malloc(sizeof(Control));
	if(bishopGameW == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	bishopGameWRect = geneRect(280, 230, 80, 80);
	bishopGameW = createImage(bishopGameWRect, "GUI_components/WhiteBishop.png");	
	addChild(setGame, bishopGameW);

	bishopGameB = (Control*) malloc(sizeof(Control));
	if(bishopGameB == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	bishopGameBRect = geneRect(280, 320, 80, 80);
	bishopGameB = createImage(bishopGameBRect, "GUI_components/BlackBishop.png");	
	addChild(setGame, bishopGameB);

	rookGameW = (Control*) malloc(sizeof(Control));
	if(rookGameW == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	rookGameWRect = geneRect(370, 230, 80, 80);
	rookGameW = createImage(rookGameWRect, "GUI_components/WhiteRook.png");	
	addChild(setGame, rookGameW);

	rookGameB = (Control*) malloc(sizeof(Control));
	if(rookGameB == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	rookGameBRect = geneRect(370, 320, 80, 80);
	rookGameB = createImage(rookGameBRect, "GUI_components/BlackRook.png");	
	addChild(setGame, rookGameB);


	queenGameW = (Control*) malloc(sizeof(Control));
	if(queenGameW == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	queenGameWRect = geneRect(460, 230, 80, 80);
	queenGameW = createImage(queenGameWRect, "GUI_components/WhiteQueen.png");	
	addChild(setGame, queenGameW);

	queenGameB = (Control*) malloc(sizeof(Control));
	if(queenGameB == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	queenGameBRect = geneRect(460, 320, 80, 80);
	queenGameB = createImage(queenGameBRect, "GUI_components/BlackQueen.png");	
	addChild(setGame, queenGameB);
	
	kingGameW = (Control*) malloc(sizeof(Control));
	if(kingGameW == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	kingGameWRect = geneRect(550 , 230, 80, 80);
	kingGameW = createImage(kingGameWRect, "GUI_components/WhiteKing.png");		
	addChild(setGame, kingGameW);

	kingGameB = (Control*) malloc(sizeof(Control));
	if(kingGameB == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	kingGameBRect = geneRect(550 , 320, 80, 80);
	kingGameB = createImage(kingGameBRect, "GUI_components/BlackKing.png");		
	addChild(setGame, kingGameB);

	drawPanel(setGame, screen);
	SDL_Flip(screen);

	while(cancel == 0 && quit == 0){
		while (SDL_PollEvent(&e) != 0){
			switch (e.type) {
				case (SDL_QUIT):
					quit = 1;
					break;
				case (SDL_MOUSEBUTTONDOWN):
					clickRect = geneRect(e.button.x, e.button.y, 0, 0);

					if(clickedButton(pawnGameW, clickRect)){
						freePanel(setGame);
						return WHITE_P;
					}
					if(clickedButton(pawnGameB, clickRect)){
						freePanel(setGame);
						return BLACK_P;
					}
					if(clickedButton(bishopGameW, clickRect)){
						freePanel(setGame);
						return WHITE_B;
					}					
					if(clickedButton(bishopGameB, clickRect)){
						freePanel(setGame);
						return BLACK_B;
					}
					if(clickedButton(knightGameW, clickRect)){
						freePanel(setGame);
						return WHITE_N;
					}
					if(clickedButton(knightGameB, clickRect)){
						freePanel(setGame);
						return BLACK_N;
					}
					if(clickedButton(rookGameW, clickRect)){
						freePanel(setGame);
						return WHITE_R;
					}
					if(clickedButton(rookGameB, clickRect)){
						freePanel(setGame);
						return BLACK_R;
					}
					if(clickedButton(queenGameW, clickRect)){
						freePanel(setGame);
						return WHITE_Q;
					}					
					if(clickedButton(queenGameB, clickRect)){
						freePanel(setGame);
						return BLACK_Q;
					}
					if(clickedButton(kingGameW, clickRect)){
						freePanel(setGame);
						return WHITE_K;
					}
					if(clickedButton(kingGameB, clickRect)){
						freePanel(setGame);
						return BLACK_K;
					}
					if(clickedButton(removeGame, clickRect)){
						freePanel(setGame);
						return EMPTY; 
					}
			}
		}
	}
	freePanel(setGame);
	return EMPTY; 
}


void drawBoard(){
	// this function draws the updated board on the screen
	SDL_Rect newRect;
	Control *boardd, *piece;
	int i,j;

	// we want to remove the previous board state and then to create a new one by running through chessBoard
	boardd = gameWindow->children[0];
	while(boardd->size != 1)
		removeChild(boardd);

	piece = (Control*) malloc(sizeof(Control));
	for(i = 0; i < BOARD_SIZE; i++){
		for(j = 0; j < BOARD_SIZE; j++){
			if((chessBoard[i][j]).player == WHITE_P){
				newRect = geneRect(80*i, 640 - 80*(j + 1), 80, 80);
				piece = createButton(newRect, "GUI_components/WhitePawn.png");
				addChild(boardd, piece);
            }
			else if((chessBoard[i][j]).player == WHITE_B){
				newRect = geneRect(80*i, 640 - 80*(j + 1), 80, 80);
				piece = createButton(newRect, "GUI_components/WhiteBishop.png");
				addChild(boardd, piece);
			}
			else if((chessBoard[i][j]).player == WHITE_N){
				newRect = geneRect(80*i, 640 - 80*(j + 1), 80, 80);
				piece = createButton(newRect, "GUI_components/WhiteKnight.png");
				addChild(boardd, piece);
			}
			else if((chessBoard[i][j]).player == WHITE_R){
				newRect = geneRect(80*i, 640 - 80*(j + 1), 80, 80);
				piece = createButton(newRect, "GUI_components/WhiteRook.png");
				addChild(boardd, piece);			
			}
			else if((chessBoard[i][j]).player == WHITE_Q){
				newRect = geneRect(80*i, 640 - 80*(j + 1), 80, 80);
				piece = createButton(newRect, "GUI_components/WhiteQueen.png");
				addChild(boardd, piece);
			}
			else if((chessBoard[i][j]).player == WHITE_K){
				newRect = geneRect(80*i, 640 - 80*(j + 1), 80, 80);
				piece = createButton(newRect, "GUI_components/WhiteKing.png");
				addChild(boardd, piece);
			}
			else if((chessBoard[i][j]).player == BLACK_P){
				newRect = geneRect(80*i, 640 - 80*(j + 1), 80, 80);
				piece = createButton(newRect, "GUI_components/BlackPawn.png");
				addChild(boardd, piece);
            }
			else if((chessBoard[i][j]).player == BLACK_B){
				newRect = geneRect(80*i, 640 - 80*(j + 1), 80, 80);
				piece = createButton(newRect, "GUI_components/BlackBishop.png");
				addChild(boardd, piece);
			}
			else if((chessBoard[i][j]).player == BLACK_N){
				newRect = geneRect(80*i, 640 - 80*(j + 1), 80, 80);
				piece = createButton(newRect, "GUI_components/BlackKnight.png");
				addChild(boardd, piece);
			}
			else if((chessBoard[i][j]).player == BLACK_R){
				newRect = geneRect(80*i, 640 - 80*(j + 1), 80, 80);
				piece = createButton(newRect, "GUI_components/BlackRook.png");
				addChild(boardd, piece);
			}
			else if((chessBoard[i][j]).player == BLACK_Q){
				newRect = geneRect(80*i, 640 - 80*(j + 1), 80, 80);
				piece = createButton(newRect, "GUI_components/BlackQueen.png");
				addChild(boardd, piece);
			}
			else if((chessBoard[i][j]).player == BLACK_K){
				newRect = geneRect(80*i, 640 - 80*(j + 1), 80, 80);
				piece = createButton(newRect, "GUI_components/BlackKing.png");
				addChild(boardd, piece);
			}
		}
	}
	drawPanel(boardd, screen);
	SDL_Flip(screen);
	return;

}


int settingsStateGui(){
	 //this function gets commands (by mouse selection) from the user as an input and sets initial setting for the game, before the game starts.
     //returns 0 if the programd doesn't succes to allocate some component , otherwise returns 1;
	Control *menu, *menuPage, *pvspGame, *pvscGame, *cancelGame;
	SDL_Rect menuRect, pvspGameRect, pvscGameRect, cancelRect, clickRect;
	cancel = 0;

	// initializing and creating all the essential controls to run the player selection window, which contains a starting menu of the game options
	menuRect = geneRect(230, 100, 340, 400);
	menu = (Control*) malloc(sizeof(Control));
	if(menu == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	} 
	menuPage = (Control*) malloc(sizeof(Control));
	if(menuPage == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	} 

	menu = createPanel(menuRect);
	menuPage = createImage(menuRect, "GUI_components/menuPage2.png");
	addChild(menu, menuPage);

	pvspGame = (Control*) malloc(sizeof(Control));
	if(pvspGame == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	pvspGameRect = geneRect(MENU2_BUTTON_X,MENU2_BUTTON_Y,MENU2_BUTTON_W,MENU2_BUTTON_H);
	pvspGame = createButton(pvspGameRect, "GUI_components/pvspGame.png");
	addChild(menu, pvspGame);

	pvscGame = (Control*) malloc(sizeof(Control));
	if(pvscGame == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	pvscGameRect = geneRect(MENU2_BUTTON_X,MENU2_BUTTON_Y + 70,MENU2_BUTTON_W,MENU2_BUTTON_H);
	pvscGame = createButton(pvscGameRect, "GUI_components/pvscGame.png");
	addChild(menu, pvscGame);

	cancelGame = (Control*) malloc(sizeof(Control));
	if(cancelGame == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	cancelRect = geneRect(MENU2_BUTTON_X,MENU2_BUTTON_Y + 140,MENU2_BUTTON_W,MENU2_BUTTON_H);
	cancelGame = createButton(cancelRect, "GUI_components/cancelGame.png");
	addChild(menu, cancelGame);

	drawPanel(menu, screen);
	SDL_Flip(screen);
	while(cancel == 0 && quit == 0){
		// Poll for keyboard & mouse events
		while (SDL_PollEvent(&e) != 0){
			switch (e.type) {
				case (SDL_QUIT):
					quit = 1;
					break;
				case (SDL_MOUSEBUTTONDOWN):
					clickRect = geneRect(e.button.x ,e.button.y, 0, 0);	
					if(clickedButton(pvspGame, clickRect)){ //if the user chose the Player vs. Player mode
						gameMode = 1;
						vsPlayerStateGui();
					}
					if(clickedButton(pvscGame, clickRect)){ //if the user chose the Player vs. Computer mode
						gameMode = 2;
						pvscSettings();
					}
					cancel = 1;
					if(clickedButton(cancelGame, clickRect)){ //if user chose to go back to the main menu
						cancel = 1;
						gameMode = 1;
					}
					break;
			}
		}
	}
	
	//freeing the resources of the menu panel
	freePanel(menu);
	return 1;
}

int playStateGui(){
	// Auxiliary function for vsPlayerStateGui() and vsCompStateGui()
	// this function prepares the program to run the game - it creates the board and the side menu components and all the buttons we need
	// in order to run the game
	//int i;
	Control *board, *boardPage, *sideMenu, *sideMenuPage, *saveGame, *mainMenu, *quitGame;
	SDL_Rect sideMenuRect, boardRect, saveGameRect, mainMenuRect, quitGameRect;

	gameWindow = (Control*) malloc(sizeof(Control));
	if(gameWindow == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	gameWindow = createPanel(screenOffset);

	board = (Control*) malloc(sizeof(Control));
	if(board == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}

	boardPage = (Control*) malloc(sizeof(Control));
	if(boardPage == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}

	boardRect = geneRect(0,0,640,640);
	board = createPanel(boardRect);
	boardPage = createImage(boardRect, "GUI_components/boardPerSize.png");

	sideMenu = (Control*) malloc(sizeof(Control));
	if(sideMenu == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}

	sideMenuPage = (Control*) malloc(sizeof(Control));
	if(sideMenuPage == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	addChild(board, boardPage);
	sideMenuRect = geneRect(640,0,160,640);
	sideMenu = createPanel(sideMenuRect);
	sideMenuPage = createImage(sideMenuRect, "GUI_components/sideMenu.png");
	addChild(sideMenu, sideMenuPage);
	saveGame = (Control*) malloc(sizeof(Control));
	if(saveGame == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	saveGameRect = geneRect(655, 80, 130, 39);
	saveGame = createButton(saveGameRect, "GUI_components/saveGame.png");
	addChild(sideMenu, saveGame);

	mainMenu = (Control*) malloc(sizeof(Control));
	if(mainMenu == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	mainMenuRect = geneRect(655, 140, 130, 39);
	mainMenu = createButton(mainMenuRect, "GUI_components/mainMenu.png");
	addChild(sideMenu, mainMenu);

	quitGame = (Control*) malloc(sizeof(Control));
	if(quitGame == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	quitGameRect = geneRect(657, 370, 130, 35);
	quitGame = createButton(quitGameRect, "GUI_components/quitMenu.png");
	addChild(sideMenu, quitGame);
	
	addChild(gameWindow, board);
	addChild(gameWindow, sideMenu);

	drawPanel(gameWindow, screen);
	SDL_Flip(screen);

	if(loaded == 0)
		firstPlayerGui();
	
	updateKings();

	return 1;

}

int pvscSettings(){
	// Auxiliary function for playStateGui()
	// this function gives the user the ability to choose the diffiulty and his color in the game VS the Al
	//returns 0 in the case of allocation error when using the malloc function , otherwise return 1;
	int okey = 0, colFlag = 0, levelFlag = 0;
	Control *optionsGame, *optionsGamePage, *chooseGame, *whiteGame, *blackGame, *okGame, *cancelGame; 
	Control *levelBestGame, *level1Game, *level2Game, *level3Game, *level4Game;
	SDL_Rect optionsGameRect, chooseGameRect, whiteGameRect, blackGameRect, okGameRect, cancelGameRect, clickRect;
	SDL_Rect levelBestGameRect, level1GameRect, level2GameRect, level3GameRect, level4GameRect;

	optionsGame = (Control*) malloc(sizeof(Control));
	if(optionsGame == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	optionsGameRect = geneRect(230, 100, 340, 400);
	optionsGame = createPanel(optionsGameRect);

	optionsGamePage = (Control*) malloc(sizeof(Control));
	if(optionsGamePage == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	optionsGamePage = createImage(optionsGameRect, "GUI_components/menuPage2.png");	
	addChild(optionsGame, optionsGamePage);

	chooseGameRect = geneRect(240, 120, 304, 32);
	chooseGame = (Control*) malloc(sizeof(Control));
	if(chooseGame == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	chooseGame = createImage(chooseGameRect, "GUI_components/chooseColDiff.png");	
	addChild(optionsGame, chooseGame);

	whiteGameRect = geneRect(290, 175, 81, 38);
	whiteGame = (Control*) malloc(sizeof(Control));
	if(whiteGame == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	whiteGame = createImage(whiteGameRect, "GUI_components/whiteGame.png");	
	addChild(optionsGame, whiteGame);

	blackGameRect = geneRect(390, 175, 82, 38);
	blackGame = (Control*) malloc(sizeof(Control));
	if(blackGame == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	blackGame = createImage(blackGameRect, "GUI_components/blackGame.png");	
	addChild(optionsGame, blackGame);

	level1GameRect = geneRect(235, 240, 75, 28);
	level1Game = (Control*) malloc(sizeof(Control));
	if(level1Game == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	level1Game = createImage(level1GameRect, "GUI_components/level1.png");	
	addChild(optionsGame, level1Game);

	level2GameRect = geneRect(320, 240, 75, 25);
	level2Game = (Control*) malloc(sizeof(Control));
	if(level2Game == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	level2Game = createImage(level2GameRect, "GUI_components/level2.png");	
	addChild(optionsGame, level2Game);

	level3GameRect = geneRect(405, 240, 75, 25);
	level3Game = (Control*) malloc(sizeof(Control));
	if(level3Game == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	level3Game = createImage(level3GameRect, "GUI_components/level3.png");	
	addChild(optionsGame, level3Game);

	level4GameRect = geneRect(480, 240, 75, 25);
	level4Game = (Control*) malloc(sizeof(Control));
	if(level2Game == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	level4Game = createImage(level4GameRect, "GUI_components/level4.png");	
	addChild(optionsGame, level4Game);

	levelBestGameRect = geneRect(343, 265, 114, 50);
	levelBestGame = (Control*) malloc(sizeof(Control));
	if(levelBestGame == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	levelBestGame = createImage(levelBestGameRect, "GUI_components/levelBest.png");	
	addChild(optionsGame, levelBestGame);

	okGameRect = geneRect(280, 340, 70, 35);
	okGame = (Control*) malloc(sizeof(Control));
	if(okGame == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	okGame = createImage(okGameRect, "GUI_components/okGame.png");	
	addChild(optionsGame, okGame);

	cancelGameRect = geneRect(410, 340, 96, 35);
	cancelGame = (Control*) malloc(sizeof(Control));
	if(cancelGame == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	cancelGame = createImage(cancelGameRect, "GUI_components/cancelGame2.png");	
	addChild(optionsGame, cancelGame);

	drawPanel(optionsGame, screen);
	SDL_Flip(screen);

	while(okey == 0 && cancel == 0 && quit == 0){
		while (SDL_PollEvent(&e) != 0){
			switch (e.type) {
				case (SDL_QUIT):
					quit = 1;
					break;
				case (SDL_MOUSEBUTTONDOWN):
					if(quit == 0){
						clickRect = geneRect(e.button.x ,e.button.y, 0, 0);	
						if(clickedButton(whiteGame, clickRect)){
							colFlag = 1;
							userColor = WHITE;
							// possible improvement : to make the selection visible, by V on it or a circle
						}
						else if(clickedButton(blackGame, clickRect)){
							colFlag = 1;
							userColor = BLACK;
							// possible improvement : to make the selection visible, by V on it or a circle
						}	
						else if(clickedButton(level1Game, clickRect)){
							levelFlag = 1;
							miniMaxDepth = 1;
							// possible improvement : to make the selection visible, by V on it or a circle
						}
													
						else if(clickedButton(level2Game, clickRect)){
							levelFlag = 1;
							miniMaxDepth = 2;
							// possible improvement : to make the selection visible, by V on it or a circle
						}
											
						else if(clickedButton(level3Game, clickRect)){
							levelFlag = 1;
							miniMaxDepth = 3;
							// possible improvement : to make the selection visible, by V on it or a circle
						}
										
						else if(clickedButton(level4Game, clickRect)){
							levelFlag = 1;
							miniMaxDepth = 4;
							// possible improvement : to make the selection visible, by V on it or a circle
						}

						else if(clickedButton(levelBestGame, clickRect)){
							levelFlag = 1;
							BEST = 1;
							// possible improvement : to make the selection visible, by V on it or a circle
						}

						else if(clickedButton(okGame, clickRect)){
							if(levelFlag > 0 && colFlag > 0){
								//freePanel(optionsGame);
								//closeFlag = 1;
								vsCompStateGui();
								cancel = 1;
								// possible improvement : to make the selection visible, by V on it or a circle
							}
							break;
						}

						else if(clickedButton(cancelGame, clickRect)){
							miniMaxDepth = 1;
							userColor = WHITE;
							BEST = 0;
							cancel = 1;
							break;
						}
					}
			}
		}
	}
	freePanel(optionsGame);
	return 1;
}

int userTurnGui(char COLOR){
	//Auxiliary function for vsPlayerStateGui and vsCompStateGui - in this function we simulate an user turn 
	// which may make a move, to ask for the possible moves, save the game at the current point, go back to the main menu and much more
	//the function outputs 1 if the user loses, 0 if the user quits, 
    //2 if the user asked for possbile moves or typed an illegal command (then it still the user turn)
    //and 3 otherwise

	int getM = 0, row, boardsize = 33, tempr, counter = 0, bestM, tempMax,temps = 0;
	char column, tempc,p,promotion,tt;
	Move* curr = head, *userMove = NULL, *tempMove = NULL;
	//Location loc;
	Control *getMoves, *getBestMove, *targetGame;
	Control *saveGame = (gameWindow->children[1])->children[1];
	Control *mainMenuGame = (gameWindow->children[1])->children[2];
	Control *quitGame = (gameWindow->children[1])->children[3];
	Control *boardd = gameWindow->children[0];

	SDL_Rect clickRect, getMovesRect, getBestMoveRect, targetGameRect;
	

	if(cancel != 1)
		drawBoard();

	if(resultStateGui(COLOR) == 1){
		cancel = 1;
		return 1;
	}
	
	getMovesRect = geneRect(640, 470, 160, 58);
	getMoves = (Control*) malloc(sizeof(Control));
	if(getMoves == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	getMoves = createImage(getMovesRect, "GUI_components/getMoves.png");
	addChild(gameWindow->children[1],getMoves);

	getBestMoveRect = geneRect(640, 550, 160, 44);
	getBestMove = (Control*) malloc(sizeof(Control));
	if(getBestMove == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	getBestMove = createImage(getBestMoveRect, "GUI_components/getBestMove.png");
	addChild(gameWindow->children[1],getBestMove);
	drawPanel(gameWindow->children[1], screen);
	SDL_Flip(screen);

	targetGame = (Control*) malloc(sizeof(Control));
	if(targetGame == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}	

	while(cancel == 0 && quit == 0){
		while (SDL_PollEvent(&e) != 0){
			switch (e.type) {
				case (SDL_QUIT):
					quit = 1;
					break;
				case (SDL_MOUSEBUTTONDOWN):
					clickRect = geneRect(e.button.x, e.button.y, 0, 0);
					if(clickedButton(mainMenuGame, clickRect)){
						init_board(); 
						gameMode = 1;
						userColor = WHITE;
						firstPlayer = WHITE;
						miniMaxDepth = 1;
						BEST = 0;
						cancel = 1;
					}
					else if(clickedButton(saveGame, clickRect)){
						saveStateGui(COLOR);
						drawPanel(gameWindow, screen);
						SDL_Flip(screen);
					}
					else if(clickedButton(quitGame, clickRect)){
						quit = 1;
					}
					else if(clickedButton(boardd, clickRect)){
						//while(boardd->size != boardsize)
						//	removeChild(boardd);
						drawBoard();
						column = klickationCol(clickRect);
						row = klickationRow(clickRect);
						if(column >= 'a' && column <= 'h' && row >= 1 && row <= 8){
							if(checkerColor(column,row) == COLOR){
								getM = 1;
								tempc = column;
								tempr = row;
								boardsize = boardd->size;
								targetGameRect = geneRect((column - 'a') * 80, (8 - row) * 80 , 80, 80); 
								targetGame = createImage(targetGameRect, "GUI_components/Target.png");
								addChild(boardd, targetGame);
								drawPanel(boardd, screen);
								SDL_Flip(screen);
							}
							else if(checkerColor(column,row) != COLOR){
								if(getM == 1){
									possibleMoves(tempc,tempr);
									tempMove = NULL;
									userMove = NULL;
									curr = head;
									p = chessBoard[tempc - 'a'][tempr - 1].player;
									
									while(curr != NULL){
										if((curr->to).column == column && (curr->to).row == row){
											tempMove = curr;
											break;
										}
										if((p == WHITE_K || p == BLACK_K ) && (curr->from).column == 'a' && (curr->to).column == 'd'){
											tempMove = curr;
											temps = 1;
										}
										if((p == WHITE_K || p == BLACK_K ) && (curr->from).column == 'h' && (curr->to).column == 'f'){
											tempMove = curr;
											temps = 1;
										}
										if(temps == 1)
											break;
										curr = curr->next;
									}
									if(tempMove == NULL){
										getM = 0;
										continue;
									}

									curr = head;
									if(head == NULL){
										continue;
									}
									
									while (curr != NULL) {
										if((curr->to).row == row && (curr->to).column == column){
											if((p == WHITE_P && tempr == 7) || (p == BLACK_P && tempr == 2)){
												  promotion = promotionGui(COLOR);
												  userMove = doesTheListContainThisMove(tempc, tempr, column, row, promotion, 0);
												  temps = 1;
											}
											else if(curr->castle != NULL){
												if(p  == WHITE_R || p == BLACK_R){
													curr = curr->next;
													continue;
												}
											}
											else{
												userMove = doesTheListContainThisMove(tempc, tempr, column, row, 0, 0);
												temps = 1;
											}
										}
										if(temps == 1){
											if(userMove == NULL)
												applyMove(*tempMove);
											else
												applyMove(*userMove);
											drawBoard();
											counter++;
											getM = 0;
											return 3;
										}
										curr = curr->next;
									}
									getM = 0;
									if(counter != 0){
										while(boardd->size != boardsize)
											removeChild(boardd);
										drawPanel(boardd, screen);
										SDL_Flip(screen);
									}
									counter = 0;
								}
							}
						}
						else{
							getM = 0;
							continue;
						}
					}
					else if(clickedButton(getMoves, clickRect)){
						if(getM == 1){
							if(checkerColor(column,row) == COLOR){
								possibleMoves(column,row);
								curr = head;
								if(head == NULL)
									continue;
								
								while (curr != NULL) {

									if(curr->castle == NULL){
										targetGameRect = geneRect(((curr->to).column - 'a') * 80, (8 -(curr->to).row) * 80 , 80, 80); 
										targetGame = createImage(targetGameRect, "GUI_components/Target.png");
										addChild(boardd, targetGame);
										curr = curr->next;
									}
									else if(curr->castle != NULL){
										tt = chessBoard[column - 'a'][row - 1].player;
										if(tt == WHITE_K || tt == BLACK_K){
											if((curr->from).column == 'a'){
												targetGameRect = geneRect(((curr->to).column - 'a' - 1) * 80, (8 -(curr->to).row) * 80 , 80, 80); 
												targetGame = createImage(targetGameRect, "GUI_components/Target.png");
												addChild(boardd, targetGame);
											}
											else if((curr->from).column == 'h'){
												targetGameRect = geneRect(((curr->to).column - 'a' + 1) * 80, (8 -(curr->to).row) * 80 , 80, 80); 
												targetGame = createImage(targetGameRect, "GUI_components/Target.png");
												addChild(boardd, targetGame);
											}
										}
										curr = curr->next;
									}
								}
							}	
						}
						drawPanel(boardd, screen);
						SDL_Flip(screen);

						getM = 1;
					}
					else if(clickedButton(getBestMove, clickRect)){

						
						tempMax = miniMaxDepth;
						bestM = bestMoveGui(COLOR);
						
						drawBoard();				

						if(bestM == 5)
							updateBestDepth(&miniMaxDepth, COLOR);

						else
							miniMaxDepth = bestM;

						minimax(COLOR,NULL);

						curr = head;

						boardsize = boardd->size;
						
						targetGameRect = geneRect(((curr->from).column - 'a') * 80, (8 -(curr->from).row) * 80 , 80, 80); 
						targetGame = createImage(targetGameRect, "GUI_components/Target.png");
						addChild(boardd, targetGame);
						
						targetGameRect = geneRect(((curr->to).column - 'a') * 80, (8 -(curr->to).row) * 80 , 80, 80); 
						targetGame = createImage(targetGameRect, "GUI_components/Target.png");
						addChild(boardd, targetGame);
						
						drawPanel(boardd, screen);
						SDL_Flip(screen);

						miniMaxDepth = tempMax;
						BEST = 0;

						tempc = (curr->from).column;
						tempr = (curr->from).row;
						column = (curr->to).column;
						row = (curr->to).row;

						getM = 1;
	
					}

					if(cancel == 1 || quit == 1)
						break;
			}
		}
	}

	return 3;
}


int compTurnGui(char COLOR){
	// Auxiliary function for vsCompStateGui - in this function we simulate a computer turn turn 
	// the computer does a single move by using the minimax algorithm with respect to the difficulty the user chose
	//returns ..

	drawBoard();

	if(resultStateGui(COLOR) == 1){
		cancel = 1;
		return 1;
	}
    
    if(BEST == 1)
          updateBestDepth(&miniMaxDepth, COLOR);
    minimax(COLOR,NULL);
    applyMove(*head);
	drawBoard();

	return 3;
}

char promotionGui(char COLOR){
	// Auxiliary function for userTurnGui()
	// this functions opens a dialog in a case of a promotion, and the user can choose a kind of piece that the pawn will be promoted to

	Control *promotorGame, *promotorGamePage, *knightGame, *bishopGame, *rookGame, *queenGame;
	SDL_Rect promotorGameRect, knightGameRect, bishopGameRect, rookGameRect, queenGameRect, clickRect;

	promotorGame = (Control*) malloc(sizeof(Control));
	if(promotorGame == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	promotorGameRect = geneRect(95, 163, 450, 315);
	promotorGame = createPanel(promotorGameRect);

	promotorGamePage = (Control*) malloc(sizeof(Control));
	if(promotorGamePage == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	promotorGamePage = createImage(promotorGameRect, "GUI_components/promote.png");	
	addChild(promotorGame, promotorGamePage);

	knightGame = (Control*) malloc(sizeof(Control));
	if(knightGame == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	knightGameRect = geneRect(120, 280, 80, 80);
	if(COLOR == WHITE)
		knightGame = createImage(knightGameRect, "GUI_components/WhiteKnight.png");	
	else if(COLOR == BLACK)
		knightGame = createImage(knightGameRect, "GUI_components/BlackKnight.png");	
	addChild(promotorGame, knightGame);

	bishopGame = (Control*) malloc(sizeof(Control));
	if(bishopGame == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	bishopGameRect = geneRect(225, 280, 80, 80);
	if(COLOR == WHITE)
		bishopGame = createImage(bishopGameRect, "GUI_components/WhiteBishop.png");	
	else if(COLOR == BLACK)
		bishopGame = createImage(bishopGameRect, "GUI_components/BlackBishop.png");	
	addChild(promotorGame, bishopGame);

	rookGame = (Control*) malloc(sizeof(Control));
	if(rookGame == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	rookGameRect = geneRect(330, 280, 80, 80);
	if(COLOR == WHITE)
		rookGame = createImage(rookGameRect, "GUI_components/WhiteRook.png");	
	else if(COLOR == BLACK)
		rookGame = createImage(rookGameRect, "GUI_components/BlackRook.png");	
	addChild(promotorGame, rookGame);

	queenGame = (Control*) malloc(sizeof(Control));
	if(queenGame == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	queenGameRect = geneRect(435, 280, 80, 80);
	if(COLOR == WHITE)
		queenGame = createImage(queenGameRect, "GUI_components/WhiteQueen.png");	
	else if(COLOR == BLACK)
		queenGame = createImage(queenGameRect, "GUI_components/BlackQueen.png");	
	addChild(promotorGame, queenGame);
	
	drawPanel(promotorGame, screen);
	SDL_Flip(screen);

	while(cancel == 0 && quit == 0){
		while (SDL_PollEvent(&e) != 0){
			switch (e.type) {
				case (SDL_QUIT):
					quit = 1;
					break;
				case (SDL_MOUSEBUTTONDOWN):
					clickRect = geneRect(e.button.x, e.button.y, 0, 0);
					if(clickedButton(bishopGame, clickRect)){
						if(COLOR == WHITE){
							freePanel(promotorGame);
							return WHITE_B;
						}
						else if(COLOR == BLACK){
							freePanel(promotorGame);
							return BLACK_B;
						}
					}
					if(clickedButton(knightGame, clickRect)){
						if(COLOR == WHITE){
							freePanel(promotorGame);
							return WHITE_N;
						}
						else if(COLOR == BLACK){
							freePanel(promotorGame);
							return BLACK_N;
						}
					}
					if(clickedButton(rookGame, clickRect)){
						if(COLOR == WHITE){
							freePanel(promotorGame);
							return WHITE_R;
						}
						else if(COLOR == BLACK){
							freePanel(promotorGame);
							return BLACK_R;
						}
					}
					if(clickedButton(queenGame, clickRect)){
						if(COLOR == WHITE){
							freePanel(promotorGame);
							return WHITE_Q;
					}
						else if(COLOR == BLACK){
							freePanel(promotorGame);
							return BLACK_Q;
						}
					}
			}
		}
	}
	freePanel(promotorGame);
	return EMPTY;
}


int bestMoveGui(char COLOR){
	// Auxiliary function for userTurnGui()
	// this functions opens a dialog in a case of a promotion, and the user can choose a kind of piece that the pawn will be promoted to

	Control *bestMoveGame, *bestMoveGamePage, *oneGame, *twoGame, *threeGame, *fourGame, *bestGame;
	SDL_Rect bestMoveGameRect, oneGameRect, twoGameRect, threeGameRect, fourGameRect, bestGameRect, clickRect;

	bestMoveGame = (Control*) malloc(sizeof(Control));
	if(bestMoveGame == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	bestMoveGameRect = geneRect(95, 163, 450, 315);
	bestMoveGame = createPanel(bestMoveGameRect);

	bestMoveGamePage = (Control*) malloc(sizeof(Control));
	if(bestMoveGamePage == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	bestMoveGamePage = createImage(bestMoveGameRect, "GUI_components/bestMove.png");	
	addChild(bestMoveGame, bestMoveGamePage);

	oneGame = (Control*) malloc(sizeof(Control));
	if(oneGame == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	oneGameRect = geneRect(130, 270, 75, 25);
	oneGame = createImage(oneGameRect, "GUI_components/level1.png");	
	addChild(bestMoveGame, oneGame);

	twoGame = (Control*) malloc(sizeof(Control));
	if(twoGame == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	twoGameRect = geneRect(230, 270, 75, 25);
	twoGame = createImage(twoGameRect, "GUI_components/level2.png");		
	addChild(bestMoveGame, twoGame);

	threeGame = (Control*) malloc(sizeof(Control));
	if(threeGame == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	threeGameRect = geneRect(330, 270, 75, 25);
	threeGame = createImage(threeGameRect, "GUI_components/level3.png");		
	addChild(bestMoveGame, threeGame);

	fourGame = (Control*) malloc(sizeof(Control));
	if(fourGame == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	fourGameRect = geneRect(430, 270, 75, 25);
	fourGame = createImage(fourGameRect, "GUI_components/level4.png");		
	addChild(bestMoveGame, fourGame);

	bestGame = (Control*) malloc(sizeof(Control));
	if(bestGame == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	bestGameRect = geneRect(250, 310, 114, 50);
	bestGame = createImage(bestGameRect, "GUI_components/levelBest.png");		
	addChild(bestMoveGame, bestGame);
	
	drawPanel(bestMoveGame, screen);
	SDL_Flip(screen);

	while(cancel == 0 && quit == 0){
		while (SDL_PollEvent(&e) != 0){
			switch (e.type) {
				case (SDL_QUIT):
					quit = 1;
					break;
				case (SDL_MOUSEBUTTONDOWN):
					clickRect = geneRect(e.button.x, e.button.y, 0, 0);
					if(clickedButton(oneGame, clickRect)){
						freePanel(bestMoveGame);
						return 1;
					}
					if(clickedButton(twoGame, clickRect)){
						freePanel(bestMoveGame);
						return 2;
					}
					if(clickedButton(threeGame, clickRect)){
						freePanel(bestMoveGame);
						return 3;
					}					
					if(clickedButton(fourGame, clickRect)){
						freePanel(bestMoveGame);
						return 4;
					}
					if(clickedButton(bestGame, clickRect)){
						freePanel(bestMoveGame);
						return 5;
					}
			}
		}
	}
	freePanel(bestMoveGame);
	return 1;
}


int vsPlayerStateGui(){
	//this function simulates the game between the user and the computer. Which player starts first is decided by the value of firstPlayer constant.
    //In each turn, the current player performs its move and //the game continues until one of the players wins
    //or until the game is stopped by one of the users.
    //returns 0 if performed a move , 1 if one of the players wins or its a tie or one of the users quits the game;
	int userMove;
	toSet = 0;
	playStateGui();
	//if(loaded == 0)
		//firstPlayerGui();
	toSetBoard();
	if(toSet == 1){
		setBoardGui();
		updateKings();
	}
	gameStarted = 1;
	colorGui = firstPlayer;
	//updateKings();
	while(quit == 0 && cancel == 0){
		userMove = userTurnGui(firstPlayer);
		if(userMove == 1){
			return 1;
		}
		userMove = userTurnGui(OPPOSITE_COLOR(firstPlayer));
		if(userMove == 1){
			return 1;
		}
	}
	return 1;
}

int vsCompStateGui(){
    //this function simulates the game between the user and the computer. Which player starts first is decided by the value of firstPlayer constant. 
    //In each turn, the current player performs its move and //the game continues until one of the players wins
    //or until the game is stopped by the user.
    //the function is used repeatedly by the main function until the user quits the program
    //returns 0 if performed a move , 1 if one of the players wins or the user quits the game;
	int move;
	toSet = 0;
	playStateGui();
	//if(loaded == 0)
		//firstPlayerGui();
	toSetBoard();
	if(toSet == 1)
		setBoardGui();
	colorGui = firstPlayer;
	gameStarted = 1;
	//updateKings();
	while(quit == 0 && cancel == 0){
		if(firstPlayer == userColor){
			move  = userTurnGui(firstPlayer);
			if(move == 1)
				return 1;
			move = compTurnGui(OPPOSITE_COLOR(firstPlayer));
			if(move == 1)
				return 1;
		}
		else{
			move = compTurnGui(firstPlayer);
			if(move == 1)
				return 1;
			move = userTurnGui(OPPOSITE_COLOR(firstPlayer));
			if(move == 1)
				return 1;
		}
	}
	return 1;
}


 /* TO REMEMBER - number of slots = 7 , but must to be variable, to create it dynamicly */
int saveStateGui(char color){
	 // Auxiliary function for mainGraphics
     // this function saves to the chosen slot the current state of the game
     // returns 1 on succes , 0 on failure

	int i = 0, j = 0, len, lenSlot, close = 0, num = 0;
	char temp1[30],temp2[10], temp3[10];
	Control *saveGame, *saveGamePage, *slotGame, *cancelGame;
	SDL_Rect saveGameRect, slotGameRect, cancelGameRect, clickRect;

	saveGame = (Control*) malloc(sizeof(Control));
	if(saveGame == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	saveGameRect = geneRect(0, 150, 640, 214);
	saveGame = createPanel(saveGameRect);

	saveGamePage = (Control*) malloc(sizeof(Control));
	if(saveGamePage == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	saveGamePage = createImage(saveGameRect, "GUI_components/savePage.png");	
	addChild(saveGame, saveGamePage);

	len = strlen("GUI_components/");
	if(slotsNum < 10)
		lenSlot = 9;
	else
		lenSlot = 10;
	temp1[len + lenSlot] = '\0';
	temp2[lenSlot] = '\0';
	
	//Dinamicly creating the slots, we use a constant "slotsNum" which tells us what is the actual number of the slots
	for(i = 1; i <= slotsNum; i++){
		slotGame = (Control*) malloc(sizeof(Control));
		if(slotGame == NULL){
			perror("Error: standard function malloc has failed");
			return 0;
		}
		slotGameRect = geneRect(15 + ((i - 1) * 70), 175, 60, 48);
		sprintf(temp1,"GUI_components/"); 
		sprintf(temp2, "slot%d.png",i);
		for(j = len ; j < len + lenSlot; j++){
			temp1[j] = temp2[j - len];
		}
		slotGame = createImage(slotGameRect, temp1);	
		addChild(saveGame, slotGame);
	}

	cancelGameRect = geneRect(250, 320, 96, 35);
	cancelGame = (Control*) malloc(sizeof(Control));
	if(cancelGame == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	cancelGame = createImage(cancelGameRect, "GUI_components/cancelGame2.png");	
	addChild(saveGame, cancelGame);	
	drawPanel(saveGame, screen);
	SDL_Flip(screen);	

	while(close == 0){
		while(SDL_PollEvent(&e) != 0){
			switch (e.type) {
				case (SDL_QUIT):
					quit = 1;
					close = 1;
					break;
				case (SDL_MOUSEBUTTONDOWN):
					clickRect = geneRect(e.button.x, e.button.y , 0, 0);
					if(clickedButton(cancelGame,clickRect) == 1){
						close = 1;
						break;
					}
					num = -1;
					for(i = 0 ; i < slotsNum ; i++)
						if(clickedButton(saveGame->children[i+1],clickRect) == 1)
							num = i;
					if(num == -1)
						continue;
					temp3[lenSlot] = '\0';
					sprintf(temp3, "slot%d.xml",num + 1);
					saveState(temp3, color);
					close = 1;
					break;
			}		
		}
	}
	freePanel(saveGame);
	return 1;
}



void loadStateGui(){
	 // Auxiliary function for mainGraphics
     // this function loads the state of the game that saved in the chosen slot
	 
	int i = 0, j = 0, len, lenSlot, close = 0, num = 0;
	char temp1[30],temp2[10], temp3[10];
	Control *loadGame, *loadGamePage, *slotGame, *cancelGame;
	SDL_Rect loadGameRect, slotGameRect, cancelGameRect, clickRect;

	loadGame = (Control*) malloc(sizeof(Control));
	if(loadGame == NULL){
		perror("Error: standard function malloc has failed");
		exit(0);
	}
	loadGameRect = geneRect(0, 150, 800, 267);
	loadGame = createPanel(loadGameRect);

	loadGamePage = (Control*) malloc(sizeof(Control));
	if(loadGamePage == NULL){
		perror("Error: standard function malloc has failed");
		exit(0);
	}
	loadGamePage = createImage(loadGameRect, "GUI_components/loadPage.png");	
	addChild(loadGame, loadGamePage);

	len = strlen("GUI_components/");
	if(slotsNum < 10)
		lenSlot = 9;
	else
		lenSlot = 10;
	temp1[len + lenSlot] = '\0';
	temp2[lenSlot] = '\0';
	
	//Dinamicly creating the slots, we use a constant "slotsNum" which tells us what is the actual number of the slots
	for(i = 1; i <= slotsNum; i++){
		slotGame = (Control*) malloc(sizeof(Control));
		if(slotGame == NULL){
			perror("Error: standard function malloc has failed");
			exit(0);
		}
		slotGameRect = geneRect(30 + ((i - 1) * 80), 175, 60, 48);
		sprintf(temp1,"GUI_components/"); 
		sprintf(temp2, "slot%d.png",i);
		for(j = len ; j < len + lenSlot; j++){
			temp1[j] = temp2[j - len];
		}
		slotGame = createImage(slotGameRect, temp1);	
		addChild(loadGame, slotGame);
	}

	cancelGameRect = geneRect(352, 320, 96, 35);
	cancelGame = (Control*) malloc(sizeof(Control));
	if(cancelGame == NULL){
		perror("Error: standard function malloc has failed");
		exit(0);
	}
	cancelGame = createImage(cancelGameRect, "GUI_components/cancelGame2.png");	
	addChild(loadGame, cancelGame);	
	drawPanel(loadGame, screen);
	SDL_Flip(screen);	

	while(close == 0){
		while(SDL_PollEvent(&e) != 0){
			switch (e.type) {
				case (SDL_QUIT):
					quit = 1;
					close = 1;
					break;
				case (SDL_MOUSEBUTTONDOWN):
					clickRect = geneRect(e.button.x, e.button.y , 0, 0);
					if(clickedButton(cancelGame,clickRect) == 1){
						close = 1;
						break;
					}
					num = -1;
					for(i = 1 ; i <= slotsNum ; i++)
						if(clickedButton(loadGame->children[i],clickRect) == 1){
							num = i;
							break;
						}
					if(num == -1)
						continue;
					temp3[lenSlot] = '\0';
					sprintf(temp3, "slot%d.xml",num);
					if(fileExists(temp3) == 1){
						loadState(temp3);
						loaded = 1;
						if(gameMode == 1)
							vsPlayerStateGui();
						else if(gameMode == 2)
							vsCompStateGui();
					}
					else
						continue;
					close = 1;
					break;
			}		
		}
	}
	freePanel(loadGame);
}




/* TO REMEMBER - to use reportStatus() from chessprog */
int resultStateGui(char COLOR){
	// Auxiliary function for vsCompStateGui and vsPlayerStateGui 
	// the input is the color 'w'/'b' or 't' in case of a TIE
	// this function announces if one of the players won the game, or that there is a tie
	// returns 0 for error, 1 for quit (end of game) and 2 for check (to continue the game)
	int status, back = 2;
	Control* statusGame;
	SDL_Rect statusGameRect, clickRect;
	Control *boardd = gameWindow->children[0];

	status = reportStatus(COLOR);
	statusGameRect = geneRect(95, 163, 450, 315);
	statusGame = (Control*) malloc(sizeof(Control));
	if(statusGame == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}


	if(status == TIE_STATUS || status == MATE_STATUS){
		if(status == TIE_STATUS)
			statusGame = createImage(statusGameRect, "GUI_components/tie.png");	
		else if(status == MATE_STATUS){
			if(COLOR == WHITE){
				statusGame = createImage(statusGameRect, "GUI_components/blackWin.png");
			}
			else if(COLOR == BLACK){
				statusGame = createImage(statusGameRect, "GUI_components/whiteWin.png");
			}
		}	

		addChild(boardd, statusGame);
		drawPanel(boardd, screen);
		SDL_Flip(screen);
		SDL_Delay(5000);

		init_board(); 
		gameMode = 1;
		userColor = WHITE;
		firstPlayer = WHITE;
		miniMaxDepth = 1;
		BEST = 0;
		back = 1;
		cancel = 1;
    }
    else if (status == CHECK_STATUS){
		statusGameRect = geneRect(95, 150, 450, 315);
		statusGame = createImage(statusGameRect, "GUI_components/check.png");	
		addChild(boardd, statusGame);
		drawPanel(boardd, screen);
		SDL_Flip(screen);	
	}

	if (status == NORMAL_STATUS){
		back = 2;
		drawPanel(boardd, screen);
		SDL_Flip(screen);	
	}
	while(SDL_PollEvent(&e) != 0){
			switch (e.type) {
				case (SDL_QUIT):
					quit = 1;
					break;
				case (SDL_MOUSEBUTTONDOWN):
					clickRect = geneRect(e.button.x, e.button.y , 0, 0);
					if(clickedButton(boardd,clickRect) == 1){
						init_board(); 
						gameMode = 1;
						userColor = WHITE;
						firstPlayer = WHITE;
						miniMaxDepth = 1;
						BEST = 0;
						status = 0;
		
						return back;
					}
			}
	}
	
	return back;
}


void freeGame(){		
	// this function frees the memory that was allocated along the game, including regular memory and SDL surfaces
	// also, we use it to close the SDL (SDL_Quit())
	freePanel(menuPanel);
	if(gameStarted == 1)
		freePanel(gameWindow);
	SDL_FreeSurface(screen);
	SDL_Quit();
}


int mainGraphics(){
	// this is the main function of the GUI part of the game
	// this function actually runs the game (not directly, but by calling all the necessary functions that are running the game)
	// returns 0 if the program doesn't succes to initialize the graphic components or allocate some component, returns 1 if everything went as well 
	// and the program has finished to run because the game ended or the user manually quiting the game
	Control *menuPage, *newGame, *loadGame, *quitGame;
	SDL_Rect newGameRect, loadGameRect, quitGameRect, clickRect;

	// initialization of the main gui components, declared in the head file graphics.h
	screen = NULL;
	quit = 0;
	cancel = 0;
	toSet = 0;
	loaded = 0;
	colorGui = 'w';
	gameStarted = 0;
	
	screenOffset.x = 0;
	screenOffset.y = 0;
	screenOffset.w = SCREEN_W;
	screenOffset.h = SCREEN_H;
	
	// initializing and creating all the essential controls to run the main window of the game, which contains a starting menu of the game options
	menuPanel = (Control*) malloc(sizeof(Control));	
	if(menuPanel == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	}
	menuPage = (Control*) malloc(sizeof(Control));	
	if(menuPage == NULL){
		perror("Error: standard function malloc has failed");
		return 0;
	} 
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("ERROR: unable to init SDL: %s\n", SDL_GetError());
		return 0;
	}
	screen  = SDL_SetVideoMode(SCREEN_W, SCREEN_H, 0, SDL_HWSURFACE | SDL_DOUBLEBUF); // initialize SDL video components
	if (screen == NULL) {
		printf("ERROR: failed to set video mode: %s\n", SDL_GetError());
		return 0;
	}
	SDL_WM_SetCaption("CHESS by Roman & Ori","GUI_components/chessCaption.png"); // setting the title of the window of the program
	menuPanel = createPanel(screenOffset);	
	menuPage = createImage(screenOffset, "GUI_components/menuPage.png");	
	addChild(menuPanel, menuPage);

	newGame = (Control*) malloc(sizeof(Control));
	if(newGame == NULL){
		printf("ERROR: failed to set video mode: %s\n", SDL_GetError());
		return 0;
	}
	newGameRect = geneRect(MENU_BUTTON_X,MENU_BUTTON_Y,MENU_BUTTON_W,MENU_BUTTON_H);
	newGame = createButton(newGameRect, "GUI_components/newGame.png");
	addChild(menuPanel, newGame);

	loadGame = (Control*) malloc(sizeof(Control));
	if(loadGame == NULL){
		printf("ERROR: failed to set video mode: %s\n", SDL_GetError());
		return 0;
	}
	loadGameRect = geneRect(MENU_BUTTON_X,MENU_BUTTON_Y + 60,MENU_BUTTON_W,MENU_BUTTON_H);
	loadGame = createButton(loadGameRect, "GUI_components/loadGame.png");
	addChild(menuPanel, loadGame); 

	quitGame = (Control*) malloc(sizeof(Control));
	if(quitGame == NULL){
		printf("ERROR: failed to set video mode: %s\n", SDL_GetError());
		return 0;
	}
	quitGameRect = geneRect(MENU_BUTTON_X,MENU_BUTTON_Y + 120,MENU_BUTTON_W,MENU_BUTTON_H);
	quitGame = createButton(quitGameRect, "GUI_components/quitGame.png");
	addChild(menuPanel, quitGame);

	//drawPanel(menuPanel, screen);
	//SDL_Flip(screen);
	while(quit == 0){
		drawPanel(menuPanel, screen);
		SDL_Flip(screen);
		// Poll for keyboard & mouse events
		while (SDL_PollEvent(&e) != 0) {
			cancel = 0;
			switch (e.type) {
				case (SDL_QUIT):
					quit = 1;
					break;
				case (SDL_MOUSEBUTTONDOWN):
					if(quit == 0){
						clickRect = geneRect(e.button.x ,e.button.y, 0, 0);	
						if(clickedButton(newGame, clickRect))
							if(settingsStateGui() == 0){
								freeGame();
								return 0;
							}
						if(clickedButton(loadGame, clickRect))
							loadStateGui();
						drawPanel(menuPanel, screen);
						if(clickedButton(quitGame, clickRect)){
							quit = 1;
							SDL_Flip(screen);
							break;
						}					
					}
			}
		}
	}
	// freeing all the recourses of the game
	freeGame();
	return 1;
}


