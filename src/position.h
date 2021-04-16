//
// Created by bitterboyy on 4/16/21.
//

#ifndef CHAMELEON_POSITION_H
#define CHAMELEON_POSITION_H

#include <string>
#include "defs.h"

using namespace Chameleon;

namespace Position {
    class position {
    protected:
        piece m_board[0x88]; //The board holding piece placement information
        bool m_side = 0; //Variable to keep track of side to move
        uint8_t m_castling = 0b1111; //Variable keeping track of castling rights
        int m_ep = 0x88; //Variable keeping track of en passant square (0x88 means no en passant square is available)
        int m_ply = 0; //Number of moves
        int m_fifty = 0; //Keeps track of fifty move rule

    public:

        /*
         * GENERATORS
         */
        //Generates all legal moves for the given position
        void gen(movestack &stack);

        //Generates non quiet moves (captures, promotions, checks)
        void genNoisy(movestack &stack);

        //Generates moves to get the king out of check
        void genEvasion(movestack &stack);

        /*
         * MAKE/UNMAKE
         */
        //Makes the move on the board
        void make(movebits move);

        //Unmakes the last move played
        void unmake();

        /*
         * UTILITY
         */
        //Sets the board state to the given FEN string
        void setFEN(std::string fen);

        //Used to debug the position class, by making and unmaking all possible moves and comparing the results to
        //correct move generators
        void perft(int depth, bool divide);
    };
}
#endif //CHAMELEON_POSITION_H
