#include "chessprog.h"

// Oct 05 22:07

int initialize = ON; // helper global flag for the function possibleMoves(..)
// this flag determines whether the global list will be initizlied prior action
// or not (if not, the new moves will be added to the current list)
int CHECK = OFF;

char possibleMovesCastleFlags[4]; // prevents excessive
// insertions\checks of castle moves which have already being added to the list
// at current call.

int progressAllowed(char column, int row, char height, char side) {
    // A. If the 2 leftmost args' values aren't valid, returns 0 of course.
    // B. verify that at least 1 step can be made at that direction. The desired
    // direction is given by height and side, and curr cell is given by
    // column and row.
    
    if (column<'a' || column>'h' || row<1 || row>BOARD_SIZE)
       return 0;
    
    // height check:
    if ((height==UP && row==BOARD_SIZE) || (height==DOWN && row==1))
          return 0;
     
    // side check:
    if ((side==RIGHT && column=='h') || (side==LEFT && column=='a'))
          return 0;

    // margins check ended.  no forbidden progress found. Therefore:
  //  printf("progressAllowed for [%c][%d]\n",column,row);
    return 1;
}

void increment(char* column,int* row, int increment, char height, char side){
    // this function increment the 2 leftmost args by 'increment' according
    // to height (UP or DOWN) and side (RIGHT and LEFT) desired.
    
    if (height == UP)
       (*row) += increment;
    else if (height == DOWN)
        (*row) -= increment;
                             
    if (side == RIGHT)
       (*column) += increment;
    else if (side == LEFT)
        (*column) -= increment;

    return;
}

void applyMove(Move m){
     
     char type = playerType(m.from.column,m.from.row);
     if (type==WHITE_K) { wKing.column=m.to.column; wKing.row=m.to.row; }
     if (type==BLACK_K) { bKing.column=m.to.column; bKing.row=m.to.row; }
     
     // update castle_alerts array:
     // (assures castling is made iff players didn't move before)
     updateCastleFlags(castle_flags,m.from.column,m.from.row);
     
     // applies the move to the global board.
     if (m.promotion==0)
        setPosition(m.to,type);
     else
         setPosition(m.to,m.promotion);
     
     setPosition(m.from,EMPTY);
     if (m.castle != NULL) // castle
        applyMove(*(m.castle));

     return;
}

void possibleMoves(char col, int row) {
     // updates the global linked list (accesible through Move* head) with all
     // possible leagal Moves of the player at given position (arguments).     
      // first, clear any previous list:
      int i;
      char type;
                 
      if (initialize==ON) {
         initializeMovesList();
         for(i=0;i<4;i++)
              possibleMovesCastleFlags[i]=OFF;
      }
      
      if (IS_EMPTY(col,row))
         return; // should not happen...
      
      type =  playerType(col,row);
      if (type==BLACK_N || type==WHITE_N) { // Knight
         addKnightMoves(col,row,type);
         return;                 
      }
      
      // let's decide which directions are allowed for this player type:
      // 8 directions total: UP,DOWN,LEFT,RIGHT, and 4 diagonals.
      // We determine ahead whether checkDirection(...) checks unlimited amount
      // of steps in that direction, or 1 step only (see checkDirection 
      // documentation).
      
      // UP & DOWN:
      if (type==WHITE_P || type==WHITE_K || type==BLACK_K)
         checkDirection(col,row,UP,OFF,OFF);
      if (type==WHITE_R || type==BLACK_R || type==BLACK_Q || type==WHITE_Q) {
         checkDirection(col,row,UP,OFF,ON);
         checkDirection(col,row,DOWN,OFF,ON);
      }
      if (type==BLACK_P || type==WHITE_K || type==BLACK_K)
         checkDirection(col,row,DOWN,OFF,OFF);     

      // LEFT & RIGHT:
      if (type==WHITE_K || type==BLACK_K) {
         checkDirection(col,row,OFF,LEFT,OFF);
         checkDirection(col,row,OFF,RIGHT,OFF);
      }
      if (type==WHITE_R || type==BLACK_R || type==BLACK_Q || type==WHITE_Q) {
         checkDirection(col,row,OFF,LEFT,ON);
         checkDirection(col,row,OFF,RIGHT,ON);
      }        
      
      // Diagonals - limited for 1 step only:
      // Kings:
      if (type==WHITE_K || type==BLACK_K) {
         checkDirection(col,row,UP,LEFT,OFF);
         checkDirection(col,row,UP,RIGHT,OFF);
         checkDirection(col,row,DOWN,LEFT,OFF);
         checkDirection(col,row,DOWN,RIGHT,OFF);
      }
      
      // diagonals, unlimited:
      if (type==WHITE_B || type==BLACK_B || type==BLACK_Q || type==WHITE_Q) {
         checkDirection(col,row,UP,LEFT,ON);
         checkDirection(col,row,UP,RIGHT,ON);
         checkDirection(col,row,DOWN,LEFT,ON);
         checkDirection(col,row,DOWN,RIGHT,ON);
      } 

      return;
}

