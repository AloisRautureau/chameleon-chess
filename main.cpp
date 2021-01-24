#include <iostream>
#include "ChessEngine/Board.h"


int main() {
    Board board;
    board.setFEN("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");

    //board.makeMove(33280);
    //board.gen();

    board.perftTest();
    return 0;
}
