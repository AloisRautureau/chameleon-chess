#include <iostream>
#include "src/position.h"

using namespace Chameleon;

int main() {
    system("clear");

    position test = position();
    movestack stack;
    test.setFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    test.show();


    return 0;
}