void checkDirection(char col,int row,char height,char side,int unlimited) {
     // 2 leftmost arguments are the loction. Other args describe the direction
     // we'd like to explore (check for legal Moves in that direction).
     // each of the 4 rightmost args can also except OFF, which aborts this
     // feature. the 'unlimited' arg is a flag for unlimited number of steps
     // in that direction (until we cannot progress anymore, for whatever 
     // reason), when unlimited==OFF we check only a single step progress.
     // also, it's worth mentioning that this function does NOT check whether
     // a possible Move is legal or not, that check is being made later inside
     // addMoveToList(...).
     // 2 initial specials checks:
     // A. Pawn capture (diagonal ONLY), and Pawm promotion
     // B. Castling

     int to_row = row;
     char to_col = col;
     char type = playerType(col,row);
     char to_type;
     int promote, i;

     if (type==WHITE_P || type==BLACK_P) { // special case A (Pawn)
        promote = OFF; // promotion flag
        
        if ((type==WHITE_P && row==(BOARD_SIZE-1)) || (type==BLACK_P && row==2))
           promote = ON;
     
        // straight walk:
        increment(&to_col,&to_row,1,height,OFF);
        if (IS_EMPTY(to_col,to_row)) {
           if (promote==OFF)
              addMoveToList(createMove(col,row,to_col,to_row));
           else
                addPromotions(col,row,to_col,to_row);
        }
        
        // diagonals for Pawn: ONLY capture is allowed!
        // LEFT & RIGHT:
        i=1;
        while (i>=-2) { // 2 loops, one for LEFT and another for RIGHT
              // (the variable i determines the direction, RIGHT represented
              // by "negative LEFT") 
              increment(&to_col,&to_row,i,OFF,LEFT);
              if (progressAllowed(to_col,to_row,OFF,OFF)) {
                 to_type = playerType(to_col,to_row);
                 if (to_type==EMPTY || SAME_COLOR(type,to_type)) {
                    i-=3;                
                    continue;
                 }
                 if (promote==OFF)
                    addMoveToList(createMove(col,row,to_col,to_row));
                 else
                     addPromotions(col,row,to_col,to_row);                                   
              }        
              i-=3;
        }
        
        return;     
     } 
     
     // special case B (castling check)
     // type is a king:
     if (col=='e' && ((type==WHITE_K && row==1) || (type==BLACK_K && row==8))) {
		if (type==WHITE_K) { // WHITE KING
           if (playerType('h',1)==WHITE_R)
			  checkCastle(WHITE,SMALL);
		   if (playerType('a',1)==WHITE_R)
		      checkCastle(WHITE,BIG);
        }
        else { // BLACK KING
             if ((playerType('h',8)==BLACK_R))
                checkCastle(BLACK,SMALL);
             if ((playerType('a',8)==BLACK_R))
		        checkCastle(BLACK,BIG);
        }
      }
      
      // type is rook:
      if (row==1 && type==WHITE_R && playerType('e',1)==WHITE_K) {
         if (col=='a')
            checkCastle(WHITE,BIG);
         if (col=='h')
            checkCastle(WHITE,SMALL);
      }
      if (row==8 && type==BLACK_R && playerType('e',8)==BLACK_K) {
         if (col=='a')
            checkCastle(BLACK,BIG);
         if (col=='h')
            checkCastle(BLACK,SMALL);
      }
              
     // done with special cases.
     while (progressAllowed(to_col,to_row,height,side)) {
           increment(&to_col,&to_row,1,height,side);
           if (SAME_COLOR(type,playerType(to_col,to_row)))
              break;
           
           addMoveToList(createMove(col,row,to_col,to_row));
           if (!IS_EMPTY(to_col,to_row)) // capture !
              break;
           
           if (unlimited==OFF)
              break;
     }

     return;
}

