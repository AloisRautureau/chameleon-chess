//
// Created by bitterboyy on 28/01/2021.
//

#ifndef BAUB_CHESS_OPENINGBOOKHANDLER_H
#define BAUB_CHESS_OPENINGBOOKHANDLER_H

#include <iostream>
#include <fstream>
#include <utility>
#include "Constants.h"

/*
 * A completely separate program from the main chess engine, used to manage the opening book
 * It's purpose is to add, remove or modify entries easily through the command line
 * It affects the openingBook file
 *
 * The opening book uses the Arena's Book format (ABK), altough I made some modifications
 * It stores an array of structures to a binary file
 * The structure of an entry can be described as such :
 * struct BookMoveEntry {
 * char from;
 * char to;
 * int ngames;
 * int nwon;
 * int nlost;
 * int plycount;        //Move order
 * int nextMove;        //Index of the next move in the current line
 * int nextSibling;     //Let's us swap to a different line if necessary
 * }
 */
//A book entry takes up 256 bits of space
struct bookEntry {
    MOVEBITS move;
    int games;
    int wins;
    int losses;
    int draws;
    int ply;
    int nextMove;
    int nextSibling;
};

class BookHandler{
private:
    std::string pathToBook;
    std::fstream bookFile;

    //The book handler loads the whole book in memory to ease up management
    //Sure, it's kinda slow and certinely not optimized, but since book generation will likely be a one time occurence,
    //execution speed is not a primary concern. Correctness, on the other hand, is.
    bookEntry openingBook[100000];
    int maxEntry = -1;

public:
    explicit BookHandler(std::string path): pathToBook(std::move(path)){
        bookFile.open(pathToBook, std::ios::binary);
        if(!bookFile){
            std::cout << "Erreur lors de l'ouverture du fichier " + path << std::endl;
        }
        int i = 0;
        while(openingBook[i-1].move != 0){
                openingBook[i] = readEntry(i, true);
		        maxEntry++;
		        i++;
        }
        //We need at least an entry point, so openingBook[0] will always be e4 if it isn't found
        bookEntry e4 = {29217, 1, 1, 0, 0, 0, 1, -1};
        if(openingBook[0].move != e4.move){
            openingBook[0] = e4;
        }
    };

    ~BookHandler(){
        bookFile.close();
    }

    //CLI interface to interact with the book, modifying, adding, generating, deleting line, etc etc
    int openingBookWizard(){
        std::cout << "#### Welcome to the Opening Book Wizard of SCP ! ####" << std::endl << std::endl;

        std::cout << "Choose one of the options listed below to start : " << std::endl;
        std::cout << "(1) Generate from game collection\n"
                     "(2) Manage the current book\n"
                     "(3) Quit\n" << std::endl;

        while(true){
            int choice = 0;
            std::cin >> choice;

            switch(choice){
                case 1:
                    std::cout << "Do you want to regenerate the book entirely (1) or add lines (2) ? : ";
                    std::cin >> choice;

                    switch(choice){
                        case 1:
                            //Choose a file and generate book from it
                            break;
                        case 2:
                        default:
                            //Choose a file and add lines to the current book
                            break;
                    }
                    break;

                case 2:
                    //Here we can delete or change lines (finds the move we want to change, and modifies it)
                    //Or add an entirely new line (input the move where it changes, the program finds it's index, and goes to it's last sibling
                    //then changes it's nextSibling variable, before adding the new line after the last index, then continues from here)
                    break;

                case 3:
                    return 0;

                default:
                    std::cout << "Please enter a valid choice" << std::endl;
            }
        }

        return 0;
    }

    //Reads the entry at the given index in the book
    bookEntry readEntry(int index, bool init = false){
        //First we need to open the file
        //It will open the file from the root index (index 0), which is e4 since it is the most common first move
        //A memory block size will be 256bits here (one memory block = 1 book entry)
        bookEntry bookMove{0};
        char* buffer = (char*) &bookMove;

        //Placing the head on the entry we want to read
        //bookFileIn.clear();
        bookFile.seekg(sizeof(bookEntry)*index);
        if(bookFile.eof()){
            return bookMove;
        }
        bookFile.read(buffer, sizeof(bookEntry));

        //delete[] buffer;
        bookFile.clear();
        return bookMove;
    }

    //Writes the given entry at the current index in the book
    void writeEntry(int index, bookEntry entry){
        //Same thing but here we need to write
        char* buffer = (char*) &entry;

        //Placing the head on the entry we want to read
        bookFile.seekp(sizeof(bookEntry)*index);
        bookFile.write(buffer, sizeof(bookEntry));

        //delete[] buffer;
        bookFile.clear();
    }

    void setNextMove(bookEntry move, int nextMoveIndex) {
        int index = 0;
        //We start by going to the right ply in order to search the move to set
        for(int i = 0; i < move.ply; i++){
            index = openingBook[index].nextMove;
        }

        //We now search for the move
        while(openingBook[index].nextSibling != -1 && openingBook[index].move != move.move){
            index = openingBook[index].nextSibling;
        }

        //if the move exists we can now set it's nextMove variable
        if(openingBook[index].move == move.move){
            openingBook[index].nextMove = nextMoveIndex;
        }
    }

    void setNextsibling(bookEntry move, int nextSiblingIndex) {
        int index = 0;
        //We start by going to the right ply in order to search the move to set
        for(int i = 0; i < move.ply; i++){
            index = openingBook[index].nextMove;
        }

        //We now search for the move
        while(openingBook[index].nextSibling != -1 && openingBook[index].move != move.move){
            index = openingBook[index].nextSibling;
        }

        //if the move exists we can now set it's nextMove variable
        if(openingBook[index].move == move.move){
            openingBook[index].nextMove = nextSiblingIndex;
        }
    }

    //Adds a new line to the book
    void addLine(const std::vector<bookEntry>& line) {
        //we repeat for each move present in the given line
        for(int entry = 0; entry < line.size(); entry++){
            int index = 0;
            //Start by going to the right ply
            for(int i = 0; i < line[entry].ply; i++){
                //There is always a case where we add a move deeper than the current max ply
                index = openingBook[index].nextMove;
            }

            //We check whether or not the current move exists
            while(openingBook[index].nextSibling != -1 && openingBook[index].move != line[entry].move){
                index = openingBook[index].nextSibling;
            }

            //if the move already exists, we just have to set the nextMove variable of the last move in the line
            if(openingBook[index].move == line[entry].move){
                if(entry > 0){
                    setNextMove(line[entry-1], index);
                }
            }

            //If the move doesn't exist, we need to add it at the end of the array, and then add it as the nextSibling of last entry
            else{
                maxEntry++;
                openingBook[maxEntry] = line[entry];
                setNextsibling(openingBook[index], maxEntry);
            }
        }
    }

    void saveBook(){
        for(int i = 0; i <= maxEntry; i++){
            writeEntry(i, openingBook[i]);
        }
        std::cout << "Succesfully saved the book !" << std::endl;
    }

    //Debugging function only
    void printBook(){
        for(int i = 0; i <= maxEntry; i++){
            std::cout << openingBook[i].move << std::endl;
        }
    }
};

#endif //BAUB_CHESS_OPENINGBOOKHANDLER_H
