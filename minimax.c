#include "chessprog.h"

// Oct 05 22:07

char maximizerColor;

int scoreBoard(char color){
	// scoring function
	// This function scan each position on the board, and outputs a score
    // according to the instructions at page 12.

    int i,j,sum = 0;
    char type;
    for(i=0;i<BOARD_SIZE;i++){
       for(j=0;j<BOARD_SIZE;j++) {
                  type = (chessBoard[i][j]).player;
                  if (type == EMPTY)
                     continue;

                  if (type==WHITE_P || type==BLACK_P)
                     sum+=(color==colorOf(type))?1:-1;
                  else if (type==WHITE_N || type==BLACK_N || type==WHITE_B || type==BLACK_B)
                       sum+=(color==colorOf(type))?3:-3;
                  else if (type==WHITE_R || type==BLACK_R)
                       sum+=(color==colorOf(type))?5:-5;
                  else if (type==WHITE_Q || type==BLACK_Q)
                       sum+=(color==colorOf(type))?9:-9;
                  else // king
                       sum+=(color==colorOf(type))?400:-400;
       }
    } 
    return sum;  
}

int berlingerScore(char color){
	// Hans berlinger's score, by factor 30 to make the result an integer
	// https://en.wikipedia.org/wiki/Chess_piece_relative_value

    int i,j,sum = 0;
    char type;
    for(i=0;i<BOARD_SIZE;i++) {
       for(j=0;j<BOARD_SIZE;j++) {
                  type = (chessBoard[i][j]).player;
                  if (type == EMPTY)
                     continue;

                  if (type==WHITE_P || type==BLACK_P)
                     sum+=(color==colorOf(type))?30:-30; // 1*30
                  else if (type==WHITE_N || type==BLACK_N)
                       sum+=(color==colorOf(type))?96:-96; // 3.2 * 30
                  else if (type==WHITE_B || type==BLACK_B)
                       sum+=(color==colorOf(type))?100:-100; // 3.33... * 30
                  else if (type==WHITE_R || type==BLACK_R)
                       sum+=(color==colorOf(type))?153:-153; // 5.1 * 30
                  else if (type==WHITE_Q || type==BLACK_Q)
                       sum+=(color==colorOf(type))?264:-264; // 8.8 * 30
           }
    } 
    return sum;  
}

void updateBestDepth(int* depth, char color) {
     // update the minimax depth in best mode, to the highest depth which won't
     // excceed 1 million recusive calls. First, we assest number of possible
     // moves for maximizer \ minimizer players. Statistically this number
     // of possible moves won't change drastically (mostly not higher) in further
     // consequent moves, as captures will decrease the number of possible
     // moves. Indeed the exception for that "rule" is the Pawn promotion, which
     // increase drastically the number of possible moves for the promoted player,
     // however at the same time it decreases the possible moves for its opponent,
     // so that scenario doesn't affect the statistics-based idea of the calculation.
     
     Move* tmp;
     int best_depth = 0;
     int evaluated_calls, counterMaximizer = 0;
     int counterMinimizer = 0;
     
     allPossibleMoves(color);
     tmp = head;
     while (tmp != NULL) {
           counterMaximizer++;
           tmp = tmp->next;      
     }
     if (counterMaximizer==0)
        counterMaximizer=1;
     
     allPossibleMoves(OPPOSITE_COLOR(color));
     tmp = head;
     while (tmp != NULL) {
           counterMinimizer++;
           tmp = tmp->next;      
     }
     if (counterMinimizer==0)
        counterMinimizer=1;

     evaluated_calls = counterMaximizer;
     // we limit max possible depth to 12 (at 99% of possible scenarios, the
     // above loop will terminate at MUCH lower depth)
     while (evaluated_calls<1000000 && ++best_depth<13)
           evaluated_calls *= ((best_depth%2==1)? counterMinimizer:counterMaximizer);
     
     *depth = best_depth;   
     return;     

}

