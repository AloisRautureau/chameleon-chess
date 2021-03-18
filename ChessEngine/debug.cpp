//
// Created by bitterboyy on 2/22/21.
//

#include "debug.h"

unsigned long long Chameleon::Debug::perftRecursive(int depth, position board) {
    movebits stack[256];
    int stackIndex = 0;
    unsigned long long nodes = 0;
    board.gen(stack, stackIndex);
    if(depth == 1) return stackIndex;

    //For each move, make the move, then unmake it
    //We also increment the corresponding node counter
    for (int i = 0; i < stackIndex; i++) {
        board.make(stack[i]);
        nodes += perftRecursive(depth - 1, board);
        board.takeback();
    }
    return nodes;
}

void Chameleon::Debug::perft(const position &board) {
    std::cout << std::endl << std::endl;
    for(int depth = 1; depth < 7; depth++){
        auto start = std::chrono::high_resolution_clock::now(); //Get starting time

        auto nodes = perftRecursive(depth, board);

        auto end = std::chrono::high_resolution_clock::now(); //Get ending time
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);


        //Display the results
        std::cout << "Nodes searched at depth " << depth << " : " << nodes << " in " << duration.count()*0.000001 << " seconds ("
                  << nodes/(duration.count()*0.000001) << " nps)" << std::endl <<std::endl;
    }
}

void Chameleon::Debug::perftDivide(position &board){
    for(int depth = 2; depth < 8; depth++){
        unsigned long long totalNodes = 0;
        std::cout << "Nodes searched at depth " << depth << std::endl << std::endl;
        //We need to make each first move, then count nodes from here
        movebits stack[256];
        int stackIndex = 0;
        board.gen(stack, stackIndex);

        std::cout << "MOVE     NODES" << std::endl;
        std::cout << "----     -----" << std::endl;
        for(int i = 0; i < stackIndex; i++){
            board.make(stack[i]);
            unsigned long long nodes;
            nodes = perftRecursive(depth-1, board);
            totalNodes += nodes;
            std::cout << display::displayMove(stack[i]) << "   " << nodes << std::endl;
            board.takeback();
        }
        std::cout << "Total at depth " << depth << " : " << totalNodes;
        std::cout << std::endl << std::endl;
    }
}