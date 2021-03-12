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
        board.gen();
        //Store all the generated moves
        for(int i = 0; i < board.m_moveStackIndex; i++){
            moveStack[i] = board.m_moveStack[i];
            moveStackIndex++;
        }
    }
    else{
        for(int i = 0; i < list.size(); i++){
            moveStack[i] = list[i];
            moveStackIndex++;
        }
    }

    //TODO : limit to a given number of nodes
    //TODO : add time control, iterative deepening, etc


    movebits currentMove{0};
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
    return bestMove;
}

int search::searchNode(int alpha, int beta, int depthLeft) {
    //If there is no depth left, we return the evaluation of the current position
    if(!depthLeft) {
        return quiescence(alpha, beta);
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

    board.genNoisy();
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
            int score = -quiescence(-beta, -alpha);
            board.takeback();

            if(score >= beta) return beta;
            if(score > alpha) alpha = score;
        }
    }
    return alpha;
}
