//
// Created by bitterboyy on 3/10/21.
//

#ifndef BAUB_CHESS_SEARCH_H
#define BAUB_CHESS_SEARCH_H

#include "board_representation.h"
#include "evaluation.h"
#include <chrono>

/*
 * The search function will feature alpha-beta search inside of a negamax framework as a starting point
 * Quiescence is also obligatory for a decently playing program
 * Later on transposition tables will be added
 */

class search {
protected:
    evaluation &eval;
    board_representation &board;

    //Generates every move, then calls itself for each move.
    //At depthLeft = 0, it returns the quiescence evalutation of the current position
    int searchNode(int alpha, int beta, int depthLeft);

    //Searches for a quiet position to minimize the impact of the event horizon
    int quiescence(int alpha, int beta);

    //The transposition table, used to store already reached positions to avoid reacalculating everything
    static const unsigned long long ttableSize = 1048583;
    struct ttable{
        unsigned long long* hash = new unsigned long long[ttableSize]{0}; //The complete hash for the position
        int* depth = new int[ttableSize]{0}; //The depth at which the position was reached
        int* score = new int[ttableSize]{0}; //The calculated score
        char* flag = new char[ttableSize]{0}; //Type of entry
        movebits* response = new movebits[ttableSize]{0}; //Best move to respond with
    }m_ttable;

    static const char EXACT_NODE = 0,
                      BETA_NODE = 1,
                      ALPHA_NODE = 2;

    unsigned long long getTableIndex(unsigned long long hash);

    void storeTransposition(unsigned long long hash, int depth, int score, char flag, movebits response);

    struct pv{
        int lenght;
        movebits pvMoves[16];
    }m_pv;

    friend display;

public:
    explicit search(board_representation &board, evaluation &eval): board(board), eval(eval){};
    //Calls searchNode recursively for every move generated in the root position, then compares the scores and
    //returns the highest scoring move
    movebits bestMove(int depth, std::vector<movebits> list = {}, int nodes = 0, int maxTime = 0, bool infinite = false);

    //Searches a mate in x moves
    movebits searchMate(int depth);

    //Collects the principal variation from the hash table after a search
    void collectPV();

    //Resets any info we have on the ongoing game to start fresh
    void newGame();
};


#endif //BAUB_CHESS_SEARCH_H
