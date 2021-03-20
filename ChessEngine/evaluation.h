//
// Created by bitterboyy on 3/4/21.
//

#ifndef BAUB_CHESS_EVALUATION_H
#define BAUB_CHESS_EVALUATION_H

/*
 * The program will use a simple tappered evaluation function dependant on side to move at first
 * The goal will be to implement more complex evaluation schemes as time goes on
 */

/* List of features :
 * [*] Material balance
 * [*] Piece-Square tables
 * [*] Tappered eval
 * [ ] Pawn structure
 */

#include "position.h"
#include "display.h"

namespace Chameleon {
    namespace Evaluation {
        void init();

        //Simply returns the given board's evaluation (positive => sideToMove is winning)
        int eval(position &board);

        //A function that evaluates a capture/move from a pure material perspective, to take a guess at whether or not it might
        //even be worth playing without actually making any move
        int see(const position& pos, movebits move, bool side);
    }
}


#endif //BAUB_CHESS_EVALUATION_H
