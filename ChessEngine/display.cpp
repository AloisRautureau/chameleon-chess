//
// Created by bitterboyy on 2/22/21.
//

#include "display.h"

void display::showPosition(const board_representation &board) {
    sq square;
    std::cout << std::endl << (board.m_side == WHITE ? "White " : "Black ") << "to move!" << std::endl << std::endl;
    std::cout << "+-----+-----+-----+-----+-----+-----+-----+-----+" << std::endl;
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            square = sq((7-i) * 0x10 + j);

            switch(board.m_pieces[square]){
                case PAWN:
                    std::cout << (board.m_color[square] == WHITE ? "|  P  " : "|  p  ");
                    break;
                case KNIGHT:
                    std::cout << (board.m_color[square] == WHITE ? "|  N  " : "|  n  ");
                    break;
                case BISHOP:
                    std::cout << (board.m_color[square] == WHITE ? "|  B  " : "|  b  ");
                    break;
                case ROOK:
                    std::cout << (board.m_color[square] == WHITE ? "|  R  " : "|  r  ");
                    break;
                case QUEEN:
                    std::cout << (board.m_color[square] == WHITE ? "|  Q  " : "|  q  ");
                    break;
                case KING:
                    std::cout << (board.m_color[square] == WHITE ? "|  K  " : "|  k  ");
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

    std::cout << std::endl << std::endl << "En passant square : " << std::hex << board.m_ep << std::dec << std::endl;
    std::cout << "Castling rights : " << (board.m_castling & WKCASTLE ? "K" : "")
    << (board.m_castling & WQCASTLE ? "Q" : "")
    << (board.m_castling & BKCASTLE ? "k" : "")
    << (board.m_castling & BQCASTLE ? "q" : "") << std::endl << std::endl;
}

std::string display::indexToSquare(sq index) {
    if(index == inv) return "INV";
    return std::string{char('a' + file(index)), char('1' + rank(index))};
}

sq display::squareToIndex(std::string square) {
    if(square == "INV") return inv;
    return sq(square[0] - 'a' + square[1] - '0');
}

std::string display::displayMove(movebits move, bool showFlag) {
    std::string writtenMove{};
    writtenMove += indexToSquare(board_representation::fromSq(move));
    writtenMove += " -> ";
    writtenMove += indexToSquare(board_representation::toSq(move));

    if(showFlag){
        writtenMove += " : ";
        writtenMove += flagRep[board_representation::getFlag(move)];
    }

    return writtenMove;
}



