//
// Created by bitterboyy on 2/22/21.
//

#ifndef BAUB_CHESS_DEBUG_H
#define BAUB_CHESS_DEBUG_H

#include "position.h"
#include "display.h"
#include <chrono>
#include <map>

namespace Chameleon{
    namespace Debug {
        void perft(const position &board);

        unsigned long long int
        perftRecursive(int depth, position board);

        void perftDivide(position &board);
    }
}

#endif //BAUB_CHESS_DEBUG_H
