//
// Created by bitterboyy on 2/22/21.
//

#include "display.h"

void display::showPosition(const board_representation &board) {
    sq square;
    std::cout << std::endl << (board.m_sideToMove == WHITE ? "White " : "Black ") << "to move!" << std::endl;
    std::cout << "+-----+-----+-----+-----+-----+-----+-----+-----+" << std::endl;
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            square = sq((7-i) * 0x10 + j);

            switch(board.m_piecesBoard[square]){
                case PAWN:
                    std::cout << (board.m_colorBoard[square] == WHITE ? "|  P  " : "|  p  ");
                    break;
                case KNIGHT:
                    std::cout << (board.m_colorBoard[square] == WHITE ? "|  N  " : "|  n  ");
                    break;
                case BISHOP:
                    std::cout << (board.m_colorBoard[square] == WHITE ? "|  B  " : "|  b  ");
                    break;
                case ROOK:
                    std::cout << (board.m_colorBoard[square] == WHITE ? "|  R  " : "|  r  ");
                    break;
                case QUEEN:
                    std::cout << (board.m_colorBoard[square] == WHITE ? "|  Q  " : "|  q  ");
                    break;
                case KING:
                    std::cout << (board.m_colorBoard[square] == WHITE ? "|  K  " : "|  k  ");
                    break;
                case EMPTY:
                    std::cout << "|     ";
                    break;
            }


            //End of rank, print the rank and return
            if(file(square) == 7) std::cout << "|  " << rank(square)+1 << std::endl << "+-----+-----+-----+-----+-----+-----+-----+-----+" << std::endl;
        }
    }

    //We end up by printing the file names
    for(int i = 0; i < 8; i++){
        std::cout << "   " << char('A' + i) << "  ";
    }

    std::cout << std::endl << "En passant square : " << std::hex << board.m_ep << std::dec << std::endl;
}