void printPossibleMoves(){
     // this funtion prints all possible moves that were 
     // previoulsy evaluated by last possibleMoves(...) call.    
     Move* curr = head;
     
     if (head == NULL)
        return;

     while (curr != NULL) {
           printSingleMove(curr);
           curr = curr->next;
     }
     return;
}

Location** boardMirrorCopy() {
           // return a NEW copy of the board array, with sames values.
           // here's the beginning:
           int i, j;
           Location **result = malloc(BOARD_SIZE * sizeof(Location*));
           for (i=0;i<BOARD_SIZE;i++)
               result[i] = malloc(BOARD_SIZE * sizeof(Location));

           for (i=0;i<BOARD_SIZE;i++) {
               for (j=0;j<BOARD_SIZE;j++){
                   result[i][j].column =  chessBoard[i][j].column;
                   result[i][j].row =  chessBoard[i][j].row;
                   result[i][j].player =  chessBoard[i][j].player;
               }
           }
           
           return result;
                          
}

void freeBoard(Location*** board) {
     int i;
     for(i=0;i<BOARD_SIZE;i++) {
                              free((*board)[i]);
     }
     
     free(*board);
     return;
}

Move* createMove(char from_col,int from_row,char to_col,int to_row) {
      // creates a new Move address in memory
      Move* result = (Move*)malloc(sizeof(Move));
      result->promotion = OFF;
      result->score = INITIAL;
      result->next = NULL;
      result->castle = NULL;
      result->from.column = from_col;
      result->from.row = from_row;
      result->to.column = to_col;
      result->to.row = to_row;
      result->alpha = -INFINITY;
      result->beta = INFINITY;
      return result;      
}

void addKnightMoves(char col, int row, char type) { 
     // add possible knight Moves
     char to_col = col;
     int to_row = row;
     int i = 2;
     char to_type;
     
     while (i>=-2) { // loop twice, for UP and DOWN initial 2 steps
           increment(&to_col,&to_row,i,UP,OFF);
           increment(&to_col,&to_row,1,OFF,LEFT);
           if (progressAllowed(to_col,to_row,OFF,OFF)) {
              to_type = playerType(to_col,to_row);
              if (to_type==EMPTY || colorOf(type)!=colorOf(to_type))
                 addMoveToList(createMove(col,row,to_col,to_row));
           } 
           increment(&to_col,&to_row,2,OFF,RIGHT);
           if (progressAllowed(to_col,to_row,OFF,OFF)) {
              to_type = playerType(to_col,to_row);
              if (to_type==EMPTY || colorOf(type)!=colorOf(to_type))
                 addMoveToList(createMove(col,row,to_col,to_row));
           } 
           to_col = col;
           to_row = row;
           i -= 4;
     }
     
     i=2;   
     while (i>=-2) { // loop twice, for LEFT and RIGHT initial 2 steps
           increment(&to_col,&to_row,i,OFF,RIGHT);
           increment(&to_col,&to_row,1,UP,OFF);
           if (progressAllowed(to_col,to_row,OFF,OFF)) {
              to_type = playerType(to_col,to_row);
              if (to_type==EMPTY || colorOf(type)!=colorOf(to_type))
                 addMoveToList(createMove(col,row,to_col,to_row));
           } 
           increment(&to_col,&to_row,2,DOWN,OFF);
           if (progressAllowed(to_col,to_row,OFF,OFF)) {
              to_type = playerType(to_col,to_row);
              if (to_type==EMPTY || colorOf(type)!=colorOf(to_type))
                 addMoveToList(createMove(col,row,to_col,to_row));
           } 
           to_col = col;
           to_row = row;
           i -= 4;
     }

     return;
}

