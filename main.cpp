#include <iostream>
#include "ChessEngine/board_representation.h"
#include "ChessEngine/display.h"
#include "ChessEngine/debug.h"

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

    //board.setFEN("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");

    display::showPosition(board);
    display::showPieceList(board);

    debug::perftDivide(board);

    return 0;
}
