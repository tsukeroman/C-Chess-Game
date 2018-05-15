// Oct 05 17:30


#include "chessprog.h"
#include "graphics.h"




int wordsArrayLen; // number of words in current wordsArray result
char** wa = NULL; // will be updated each wordArray call, contains an array of words

//main

int main(int argc, char** argv){
	int i;
	int sett = 0;
    int game = 0;
	int graphic = 0;
	char* mode = "GARBAGE";
    assert(argc == 1 || argc == 2);
    
	// initialization of the most important game components, declared in the head file chessprog.h
	gameMode = 1;
	userColor = WHITE;
	firstPlayer = WHITE;
	miniMaxDepth = 1;
	BEST = 0;
	for(i = 0; i < 4; i++)
		castle_flags[i] = OFF;

   	//Allocation of the board
    chessBoard = (Location**)malloc(BOARD_SIZE * sizeof(Location*));
    if (chessBoard == NULL){
         perror("Error: standard function malloc has failed"); 
         exit(0);
    }
    for (i = 0; i < BOARD_SIZE; i++){
        chessBoard[i] = (Location*)malloc(BOARD_SIZE * sizeof(Location));    
        if (chessBoard[i] == NULL){
           perror("Error: standard function malloc has failed");
           exit(0);
        }
    }
    

    // initizlization of global pointers:
    head = NULL;
    tail = NULL;
    
    //At the beginning of the program in command line mode, the game board is being initialized
    //and printed to the console.
    if(argc == 2){
	    mode = argv[1];
			if(strcmp(mode,"gui") != 0 && strcmp(mode,"console") != 0){
		                      print_message(WRONG_GAME_MODE);
		                      return -1;
	    } 
    }
	init_board();
    //GUI mode
    if(strcmp(mode,"gui") == 0){
        
        // here we'll implement the GUI part
		 graphic = mainGraphics();
	     freeBoard(&chessBoard);
		 if(graphic == 1)
			 recursiveFree(&head);
         return 0;
   }
    

    //console mode
    if(strcmp(mode,"console") == 0 || argc == 1){  
		print_board();
        // here we'll implement the console part
    	while(1){
             if(sett != 1 && sett != 2)
                    sett = settingsState();     
             else 
                  break;
        }
        
        //if sett == 2 that means that the user quited the game at the setting state, so there the program will return 0
        //and it won't enter the game state
        if(sett == 2){ 
	       freeBoard(&chessBoard);
           return 0;
    	}
	
        if(gameMode == 1){
		            while(1){
		                     if(game != 1)
                                     game = vsPlayerState();     
                             else 
		                          break;
                    }  
	    }
	    else if(gameMode == 2){
		     while(1){
		              if(game != 1)
		                      game = vsCompState();     
                      else 
		                   break;
             }  
	    }                 
    }
    
    recursiveFree(&head);
	freeBoard(&chessBoard);
	freeWordsArray();
    return 0;      
}


// functions implementation:

void print_line(){
    //Auxiliary function for print_board 
	int i;
	printf("  |");
	for (i = 1; i < BOARD_SIZE*4; i++){
		printf("-");
	}
	printf("|\n");
}

void print_board() {
     //this function prints the state of the game board in the correct format to the console
	int i,j;
	print_line();
	for (j = BOARD_SIZE-1; j >= 0 ; j--)
	{
		printf((j < 8 ? " %d" : "%d"), j+1);
		for (i = 0; i < BOARD_SIZE; i++){
            printf("| %c ", (chessBoard[i][j]).player);  
		}
		printf("|\n");
		print_line();
	}
	printf("   ");
	for (j = 0; j < BOARD_SIZE; j++){
		printf(" %c  ", (char)('a' + j));
	}
	printf("\n");
}

void init_board(){
    //this function initializes the board for a game
	int i,j;
	for (i = 0; i < BOARD_SIZE; i++){
        chessBoard[i][1].player = WHITE_P;
        chessBoard[i][6].player = BLACK_P;
    }
    chessBoard[0][0].player = WHITE_R;
    chessBoard[1][0].player = WHITE_N;
    chessBoard[2][0].player = WHITE_B;
    chessBoard[3][0].player = WHITE_Q;
    chessBoard[4][0].player = WHITE_K;
    chessBoard[5][0].player = WHITE_B;
    chessBoard[6][0].player = WHITE_N;
    chessBoard[7][0].player = WHITE_R;
    
    chessBoard[0][7].player = BLACK_R;
    chessBoard[1][7].player = BLACK_N;
    chessBoard[2][7].player = BLACK_B;
    chessBoard[3][7].player = BLACK_Q;
    chessBoard[4][7].player = BLACK_K;
    chessBoard[5][7].player = BLACK_B;
    chessBoard[6][7].player = BLACK_N;
    chessBoard[7][7].player = BLACK_R;
    
    for (i = 0; i < BOARD_SIZE; i++)
        for (j = 2; j < 6; j++) 
            chessBoard[i][j].player = EMPTY;
    whites[0] = 8;
    blacks[0] = 8;
    whites[1] = 2;
    blacks[1] = 2;
    whites[2] = 2;
    blacks[2] = 2;
    whites[3] = 2;
    blacks[3] = 2;
    whites[4] = 1;
    blacks[4] = 1;
    whites[5] = 1;
    blacks[5] = 1;
}