void recursiveFree(Move** m){
     // free m and all its content, set NULL to its pointer
     if (*m == NULL)
        return;
     if ((*m)->castle != NULL)
       recursiveFree(&((*m)->castle));
     if ((*m)->next != NULL)
       recursiveFree(&((*m)->next));
     
     free(*m);
     *m = NULL;
     return;
}

void addMoveToList(Move* m) {
     // add to list should FIRST check whether
     // A. the move does NOT eat the king
     // B. the move is legal (doesn't risk the mover's king)
     // ONLY IF A and B both exist, add the move to the list.
     // else, the move is being freed by recursiveFree

     char to_type = playerType(m->to.column,m->to.row);

     // update possibleMovesCastleFlags
     if (m->castle != NULL) {
        if (m->from.column=='h' && m->from.row==1)
           possibleMovesCastleFlags[0]=ON;
        else if (m->from.column=='a' && m->from.row==1)
             possibleMovesCastleFlags[1]=ON;
        else if (m->from.column=='h' && m->from.row==8)
             possibleMovesCastleFlags[2]=ON;
        else
            possibleMovesCastleFlags[3]=ON;              
     }
   
     if (to_type==WHITE_K || to_type==BLACK_K || legalMove(*m)==OFF) {
        recursiveFree(&m);
        return;           
     }
     
     // Insert m after tail, and updates head\tail as necessary
     if (tail != NULL)
        tail->next = m;
     else {
          head = m; 
     }
     tail = m;
     
     return;     
}

Move* copyList(Move* head_) {
      // copy the current list of moves, which has been created by the last 
      // call to possibleMoves(...)

      Move* result = copyMove(head_);
      Move* currOriginal = head_->next;
      Move* currCopy = result;
      while (currOriginal != NULL) {
            currCopy->next = copyMove(currOriginal);
            currCopy = currCopy->next;
            currOriginal = currOriginal->next;
      }

      return result;      
}

Move* copyMove(Move* m) {
      // new Move, different addresses (also new addresses for the consequents)
      // , same values. exception:
      // we do NOT copy the next field, that's the caller's (copyList) job.
      
      Move* result = (Move*)malloc(sizeof(Move));
      copyLocationFields(result,m);
      result->score = m->score;
      result->promotion = m->promotion;
      result->next = NULL;
      result->castle = NULL;    
      result->alpha = m->alpha;
      result->beta = m->beta;
      
      if (m->castle != NULL)
            result->castle = copyMove(m->castle);

      return result;      
}

void copyLocationFields(Move *A, Move *B) {
     // copy B Location field's values, to A
      A->from.column =  B->from.column;
      A->from.row =  B->from.row;
      A->to.column =  B->to.column;
      A->to.row =  B->to.row;
}

int legalMove(Move m) {
    // returns ON iff m doesn't risk the King, otherwise OFF.

    char color = colorOf(playerType(m.from.column,m.from.row));
    int threat, k;
    Location wKB, bKB;
    Location** backup;
    char flags_backup[4];
    
    // create backup:
    backup=boardMirrorCopy();
    for(k=0;k<4;k++) flags_backup[k]=castle_flags[k];
    wKB=wKing; bKB=bKing;
    // done creating backup
    applyMove(m);
    // This Move has been VIRTUALLY applied, now we're going to
    // check whether the king can bee eaten or not.
    threat = kingIsBeingThreatened(color);    
    // restore backup:
    freeBoard(&chessBoard);
    chessBoard = backup;
    for(k=0;k<4;k++) castle_flags[k]=flags_backup[k];
    wKing=wKB; bKing=bKB;
    // done restoring backup
    return (threat?OFF:ON);   
}

void addPromotions(char col,int row,char to_col,int to_row) {
     Move* tmp;
     char whites[4] = {WHITE_B,WHITE_N,WHITE_R,WHITE_Q};
     char blacks[4] = {BLACK_B,BLACK_N,BLACK_R,BLACK_Q};
     int i = -1;
     char color = colorOf(playerType(col,row));
     while ((++i) < 4) {
        tmp = createMove(col,row,to_col,to_row);
        tmp->promotion = (color==WHITE)?(whites[i]):(blacks[i]);
        addMoveToList(tmp);
     }
     return;                                
}

