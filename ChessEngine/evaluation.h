//
// Created by bitterboyy on 3/4/21.
//

#ifndef BAUB_CHESS_EVALUATION_H
#define BAUB_CHESS_EVALUATION_H

/*
 * The program will use a simple tappered evaluation function dependant on side to move at first
 * The goal will be to implement more complex evaluation schemes as time goes on
 */

#include "board_representation.h"

namespace Chameleon {
    namespace Evaluation {
        void init();

        //Simply returns the given board's evaluation (positive => sideToMove is winning
        int eval(board_representation &board);
    }
}


#endif //BAUB_CHESS_EVALUATION_H
