#include <iostream>
#include "ChessEngine/board_representation.h"
#include "ChessEngine/display.h"
#include "ChessEngine/debug.h"
#include "ChessEngine/evaluation.h"
#include "ChessEngine/search.h"
#include "ChessEngine/cli.h"

int main() {
    system("clear");

    board_representation board;
    evaluation eval; //Initialize the tables of the evaluation function
    search search(eval);

    cli::showLogo();

    display::showPosition(board);

    cli::mainMenu(board);

    return 0;
}
