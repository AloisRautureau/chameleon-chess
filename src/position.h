//
// Created by bitterboyy on 4/16/21.
//

#ifndef CHAMELEON_POSITION_H
#define CHAMELEON_POSITION_H

#include <string>
#include "defs.h"

namespace Chameleon {
        class position {
        protected:
            piece m_board[0x88]{EMPTY}; //The board holding piece placement information
            plist m_plists[2][6]; //Piece position info, with an array of 2x6 (color x pieces)

            bool m_side = W; //Variable to keep track of side to move with 0 = white and 1 = black
            uint8_t m_castling = 0b1111; //Variable keeping track of castling rights (KQkq)
            int m_ep = 0x88; //Variable keeping track of en passant square (0x88 means no en passant square is available)
            int m_ply = 0; //Number of moves played
            int m_fifty = 0; //Keeps track of fifty move rule
            bool m_checked = false; //Set to true if king checked

            pins m_pinned; //Pinned pieces
            pins m_pinning; //Pinning pieces

            std::stack<history_entry> m_history; //Keeps the moves made during the game, with additionnal info for unmake

            bool m_push[0x88]{true}; //Used mainly for check evasion, this is the push mask indicating which squares you can push to in order to get out of check
            bool m_capture[0x88]{true}; //Same as the above, but for captures
            bool m_doublechecked = false; //Keeps track of whether or not we're in a double check situation

        public:
            position();

            /*
            * GENERATORS
            */
            //Generates all legal moves for the given position
            void gen(movestack &stack);

            //Generates non quiet moves (captures, promotions, checks)
            void genNoisy(movestack &stack);

            /*
            * MAKE/UNMAKE
            */
            //Makes the move on the board
            void make(movebyte move);

            //Unmakes the last move played
            void unmake();

            /*
            * UTILITY
            */
            //Returns true if the given square is attacked by the opponent.
            //The kingxray flag dictates whether or not side-to-move's king should be ignored (useful to check for legal king moves)
            bool isAttacked(int square, bool kingxray = false);

            //Same as the above function, but this one fills an array with all of the attacking pieces instead
            void getAttackers(int square, std::vector<int>& attackers, std::vector<int>& deltas);

            //Calculates pinned pieces and stores them in an appropriate container (square of the pinned piece + delta it can move on)
            void updatePins();

            //Generates masks to get out of check
            void genEvasionMasks();

            //Checks for a rare case in which en passant moves can get illegal when the attacking pawn isn't pinned
            bool epGotcha(int epSquare, int captured);

            //Sets the board state to the given FEN string
            void setFEN(std::string fen);

            //Used to debug the position class, by making and unmaking all possible moves and comparing the results to
            //correct move generators
            void perft(int depth);
            uint64_t perftRecursive(int depth);

            //Display method
            void show();
        };
}
#endif //CHAMELEON_POSITION_H
