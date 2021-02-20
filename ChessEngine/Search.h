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
    int max_depth;
    std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<double>> startTime;
    double searchTime = 0;
    bool timeElapsed = false;
    std::vector<MOVEBITS> principalVariation;

public:
    Search(Board& board, Evaluation eval, int depth): board(board), max_depth(depth), eval(eval){
    }

    MOVEBITS searchBestMove(double maxTime){
        //This is where we implement iterative deepening
        //The search gets a limited time where it searches deeper and deeper, until the time is elapsed.
        //It then returns the last move it has found!
        searchTime = maxTime;
        timeElapsed = false;
        MOVEBITS moveFound = 0;
        startTime = std::chrono::high_resolution_clock::now();

        for(int depth = 0; depth < max_depth && !timeElapsed; depth++){
            MOVEBITS checking = searchRootNode(depth);

            auto endTime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> diff =  endTime - startTime;
            timeElapsed = (diff.count() >= searchTime);
             if(!timeElapsed) moveFound = checking;
        }

        return moveFound;
    }

    //Seaches the root node, calls searchNode recursively to get a score for each move, and returns the best move based on score
    MOVEBITS searchRootNode(int depth){
        int alpha = -999999;
        int beta = 99999;
        MOVEBITS bestMove;

        int moveStackIndx = 0;
        MOVEBITS moveStack[256];

        board.gen();
        moveStackIndx = board.moveListIndx;
        for(int i = 0; i < board.moveListIndx; i++){
            moveStack[i] = board.moveList[i];
        }

        sortMoves(moveStack, moveStackIndx);

        ZOBHASH hash = board.getPositionHash();
        int tTableIndex = tTable.getTableIndex(hash);
        //We must first check if the current node is a transposition
        if(tTable.hashTable[tTableIndex] == hash){
            //If it's the case, we put this move as the first one to check in hopes to cause a cutoff
            //if the move isn't found, it was most likely illegal, this can happen in case of a key-collision
            moveToTop(moveStack, moveStackIndx, tTable.bestMoveTable[tTableIndex]);
        }

        //That's a checkmate or a stalemate
        if(moveStackIndx == 0) return 0;

        for(int move = 0; move < moveStackIndx; move++){
            if(board.makeMove(moveStack[move])){
                int score = searchNode(alpha, beta, depth);
                board.unmake();

                if(score >= beta){
                    //If the search causes a cutoff, we add it to the transposition table
                    tTable.storePosition(hash, moveStack[move], depth, score, tTable.LOWER);
                    continue;
                }

                if(score > alpha){
                    tTable.storePosition(hash, moveStack[move], depth, score, tTable.UPPER);

                    bestMove = moveStack[move];
                    alpha = score;
                }

                if(score < alpha && score > beta){
                    tTable.storePosition(hash, moveStack[move], depth, score, tTable.EXACT);
                }

                auto endTime = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> diff =  endTime - startTime;
                timeElapsed = (diff.count() >= searchTime);
                if(timeElapsed) return bestMove;
            }
        }
        std::cout << "Best move found at depth " << depth << " : " << bestMove << " with a score of " << alpha << std::endl;
        return bestMove;
    }

    //Searches a node and gives back a score to said node
    int searchNode(int alpha, int beta, int depth){
        if(depth == 0) return quiescence(alpha, beta);

        auto endTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff =  endTime - startTime;
        timeElapsed = (diff.count() >= searchTime);
        if(timeElapsed) return quiescence(alpha, beta);

        int moveStackIndx = 0;
        MOVEBITS moveStack[256];

        board.gen();
        moveStackIndx = board.moveListIndx;
        for(int i = 0; i < board.moveListIndx; i++){
            moveStack[i] = board.moveList[i];
        }

        sortMoves(moveStack, moveStackIndx);

        ZOBHASH hash = board.getPositionHash();
        int tTableIndex = tTable.getTableIndex(hash);
        //We must first check if the current node is a transposition
        if(tTable.hashTable[tTableIndex] == hash){
            //If it's the case, we put this move as the first one to check in hopes to cause a cutoff
            //if the move isn't found, it was most likely illegal, this can happen in case of a key-collision
            moveToTop(moveStack, moveStackIndx, tTable.bestMoveTable[tTableIndex]);
        }

        //That's a checkmate or a stalemate
        if(moveStackIndx == 0) return 0;

        for(int i = 0; i < moveStackIndx; i++){
            if(board.makeMove(moveStack[i])){
                int score = -searchNode(-beta, -alpha, depth-1);
                board.unmake();

                if(score >= beta){
                    //If the search causes a cutoff, we add it to the transposition table
                    tTable.storePosition(hash, moveStack[i], depth, score, tTable.LOWER);
                    return beta;
                }
                if(score > alpha){
                    tTable.storePosition(hash, moveStack[i], depth, score, tTable.UPPER);
                    alpha = score;
                }
                if(score < alpha && score > beta){
                    tTable.storePosition(hash, moveStack[i], depth, score, tTable.EXACT);
                }
            }
        }
        return alpha;
    }

    int quiescence(int alpha, int beta){
        int standPat = eval.evaluation();
        if(standPat >= beta) return beta;
        if(standPat < alpha - 900) return alpha;
        if(standPat > alpha) alpha = standPat;

        auto endTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff =  endTime - startTime;
        timeElapsed = (diff.count() >= searchTime);
        if(timeElapsed) return standPat;

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

    static void moveToTop(MOVEBITS moveStack[], int moveStackIndx, MOVEBITS move){
        for(int i = 0; i < moveStackIndx; i++){
            if(moveStack[i] == move){
                //Move all the elements one to the right and insert the move at the beginning
                for(int j = i; j > 0; j--){
                    MOVEBITS swap = moveStack[j];
                    moveStack[j] = moveStack[j-1];
                    moveStack[j-1] = swap;
                }
            }
        }
    }

    void collectPV(ZOBHASH hash){
        //Goes from the root position hash, then makes the best move, searches the new hash, etc
        if(tTable.getBestMove(hash) == 0 || principalVariation.size() > 10 || tTable.hashTable[tTable.getTableIndex(hash)] != hash) return;

        principalVariation.push_back(tTable.getBestMove(hash));
        board.makeMove(tTable.getBestMove(hash));
        std::cout << "Played the move " << tTable.getBestMove(hash) << std::endl;
        board.showBoard();
        collectPV(board.getPositionHash());
        board.unmake();
    }


};

#endif //BAUB_CHESS_SEARCH_H
