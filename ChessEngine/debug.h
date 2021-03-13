//
// Created by bitterboyy on 2/22/21.
//

#ifndef BAUB_CHESS_DEBUG_H
#define BAUB_CHESS_DEBUG_H

#include "board_representation.h"
#include "display.h"
#include <chrono>

namespace Chameleon{
    namespace Debug {
        void perft(const board_representation &board);

        unsigned long long int
        perftRecursive(int depth, board_representation board, int* caps, int* ep, int* castles, int* prom, int* check, int* mate);

        void perftDivide(board_representation &board);

        void perftSuite(board_representation &board, const std::vector<std::string>& fenStack);
    }
}

#endif //BAUB_CHESS_DEBUG_H
