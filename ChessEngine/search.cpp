//
// Created by bitterboyy on 3/10/21.
//

#include "search.h"

movebits search::bestMove(board_representation &board, int depth) {
    movebits bestMove{0};
    int bestScore{-9999};

    //Generate moves
    board.gen();

    //Store all the generated moves
    movebits moveStack[256]{0};
    int moveStackIndex{0};
    for(int i = 0; i < board.m_moveStackIndex; i++){
        moveStack[i] = board.m_moveStack[i];
        moveStackIndex++;
    }

    movebits currentMove{0};
    for(int move = 0; move < moveStackIndex; move++){
        currentMove = moveStack[move];
        board.make(currentMove);
        int score = searchNode(-9999, 9999, depth, board);
        board.takeback();

        if(score > bestScore){
            bestMove = currentMove;
            bestScore = score;
        }
    }
    return bestMove;
}

int search::searchNode(int alpha, int beta, int depthLeft, board_representation &board) {
    //If there is no depth left, we return the evaluation of the current position
    if(!depthLeft) {
        return evaluation::eval(board);
    }
    //Otherwise, generate moves and search them
    board.gen();
    //Store all the generated moves
    movebits moveStack[256]{0};
    int moveStackIndex{0};
    for(int i = 0; i < board.m_moveStackIndex; i++){
        moveStack[i] = board.m_moveStack[i];
        moveStackIndex++;
    }

    movebits currentMove{0};
    for(int move = 0; move < moveStackIndex; move++){
        currentMove = moveStack[move];
        if(board.make(currentMove)){
            int score = -searchNode(-beta, -alpha, depthLeft-1, board);
            board.takeback();

            if(score >= beta) return beta;
            if(score > alpha) alpha = score;
        }
    }
    return alpha;
}
