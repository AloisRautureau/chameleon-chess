#include <iostream>
#include "ChessEngine/Board.h"
#include "ChessEngine/Search.h"
#include "ChessEngine/Debug.h"
#include "ChessEngine/OpeningBookHandler.h"

void debugMenu(Debug& debug, Board& board){
    while(true){
        std::cout << "(1) Perft\n"
                     "(2) Debugging environement\n"
                     "(3) Menu\n" << std::endl;

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
                             "(3) Position 3\n"
                             "(4) Position 5\n"
                             "(5) Position 6\n"
                             "(6) Menu\n" << std::endl;

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
                        board.setFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ");

                    case 3:
                        board.setFEN("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ");
                        debug.perftTest(perftoption, perftoption);
                        board.setFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

                    case 4:
                        board.setFEN("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
                        debug.perftTest(perftoption, perftoption);
                        board.setFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

                    case 5:
                        board.setFEN("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");
                        debug.perftTest(perftoption, perftoption);
                        board.setFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

                    case 6:
                        break;
                }
                break;

            case 2:
                debug.debugEnv();
                break;

            case 3:
                return;

            default:
                std::cout << "Please enter a valid option..." << std::endl << std::endl;
        }

    }



}

int main() {
    //Test for book handler
    BookHandler openingBook("openingBook.bin");

    bookEntry e4 = {29217, 1, 1, 0, 0, 0, 1, -1};
    bookEntry c5 = {127649, 1, 1, 0, 0, 1, -1, -1};
    bookEntry f3 = {12688, 1, 1, 0, 0, 2, -1, -1};
    std::vector<bookEntry> defaultScicillian = {e4, c5, f3};
    openingBook.addLine(defaultScicillian);
    openingBook.printBook();

    openingBook.saveBook();

    //bookEntry testRead = openingBook.readEntry(0);
    //std::cout << testRead.move << " " << testRead.nextSibling << std::endl;

    //Always initialize the zobrist keys
    initHash();

    //Welcome screen
    system("clear");
    std::cout << " ______     ______     ______   _____     ______     ______     __  __     ______    \n"
                 "/\\  ___\\   /\\  ___\\   /\\  == \\ /\\  __-.  /\\  ___\\   /\\  == \\   /\\ \\/\\ \\   /\\  ___\\   \n"
                 "\\ \\___  \\  \\ \\ \\____  \\ \\  _-/ \\ \\ \\/\\ \\ \\ \\  __\\   \\ \\  __<   \\ \\ \\_\\ \\  \\ \\ \\__ \\  \n"
                 " \\/\\_____\\  \\ \\_____\\  \\ \\_\\    \\ \\____-  \\ \\_____\\  \\ \\_____\\  \\ \\_____\\  \\ \\_____\\ \n"
                 "  \\/_____/   \\/_____/   \\/_/     \\/____/   \\/_____/   \\/_____/   \\/_____/   \\/_____/ \n"
                 "                                                                                     \n"
                 "" << std::endl << std::endl;

    Board board;
    Evaluation eval(board);
    Search search(board, eval, 6);
    Debug debugger(board, search, eval);
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