void clearBoard(){
     //Auxiliary function for settingState
     //the function clears the board - makes it empty from disks
    int i,j;
	for (i = 0; i < BOARD_SIZE; i++)
		for (j = 0; j < BOARD_SIZE; j++)
            (chessBoard[i][j]).player = EMPTY;
            
    for(i = 0; i < 6; i++){
          whites[i] = 0;
          blacks[i] = 0;
    }
}


char* readline(){
      //we use this function to get the input from the user
      // dynamic memory allocation for input string
      size_t size = 20;
      char* str = (char*)malloc(sizeof(char) * size);
      int c;
      size_t len = 0;
      while (EOF != (c = getchar()) && c != '\r' && c != '\n') {
            str[len++] = c;
            if(len == size)
                   str = realloc(str, sizeof(char) * (size *= 2));
      }
      str[len++]='\0';
      return realloc(str, sizeof(char) * len);
}

void freeWordsArray() {
     // free the array of strings, represented by the global variable char** wa.
     // The ONLY time user has to call this function by himself, is at the end
     // of the program (end of main).
     int i;
     if (wa==NULL)
        return;

     for(i=0;i<wordsArrayLen;i++)
                                 free(wa[i]);
     
     free(wa);
     wa = NULL;
     return;
}

void wordsArray(){
       // This function does 3 things:
       // A. Reads input from user
       // B. Separate it into an array of words (type char**)
       // C. Each call, the previous result (array of 'old' words) is being
       // freed automatically, so caller doesn't has to worry about memory leaks
       // by himself.
         
      int index = 0;    
      int start, end, j; // start\end index of current word in str
      char* input = readline();
      while (strlen(input)<1)
          input = readline();
      
      // free previous array
      if (wa != NULL)
         freeWordsArray();
     
     wordsArrayLen = 0; // num of words 
            
      // first stage: count the number of words
      while (input[index] != 0) {
            
            while (' ' == input[index])
		             ++index;
            
            if (input[index] == 0)   
               break;
      
            while (' ' != input[index] && input[index]!=0)
                  ++index;
            
            wordsArrayLen++;
      }
      
      // 2nd stage: counter (wordsArrayLen) is updated. We can create the
      // result array
      if (wordsArrayLen == 0) {
         if (strlen(input) > 0)
            free(input);
         return;
      }
      
      wa = malloc(wordsArrayLen * sizeof(char*));      
      index = 0;
      j = 0;
      while (input[index] != 0 && j<wordsArrayLen) {
            
            while (' ' == input[index])
		             ++index;
            
            if (input[index] == 0)   
               break;

            start = index++;
            while (' ' != input[index] && input[index]!=0)
                  ++index;
      
            end = index -1;
            wa[j] = malloc((2+end-start) * sizeof(char));
            // only (1+end-start) chars for the word, and additional one for '\0'
            strncpy(wa[j],input+start,1+end-start);
            (wa[j])[1+end-start] = 0;
            j++;

      }
     
      free(input);
      return;
}


int correctFormat(char* str){
	//Auxiliart function for settingState
	//if the input has the form < , > return 1, otherwise return 0
	if(str[0] == '<' && str[2] == ',' && str [4] == '>' && strlen(str) == 5)
		return 1;
	return 0; 
}

char firstParameter(char* str){
     //Auxiliary function for settingState
     //the function returns the first paramter in < , > which presents the column
     //returned value has to be within the range of 'a' to 'h', in any other case the funtion returns 'z'
    char first;
	size_t len = strlen(str);
    if(str[0] != '<')
        return 'z';
    if (len > 5 || str[len-1] != '>')
       return 'z';
    if(str[2] == ','){
        first = str[1];
        if(first >= 'a' && first <= 'h')
            return first;
        else
            return 'z';  
    } 
    return 'z';
}

int secondParameter(char* str){
    //Auxiliary function for settingState
    //the function returns the second paramter in < , > which presents the row
    //returned value has to be within the range of 1 to 8, in any other case the funtion returns 0
    int i = 0;
    char secondd;
    if(str[0] != '<')
        return 0; 
    if(str[2] == ','){
       if (str[4] == '>'){
          secondd = str[3];
          i = secondd - '0';
       }
       if(i >= 1 && i <= 8)
            return i;
    }
    return 0;     
}


void updateKings(){
     // Auxiiary function for settingState
     // this function updates bKing and wKing at the moment the game starts to the kings current locations
     int i, j;
     for(i = 0 ; i < BOARD_SIZE; i++){
           for(j = 0 ; j < BOARD_SIZE; j++){
                 if((chessBoard[i][j]).player == 'k'){
                      wKing.column = 'a' + i;
                      wKing.row = j + 1;
                      wKing.player = 'k';
                 }
                 else if((chessBoard[i][j]).player == 'K'){
                      bKing.column = 'a' + i;
                      bKing.row = j + 1;
                      bKing.player = 'K';
                 }
           }
     }
}