void checkCastle(char color, char size) {
     // check whether castling is legal, if it does, add this Move to the list.
     // There're two types: SMALL castle and BIG castle, according to the
     // distance between the rook and the king
     
     int row, legal;
     char col;
     Location king;
     Move* tmp;
     
     // don't add castle which has already been added to the list:
     if (color==WHITE && size==SMALL && possibleMovesCastleFlags[0]==ON) return;
     if (color==WHITE && size==BIG && possibleMovesCastleFlags[1]==ON) return;
     if (color==BLACK && size==SMALL && possibleMovesCastleFlags[2]==ON) return;
     if (color==BLACK && size==BIG && possibleMovesCastleFlags[3]==ON) return;
     
     // the king is NOT currently in check:
     if (CHECK==ON)
        return;
     
     // verify none of the players moved previously (using the global array)
     // castle_alrets:
     if (color==WHITE && size==SMALL && castle_flags[0]==ON) return;
     if (color==WHITE && size==BIG && castle_flags[1]==ON) return;
     if (color==BLACK && size==SMALL && castle_flags[2]==ON) return;
     if (color==BLACK && size==BIG && castle_flags[3]==ON) return;
     // castle alerts check ended, it means that both player did NOT move
     // previously in this game.
     
     row = (color==WHITE)?1:8;
     // No leap check: ("there are no pieces between")
     if (size==SMALL && (!IS_EMPTY('f',row) || !IS_EMPTY('g',row)))
        return;
     if (size==BIG && (!IS_EMPTY('b',row) || !IS_EMPTY('c',row) || !IS_EMPTY('d',row)))
        return;
     // no leap check ended.
     
     // "The king does not pass through a square that is attacked by an enemy
     // piece" check:
     col = (size==SMALL)?'f':'d';
     king = (color==WHITE)?wKing:bKing;
     tmp = createMove(king.column,king.row,col,row);
     legal = legalMove(*tmp);
     recursiveFree(&tmp);
     if (legal==OFF)
        return;
     if (size==BIG) {
        tmp = createMove(king.column,king.row,'c',row);
        legal = legalMove(*tmp);
        recursiveFree(&tmp);
        if (legal==OFF)
           return;              
     }
     
     // Ta da! Almost all checks passed, the last check for non-illegality
     // (like any other Move) will be made inside the function addMoveToList.
     // Castle Moves are represented with a castle field, which represents the
     // KING's Move. The 'initial' Move would always be the Knight.
     tmp = createMove(((size==SMALL)?'h':'a'),row,((size==SMALL)?'f':'d'),row);
     tmp->castle = createMove('e',row,((size==SMALL)?'g':'c'),row);
     addMoveToList(tmp);
     return;
}

