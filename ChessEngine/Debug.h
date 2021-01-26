//
// Created by bitterboyy on 1/26/21.
//

#ifndef BAUB_CHESS_DEBUG_H
#define BAUB_CHESS_DEBUG_H

#include "Board.h"

/*
 * This class was made because I despise having to search for bugs on my own, and GTest was too complicated to setup
 * So I made my own chess debugging framework, which isn't that great, but does the job
 */

class Debug{
private:
    Board& debug_board;

public:
    Debug(Board& board) : debug_board(board){};

    long long perft(int depth, int* quiet, int* caps, int* ep, int* castles, int* prom, int* checks, bool debug = false, bool verbose = false)
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

                if(verbose){
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

        for(int i = 0; i < 14; i++){
            caps = 0; ep = 0; castles = 0; prom = 0; checks = 0; quiet = 0;
            auto startTime = std::chrono::high_resolution_clock::now();

            long long nodes = perft(i, quietptr, capsptr, epptr, castlesptr, promptr, checksptr, debug, verbose);

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

};

#endif //BAUB_CHESS_DEBUG_H
