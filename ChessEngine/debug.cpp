//
// Created by bitterboyy on 2/22/21.
//

#include "debug.h"

unsigned long long debug::perftRecursive(int depth, board_representation board, int * caps, int* ep, int* castles, int* prom, int* check, int* mate) {
    movebits stack[256];
    int stackIndex = 0;
    unsigned long long nodes = 0;

    //Generate and copy the move stack
    if((board.m_check & 0b1001) == board.m_sideToMove){
        if(board.genCheckEscape()){
            *check += 1;
        }
        else{
            *mate += 1;
        }
    }
    else board.gen();

    if(depth == 0) return 1;

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
                    case KCASTLE:
                    case QCASTLE:
                        *castles += 1;
                        break;
                    case NPROM:
                    case BPROM:
                    case RPROM:
                    case QPROM:
                    case NPROMCAP:
                    case BPROMCAP:
                    case RPROMCAP:
                    case QPROMCAP:
                        *prom += 1;
                        break;
                    default: break;
                }
            nodes += perftRecursive(depth-1, board, caps, ep, castles, prom, check, mate);
            board.takeback();
        }
    }
    return nodes;
}

void debug::perft(const board_representation &board) {
    //Initialize counting variables
    int ep = 0;
    int caps = 0;
    int castles = 0;
    int prom = 0;
    int check = 0;
    int mate = 0;

    std::cout << std::endl << std::endl;
    for(int depth = 0; depth < 8; depth++){
        caps = 0;
        ep = 0;
        check = 0;
        mate = 0;
        prom = 0;
        castles = 0;
        auto nodes = perftRecursive(depth, board, &caps, &ep, &castles, &prom, &check, &mate);

        //Display the results
        std::cout << "Nodes searched at depth " << depth << " : " << nodes << std::endl <<std::endl;
        std::cout << "             NODES " << std::endl;
        std::cout << "             ----- " << std::endl;
        std::cout << "CAPTURES     " << caps << std::endl;
        std::cout << "EN_PASSANT   " << ep << std::endl;
        std::cout << "PROMOTIONS   " << prom << std::endl;
        std::cout << "CASTLES      " << castles << std::endl;
        std::cout << "CHECKS       " << check << std::endl;
        std::cout << "CHECKMATES   " << mate << std::endl << std::endl << std::endl;
    }
}

void debug::perftDivide(board_representation &board){
    //Initialize counting variables
    int ep = 0;
    int caps = 0;
    int castles = 0;
    int prom = 0;
    int check = 0;
    int mate = 0;

    for(int depth = 1; depth < 8; depth++){
        std::cout << "Nodes searched at depth " << depth << std::endl << std::endl;
        //We need to make each first move, then count nodes from here
        movebits stack[256];
        int stackIndex = 0;
        board.gen();
        for(int i = 0; i < board.m_moveStackIndex; i++){
            stack[stackIndex] = board.m_moveStack[i];
            stackIndex++;
        }

        std::cout << "  MOVE     NODES" << std::endl;
        std::cout << "  ----     -----" << std::endl;
        for(int i = 0; i < stackIndex; i++){
            board.make(stack[i]);
            unsigned long long nodes;
            nodes = perftRecursive(depth-1, board, &caps, &ep, &castles, &prom, &check, &mate);
            std::cout << display::displayMove(stack[i]) << "   " << nodes << std::endl;
            board.takeback();
        }
        std::cout << std::endl << std::endl;
    }
}
