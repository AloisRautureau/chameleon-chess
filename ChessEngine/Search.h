//
// Created by bitterboyy on 1/26/21.
//

#ifndef BAUB_CHESS_SEARCH_H
#define BAUB_CHESS_SEARCH_H

#include "Evaluation.h"

/*
 * The Search class will, given a board and a depth, serach the best move from the board's position using alpha-beta pruning
 */

class Search{
private:
    /*
     * The search functions need to make changes to the board, as well as evaluate.
     * Therefore, we need boath to be implemented
     */
    Board& board;
    Evaluation eval;
    transpositionTable tTable;
    const int depth;

public:
    Search(Board& board, Evaluation eval, int depth): board(board), depth(depth), eval(eval){
        initTable(tTable);
    }

    //Seaches the root node, calls searchNode recursively to get a score for each move, and returns the best move based on score
    MOVEBITS searchBestMove(){
        int bestScore = -999999;
        MOVEBITS bestMove;

        int moveStackIndx = 0;
        MOVEBITS moveStack[256];

        board.gen();
        moveStackIndx = board.moveListIndx;
        for(int i = 0; i < board.moveListIndx; i++){
            moveStack[i] = board.moveList[i];
        }

        //That's a checkmate or a stalemate
        if(moveStackIndx == 0) return 0;

        for(int move = 0; move < moveStackIndx; move++){
            if(board.makeMove(moveStack[move])){
                int score = searchNode(-99999, 99999, 1);
                board.unmake();

                if(score > bestScore){
                    bestMove = moveStack[move];
                    bestScore = score;
                }
            }
        }
        return bestMove;
    }

    //Searches a node and gives back a score to said node
    int searchNode(int alpha, int beta, int currentDepth){
        if(currentDepth == depth) return quiescence(alpha, beta);

        int moveStackIndx = 0;
        MOVEBITS moveStack[256];

        board.gen();
        moveStackIndx = board.moveListIndx;
        for(int i = 0; i < board.moveListIndx; i++){
            moveStack[i] = board.moveList[i];
        }

        sortMoves(moveStack, moveStackIndx);

        //That's a checkmate or a stalemate
        if(moveStackIndx == 0) return 0;

        for(int i = 0; i < moveStackIndx; i++){
            if(board.makeMove(moveStack[i])){
                int score = -searchNode(-beta, -alpha, currentDepth+1);
                board.unmake();

                if(score >= beta) return beta;
                if(score > alpha) alpha = score;
            }
        }
        return alpha;
    }

    int quiescence(int alpha, int beta){
        int standPat = eval.evaluation();
        if(standPat >= beta) return beta;
        if(standPat < alpha - 900) return alpha;
        if(standPat > alpha) alpha = standPat;

        int moveStackIndx = 0;
        MOVEBITS moveStack[256];

        board.genCaps();
        moveStackIndx = board.moveListIndx;
        for(int i = 0; i < board.moveListIndx; i++){
            moveStack[i] = board.moveList[i];
        }

        sortMoves(moveStack, moveStackIndx);

        //That's a checkmate or a stalemate
        if(moveStackIndx == 0) return 0;

        for(int i = 0; i < moveStackIndx; i++){
            if(board.makeMove(moveStack[i])){
                int score = -quiescence(-beta, -alpha);
                board.unmake();

                if(score >= beta) return beta;
                if(score > alpha) alpha = score;
            }
        }
        return alpha;
    }

    //Sorts a given movestack, castling is prioritized, then captures come close then lastly quiet moves don't get any sort of bonus
    static void sortMoves(MOVEBITS moveStack[], int moveStackIndex){
        for(int i = 1; i < moveStackIndex; i++){
                MOVEBITS key = moveStack[i];
                if(flag(key) > flag(moveStack[i-1])){
                        int prec = i-1;
                        while(prec >= 0 && flag(moveStack[prec]) > flag(key)){
                                moveStack[prec + 1] = moveStack[prec];
                                prec--;
                        }
                        moveStack[prec+1] = key;
                }
        }
    }


};

#endif //BAUB_CHESS_SEARCH_H
