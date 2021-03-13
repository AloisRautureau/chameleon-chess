//
// Created by bitterboyy on 2/22/21.
//

#include "display.h"

using namespace Chameleon;

void display::showPosition(const board_representation &board) {
    int square;
    std::cout << std::endl << (board.m_side == WHITE ? "White " : "Black ") << "to move!" << std::endl << std::endl;
    std::cout << "+-----+-----+-----+-----+-----+-----+-----+-----+" << std::endl;
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            square = (7-i) * 0x10 + j;

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

    std::cout << std::endl << std::endl << "Move " << board.m_ply << " (" << board.m_halfclock << " moves since last capture or pawn move)" << std::endl;
    std::cout << "En passant square : " << indexToSquare(board.m_ep) << std::endl;
    std::cout << "Castling rights : " << (board.m_castling & WKCASTLE ? "K" : "")
    << (board.m_castling & WQCASTLE ? "Q" : "")
    << (board.m_castling & BKCASTLE ? "k" : "")
    << (board.m_castling & BQCASTLE ? "q" : "") << std::endl << std::endl;

    std::cout << "Position hash : " << board.positionHash << std::endl << std::endl;
}

std::string display::indexToSquare(int index) {
    if(index == inv) return "-";
    return std::string{char('a' + file(index)), char('1' + rank(index))};
}

int display::squareToIndex(std::string square) {
    if(square == "-") return inv;
    return (square[0] - 'a') + (square[1] - '1')*0x10 ;
}


std::string display::displayMove(movebits move, bool showFlag) {
    std::string writtenMove{};
    writtenMove += indexToSquare(board_representation::fromSq(move));
    writtenMove += indexToSquare(board_representation::toSq(move));

    if(showFlag){
        writtenMove += " : ";
        writtenMove += flagRep[board_representation::getFlag(move)];
    }

    return writtenMove;
}

void display::showPieceList(board_representation &board){
    for(int i = 0; i < 2; i++){
        std::cout << (i ? "BLACK " : "WHITE ") << "PIECES : " << std::endl;
        for(int j = 0; j < 6; j++){
            std::cout << pieceRepresentations[i][j] << " : ";
            for(int k = 0; k < board.m_plist[i][j].size(); k++){
                std::cout << std::hex << board.m_plist[i][j].get(k) << ", " << std::dec;
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
}

void display::showPV() {
    std::cout << "Principal variation : ";
    std::vector<movebits> pv = Search::getPV();
    for(int i = 0; i < pv.size(); i++) {
        std::cout << displayMove(pv[i]) << " ";
    }
    std::cout << std::endl;
}



