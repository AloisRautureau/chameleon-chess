//
// Created by bitterboyy on 12/29/20.
//

#ifndef BAUB_CHESS_ENGINE_H
#define BAUB_CHESS_ENGINE_H

#include "BoardRepresentation.h"


class Engine{
    friend BoardRepresentation;

private:
    int m_depth;
    int m_side;
    BoardRepresentation* m_board;

public:
    Engine(int depth, int side, BoardRepresentation* board);
    std::vector<int> minimax();

    float followLine(BoardRepresentation board, int depth);
};


#endif //BAUB_CHESS_ENGINE_H
