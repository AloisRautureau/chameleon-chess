//
// Created by bitterboyy on 4/19/21.
//

#ifndef CHAMELEON_EVAL_H
#define CHAMELEON_EVAL_H

#include "defs.h"
#include "position.h"

using namespace Chameleon;

namespace Eval {
    //Return a final evaluation score for the given position
    int eval(position &pos);

    /*
     * Auxilizary functions that help break down different evaluation aspects
     */
    //Simple piece-value evaluation for each side
    int materialEval(position &pos, bool endgame);

    //Evaluates the pawn structure
    int structureEval(position &pos);

    //Bonus depending on the mobility of pieces
    int mobilityEval(position &pos);

    //Evaluates game phase depending on material
    int gamephase(position &pos);

    /*
     * Other evaluation functions, that do not enter in the process of evaluating a position by themselves
     */
    //SEE evaluates an exchange without actually making the moves, saving some speed
    int see(position &pos, int from, int to);
}




#endif //CHAMELEON_EVAL_H
