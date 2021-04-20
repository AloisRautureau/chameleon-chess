//
// Created by bitterboyy on 4/20/21.
//

#ifndef CHAMELEON_UCI_H
#define CHAMELEON_UCI_H

#include "defs.h"
#include "search.h"

//This will hold everything UCI related
using namespace Chameleon;

namespace UCI {
    //UCI commands are all passed through standard input and output, which makes it quite easy to implement
    //through a listen function meant to run in a separate thread from everything else
    void listen();

    //Helper function to parse input given through stdin as command + arguments
    void cmd_parser(const std::string& input, std::string &cmd, std::vector<std::string> &args);

    //Launches a search and returns a best move
    void go(position &pos, const std::vector<std::string>& args);

    //Sets the position of the board through the use of a fen string or a succession of moves
    void setPosition(position &pos, const std::vector<std::string>& args);
}

#endif //CHAMELEON_UCI_H
