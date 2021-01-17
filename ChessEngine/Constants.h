//
// Created by bitterboyy on 1/16/21.
//

#ifndef BAUB_CHESS_CONSTANTS_H
#define BAUB_CHESS_CONSTANTS_H

#include <stack>
#include <iostream>

//Move type
//It keeps 12bits for from-to squares (6bits each), and 4 bits for special stuff
typedef unsigned short movebits;

//Converts from, to, spe to movebits
movebits encodeMove(int from, int to, int spe){
    movebits move = 0;
    move ^= from << 10;
    move ^= to << 4;
    move ^= spe;

    return move;
}

//Getters for movebits
int from(movebits move){
    return move >> 10;
}

int to(movebits move){
    return (move & 0b0000001111110000) >> 4;
}

int spe(movebits move){
    return (move & 0b0000000000001111);
}

//Move constants for spe
const int QUIET = 0;
const int DPAWNPUSH = 1;
const int KINGC = 2;
const int QUEENC = 3;
const int CAP = 4;
const int EPCAP = 5;
const int KNIGHTPROM = 8;
const int BISHOPPROM = 9;
const int ROOKPROM = 10;
const int QUEENPROM = 11;
const int KNIGHTPROMCAP = 12;
const int BISHOPPROMCAP = 13;
const int ROOKPROMCAP = 14;
const int QUEENPROMCAP = 15;


//Positional information
//Position info stores ep square on 6bits, castling on 4bits, fiftymoves rule on 7bits and captured piece on 3bits (20bits total, rounded to 32bits)
typedef unsigned int position;

//Encodes a position
position encodePosition(int ep, int castling, int fiftymoves, int cap){
    position pos = 0;
    pos ^= ep << 14;
    pos ^= castling << 10;
    pos ^= fiftymoves << 3;
    pos ^= cap;

    return pos;
}

int ep(position pos){
    return (pos >> 14);
}

int castle(position pos){
    return (pos & 0b00000011110000000000) >> 10;
}

int fiftymoves(position pos){
    return (pos & 0b00000000001111111000) >> 3;
}

int cap(position pos){
    return (pos & 0b00000000000000000111);
}



const int EMPTY = 7;

//Pieces
const int PAWN = 0;
const int KNIGHT = 1;
const int BISHOP = 2;
const int ROOK = 3;
const int QUEEN = 4;
const int KING = 5;

//Movements/Specifics

/*
 * -17 -16 -15
 * -1   .   +1
 * +15 +16 +17
 */
const int PIECEMV[6][8] = {
        //Sliding or not
        {0, 0, 1, 1, 1, 0, 0, 0},

        //Knights
        {17, -17, 15, -15, 10, -10, 6, -6},

        //Bishops
        {7, -7, 9, -9, 0, 0, 0, 0},

        //Rooks
        {1, -1, 8, -8, 0, 0, 0, 0},

        //Queen
        {7, -7, 9, -9, 1, -1, 8, -8},

        //King
        {7, -7, 9, -9, 1, -1, 8, -8},
};

const int PIECEMV88[6][8] = {
        //Sliding or not
        {0, 0, 1, 1, 1, 0, 0, 0},

        //Knights
        {33, -33, 31, -31, 18, -18, 14, -14},

        //Bishops
        {15, -15, 17, -17, 0, 0, 0, 0},

        //Rooks
        {1, -1, 16, -16, 0, 0, 0, 0},

        //Queen
        {15, -15, 17, -17, 1, -1, 16, -16},

        //King
        {15, -15, 17, -17, 1, -1, 16, -16}
};

const bool isValid(int square){
    return square < 64 && square >= 0;
}

//Couleurs
const int WHITE = 0;
const int BLACK = 1;

//Parsing human moves
movebits humanMove(std::string writtenMove){
    if(writtenMove.length() < 4 || writtenMove.length() > 5){
        //Move is invalid
        std::cout << "Please enter a move between 4 and 5 characters" << std::endl;
        return 0;
    }
    int promote = QUIET;
    std::string start = writtenMove.substr(0, 2);
    std::string end = writtenMove.substr(2, 2);
    if(writtenMove.length() == 5) promote = writtenMove[5] - '0';

    int startSquare = (start[0] - 'a')%8;
    int endSquare = (end[0] - 'a')%8;

    startSquare += ((start[1] - '0') - 1)*8;
    endSquare += ((end[1] - '0') - 1)*8;

    std::cout << startSquare << std::endl;
    std::cout << endSquare << std::endl;
    return encodeMove(startSquare, endSquare, promote);
}



#endif //BAUB_CHESS_CONSTANTS_H
