//
// Created by bitterboyy on 3/10/21.
//

#include "search.h"

movebits search::bestMove(int depth, std::vector<movebits> list, int nodes, int maxTime, bool infinite) {
    movebits bestMove{0};
    int bestScore{-9999};
    int alpha = -9999;
    int beta = 9999;
    movebits moveStack[256]{0};
    int moveStackIndex{0};
    int aspirationWindow = 25;

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
                    int score = searchNode(alpha, beta, searchDepth);
                    if(score <= alpha || score >= beta){
                        alpha = -9999;
                        beta = 9999;
                        aspirationWindow *= 10;
                        score = searchNode(alpha, beta, depth);
                    };
                    board.takeback();

                    if(score > bestScore){
                        bestMove = currentMove;
                        bestScore = score;
                    }
                }
                if((clock() - startTime)/CLOCKS_PER_SEC >= maxTime) break;
            }
            if((clock() - startTime)/CLOCKS_PER_SEC >= maxTime) break;
            alpha = bestScore - aspirationWindow;
            beta = bestScore + aspirationWindow;
        }
    }

    //Infinite search
    else if(infinite){
        for(int searchDepth = 1;; searchDepth++){
            for(int move = 0; move < moveStackIndex; move++){
                currentMove = moveStack[move];
                if(board.make(currentMove)){
                    int score = searchNode(alpha, beta, depth);
                    if(score <= alpha || score >= beta){
                        alpha = -9999;
                        beta = 9999;
                        aspirationWindow *= 10;
                        score = searchNode(alpha, beta, depth);
                    }
                    board.takeback();

                    if(score > bestScore){
                        bestMove = currentMove;
                        bestScore = score;
                    }
                }
            }
            alpha = bestScore - aspirationWindow;
            beta = bestScore + aspirationWindow;
        }
    }

    else{
        //We can't integrate an aspiration window here since we search for one depth only
        for(int move = 0; move < moveStackIndex; move++){
            currentMove = moveStack[move];
            if(board.make(currentMove)){
                int score = searchNode(alpha, beta, depth);
                board.takeback();

                if(score > bestScore){
                    bestMove = currentMove;
                    bestScore = score;
                }
            }
        }
    }

    collectPV();
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

            if(score < alpha && score > beta) storeTransposition(board.positionHash, depthLeft, score, EXACT_NODE, currentMove);
            if(score > alpha) alpha = score;
            else storeTransposition(board.positionHash, depthLeft, score, ALPHA_NODE, currentMove);
            if(alpha >= beta) {
                storeTransposition(board.positionHash, depthLeft, score, BETA_NODE, currentMove);
                return alpha;
            }
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

unsigned long long search::getTableIndex(unsigned long long int hash) {
    return (hash%ttableSize);
}

void search::storeTransposition(unsigned long long int hash, int depth, int score, char flag, movebits response) {
    int index = getTableIndex(hash);
    if(m_ttable.depth[index] < depth){
        m_ttable.hash[index] = hash;
        m_ttable.depth[index] = depth;
        m_ttable.score[index] = score;
        m_ttable.flag[index] = flag;
        m_ttable.response[index] = response;
    }
}

void search::collectPV() {
    int ttIndex = getTableIndex(board.positionHash);
    do {
        if(board.make(m_ttable.response[ttIndex])){
            m_pv.pvMoves[m_pv.lenght++] = m_ttable.response[ttIndex];
        }
        else break;
        ttIndex = getTableIndex(board.positionHash);
    } while(m_ttable.hash[ttIndex] == board.positionHash);
    for(int i = 0; i < m_pv.lenght; i++){
        board.takeback();
    }
}
