//
// Created by bitterboyy on 2/22/21.
//

#include "debug.h"

unsigned long long debug::perftRecursive(int depth, board_representation board) {
    movebits stack[256];
    int stackIndex = 0;
    unsigned long long nodes = 0;


    if(depth == 0) return 1;

    //Generate and copy the move stack
    board.gen();
    for(int i = 0; i < board.m_moveStackIndex; i++){
        stack[stackIndex] = board.m_moveStack[i];
        stackIndex++;
    }

    //For each move, make the move, then unmake it
    //We also increment the corresponding node counter
    for(int i = 0; i < stackIndex; i++){
        if(board.make(stack[i])){
            nodes += perftRecursive(depth-1, board);
            board.takeback();
        }
    }
    return nodes;
}

void debug::perft(const board_representation &board) {
    std::cout << std::endl << std::endl;
    for(int depth = 0; depth < 10; depth++){
        auto nodes = perftRecursive(depth, board);
        std::cout << std::endl << "Nodes searched on depth " << depth << " : " << nodes << std::endl;
    }
}
