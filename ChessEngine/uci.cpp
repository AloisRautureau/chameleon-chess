//
// Created by bitterboyy on 3/11/21.
//

#include "uci.h"


std::vector<std::string> engineUCI::split(const std::string &s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}

void engineUCI::uciListen() {
    std::string commandString;
    std::cin >> commandString;
    std::vector<std::string> commandArguments = split(commandString, ' ');
    std::string command = commandArguments[0];
    commandArguments.erase(commandArguments.begin());

    switch(commandMapGUI[command]){
        case uci: //DONE
            uciSend("id name Chameleon");
            uciSend("id author Alois R");
            uciSend("uciok");
            break;
        case debug: //DONE
            m_debug = commandArguments[0] == "on";
            break;
        case isready: //DONE
            uciSend("readyok");
            break;
        case setoption:
            break;
        case registerr: //DONE
            registerCommand(commandArguments);
            break;
        case ucinewgame: //DONE
            //Resets the search, notably any transposition table
            m_search.newGame();
            break;
        case position: //DONE
            positionCommand(commandArguments);
            break;
        case go:
            goCommand(commandArguments);
            break;
        case stop:
            break;
        case ponderhit:
            break;
        case quit: //DONE
            exit(0);
    }
}

void engineUCI::uciSend(std::string commandString) {
    std::cout << commandString << std::endl;
}

void engineUCI::registerCommand(std::vector<std::string> args) {
    std::map<std::string, int> argsMap{
            {"name", 0},
            {"code", 1}
    };

    switch(args.size()){
        case 1: //The only possibility here is "later"
        default:
            break;
        case 2:
            switch(argsMap[args[0]]){
                case 0:
                    m_name = args[1];
                    break;
                case 1:
                    m_code = std::stoi(args[1]);
                    break;
            }
            break;

        case 4:
            switch(argsMap[args[0]]){
                case 0:
                    m_name = args[1];
                    break;
                case 1:
                    m_code = std::stoi(args[1]);
                    break;
            }
            switch(argsMap[args[2]]){
                case 0:
                    m_name = args[3];
                    break;
                case 1:
                    m_code = std::stoi(args[3]);
                    break;
            }
            break;
    }
}

void engineUCI::goCommand(std::vector<std::string> args) {
    enum goArgsEnum{
        searchmoves,
        ponder,
        wtime,
        btime,
        winc,
        binc,
        movestogo,
        depth,
        nodes,
        mate,
        movetime,
        infinite
    };
    std::map<std::string, int> goArgs{
            {"searchmoves", searchmoves},
            {"ponder", ponder},
            {"wtime", wtime},
            {"btime", btime},
            {"winc", winc},
            {"binc", binc},
            {"movestogo", movestogo},
            {"depth", depth},
            {"nodes", nodes},
            {"mate", mate},
            {"movetime", movetime},
            {"infinite", infinite},
    };

    std::vector<movebits> moveList = {};
    int nbNodes = 0;
    int time = 0;
    int maxDepth = 0;
    bool inf = false;
    int mateIn = 0;


    //We iterate the arguments given
    for(int i = 0; i < args.size(); i++){
        switch(goArgs[args[i]]){
            case searchmoves:
                //Increment i until we find something that is not a move
                for(int j = ++i; j < args.size(); j++, i++){
                    if(!j) break; //We need to use regex here
                    movebits move = board_representation::encodeMove(display::squareToIndex(std::string(args[j][0], args[j][1])), display::squareToIndex(std::string(args[j][2], args[j][3])), QUIET);
                    moveList.push_back(move);
                }
                break;
            case ponder:
                break;
            case wtime:
                //Sets the time left for white
                break;
            case btime:
                //Sets the time left for black
                break;
            case winc:
                //Sets the increment for white
                break;
            case binc:
                //Sets the increment for black
                break;
            case movestogo:
                //Sets move left until time control
                break;
            case depth:
                m_depth = std::stoi(args[++i]);
                break;
            case nodes:
                //Set a limited number of nodes to search
                nbNodes = std::stoi(args[++i]);
                break;
            case mate:
                //Search for a mate in x number of moves
                mateIn = std::stoi(args[++i]);
                break;
            case movetime:
                //Search for exactly x seconds
                time = std::stoi(args[++i]);
                break;
            case infinite:
                //Search until the stop command
                inf = true;
                break;
        }
    }
    movebits best;
    best = m_search.bestMove(maxDepth, moveList, nbNodes, time, infinite);
    int closestMate;
    closestMate = m_search.searchMate(mateIn);

    //Response to the GUI

    uciSend("bestmove " + display::displayMove(best));
}

void engineUCI::positionCommand(std::vector<std::string> args) {
    //First set the FEN
    if(args[0] == "startpos"){
        m_board.setFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    }
    else{
        m_board.setFEN(args[0]);
    }

    //Then iterate over all moves and make them
    for(int i = 2; i < args.size() - 1; i++){
        //TODO : Find a way to find move flag with from-to square
        movebits move = board_representation::encodeMove(display::squareToIndex(std::string(args[i][0], args[i][1])), display::squareToIndex(std::string(args[i][2], args[i][3])), QUIET);
        m_board.make(move);
    }
}