int canCaptureKing(char type,int height_diff,int side_diff) {
    // returns ON iff a player of type 'type' can capture a king's of opposite
    // color, with given height and side diffs from the king.
    // if height_diff is positive, the king is above (UP) the current checked
    // player. if side_diff is positive, the king is placed to the RIGHT of the
    // current player.
    // This function works at O(1).
    int j, m, i, captureAllowed;
    Location king;
    int regularDistance = -1; // -1 symbols NOT a regular distance
    // WHAT IS a regular distance?
    // one of those 8 common:
    // LEFT, RIGHT, UP, DOWN, and the 4 diagonals
    if (height_diff==0 || ABS(height_diff)==ABS(side_diff))
       regularDistance = ABS(side_diff);
    if (side_diff==0) 
       regularDistance = ABS(height_diff);

    if ((type==WHITE_K || type==BLACK_K) && regularDistance==1)
       return ON;
    
    if (type==WHITE_P && ABS(side_diff)==1 && height_diff==1)
       return ON;
    
    if (type==BLACK_P && ABS(side_diff)==1 && height_diff==-1)
       return ON;
    
    if ((type==WHITE_B || type==BLACK_B || type==BLACK_Q || type==WHITE_Q) &&
                       (ABS(height_diff)==ABS(side_diff))) { // diagonal capture
       // we should verify that THERE'S NO LEAP.
       king = (colorOf(type)==WHITE)?bKing:wKing;
       i = 1;
       captureAllowed = ON;
       while (i < ABS(height_diff)) {
             j = (height_diff<0)? i:(-1*i);
             m = (side_diff<0)? i:(-1*i);
             if (!IS_EMPTY(king.column+m,king.row+j)) {
                captureAllowed = OFF;
                break;
             }
             i++; 
       }
       
       if (captureAllowed) {
          return ON;
       }                                       
    } // diagonal capture check ended. 
    
    if ((type==WHITE_R || type==BLACK_R || type==BLACK_Q || type==WHITE_Q) &&
                       (height_diff==0 || side_diff==0)) {
    // straight capture (along a line or column)
    // it's allowed iff there's NO LEAP, let's verify:
       king = (colorOf(type)==WHITE)?bKing:wKing;
       i = 1;
       captureAllowed = ON;
       if (height_diff==0) { // both players in the same row
          while (i < ABS(side_diff)) {
             j = (side_diff<0)? i:(-1*i);
             if (!IS_EMPTY(king.column+j,king.row)) {
                captureAllowed = OFF;
                break;
             }
             i++; 
          }
       }
       else { // side_diff==0, both players in the same column
            while (i < ABS(height_diff)) {
                  j = (height_diff<0)? i:(-1*i);
                  if (!IS_EMPTY(king.column,king.row+j)) {
                     captureAllowed = OFF;
                     break;
                  }
                  i++; 
            }            
       }
             
       if (captureAllowed)
          return ON;                                   
    } 
    
    // check for knight captures (L shape)
    if (type==WHITE_N || type==BLACK_N) {
            if (ABS(height_diff)==1 && ABS(side_diff)==2)
               return ON;
            if (ABS(height_diff)==2 && ABS(side_diff)==1)
               return ON;
    }
    
    // checks ended. Reaching here means no successful capture has been found.
    return OFF;
}

int reportStatus(char color) {
    // this function is being called at the BEGINNING of each turn (before the
    // user\computer actually "see" a new move began, as in MATE or TIE states,
    // this function will not allow the current 'turn' to happen.
    CHECK = kingIsBeingThreatened(color);
    if (atLeastOneMoveIsPossibleFor(color))
       return (CHECK? CHECK_STATUS:NORMAL_STATUS);
    else // there are NO legal moves for this color (end of game at the END OF
    // LAST TURN, because reportStatus is being called at the BEGINNING of each
    // turn)
        return (CHECK? MATE_STATUS:TIE_STATUS);
}

Move* doesTheListContainThisMove(char from_col, int from_row, char to_col, int to_row, char promotion, char castle) {
      // returns NULL if specified move isn't in current list, otherwise returns
      // a pointer to this move.
      // castle gets ON\OFF, promotion can get any of the 8 black\white types
      // of promotion (WHITE_N\BLACK_N\WHITE_Q\...)

      Move* curr = head;
      while (curr != NULL) {
            if (castle) {
               if (curr->castle==NULL) {
                  curr = curr->next;
                  continue;
               }
               if (curr->from.column==from_col && curr->from.row==from_row)
                  return curr;           
            }
            else if (curr->from.column==from_col && curr->from.row==from_row &&
                       curr->to.column==to_col && curr->to.row==to_row
                          && curr->castle == NULL) {
               if (curr->promotion==OFF || curr->promotion==promotion)
                  return curr;
               if ((curr->promotion==WHITE_Q || curr->promotion==BLACK_Q) &&
                     (promotion==OFF || promotion==WHITE_Q || promotion==BLACK_Q))
                  return curr;                               
            }
            
            curr = curr->next;      
      }  
      return NULL;
}

int kingIsBeingThreatened(char color) {
    Location king = (color==WHITE)? wKing : bKing;
    int height_diff, side_diff,i; // if height_diff is positive, the king is
    // above (UP) the current checked player. if side_diff is positive, the king
    // is placed to the RIGHT of the current player. 
    char type,j;
    for (i=1;i<=BOARD_SIZE;i++) {
        for (j='a';j<='h';j++) {
            type = playerType(j,i);
            if (type==EMPTY || colorOf(type)==color)
               continue;
            height_diff=king.row-i;
            side_diff=king.column-j;
            if (canCaptureKing(type,height_diff,side_diff))
               return ON;            
        }    
    }
    return OFF;    
}

