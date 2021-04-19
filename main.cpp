#include <iostream>
#include "src/position.h"

using namespace Chameleon;

int main() {
    system("clear");

    position test = position();
    movestack stack;
    test.show();
    std::cout << "INITIAL" <<std::endl << std::endl;
    test.perft(7); //Initial

    std::cout << "KIWIPETE" <<std::endl << std::endl;
    test.setFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"); //Kiwipete
    test.show();
    test.perft(6);

    std::cout << "POSITION 3" <<std::endl << std::endl;
    test.setFEN("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1"); //position 3
    test.show();
    test.perft(7);

    std::cout << "POSITION 4" <<std::endl << std::endl;
    test.setFEN("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1"); //Pos 4 (white perspective)
    test.show();
    test.perft(6);

    std::cout << "POSITION 4 (mirror)" <<std::endl << std::endl;
    test.setFEN("r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1"); //Pos 4 (mirrored)
    test.show();
    test.perft(6);

    std::cout << "POSITION 5" <<std::endl << std::endl;
    test.setFEN("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8"); //Pos 5
    test.show();
    test.perft(5);

    std::cout << "POSITION 6" <<std::endl << std::endl;
    test.setFEN("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10"); //Pos 6
    test.show();
    test.perft(7);


    return 0;
}
