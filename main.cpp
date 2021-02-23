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

    display::showPosition(board);

    debug::perft(board);

    display::showPosition(board);

    return 0;
}
