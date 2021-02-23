//
// Created by bitterboyy on 2/22/21.
//

#ifndef BAUB_CHESS_DEBUG_H
#define BAUB_CHESS_DEBUG_H

#include "board_representation.h"
#include "display.h"

class debug {
public:
    static unsigned long long perftRecursive(int depth, board_representation board);
    static void perft(const board_representation &board);
};


#endif //BAUB_CHESS_DEBUG_H
