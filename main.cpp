#include <iostream>
#include "ChessEngine/board_representation.h"
#include "ChessEngine/display.h"
#include "ChessEngine/debug.h"
#include "ChessEngine/evaluation.h"
#include "ChessEngine/search.h"

using namespace Chameleon;

int main() {
    system("clear");

    board_representation board;

    display::showPosition(board);
    Evaluation::init();
    Debug::perft(board);



    return 0;
}
