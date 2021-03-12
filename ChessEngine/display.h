//
// Created by bitterboyy on 2/22/21.
//

#ifndef BAUB_CHESS_DISPLAY_H
#define BAUB_CHESS_DISPLAY_H

#include "board_representation.h"
#include "search.h"

static const char pieceRepresentations[2][6] = {
        {'P', 'N', 'B', 'R', 'Q', 'K'},
        {'p', 'n', 'b', 'r', 'q', 'k'}
};

static const std::string flagRep[16]{
    "NULL", "DOUBLE PAWN PUSH", "KINGSIDE CASTLE", "QUEENSIDE CASTLE", "CAPTURE", "EN PASSANT",
    "", "", "KNIGHT PROMOTION", "BISHOP PROMOTION", "ROOK PROMOTION", "QUEEN PROMOTION",
    "KNIGHT PROMOCAPTURE", "BISHOP PROMOCAPTURE", "ROOK PROMOCAPTURE", "QUEEN PROMOCAPTURE",
};

class display {
public:
    //Displays the current position in the console
    static void showPosition(const board_representation &board);

    static std::string indexToSquare(int index);

    static int squareToIndex(std::string square);

    static std::string displayMove(movebits move, bool showFlag = false);

    static void showPieceList(board_representation &board);

    static void showPV(search &search);
};


#endif //BAUB_CHESS_DISPLAY_H
