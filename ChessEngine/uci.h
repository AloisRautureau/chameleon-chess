//
// Created by bitterboyy on 3/13/21.
//

#ifndef CHAMELEON_UCI_H
#define CHAMELEON_UCI_H

/*
 * Implementation of the UCI protocol
 * It uses standard output
 */

#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <thread>
#include "search.h"
#include "debug.h"

namespace Chameleon{
    namespace UCI{
        // Helper functions for separating command/args among or stringify info
        std::vector<std::string> split(const std::string& s, char delimiter);
        std::string move_to_str(movebits move);
        movebits str_to_move(std::string move);
        std::string sq_to_str(int index);

        //Two functions made to either get or send info
        void uciListen(position &pos);
        void uciSend(const std::string& output);

        //Functions to answer uci input
        void init();
        void setoption(std::vector<std::string> args);
        void ucinewgame(std::vector<std::string> args);
        void setpos(position &pos, std::vector<std::string> args);
        bool go(position &pos, std::vector<std::string> args);
        void showPosition(position &pos);
    }
}


#endif //CHAMELEON_UCI_H
