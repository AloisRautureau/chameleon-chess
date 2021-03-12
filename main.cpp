#include <iostream>
#include "ChessEngine/board_representation.h"
#include "ChessEngine/display.h"
#include "ChessEngine/debug.h"
#include "ChessEngine/evaluation.h"
#include "ChessEngine/search.h"

int main() {
    system("clear");

    std::vector<std::string> fenTestPositions = {
            "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ", //startpos
            "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", //Kiwipete
            "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", //Position 5 from chessprogramming.org
            "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10" //Alternative pert @ Steven Edwards
    };

    board_representation board;
    evaluation eval; //Initialize the tables of the evaluation function
    search search(board, eval);

    display::showPosition(board);

    search.bestMove(0, {}, 0, 10);

    display::showPV(search);

    display::showPosition(board);


    return 0;
}
