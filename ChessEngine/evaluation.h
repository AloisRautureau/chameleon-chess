//
// Created by bitterboyy on 3/4/21.
//

#ifndef BAUB_CHESS_EVALUATION_H
#define BAUB_CHESS_EVALUATION_H
#pragma once

/*
 * The program will use a simple tappered evaluation function dependant on side to move at first
 * The goal will be to implement more complex evaluation schemes as time goes on
 */

/* List of features :
 * [*] Material balance
 * [*] Piece-Square tables
 * [*] Tappered eval
 * [ ] Pawn structure
 */

#include "position.h"
#include "display.h"
class position;
typedef unsigned short movebits;

namespace Chameleon {
    namespace Evaluation {
        //Purely material considerations
        static const int m_pieceValue[6] = {100, 350, 350, 525, 1000, 10000};
        static const int knightPawnsValue[8] = {-20, -15, -10, 0, 10, 15, 20, 25};
        static const int rookPawnsValue[8] = {25, 20, 15, 10, 0, -10, -15, -20};
        static const int bishopPair = 50;
        static const int knightPair = -20;
        static const int rookPair = -10;
        static const int noPawnPenalty = -100;

        //Piece square tables are used to give a bonus for good piece placement on the board
        static int pcsqTable[6][0x88]{
                {// pawn
                        0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
                        50, 50, 50, 50, 50, 50, 50, 50, 0, 0, 0, 0, 0, 0, 0, 0,
                        10, 10, 20, 30, 30, 20, 10, 10, 0, 0, 0, 0, 0, 0, 0, 0,
                        5,  5, 10, 25, 25, 10,  5,  5, 0, 0, 0, 0, 0, 0, 0, 0,
                        0,  0,  0, 20, 20,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
                        5, -5,-10,  0,  0,-10, -5,  5, 0, 0, 0, 0, 0, 0, 0, 0,
                        5, 10, 10,-20,-20, 10, 10,  5, 0, 0, 0, 0, 0, 0, 0, 0,
                        0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
                },
                {// knight
                        -50,-40,-30,-30,-30,-30,-40,-50, 0, 0, 0, 0, 0, 0, 0, 0,
                        -40,-20,  0,  0,  0,  0,-20,-40, 0, 0, 0, 0, 0, 0, 0, 0,
                        -30,  0, 10, 15, 15, 10,  0,-30, 0, 0, 0, 0, 0, 0, 0, 0,
                        -30,  5, 15, 20, 20, 15,  5,-30, 0, 0, 0, 0, 0, 0, 0, 0,
                        -30,  0, 15, 20, 20, 15,  0,-30, 0, 0, 0, 0, 0, 0, 0, 0,
                        -30,  5, 10, 15, 15, 10,  5,-30, 0, 0, 0, 0, 0, 0, 0, 0,
                        -40,-20,  0,  5,  5,  0,-20,-40, 0, 0, 0, 0, 0, 0, 0, 0,
                        -50,-40,-30,-30,-30,-30,-40,-50, 0, 0, 0, 0, 0, 0, 0, 0,
                },
                {// bishop
                        -20,-10,-10,-10,-10,-10,-10,-20, 0, 0, 0, 0, 0, 0, 0, 0,
                        -10,  0,  0,  0,  0,  0,  0,-10, 0, 0, 0, 0, 0, 0, 0, 0,
                        -10,  0,  5, 10, 10,  5,  0,-10, 0, 0, 0, 0, 0, 0, 0, 0,
                        -10,  5,  5, 10, 10,  5,  5,-10, 0, 0, 0, 0, 0, 0, 0, 0,
                        -10,  0, 10, 10, 10, 10,  0,-10, 0, 0, 0, 0, 0, 0, 0, 0,
                        -10, 10, 10, 10, 10, 10, 10,-10, 0, 0, 0, 0, 0, 0, 0, 0,
                        -10,  5,  0,  0,  0,  0,  5,-10, 0, 0, 0, 0, 0, 0, 0, 0,
                        -20,-10,-10,-10,-10,-10,-10,-20, 0, 0, 0, 0, 0, 0, 0, 0,
                },
                {//rook
                        0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
                        5, 10, 10, 10, 10, 10, 10,  5, 0, 0, 0, 0, 0, 0, 0, 0,
                        -5,  0,  0,  0,  0,  0,  0, -5, 0, 0, 0, 0, 0, 0, 0, 0,
                        -5,  0,  0,  0,  0,  0,  0, -5, 0, 0, 0, 0, 0, 0, 0, 0,
                        -5,  0,  0,  0,  0,  0,  0, -5, 0, 0, 0, 0, 0, 0, 0, 0,
                        -5,  0,  0,  0,  0,  0,  0, -5, 0, 0, 0, 0, 0, 0, 0, 0,
                        -5,  0,  0,  0,  0,  0,  0, -5, 0, 0, 0, 0, 0, 0, 0, 0,
                        0,  0,  0,  5,  5,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
                },
                {//queen
                        -20,-10,-10, -5, -5,-10,-10,-20, 0, 0, 0, 0, 0, 0, 0, 0,
                        -10,  0,  0,  0,  0,  0,  0,-10, 0, 0, 0, 0, 0, 0, 0, 0,
                        -10,  0,  5,  5,  5,  5,  0,-10, 0, 0, 0, 0, 0, 0, 0, 0,
                        -5,  0,  5,  5,  5,  5,  0, -5, 0, 0, 0, 0, 0, 0, 0, 0,
                        0,  0,  5,  5,  5,  5,  0, -5, 0, 0, 0, 0, 0, 0, 0, 0,
                        -10,  5,  5,  5,  5,  5,  0,-10, 0, 0, 0, 0, 0, 0, 0, 0,
                        -10,  0,  5,  0,  0,  0,  0,-10, 0, 0, 0, 0, 0, 0, 0, 0,
                        -20,-10,-10, -5, -5,-10,-10,-20, 0, 0, 0, 0, 0, 0, 0, 0,
                },
                {//king
                        -30,-40,-40,-50,-50,-40,-40,-30, 0, 0, 0, 0, 0, 0, 0, 0,
                        -30,-40,-40,-50,-50,-40,-40,-30, 0, 0, 0, 0, 0, 0, 0, 0,
                        -30,-40,-40,-50,-50,-40,-40,-30, 0, 0, 0, 0, 0, 0, 0, 0,
                        -30,-40,-40,-50,-50,-40,-40,-30, 0, 0, 0, 0, 0, 0, 0, 0,
                        -20,-30,-30,-40,-40,-30,-30,-20, 0, 0, 0, 0, 0, 0, 0, 0,
                        -10,-20,-20,-20,-20,-20,-20,-10, 0, 0, 0, 0, 0, 0, 0, 0,
                        20, 20,  0,  0,  0,  0, 20, 20, 0, 0, 0, 0, 0, 0, 0, 0,
                        20, 30, 10,  0,  0, 10, 30, 20, 0, 0, 0, 0, 0, 0, 0, 0,
                },
        };

        void init();

        //Simply returns the given board's evaluation (positive => sideToMove is winning)
        int eval(position &board);

        //Uses the observation that when both sides have no pawns, they'll need at least a 4 pawn advantage in order
        //not to draw
        //It of course includes combinations of pieces that are a certain draw as well
        bool drawDetection(position &board, int materialScore);

        //A function that evaluates a capture/move from a pure material perspective, to take a guess at whether or not it might
        //even be worth playing without actually making any move
        static int see(const position& pos, movebits move, bool side);

        int getPieceValue(position &pos, char pieceType);
    }
}


#endif //BAUB_CHESS_EVALUATION_H
