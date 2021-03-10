#include <iostream>
#include "ChessEngine/board_representation.h"
#include "ChessEngine/display.h"
#include "ChessEngine/debug.h"
#include "ChessEngine/evaluation.h"
#include "ChessEngine/search.h"

int main() {
    system("clear");
    std::cout << " ______     ______     ______   _____     ______     ______     __  __     ______    \n"
                 "/\\  ___\\   /\\  ___\\   /\\  == \\ /\\  __-.  /\\  ___\\   /\\  == \\   /\\ \\/\\ \\   /\\  ___\\   \n"
                 "\\ \\___  \\  \\ \\ \\____  \\ \\  _-/ \\ \\ \\/\\ \\ \\ \\  __\\   \\ \\  __<   \\ \\ \\_\\ \\  \\ \\ \\__ \\  \n"
                 " \\/\\_____\\  \\ \\_____\\  \\ \\_\\    \\ \\____-  \\ \\_____\\  \\ \\_____\\  \\ \\_____\\  \\ \\_____\\ \n"
                 "  \\/_____/   \\/_____/   \\/_/     \\/____/   \\/_____/   \\/_____/   \\/_____/   \\/_____/ \n"
                 "                                                                                     \n"
                 "" << std::endl << std::endl;

    board_representation board;
    evaluation eval; //Initialize the tables of the evaluation function
    search search(eval);

    display::showPosition(board);

    std::cout << display::displayMove(search::bestMove(board, 6)) << std::endl;

    return 0;
}