void minimax(char color,Move* m){
     // Return best Moves for player with the argument's color,
     // based on the information up to the given depth.
     // Being more specific: the global Moves list (starting at head) will
     // contain only move\s with highest possible score at given depth.
     // The 2nd argument will be used only for get_score command, otherwise its
     // value is NULL.

     Move *tmp, *candidates, *curr, *prev;
     int root_alpha, backup_depth;
     if (m==NULL)
        allPossibleMoves(color);
                
     if (head==NULL)
        return;
     maximizerColor = color; // maximizer is a term used for prunning
     if (m!=NULL) { // used at get_score command, only
        tmp = copyMove(m);
        initializeMovesList();
        minimaxCalculator(OPPOSITE_COLOR(color),1,tmp,NULL);
        if (head != tmp) {
           initializeMovesList();
           head = tmp;
        }
        return; // head->score is updated, head and m represent the same move.      
     }

     candidates = copyList(head);
     if (miniMaxDepth > 4){ // best mode. Let's do a Prunning IMPROVEMENT:
        // we'll make sure that the first Move in candidates has the highest
        // potential of being the best Move, using a shallower minimax
        // run at depth 3. Yes, it's worth the slight extra cost as the earlier we
        // have the highest possible root's alpha parameter, the more prunning
        // will be made. At bigger depths that matters.
        // This improvement won't be affective, if all\most candidates get the
        // same score. 
        backup_depth = miniMaxDepth;
        miniMaxDepth = 3;
        minimax(color,NULL); // update global list
        miniMaxDepth = backup_depth; // restore pre-shallow-run depth
        // let's make sure the first node in the candidates list, is one of the
        // highest rated Moves at depth 3 (use the shallower run speculation):
        tmp = copyMove(head);
        tmp->next = candidates;
        candidates = tmp;
        curr = candidates->next;
        prev = candidates;
        while (!sameMove(curr,candidates)){
              prev = curr;
              curr = curr->next;
        }
        // now, curr and the first Move in candidates are the same Move (mirror
        // copy, different address)
        if (curr==NULL) printf("bug\n");
        removeFromList(prev,curr,OFF);
        // improvement done. First node in candidates list has a high potential
        // of being the best move, therefore prunning has high probability of
        // being more substantial.
     }

     // now we're going to bubble down up to miniMaxDepth, score the Moves
     // and bubble up scoring the calling Moves.  
     curr = candidates;
     root_alpha = -INFINITY; // initial alpha value of the tree's root
     while (curr != NULL) {
         curr->alpha = root_alpha;
         minimaxCalculator(OPPOSITE_COLOR(color),1,curr,NULL);                    
         if (curr->score > root_alpha)
            root_alpha = curr->score;
              
         curr = curr->next;
     }

     initializeMovesList();
     head = candidates;
     
     // max score is updated.
     // let's remove Moves with smaller score from the global list:
     curr = head;
     prev = NULL;
     while (curr != NULL) {
                if (curr->score == root_alpha) { // highest possible score
                   prev = curr;
                   curr = curr->next;
                   continue;
                }
                // reaching here means: curr's score isn't the highest possible
                // score, so we don't want it inside the final listed created by
                // minimax. Let's remove it:
                tmp = curr->next;
                removeFromList(prev,curr,ON);
                curr = tmp;             
     }
 
     return;
} 


void minimaxCalculator(char color, int depth, Move *curr ,Move *caller){
     // curr is a Move of depth==2nd arg in the tree, which hasn't been applied yet. 
     // we apply it carefully, saving a backup copy to restore the board's
     // state afterwards. The rightmost argument "caller", is a Move of depth
     // depth-1, so we can bubble up the score using it.
 
     Location wKB, bKB;
     Location** backup;
     Move *tmpList, *childOfCurr;
     int k,status;
     char flags_backup[4];

     // create backup:
     backup=boardMirrorCopy();
     for(k=0;k<4;k++) flags_backup[k]=castle_flags[k];
     wKB=wKing; bKB=bKing;
     // done creating backup
     applyMove(*curr);
     status = reportStatus(color);
   
     if (depth == miniMaxDepth) {
        // money time. Let's use the score function and bubble up
        if (status==MATE_STATUS) {
           curr->score = LOSE_SCORE;          
           if (depth%2==1)
              curr->score = WIN_SCORE; // represents a win for the maximizerColor
        }
        else if (status==TIE_STATUS)
               curr->score = 0;
        else
            curr->score = (miniMaxDepth<5 && BEST==0)? scoreBoard(maximizerColor) : berlingerScore(maximizerColor);
        
        // restore backup:
        freeBoard(&chessBoard);
        chessBoard = backup;
        for(k=0;k<4;k++) castle_flags[k]=flags_backup[k];
        wKing=wKB; bKing=bKB;
        // done restoring backup
        if (caller == NULL) // miniMaxDepth = 1
           return;
        // bubble score up the tree, if needed:
        BUBBLE_SCORE_UP;       
        return; // no more recursive calls.
     }

     // depth != miniMaxDepth
     // recurse:
     if (status == NORMAL_STATUS || status == CHECK_STATUS)
         allPossibleMoves(color); 
     if (status==MATE_STATUS)
        curr->score = (depth%2==1)?WIN_SCORE:LOSE_SCORE;
     else if (status==TIE_STATUS)
            curr->score = 0;
     else {
          tmpList = copyList(head);
          childOfCurr = tmpList;
          while (childOfCurr != NULL) {
                
                childOfCurr->alpha = curr->alpha; // inheriting from parent
                childOfCurr->beta = curr->beta; // as above
                minimaxCalculator(OPPOSITE_COLOR(color),1+depth,childOfCurr,curr);
                // curr->score has been updated (not necessarily to its final
                // value). We USE this update in order to perform... Prunning.    
                // Let's update curr->alpha and curr->beta, and consider whether
                // rest children in tmpList are worth being checked or not:

                if (depth%2==1) { // curr is a MINImizer node in the tree
                   if (curr->score < curr->alpha)  
                      break; // prunning done, rest nodes in tmpList won't be
                      // visited.
                   
                   if (curr->beta > curr->score)
                      curr->beta = curr->score;      
                                
                }
                else { // curr is a MAXimizer node in the tree
                     if (curr->score > curr->beta) 
                        break; // prunning done, rest nodes in tmpList won't be
                     // visited.
                     
                     if (curr->alpha < curr->score)
                        curr->alpha = curr->score; // VERIFY
                }
                // done updating alpha & beta, and prune if necessary.
                childOfCurr = childOfCurr->next;      
          }
          recursiveFree(&tmpList);
     }
     // restore backup:
     freeBoard(&chessBoard);
     chessBoard = backup;
     for(k=0;k<4;k++) castle_flags[k]=flags_backup[k];
     wKing=wKB; bKing=bKB;
     // done restoring backup
     // update caller's score (bubble up)
     if (caller == NULL) // height == 1
        return;
     BUBBLE_SCORE_UP;
     return;

}
