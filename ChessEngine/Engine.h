//
// Created by bitterboyy on 12/29/20.
//

#ifndef BAUB_CHESS_ENGINE_H
#define BAUB_CHESS_ENGINE_H

#include "TranspositionTable.h"
#include "BoardRepresentation.h"

class BoardRepresentation;


class Engine{
private:
    int m_depth;
    BoardRepresentation* m_board;
    //TranspositionTable m_table;

public:
    Engine(int depth, BoardRepresentation* board) : m_board(board), m_depth(depth){};
    int quiescence(int alpha, int beta);
    int alphaBeta(int alpha, int beta, int depth);
    Move search();

    friend BoardRepresentation;
};


#endif //BAUB_CHESS_ENGINE_H
