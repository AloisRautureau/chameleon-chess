#include <iostream>
#include "ChessEngine/Board.h"
#include "ChessEngine/Search.h"
#include "ChessEngine/Debug.h"

void debugMenu(Debug& debug, Board& board){
    while(true){
        std::cout << "(1) Perft\n"
                     "(2) Menu\n" << std::endl;

        std::cout << "Enter your choice here : ";
        int choice = 1;
        std::cin >> choice;

        int choicePos = 1;
        int perftoption = false;

        switch(choice){
            case 1:
                std::cout << "Choose an initial position to test :" << std::endl << std::endl;
                std::cout << "(1) Initial\n"
                             "(2) Kiwipete\n"
                             "(3) Menu\n" << std::endl;

                std::cout << "Enter your choice here : ";
                std::cin >> choicePos;

                std::cout << "Do you want to debug from a performance perspective (0) or in depth (1) ? : ";
                std::cin >> perftoption;

                switch(choicePos){
                    case 1:
                    default:
                        debug.perftTest(perftoption, perftoption);
                        break;

                    case 2:
                        board.setFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ");
                        debug.perftTest(perftoption, perftoption);
                }
                break;

            case 2 :
                return;

            default:
                std::cout << "Please enter a valid option..." << std::endl << std::endl;
        }

    }



}

int main() {
    //Always initialize the zobrist keys
    initHash();

    //Welcome screen
    std::cout << " ______     ______     ______   _____     ______     ______     __  __     ______    \n"
                 "/\\  ___\\   /\\  ___\\   /\\  == \\ /\\  __-.  /\\  ___\\   /\\  == \\   /\\ \\/\\ \\   /\\  ___\\   \n"
                 "\\ \\___  \\  \\ \\ \\____  \\ \\  _-/ \\ \\ \\/\\ \\ \\ \\  __\\   \\ \\  __<   \\ \\ \\_\\ \\  \\ \\ \\__ \\  \n"
                 " \\/\\_____\\  \\ \\_____\\  \\ \\_\\    \\ \\____-  \\ \\_____\\  \\ \\_____\\  \\ \\_____\\  \\ \\_____\\ \n"
                 "  \\/_____/   \\/_____/   \\/_/     \\/____/   \\/_____/   \\/_____/   \\/_____/   \\/_____/ \n"
                 "                                                                                     \n"
                 "" << std::endl << std::endl;

    Board board;
    Debug debugger(board);
    std::cout << "Initial position board initialized..." << std::endl << std::endl;

    board.showBoard();

    while(true){
        std::cout << "(1) Play a test game (WIP)\n"
                     "(2) Launch a debugging function\n"
                     "(3) Quit\n" << std::endl;
        std::cout << "Enter your choice here : ";
        int choice = 1;
        std::cin >> choice;

        switch(choice){
            case 1:
                std::cout << "A bit too WIP for now !" << std::endl;
                break;

            case 2:
                debugMenu(debugger, board);
                break;

            case 3:
                std::cout << "Hasta la vista baby!" << std::endl;
                exit(0);

            default:
                std::cout << "Please enter a valide option..." << std::endl << std::endl;
        }
    }
    //Search search(board, Evaluation(board), 5);

    return 0;
}
