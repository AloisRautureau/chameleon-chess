//
// Created by bitterboyy on 4/20/21.
//

#include "uci.h"

using namespace Chameleon;

namespace UCI {
    void listen() {
        position pos;
        std::string command{}; //Stores the main command
        std::vector<std::string> args{}; //Stores the arguments passed with the command
        std::string input{};

        //The loop itself
        do {
            args.clear();
            command.clear();
            input.clear();

            std::getline(std::cin, input); //Get user input
            cmd_parser(input, command, args);

            //Now we try to match the command to send back the correct info
            if(command == "uci") {
                std::cout << "id name Chameleon 0.4" << std::endl;
                std::cout << "id author Aloïs.R" << std::endl;
                std::cout << "uciok" << std::endl;
            }
            else if(command == "isready") std::cout << "readyok" << std::endl;
            else if(command == "ucinewgame") 0;
            else if(command == "go") go(pos, args);
            else if(command == "show") pos.show();
            else if(command == "position") setPosition(pos, args);
        } while(command != "quit");
    }

    void cmd_parser(const std::string& input, std::string &cmd, std::vector<std::string> &args) {
        std::string swap;
        bool cmd_stored{false};
        for(char c : input) {
            if(c == ' ') {
                if(!cmd_stored) {
                    cmd = swap;
                    cmd_stored = true;
                }
                else {
                    args.push_back(swap);
                }
                swap = "";
            }
            else {
                swap += c;
            }
        }

        if(!cmd_stored) {
            cmd = swap;
        }
        else args.push_back(swap);
    }

    void go(position &pos, const std::vector<std::string>& args) {
        movestack searchmoves;
        bool ponder{false};
        int wtime{9999999};
        int btime{9999999};
        int winc{0};
        int binc{0};
        int depth{99};
        int movetime{0};
        bool infinite{false};

        for(int i = 0; i < args.size(); i++) {
            if(args.at(i) == "depth") depth = std::stoi(args.at(++i));
            else if(args.at(i) == "movetime") movetime = (std::stoi(args.at(++i)))*1000;
            else if(args.at(i) == "winc") winc = (std::stoi(args.at(++i)));
            else if(args.at(i) == "binc") binc = (std::stoi(args.at(++i)));
            else if(args.at(i) == "wtime") wtime = (std::stoi(args.at(++i)));
            else if(args.at(i) == "btime") btime = (std::stoi(args.at(++i)));
        }

        Search::search_root(pos, depth, movetime, (pos.m_side ? btime : wtime), (pos.m_side ? binc : winc));
    }

    void setPosition(position &pos, const std::vector<std::string>& args) {
        movestack stack;
        pos.gen(stack);
        int i = 1;

        if(args.at(0) == "startpos"){
            pos.setFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        } else {
            //We'll first need to reconstruct the fen string, as it gets chopped up in the process of separating command
            // and arguments. We fuse the arguments until we hit a move
            std::string swap = args.at(0);
            for(i; i < args.size(); i++){
                if(!stringToMove(stack, args.at(i))) swap += " " + args.at(i);
            }

            pos.setFEN(swap);
        }

        i++;
        for(i; i < args.size(); i++){
            pos.gen(stack);
            pos.make(stringToMove(stack, args.at(i)));
        }
    }
}