void updateCells(char type, int num){
     // Auxiliary function for settingState
     // this function updates the amount array when setting or removing a piece to the board
     
			if(type == 'k')
				whites[5] = whites[5] + num;
			else if(type == 'q')
				whites[4] = whites[4] + num;
			else if(type == 'r')
				whites[2] = whites[2] + num;
			else if(type == 'n')
				whites[3] = whites[3] + num;	
			else if(type == 'b')
				whites[1] = whites[1] + num;
			else if(type == 'm')
				whites[0] = whites[0] + num;
			else if(type == 'K')
				blacks[5] = blacks[5] + num;
			else if(type == 'Q')
				blacks[4] = blacks[4] + num;
			else if(type == 'R')
				blacks[2] = blacks[2] + num;
			else if(type == 'N')
				blacks[3] = blacks[3] + num;	
			else if(type == 'B')
				blacks[1] = blacks[1] + num;
			else if(type == 'M')
				blacks[0] = blacks[0] + num;

}


int legalSet(char* piece, char* color){
    // Auxiliary function for settingState
    // the function returns 0 if setting the piece creates an invalid board, otherwise returns 1
    if(strcmp(color,"white") == 0){
			if(strcmp(piece,"king") == 0){
				if(whites[5] == 1)
                     return 0; 
			}
			else if(strcmp(piece,"queen") == 0){
				if(whites[4] == 1)
                     return 0; 
			}
			else if(strcmp(piece,"rook") == 0){
				if(whites[2] == 2)
                     return 0; 
			}
			else if(strcmp(piece,"knight") == 0){
				if(whites[3] == 2)
                     return 0;		
			}
			else if(strcmp(piece,"bishop") == 0){
				if(whites[1] == 2)
                     return 0;
			}
			else if(strcmp(piece,"pawn") == 0){
				if(whites[0] == 8)
                     return 0;
			}
		}
		if(strcmp(color,"black") == 0){
			if(strcmp(piece,"king") == 0){
				if(blacks[5] == 1)
                     return 0; 
				}
			else if(strcmp(piece,"queen") == 0){
				if(blacks[4] == 1)
                     return 0; 
			}
			else if(strcmp(piece,"rook") == 0){
				if(blacks[2] == 2)
                     return 0; 
			}
			else if(strcmp(piece,"knight") == 0){
				if(blacks[3] == 2)
                     return 0;		
			}
			else if(strcmp(piece,"bishop") == 0){
				if(blacks[1] == 2)
                     return 0;
			}
			else if(strcmp(piece,"pawn") == 0){
				if(blacks[0] == 8)
                     return 0;
			}
		}
		return 1;
}

void forSetPos(Location loc, char* piece, char* color){
     // Auxiliary function for settingState
     // set on the board in the cordinates of loc the type of "piece" and color of "color"
	if(strcmp(piece," ") == 0 && strcmp(color," ") == 0)
		setPosition(loc,EMPTY);
	else{
		if(strcmp(color,"white") == 0){
			if(strcmp(piece,"king") == 0){
				setPosition(loc,WHITE_K); 
            }
			else if(strcmp(piece,"queen") == 0)
				setPosition(loc,WHITE_Q);
			else if(strcmp(piece,"rook") == 0)
				setPosition(loc,WHITE_R);
			else if(strcmp(piece,"knight") == 0)
				setPosition(loc,WHITE_N);			
			else if(strcmp(piece,"bishop") == 0)
				setPosition(loc,WHITE_B);
			else if(strcmp(piece,"pawn") == 0)
				setPosition(loc,WHITE_P);
		}
		if(strcmp(color,"black") == 0){
			if(strcmp(piece,"king") == 0)
				setPosition(loc,BLACK_K);
			else if(strcmp(piece,"queen") == 0)
				setPosition(loc,BLACK_Q);
			else if(strcmp(piece,"rook") == 0)
				setPosition(loc,BLACK_R);
			else if(strcmp(piece,"knight") == 0)
				setPosition(loc,BLACK_N);			
			else if(strcmp(piece,"bishop") == 0)
				setPosition(loc,BLACK_B);
			else if(strcmp(piece,"pawn") == 0)
				setPosition(loc,BLACK_P);
		}
	}
}


void setPosition(Location loc, char piece){
     // Auxiliary function for forSetPos and for the game logic implementation
    char x = loc.column;
    int y = loc.row;
    (chessBoard[x-'a'][y-1]).player = piece;
}

