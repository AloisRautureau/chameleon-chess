//
// Created by bitterboyy on 3/10/21.
//

#ifndef BAUB_CHESS_SEARCH_H
#define BAUB_CHESS_SEARCH_H

#include "board_representation.h"
#include "evaluation.h"
/*
 * The search function will feature alpha-beta search inside of a negamax framework as a starting point
 * Quiescence is also obligatory for a decently playing program
 * Later on transposition tables will be added
 */

class search {
private:
    evaluation &eval;
    static board_representation &boardSearched;

    //Generates every move, then calls itself for each move.
    //At depthLeft = 0, it returns the quiescence evalutation of the current position
    static int searchNode(int alpha, int beta, int depthLeft, board_representation &board);

    //Searches for a quiet position to minimize the impact of the event horizon
    static int quiescence(int alpha, int beta);

public:
    search(evaluation &eval): eval(eval){};
    //Calls searchNode recursively for every move generated in the root position, then compares the scores and
    //returns the highest scoring move
    static movebits bestMove(board_representation &board, int depth);
};


#endif //BAUB_CHESS_SEARCH_H
