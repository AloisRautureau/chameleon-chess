#include <iostream>
#include "src/position.h"

using namespace Chameleon;

int main() {
    system("clear");

    position test = position();
    movestack stack;
    test.perft(6);
    test.perft(7);


    return 0;
}
