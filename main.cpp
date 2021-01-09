#include <iostream>
#include "ChessEngine/BoardRepresentation.h"

int main() {
    BoardRepresentation testBoard = BoardRepresentation();

    testBoard.showCurrentPosition();


    int result = testBoard.play(true);

    switch(result){
        case 1:
            std::cout << "Black won!" << std::endl;
            break;

        case 2:
            std::cout << "White won!" << std::endl;
            break;

        case 0:
        case -1:
            std::cout << "Stalemate" << std::endl;
            break;

        default:
            std::cout << "Euh aled?" << std::endl;
            break;
    }
    return 0;
}
