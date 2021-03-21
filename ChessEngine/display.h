//
// Created by bitterboyy on 2/22/21.
//

#ifndef BAUB_CHESS_DISPLAY_H
#define BAUB_CHESS_DISPLAY_H
#pragma once
#include "position.h"
class position;
typedef unsigned short movebits;


static const char pieceRepresentations[2][6] = {
        {'P', 'N', 'B', 'R', 'Q', 'K'},
        {'p', 'n', 'b', 'r', 'q', 'k'}
};

class display {
public:
    //Displays the current position in the console
    static void showPosition(const position &board);

    static std::string indexToSquare(int index);

    static int squareToIndex(std::string square);

    static std::string displayMove(movebits move);

    static void showPieceList(position &board);

};


#endif //BAUB_CHESS_DISPLAY_H
