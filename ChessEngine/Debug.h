//
// Created by bitterboyy on 1/26/21.
//

#ifndef BAUB_CHESS_DEBUG_H
#define BAUB_CHESS_DEBUG_H

#include "Board.h"
#include <map>

/*
 * This class was made because I despise having to search for bugs on my own, and GTest was too complicated to setup
 * So I made my own chess debugging framework, which isn't that great, but does the job
 */

class Debug{
private:
    Board& debug_board;
    Search& debug_search;
    Evaluation& debug_eval;

public:
    Debug(Board& board, Search& search, Evaluation& eval) : debug_board(board), debug_search(search), debug_eval(eval){};

    long long perft(int depth, int* quiet, int* caps, int* ep, int* castles, int* prom, int* checks, bool debug = false)
    {
        long long nodes = 0;

        if (depth == 0)
            return 1ULL;

        //If we're debugging, stores the original hash to check if there is no problem making/unmaking
        long long originalHash;
        if(debug) originalHash = debug_board.getPositionHash();

        //Generate and copy moves
        debug_board.gen();

        MOVEBITS stack[256];
        int moves = 0;
        for(int i = 0; i < debug_board.moveListIndx; i++){
            moves++;
            stack[i] = debug_board.moveList[i];
        }

        for (int i = 0; i < moves; i++) {
            if(debug_board.makeMove(stack[i])){
                if(debug){
                    if(originalHash == debug_board.getPositionHash()){
                        std::cout << "This position created a problem when making !" << std::endl;
                        debug_board.makeMove(stack[i]);
                        debug_board.showBoard();
                        std::cout << "Move : " << (int)fromSq(stack[i]) << " " << (int)toSq(stack[i]) << std::endl;
                        debug_board.unmake();
                        std::cin.get();
                    }
                }

                if(debug){
                    if(debug_board.isUnderAttack(debug_board.sideToMove == WHITE ? debug_board.whitePieces[5][0] : debug_board.blackPieces[5][0], debug_board.sideToMove^1)){
                        *checks = *checks+1;
                    }

                    switch(flag(stack[i])){
                        case QUIET:
                        case DPAWNPUSH:
                            *quiet = *quiet + 1;
                            break;

                        case CAP:
                            *caps = *caps + 1;
                            break;

                        case EPCAP:
                            *ep = *ep + 1;
                            break;

                        case KCASTLE:
                        case QCASTLE:
                            *castles = *castles + 1;
                            break;

                        case KNIGHTPROM:
                        case BISHOPPROM:
                        case ROOKPROM:
                        case QUEENPROM:
                            *prom = *prom + 1;
                            break;

                        default:break;
                    }
                }

                nodes += perft(depth - 1, quiet, caps, ep, castles, prom, checks, debug);
                debug_board.unmake();
            }

            if(debug){
                if(originalHash != debug_board.getPositionHash()){
                    std::cout << "This position created a problem when unmaking !" << std::endl;
                    debug_board.makeMove(stack[i]);
                    debug_board.showBoard();
                    std::cout << "Move : " << (int)fromSq(stack[i]) << " " << (int)toSq(stack[i]) << std::endl;
                    debug_board.unmake();
                    std::cin.get();
                }
            }
        }
        return nodes;
    }

    //Calls perft for each depth layer and tests whether the values are right or not with display
    void perftTest(bool debug = false, bool verbose = false){
        std::cout << "##### PERFT TIME YAY !!! #####" << std::endl << std::endl;

        int caps = 0, ep = 0, castles = 0, prom = 0, checks = 0, quiet = 0;
        int* capsptr = &caps; int* epptr = &ep; int* castlesptr = &castles;
        int* promptr = &prom; int* checksptr = &checks; int* quietptr = &quiet;

        for(int i = 0; i < 10; i++){
            caps = 0; ep = 0; castles = 0; prom = 0; checks = 0; quiet = 0;
            auto startTime = std::chrono::high_resolution_clock::now();

            long long nodes = perft(i, quietptr, capsptr, epptr, castlesptr, promptr, checksptr, debug);

            auto endTime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> diff =  endTime - startTime;

            std::cout << "Nodes at depth " << i << " : " << nodes << " calculated in " << diff.count() << " seconds (" << nodes/(diff.count()) << " n/s)" << std::endl;
            if(verbose) std::cout << "QUIET : " << quiet << "  CAPS : " << caps << "  EP : " << ep << "  CASTLES : " << castles << "  PROM : " << prom << "  CHECKS : " << checks << std::endl << std::endl;
            std::cout << std::endl;
        }
        delete capsptr;
        delete epptr;
        delete castlesptr;
        delete promptr;
        delete checksptr;
        delete quietptr;
    }

    /*
     * The debug environement lets you enter commands to test chess engine features
     * For example, make a certain move, unmake, search best move, etc etc
     */
    int debugEnv(){
        enum Commands{
            def,
            make,
            unmake,
            gen,
            inCheck,
            checkmate,
            stalemate,
            show,
            search,
            eval,
            quit
        };

        //Initialize the map
        std::map<std::string, Commands> commandsMap;
        commandsMap[""] = def;
        commandsMap["make"] = make;
        commandsMap["unmake"] = unmake;
        commandsMap["gen"] = gen;
        commandsMap["check"] = inCheck;
        commandsMap["checkmate"] = checkmate;
        commandsMap["stalemate"] = stalemate;
        commandsMap["show"] = show;
        commandsMap["search"] = search;
        commandsMap["eval"] = eval;
        commandsMap["exit"] = quit;

        std::cin.ignore();
        while(true){
            //Get user command+argument
            std::string input;
            getline(std::cin, input);

            std::vector<std::string> command = split(input, ' ');

            switch(commandsMap[command[0]]){
                case make:
                    if(command.size() > 1) debug_board.makeMove(std::stoi(command[1]));
                    else std::cout << "make needs at least 1 argument but " << command.size() - 1 << " were provided" << std::endl;
                    break;

                case unmake:
                    debug_board.unmake();
                    break;

                case gen:
                    std::cout << "Available moves : " << std::endl;
                    debug_board.gen(true);
                    break;

                case inCheck:
                    if(command.size() > 1) std::cout << (debug_board.inCheck(std::stoi(command[1])) ? "Side to move is in check" : "Side to move isn't in check") << std::endl;
                    else std::cout << "check needs at least 1 argument but " << command.size() - 1 << " were provided" << std::endl;
                    break;

                case checkmate:
                    if(command.size() > 1) std::cout << (debug_board.mated(std::stoi(command[1])) ? "Side to move is in mate" : "Side to move isn't mated") << std::endl;
                    else std::cout << "checkmate needs at least 1 argument but " << command.size() - 1<< " were provided" << std::endl;
                    break;

                case stalemate:
                    std::cout << (debug_board.stalemate() ? "This is a stalemate" : "This isn't a stalemate") << std::endl;
                    break;

                case show:
                    if(command.size() > 1) debug_board.showBoard(command[1] == "flip");
                    else debug_board.showBoard();
                    break;

                case search:
                    std::cout << "The engine found the best move to be " << debug_search.searchBestMove(5) << std::endl;
                    break;

                case eval:
                    std::cout << "The current position's evaluation is " << debug_eval.evaluation() << std::endl;
                    break;

                case quit:
                    return 0;

                default:
                    std::cout << "Invalid command, please try again" << std::endl;
            }
        }
    }

};

#endif //BAUB_CHESS_DEBUG_H
