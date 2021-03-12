//
// Created by bitterboyy on 3/10/21.
//

#include "search.h"

movebits search::bestMove(int depth, std::vector<movebits> list, int nodes, int maxTime, bool infinite) {
    movebits bestMove{0};
    int bestScore{-9999};
    movebits moveStack[256]{0};
    int moveStackIndex{0};

    //If no movelist is given, we generate all possible moves
    if(list.empty()){
        //Generate moves
        board.gen(moveStack, moveStackIndex);
    }
    else{
        for(int i = 0; i < list.size(); i++){
            moveStack[i] = list[i];
            moveStackIndex++;
        }
    }

    //TODO : limit to a given number of nodes


    movebits currentMove{0};
    //Iterative deepening
    if(maxTime){
        clock_t startTime = clock();
        for(int searchDepth = 1;; searchDepth++){
            std::cout << "Got to depth" << searchDepth << std::endl;
            for(int move = 0; move < moveStackIndex; move++){
                currentMove = moveStack[move];
                if(board.make(currentMove)){
                    int score = searchNode(-9999, 9999, searchDepth);
                    board.takeback();

                    if(score > bestScore){
                        bestMove = currentMove;
                        bestScore = score;
                    }
                }
                if((clock() - startTime)/CLOCKS_PER_SEC >= maxTime) break;
            }
            if((clock() - startTime)/CLOCKS_PER_SEC >= maxTime) break;
        }
    }

    //Infinite search
    else if(infinite){
        for(int searchDepth = 1;; searchDepth++){
            for(int move = 0; move < moveStackIndex; move++){
                currentMove = moveStack[move];
                if(board.make(currentMove)){
                    int score = searchNode(-9999, 9999, depth);
                    board.takeback();

                    if(score > bestScore){
                        bestMove = currentMove;
                        bestScore = score;
                    }
                }
            }
        }
    }

    else{
        for(int move = 0; move < moveStackIndex; move++){
            currentMove = moveStack[move];
            if(board.make(currentMove)){
                int score = searchNode(-9999, 9999, depth);
                board.takeback();

                if(score > bestScore){
                    bestMove = currentMove;
                    bestScore = score;
                }
            }
        }
    }


    return bestMove;
}

int search::searchNode(int alpha, int beta, int depthLeft) {
    //If there is no depth left, we return the evaluation of the current position
    if(!depthLeft) {
        return quiescence(alpha, beta);
    }
    //Store all the generated moves
    movebits moveStack[256]{0};
    int moveStackIndex{0};
    //Otherwise, generate moves and search them
    board.gen(moveStack, moveStackIndex);

    movebits currentMove{0};
    for(int move = 0; move < moveStackIndex; move++){
        currentMove = moveStack[move];
        if(board.make(currentMove)){
            int score = -searchNode(-beta, -alpha, depthLeft-1);
            if(score > alpha && score < beta && move > 0){
                score = -searchNode(-beta, -alpha, depthLeft - 1); //Re search case
            }
            board.takeback();

            if(score > alpha) alpha = score;
            if(alpha >= beta) return alpha;
            beta = alpha + 1;
        }
    }
    return alpha;
}

int search::quiescence(int alpha, int beta) {
    int stand_pat = evaluation::eval(board);
    if(stand_pat >= beta){
        return stand_pat;
    }
    if(stand_pat < alpha - 900){ //Apply some delta pruning here
        return alpha;
    }
    if(alpha < stand_pat){
        alpha = stand_pat;
    }


    //Store all the generated moves
    movebits moveStack[256]{0};
    int moveStackIndex{0};
    board.genNoisy(moveStack, moveStackIndex);

    unsigned long long hash = board.positionHash;

    movebits currentMove{0};
    for(int move = 0; move < moveStackIndex; move++){
        currentMove = moveStack[move];
        if(board.make(currentMove)){
            int score = -quiescence(-beta, -alpha);
            board.takeback();
            if(board.positionHash != hash){
                std::cout << "The problem comes from the move " << std::hex << board_representation::fromSq(moveStack[move]) << " " << board_representation::toSq(moveStack[move]) << " " << std::dec << (int)board_representation::getFlag(moveStack[move]) << std::endl;
                display::showPosition(board);
                display::showPieceList(board);
            }

            if(score >= beta) return beta;
            if(score > alpha) alpha = score;
        }
    }
    return alpha;
}
