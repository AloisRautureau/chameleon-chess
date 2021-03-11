#include <iostream>
#include "ChessEngine/board_representation.h"
#include "ChessEngine/display.h"
#include "ChessEngine/debug.h"
#include "ChessEngine/evaluation.h"
#include "ChessEngine/search.h"

int main() {
    system("clear");

    board_representation board;
    evaluation eval; //Initialize the tables of the evaluation function
    search search(eval);

    std::cout << display::displayMove(search::bestMove(board, 5)) << std::endl;

    display::showPosition(board);

    debug::perft(board);

    return 0;
}
