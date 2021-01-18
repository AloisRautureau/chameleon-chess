#include <iostream>
#include "ChessEngine/Board.h"


int main() {
    Board board;

    board.evaluation();

    board.perftTest();

    //While no side is in checkmate, we keep asking for moves
    while(!(board.checkmate(WHITE) || board.checkmate(BLACK))){
        //Show the current position
        board.showPosition();
        board.generate();

        movebits move = 0;

        while(move == 0){
            std::string moveh;
            std::cin >> moveh;
            //Check if the move is in the generated list
            move = board.isLegal(humanMove(moveh));
            if(move){;
                if(!board.makeMove(move)){
                    std::cout << "Illegal move" << std::endl;
                }
            }
            else std::cout << "Illegal move" << std::endl;
        }
    }
    return 0;
}
