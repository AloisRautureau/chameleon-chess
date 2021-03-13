//
// Created by bitterboyy on 3/10/21.
//

#ifndef BAUB_CHESS_SEARCH_H
#define BAUB_CHESS_SEARCH_H

#include "board_representation.h"
#include "evaluation.h"
#include <chrono>

/*
 * The search function will feature alpha-beta search inside of a negamax framework as a starting point
 * Quiescence is also obligatory for a decently playing program
 * Later on transposition tables will be added
 */
namespace Chameleon {
    namespace Search {
        unsigned long long getTableIndex(unsigned long long hash);

        void storeTransposition(unsigned long long hash, int depth, int score, char flag, movebits response);

        void init(board_representation boardToSearch);

        //Generates every move, then calls itself for each move.
        //At depthLeft = 0, it returns the quiescence evalutation of the current position
        int searchNode(int alpha, int beta, int depthLeft);

        //Searches for a quiet position to minimize the impact of the event horizon
        int quiescence(int alpha, int beta);

        //Calls searchNode recursively for every move generated in the root position, then compares the scores and
        //returns the highest scoring move
        movebits bestMove(int depth, std::vector<movebits> list = {}, int nodes = 0, int maxTime = 0, bool infinite = false);

        //Searches a mate in x moves
        movebits searchMate(int depth);

        //Collects the principal variation from the hash table after a search
        void collectPV();
        std::vector<movebits> getPV();

        //Resets any info we have on the ongoing game to start fresh
        void newGame();

        //Returns the time to make the move depending on increment and time left
        int timeManagement(int timeleft, int increment);
    };

}


#endif //BAUB_CHESS_SEARCH_H
