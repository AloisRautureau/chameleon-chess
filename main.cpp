#include <iostream>
#include "ChessEngine/BoardRep.h"
#include "ChessEngine/Engine.h"



int main() {
    BoardRep board = BoardRep();
    std::cout << &board << std::endl;

    Engine engine = Engine(3, &board);

    int engineSide = 0;
    bool engineOnly = true;

    while(true){
        Move move = Move(0, 0, 0);
        bool legal;

        board.showCurrentPosition();
        board.generatePseudoLegal();
        do{
            if(engineSide == board.getSide() || engineOnly) {
                move = engine.searchRoot(board.getSide());
                legal = true;
            }
            else{
                std::string humanMove;
                std::string startstr, endstr;
                char spestr;
                int start, end, spe;

                std::cin >> humanMove;

                if(humanMove.size() >= 5){
                    startstr = humanMove.substr(0, 2); start = coordinateToInt(startstr);
                    endstr = humanMove.substr(2, 2); end = coordinateToInt(endstr);
                    spestr = humanMove[4]; spe = spestr - '0';
                    move = board.getMoveStack().searchMove(start, end, spe);
                }
                else{
                    startstr = humanMove.substr(0, 2); start = coordinateToInt(startstr);
                    endstr = humanMove.substr(2, 2); end = coordinateToInt(endstr);
                    move = board.getMoveStack().searchMove(start, end, -1);
                }
                legal = move.start != 0 && move.end != 0;
                std::cout << "aled" << legal << std::endl;
            }
        }while(!legal);
        board.generatePseudoLegal();

        board.makeMove(move);
    }


    return 0;
}