int atLeastOneMoveIsPossibleFor(char color) {
    // returns ON iff at least one move is possible for the color at argument  
    int i;
    char j,type;
    initializeMovesList();
    for (i=1;i<=BOARD_SIZE;i++) {
        for (j='a';j<='h';j++) {
            type = playerType(j,i);
            if (type==EMPTY || colorOf(type)!=color)
               continue;
            possibleMoves(j,i);
            if (head != NULL)
               return ON;            
        }    
    }
    return OFF;    
}

void initializeMovesList() {
     if (head != NULL)
        recursiveFree(&head);
     tail = NULL;
     head = NULL;
     return;
}

void printSingleMove(Move* m) {
     // preassumption: m!=NULL
     if (m->castle == NULL) {
        printf("<%c,%d> to <%c,%d>",m->from.column,m->from.row,m->to.column,m->to.row);
        if (m->promotion==WHITE_B || m->promotion==BLACK_B) printf(" bishop");
        if (m->promotion==WHITE_R || m->promotion==BLACK_R) printf(" rook");
        if (m->promotion==WHITE_Q || m->promotion==BLACK_Q) printf(" queen");
        if (m->promotion==WHITE_N || m->promotion==BLACK_N) printf(" knight");
     }
     else // castle
          printf("castle <%c,%d>",m->from.column,m->from.row);     
     printf("\n");
}

void updateCastleFlags(char* array, char col, int row) {
     
     // updates the array[4] values, in order to alert when certain castle
     // is allowed or forbidden
     if (col=='e') {
        if (row==1) {
           array[0] = ON;
           array[1] = ON;
        }
        if (row==8) {
           array[2] = ON;
           array[3] = ON;
        }   
     }
     if (col=='h') {
        if (row==1)
           array[0] = ON;
        if (row==8)
           array[2] = ON;
     }
     if (col=='a') {
        if (row==1)
           array[1] = ON;
        if (row==8)
           array[3] = ON;
     }
     
     return;    
}

void allPossibleMoves(char color) {
    // add all possible moves of color to the list.
    int i;
    char type, j;
    initializeMovesList();
    for(i=0;i<4;i++)
              possibleMovesCastleFlags[i]=OFF;  
    initialize=OFF;
    
    for (i=1;i<=BOARD_SIZE;i++) {
        for (j='a';j<='h';j++) {
            type = playerType(j,i);
            if (type==EMPTY || colorOf(type)!=color)
               continue;
            possibleMoves(j,i);           
        }    
    }
    initialize=ON;
    return;   
}

void removeFromList(Move* prev,Move* toBeDeleted, int updateHeadTail) {
     // also deal with the special cases where head and tail (globals)
     // should be updated.
     Move* tmp;

     if (toBeDeleted == NULL) {
        return;
     }
     if (updateHeadTail && prev==NULL) { //toBeDeleted is the head
        tmp = toBeDeleted;
        head = head->next;
        tmp->next = NULL;
        recursiveFree(&tmp);
        if (head == NULL)
           tail = NULL;
     }
     else {
          tmp = toBeDeleted;
          prev->next = toBeDeleted->next;
          if (updateHeadTail && prev->next == NULL) {
             tail = prev;
          }
          tmp->next = NULL;
          recursiveFree(&tmp);
     }
     
     return;     
}

int sameMove(Move* a,Move* b){
    // return 1 iff moves a & b are mirror copies (the exact same move,
    // different address in memory)
    if (a->promotion != b->promotion) return 0;
    if (a->from.column != b->from.column) return 0;
    if (a->from.row != b->from.row) return 0;
    if (a->to.column != b->to.column) return 0;
    if (a->to.row != b->to.row) return 0;
    if (a->castle!=NULL && b->castle==NULL) return 0;
    if (a->castle==NULL && b->castle!=NULL) return 0;
    if (a->castle!=NULL && b->castle!=NULL)
       return sameMove(a->castle, b->castle);
    
    return 1;
}