int wrongBoard(){
    //Auxiliary function for settingState
    //before entering the game state, when the user presses "start", this function valiadates that 
    //the board initialization is alright
    //if the initialization is wrong it returns 1, otherwise 0
    if(blacks[5] == 1 && whites[5] == 1){ //exactly one king
		if(blacks[0] <= 8 && whites[0] <= 8){ //at most 8 pawns
			if(blacks[1] <= 2 && whites[1] <= 2){ //at most 2 bishops
				if(blacks[2] <= 2 && whites[2] <= 2){ //at most 2 rooks
					if(blacks[3] <= 2 && whites[3] <= 2){ //at most 2 knights
						if(blacks[4] <= 1 && whites[4] <= 1){ //at most 1 king
							return 0;
						}
					}
				}
			}
		}
	}
	//if didn't return 0 previously, returns now 1 for a wrong board initialization
    return 1;
}

int settingsState(){
    //this function gets commands from the user as an input and sets initial setting for the game, before the game starts.
    //the function is used repeatedly by the main function until the user quits the program or starts the game
    //returns 0 if performed a command , 1 if it is the time to start the game, and 2 to quit;
	int dep, mone = 0,mone2 = 0;
	char *type, *color;
	Location locc;
	print_message(ENTER_SETTINGS);
	wordsArray();
	if(strcmp(wa[0],"quit") != 0){
		if (strcmp(wa[0], "start") == 0 && wordsArrayLen == 1){
		     if(wrongBoard() == 1){
		         print_message(WROND_BOARD_INITIALIZATION);
		         return 0;
		     }
		     updateKings();
		     return 1;
		}
		else{
			if(strcmp(wa[0],"clear") == 0 && wordsArrayLen == 1){
				clearBoard();
			}
			else if(strcmp(wa[0],"print") == 0 && wordsArrayLen == 1){
				print_board();				
			}
			else if(strcmp(wa[0],"game_mode") == 0 && wordsArrayLen == 2){
				if(strcmp(wa[1],"1") == 0){
					gameMode = 1;
					print_message(TWO_PLAYERS_GAME_MODE);				
				}
				else if(strcmp(wa[1],"2") == 0){
					gameMode = 2;
					print_message(PLAYER_VS_AI_GAME_MODE);				
				}
				else
					print_message(WRONG_GAME_MODE);
			}
			else if(strcmp(wa[0],"difficulty") == 0 && gameMode == 2){
				if(strcmp(wa[1],"depth") == 0 && wordsArrayLen == 3){
					dep = atoi(wa[2]);
					if(dep > 0 && dep < 5)
						miniMaxDepth = dep;
					else
						print_message(WRONG_MINIMAX_DEPTH);		
				}
				else if(strcmp(wa[1],"best") == 0 && wordsArrayLen == 2)
					BEST = 1;	
			}
			else if(strcmp(wa[0],"user_color") == 0 && gameMode == 2 && wordsArrayLen == 2){
				if(strcmp(wa[1],"white") == 0)
					userColor = WHITE;
				if(strcmp(wa[1],"black") == 0){
					userColor = BLACK;	
                }
				else{
					print_message(ILLEGAL_COMMAND);
					return 0;
                }        			
			}
			else if(strcmp(wa[0],"load") == 0 && wordsArrayLen == 2){
				if(fileExists(wa[1]) == 1){                                 
                        loadState(wa[1]);
                        print_board(); 
   	             	    return 0;
                } 
                else{
                     print_message(WRONG_FILE_NAME);
                     return 0;
                }   
			}
			else if(strcmp(wa[0],"next_player") == 0 && wordsArrayLen == 2){
				if(strcmp(wa[1],"white") == 0)
					firstPlayer = WHITE;
				if(strcmp(wa[1],"black") == 0)
					firstPlayer = BLACK;
                else{
					print_message(ILLEGAL_COMMAND);
					return 0;
                }        			
			}
			else if(strcmp(wa[0],"rm") == 0 && wordsArrayLen == 2){
				if(correctFormat(wa[1]) == 1){
					locc.column = firstParameter(wa[1]);
					locc.row = secondParameter(wa[1]);
					if(locc.column != 'z' && locc.row != 0){
                        updateCells((chessBoard[locc.column - 'a'][locc.row - 1]).player, -1);           
						forSetPos(locc, " ", " ");
                    }
					else
                        			print_message(WRONG_POSITION);  
				}
				else{
					print_message(ILLEGAL_COMMAND);
					return 0;
				}
			}
			else if(strcmp(wa[0],"set") == 0 && wordsArrayLen == 4){
				locc.column = firstParameter(wa[1]);
				locc.row = secondParameter(wa[1]);
				if(strcmp(wa[2],"white") == 0 || strcmp(wa[2],"black") == 0){
					color = wa[2];
					mone++;
                }
				else{
					print_message(ILLEGAL_COMMAND);
					return 0;
				}
				type = wa[3];
				if(correctFormat(wa[1]) == 1){
					mone++;
				}
				if(strcmp(type,"king") == 0 || strcmp(type,"queen") == 0 || strcmp(type,"rook") == 0 || strcmp(type,"knight") == 0 || strcmp(type,"bishop") == 0 || strcmp(type,"pawn") == 0){
					mone++;		
				}
				if(locc.column != 'z' && locc.row != 0){
					mone2++;
				}	
				if(mone == 3 && mone2 == 1){
                    if(legalSet(type, color) == 1){			
                        forSetPos(locc, type, color);
                        updateCells((chessBoard[locc.column - 'a'][locc.row - 1]).player,1);
                    }
                    else
                        print_message(NO_PIECE);  
                }
				else{
					if(mone2 == 0){
						print_message(WRONG_POSITION);		
					}
					else{
						print_message(ILLEGAL_COMMAND);
					}				
				}
						
			}
			else{
				print_message(ILLEGAL_COMMAND);				
			}
			
			return 0;
		}

	}
	else{
		if(wordsArrayLen == 1){	    
			return 2;
		}
		else{
			print_message(ILLEGAL_COMMAND);
			return 0;
		}	
	}
	
}


