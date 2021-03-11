//
// Created by bitterboyy on 3/11/21.
//

#ifndef BAUB_CHESS_UCI_H
#define BAUB_CHESS_UCI_H

#include <iostream>
#include <map>
#include <vector>
#include "../BuildTools/rpi/arm-linux-gnueabihf/include/c++/4.8.3/sstream"
#include "board_representation.h"
#include "search.h"
#include "evaluation.h"
#include "display.h"

//Those are the commands the GUI may send to the engine
enum commandGUI{
    uci,
    debug,
    isready,
    setoption,
    registerr,
    ucinewgame,
    position,
    go,
    stop,
    ponderhit,
    quit,
};

static std::map<std::string, int> commandMapGUI = {
        {"uci", uci},
        {"debug", debug},
        {"isready", isready},
        {"setoption", setoption},
        {"register", registerr},
        {"ucinewgame", ucinewgame},
        {"position", position},
        {"go", go},
        {"stop", stop},
        {"ponderhit", ponderhit},
        {"quit", quit}
};

class engineUCI{
private:
    //The different representations
    board_representation &m_board;
    search &m_search;
    evaluation &m_eval;

    std::string m_name = "Chameleon";
    int m_code;
    bool m_debug;
    int m_depth = 0;

public:
    engineUCI(board_representation &board, search &search, evaluation & eval):
    m_board(board), m_search(search), m_eval(eval){};

    //Helper command
    static std::vector<std::string> split(const std::string& s, char delimiter);

    //Makes the engine wiat for the next command
    void uciListen();

    void uciSend(std::string command);

    //Specific commands to use with UCI
    //GUI commands
    void registerCommand(std::vector<std::string> args);
    void goCommand(std::vector<std::string> args);

    //Engine commands

    void positionCommand(std::vector<std::string> vector);
};


#endif //BAUB_CHESS_UCI_H
