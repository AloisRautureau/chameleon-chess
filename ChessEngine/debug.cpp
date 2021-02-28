//
// Created by bitterboyy on 2/22/21.
//

#include "debug.h"

unsigned long long debug::perftRecursive(int depth, board_representation board,int * caps, int* ep) {
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
            switch(board_representation::getFlag(stack[i])){
                case CAP:
                    *caps += 1;
                    break;
                case EPCAP:
                    *ep += 1;
                    break;
            }

            nodes += perftRecursive(depth-1, board, caps, ep);
            board.takeback();
        }
    }
    return nodes;
}

void debug::perft(const board_representation &board) {
    //Initialize counting variables
    int ep = 0;
    int caps = 0;

    std::cout << std::endl << std::endl;
    for(int depth = 0; depth < 10; depth++){
        caps = 0;
        ep = 0;
        auto nodes = perftRecursive(depth, board, &caps, &ep);
        std::cout << std::endl << "Nodes searched on depth " << depth << " : " << nodes << " with " << caps << "  " << ep << std::endl;
    }
}