char checkerColor(char coll, int roww){
     //Auxiliary function for all the states
     char p = (chessBoard[coll - 'a'][roww - 1]).player;
     if (p==EMPTY)
        return EMPTY;
     return colorOf(p);      
}


Move* stringToMove(char* src, char* dest, char prom){
      //Auxiliary function for gameState
      //this function "translates" a string to a move and returns a pointer to the move
      //the output is a pointer because we want later to be able to free it from the memory
      //in case that the move doesn't exist the output is NULL
      // *** IMPORTANT TO MENTION : THIS FUNCTION WORKS ONLY WITH A VALID INPUT ***      
      
      Move* move = NULL;
      Move* curr;
      
      possibleMoves(firstParameter(src),secondParameter(src)); 
      
      curr = head;
      while(curr != NULL){   
                if((curr->from).column == firstParameter(src) && (curr->from).row == secondParameter(src)){
                    if((curr->to).column == firstParameter(dest) && (curr->to).row == secondParameter(dest)){
			if(curr->promotion == prom){
                               move = curr;
                               return move;
			}
		    }
		}
                curr = curr->next;  
      }
      return move;
}


int checkMove(char* src, char* dest, char prom, char color){
    //Auxiliary function for gameState
    //this function returns 1 if the move is valid, 2 if one of the positions is invalid
    //3 if the position doesn't contain user's color, and 4 if the move is illegal
    char col_src = firstParameter(src);
    char col_dest = firstParameter(dest);
    int row_src = secondParameter(src);
    int row_dest = secondParameter(dest);
    
    if(col_src == 'z' || row_src == 0 || col_dest == 'z' || row_dest == 0){
          return 2;
    }
    if(checkerColor(col_src,row_src) != color){
          return 3;
    }
    possibleMoves(col_src,row_src);
    if(doesTheListContainThisMove(col_src, row_src, col_dest, row_dest, prom, 0) == NULL){           
          return 4;
    }
    return 1;
         
}


int fileExists(char *fname){
    //Auxiliary function for userTurn
    //this function checks if the file is exist and if it can be opened for writing
    //returns 1 in the case the file exist, otherwise returns 0
    FILE *file = NULL;
    file = fopen(fname, "r");
    if(file != NULL)
    {
        fclose(file);
        return 1;
    }
    return 0;
}


int saveState(char* fname, char color){
     //Auxiliary function for userTurn
     //this function saves to the given address the current state of the game
     //returns 1 on succes , 0 on failure
     FILE* file;
     int i,j;
     file = fopen(fname, "w");
     if(file == NULL)
             return 0;
     fseek(file, 0, SEEK_SET);     
     fprintf(file, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
     fprintf(file, "<game>\n");
     if(color == WHITE) 
		fprintf(file, "\t<next_turn>%s</next_turn>\n", "White");
	 else if(color == BLACK)
		fprintf(file, "\t<next_turn>%s</next_turn>\n", "Black");
     fprintf(file, "\t<game_mode>%d</game_mode>\n", gameMode);
     if(gameMode == 2){
        if(BEST == 1)
            fprintf(file, "\t<difficulty>%s</difficulty>\n", "best");   
        else if(miniMaxDepth > 0 && miniMaxDepth < 5)
            fprintf(file, "\t<difficulty>%d</difficulty>\n", miniMaxDepth);           
        if(userColor == WHITE)
            fprintf(file, "\t<user_color>%s</user_color>\n", "White");   
        else if(userColor == BLACK)
            fprintf(file, "\t<user_color>%s</user_color>\n", "Black");     
     }
     else if(gameMode == 1){
          fprintf(file, "\t<difficulty></difficulty>\n"); 
          fprintf(file, "\t<user_color></user_color>\n");  
     }
     
     fprintf(file, "\t<board>\n");
     for (i = BOARD_SIZE - 1; i >= 0; i--)
     {
        fprintf(file, "\t\t<row_%d>", i+1);
        for (j = 0; j < BOARD_SIZE; j++)
        {
            if (chessBoard[j][i].player == EMPTY)
				fprintf(file, "%c", '_');
			else
				fprintf(file, "%c", chessBoard[j][i].player);
        }
		fprintf(file, "</row_%d>\n", i+1);
     }
    fprintf(file, "\t</board>\n");
    fprintf(file, "\t<general>\n");
    fprintf(file, "\t\t<castle>%d%d%d%d</castle>\n", castle_flags[0], castle_flags[1], castle_flags[2], castle_flags[3]); 
    fprintf(file, "\t</general>\n");
    fprintf(file, "</game>\n");
    
    fclose(file);
    return 1;
}


void loadState(char* fname){
     //Auxiliary function for userTurn
     //this function loads the state of the game that saved in the given address
     FILE* file;
     int i,j,roww;
     char temp[10];
     int difficulty;
     char game_mode;
     
     file = fopen(fname, "r");
    
     fscanf(file, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<game>\n");
     fscanf(file, "\t<next_turn>%c%c%c%c%c</next_turn>\n", temp, temp + 1, temp + 2, temp + 3, temp + 4);
     temp[5] = '\0';
     for (i = 0; i < 5; i++)
         temp[i] = tolower(temp[i]);
     if(strcmp(temp,"white") == 0){
          firstPlayer = WHITE;
     }
     else if(strcmp(temp,"black") == 0){
          firstPlayer = BLACK;
     }
     fscanf(file, "\t<game_mode>%c</game_mode>\n", &game_mode);
     gameMode = game_mode - '0';
     if(gameMode == 2){
           fscanf(file, "\t<difficulty>%c", temp);
           difficulty = temp[0] - '0';
           if(difficulty > 0 && difficulty < 5){      
                  fscanf(file, "</difficulty>\n");
                  miniMaxDepth = difficulty;
           }
           else{
                BEST = 1;
                fscanf(file, "%c%c%c</difficulty>\n", temp, temp + 1, temp + 2);                  
           }        
           fscanf(file, "\t<user_color>%s</user_color>\n",temp);   
           temp[5] = '\0';
           for (i = 0; i < 5; i++)
               temp[i] = tolower(temp[i]);   
           if(strcmp(temp,"white") == 0)
               userColor = WHITE;
           else if(strcmp(temp,"black") == 0)
               userColor = BLACK;     
     }
     else if(gameMode == 1){
          fscanf(file, "\t<difficulty></difficulty>\n"); 
          fscanf(file, "\t<user_color></user_color>\n");  
     }

     fscanf(file, "\t<board>\n");

     temp[BOARD_SIZE] = '\0';
     for (i = 0; i < BOARD_SIZE; i++)
     {   
         
        fscanf(file, "\t\t<row_%d>%c%c%c%c%c%c%c%c</row_%d>\n", &roww , temp, temp + 1, temp + 2, temp + 3, temp + 4, temp + 5, temp + 6, temp + 7, &roww);

        for (j = 0; j < BOARD_SIZE; j++)
        {
            if (temp[j] == '_'){
                (chessBoard[j][8 - i - 1]).player = EMPTY;
            }
			else{
				(chessBoard[j][8 - i - 1]).player = temp[j];
            }
        }
     }
     if(fscanf(file, "\t</board>\n\t<general>") != 0){
             fscanf(file, "\t\t<castle>%c%c%c%c</castle>\n", castle_flags, castle_flags + 1, castle_flags + 2, castle_flags + 3); 
             fscanf(file, "\t</general>\n");        
     }
     fscanf(file, "\t</board>\n</game>\n");
     fclose(file);
     return; 
}


char typeToMacro(char* str, char color){
    //Auxiliary function for userTurn
	// used for passing the promotion argument into doesTheListContainThisMove(...).
	// str is "bishop" or "queen" or "rook" or "knight"
	// color is BLACK or WHITE
	if (strcmp(str,"bishop")==0)
		return (color==WHITE)?WHITE_B:BLACK_B;
	if (strcmp(str,"rook")==0)
		return (color==WHITE)?WHITE_R:BLACK_R;
	if (strcmp(str,"knight")==0)
		return (color==WHITE)?WHITE_N:BLACK_N;
	if (strcmp(str,"queen")==0)
		return (color==WHITE)?WHITE_Q:BLACK_Q;
	return 0; // should NOT happen! if there's no additional
	// word, pass 0 yourself. I took care off identifying
	// whether that's a promotion move, if it does I return
	// the queen promotion, even if you passed 0.
}


int userTurn(char COLOR){
    //Auxiliary function for both vsPlayerState() and vsCompState()
    //this function performs a move in users's turn according to user's input
    //the function outputs 1 if the user loses, 0 if the user quits, 
    //2 if the user asked for possbile moves or typed an illegal command (then it still the user turn)
    //and 3 otherwise
    Location locc;
    int LD;
    int dep;
    int flag;
    int tempMax;
    Move* userMove = NULL;
    int status = reportStatus(COLOR);
	char p;

    if(status == TIE_STATUS){
              printf("The game ends in a tie\n");
              return 1;    
    }
    if (status == CHECK_STATUS)
       printf("Check!\n");
    if (status == CHECK_STATUS || status == NORMAL_STATUS) {
       print_message((COLOR==WHITE)?"White ":"Black ");
       print_message("player - enter your move:\n");   
    }
    if(status == MATE_STATUS) {
              print_message("Mate! ");
              print_message((COLOR==WHITE)?"white ":"black ");
              print_message("player wins the game\n");
              return 1;
    }
    
    wordsArray();
    if(strcmp(wa[0],"quit") != 0){
           if(strcmp(wa[0],"get_moves") == 0 && wordsArrayLen == 2){
                if(correctFormat(wa[1]) == 1){
                       locc.column = firstParameter(wa[1]);
                       locc.row = secondParameter(wa[1]);
                       if(locc.column == 'z' || locc.row == 0){
                              print_message(WRONG_POSITION);
	                          return 2;
                       }
                       else{
                            if(checkerColor(locc.column,locc.row) != COLOR){
                                  print_message(WRONG_PEACE);
                            }
                            else{
                                possibleMoves(locc.column,locc.row);
                                printPossibleMoves();
                            }
                            return 2;
                       }      
                }
                else{
                     print_message(ILLEGAL_MOVE);
  		             return 2;  
                }
                     
           }
           else if(strcmp(wa[0],"get_best_moves") == 0 && wordsArrayLen == 2){
                tempMax = miniMaxDepth;
                dep = atoi(wa[1]);
                if(strcmp(wa[1],"best") == 0)
                       updateBestDepth(&miniMaxDepth, COLOR);
                else if(dep > 0 && dep < 5) 
                       miniMaxDepth = dep;
                minimax(COLOR,NULL);
                printPossibleMoves();            
                miniMaxDepth = tempMax;
                return 2;
           }
           else if(strcmp(wa[0],"get_score") == 0){
                if(wordsArrayLen == 4 || wordsArrayLen == 6 || wordsArrayLen == 7){
                      if(colorOf(playerType(firstParameter(wa[3]),secondParameter(wa[3]))) == COLOR){
                          tempMax = miniMaxDepth;
                          dep = atoi(wa[1]);
                          if(strcmp(wa[1],"best") == 0)  
                                    updateBestDepth(&miniMaxDepth, COLOR);
                          else if(dep > 0 && dep < 5)
                                    miniMaxDepth = dep;
                          possibleMoves(firstParameter(wa[3]),secondParameter(wa[3]));
                          if(wordsArrayLen == 4)
                               userMove = doesTheListContainThisMove(firstParameter(wa[3]),secondParameter(wa[3]),0,0,0,1);
                          else if(wordsArrayLen == 6)
                               userMove = doesTheListContainThisMove(firstParameter(wa[3]),secondParameter(wa[3]),firstParameter(wa[5]),secondParameter(wa[5]),0,0);
                          else if(wordsArrayLen == 7)
                               userMove = doesTheListContainThisMove(firstParameter(wa[3]),secondParameter(wa[3]),firstParameter(wa[5]),secondParameter(wa[5]),typeToMacro(wa[6],COLOR),0);
                          minimax(COLOR,userMove);
                          miniMaxDepth = tempMax;
                          printf("the score is %d\n", head->score);
                          return 2;
                      }
                      print_message(ILLEGAL_COMMAND);
    			      return 2;
                }   
                print_message(ILLEGAL_COMMAND);
                return 2;      
           }
           else if(strcmp(wa[0],"save") == 0 && wordsArrayLen == 2){
                LD = saveState(wa[1], COLOR);
                if(LD == 0){
                     print_message(WRONG_FILE_NAME);
                     return 2;
                } 
                else if(LD == 1){
                     return 2;
                }      
           }
           else if(strcmp(wa[0],"move") == 0){
                if(wordsArrayLen == 4 || wordsArrayLen == 5){
                    if(wordsArrayLen == 4){
               	     	flag = checkMove(wa[1],wa[3],0,COLOR);
                    }
                    else{
                        flag = checkMove(wa[1],wa[3],typeToMacro(wa[4],COLOR),COLOR);
        			}
    		        if(flag == 1){
                          if(wordsArrayLen == 4)
                               userMove = doesTheListContainThisMove(firstParameter(wa[1]),secondParameter(wa[1]),firstParameter(wa[3]),secondParameter(wa[3]),0,0);
        		          else
        		               userMove = doesTheListContainThisMove(firstParameter(wa[1]),secondParameter(wa[1]),firstParameter(wa[3]),secondParameter(wa[3]),typeToMacro(wa[4],COLOR),0);
                          applyMove(*userMove);
        		          print_board();
	                      return 3;
                    }
    		        else{
    		            if(flag == 2)
    		                 print_message(WRONG_POSITION);
    		            if(flag == 3)
    		                 print_message(WRONG_PEACE);
    		            if(flag == 4)
    		                 print_message(ILLEGAL_MOVE);
    		            return 2;  
    		        }
                }
            }
       
	
    	   /* BONUS - CASTELING */  
    	   //more specific explaination at page 13
    	   else if(strcmp(wa[0],"castle") == 0 && wordsArrayLen == 2){
                if(correctFormat(wa[1]) == 1){
                    if(firstParameter(wa[1]) != 'z' && secondParameter(wa[1]) != 0){
                         if(checkerColor(firstParameter(wa[1]),secondParameter(wa[1])) == COLOR){
                             possibleMoves(firstParameter(wa[1]),secondParameter(wa[1]));                                             
                             p = playerType(firstParameter(wa[1]),secondParameter(wa[1]));
                             if((COLOR==WHITE && p==WHITE_R) || (COLOR==BLACK && p==BLACK_R))                                             
                                         userMove = doesTheListContainThisMove(firstParameter(wa[1]),secondParameter(wa[1]),0,0,0,1);
                             else{
                                  print_message(WRONG_ROOK_POSITION);
    			                  return 2;
                             }
                             
                             if (userMove==NULL){
                                  print_message(ILLEGAL_CALTLING_MOVE);
    			                  return 2;
                             }
                             applyMove(*userMove);  
                             print_board();             
                         } 
                         else{
                              print_message(WRONG_PEACE);
                              return 2;  
                         }                    
                    }
                    else{
                         print_message(WRONG_POSITION);
                         return 2;  
                    } 
                }
                else{
    			     print_message(ILLEGAL_COMMAND);
    			     return 2;
                }
                return 3;
    	   }
           else{
                print_message(ILLEGAL_COMMAND);
			    return 2;
           }
    }
    else{
	     return 0;
	}	    
	return 3;
}   

int compTurn(char COLOR){
    //Auxiliary function for vsCompState()
    //this function performs a move in computer's turn according to the minimax algorithm 
    //this function outputs 1 if the computer wins, 0 otherwise
     int status = reportStatus(COLOR);
     
     if(status == TIE_STATUS){
              printf("The game ends in a tie\n");
              return 1;    
     }
     if(status == MATE_STATUS) {
              print_message("Mate! ");
              print_message((COLOR==WHITE)?"white ":"black ");
              print_message("player wins the game\n");
              return 1;
     }
     if(BEST == 1)
           updateBestDepth(&miniMaxDepth, COLOR);
     minimax(COLOR,NULL);
     applyMove(*head);
     printf("Computer: ");
     printSingleMove(head);
     print_board();
     return 0;
    
}

int vsPlayerState(){
    //this function simulates the game between the user and the computer. Which player starts first is decided by the value of firstPlayer constant.
    //In each turn, the current player performs its move and //the game continues until one of the players wins
    //or until the game is stopped by one of the users.
    //the function is used repeatedly by the main function until one of the users quits the program
    //returns 0 if performed a move , 1 if one of the players wins or its a tie or one of the users quits the game;
    int user1;
    int user2;
    if(firstPlayer == WHITE){
         user1 = userTurn(WHITE);
         while(user1 == 2)
              user1 = userTurn(WHITE);
         if(user1 == 0 || user1 == 1)
              return 1;
         user2 = userTurn(BLACK);
         while(user2 == 2)
              user2 = userTurn(BLACK);
         if(user2 == 0 || user2 == 1)
              return 1;
         return 0;             
    }
    
    else{
         user1 = userTurn(BLACK);
         while(user1 == 2)
               user1 = userTurn(BLACK);
         if(user1 == 0 || user1 == 1)
               return 1;
         user2 = userTurn(WHITE);
         while(user2 == 2)
               user2 = userTurn(WHITE);
         if(user2 == 0 || user2 == 1)
               return 1;
         return 0;             
    }
}

int vsCompState(){
    //this function simulates the game between the user and the computer. Which player starts first is decided by the value of firstPlayer constant. 
    //In each turn, the current player performs its move and //the game continues until one of the players wins
    //or until the game is stopped by the user.
    //the function is used repeatedly by the main function until the user quits the program
    //returns 0 if performed a move , 1 if one of the players wins or the user quits the game;
    int comp;
    int user;
    if(firstPlayer == WHITE){
        if(userColor == WHITE){
             user = userTurn(WHITE);
             while(user == 2)
                 user = userTurn(WHITE);
             if(user == 0 || user == 1)
                 return 1;
             comp = compTurn(BLACK);
             if(comp == 1)
                 return 1;
        }
        else{
             comp = compTurn(WHITE);
             if(comp == 1)
                 return 1;
             user = userTurn(BLACK);
             while(user == 2)
                 user = userTurn(BLACK);
             if(user == 0 || user == 1)
                 return 1;
        }
        
        return 0;             
    }
    
    else if(firstPlayer == BLACK){
         if(userColor == BLACK){
             user = userTurn(BLACK);
             while(user == 2)
                 user = userTurn(BLACK);
             if(user == 0 || user == 1)
                 return 1;
             comp = compTurn(WHITE);
             if(comp == 1)
                 return 1;
        }
        else{
             comp = compTurn(BLACK);
             if(comp == 1)
                 return 1;
             user = userTurn(WHITE);
             while(user == 2)
                 user = userTurn(WHITE);
             if(user == 0 || user == 1)
                 return 1;
        }
        return 0;             
    }
	return 0;
}


char playerType(char column, int row) {
         
     return chessBoard[column-'a'][row-1].player; 
}

