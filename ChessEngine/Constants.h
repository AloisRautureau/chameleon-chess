//
// Created by bitterboyy on 1/16/21.
//

#ifndef BAUB_CHESS_CONSTANTS_H
#define BAUB_CHESS_CONSTANTS_H

#include <stack>
#include <iostream>
#include <stack>

/*
 * Definition of useful types
 */

typedef unsigned char BYTE;
typedef unsigned int MOVEBITS;
typedef unsigned long long POSITIONBITS;
typedef unsigned short FLAG;

/*
 * Pieces constants
 */

const BYTE PAWN = 0;
const BYTE KNIGHT = 1;
const BYTE BISHOP = 2;
const BYTE ROOK = 3;
const BYTE QUEEN = 4;
const BYTE KING = 5;

const BYTE WHITE = 0;
const BYTE BLACK = 1;

const BYTE EMPTY = 6;
const BYTE INV = 7;

const FLAG QUIET = 0b0000;
const FLAG DPAWNPUSH = 0b0001;
const FLAG KCASTLE = 0b0010;
const FLAG QCASTLE = 0b0011;
const FLAG CAP = 0b0100;
const FLAG EPCAP = 0b0101;
const FLAG KNIGHTPROM = 0b1000;
const FLAG BISHOPPROM = 0b1001;
const FLAG ROOKPROM = 0b1010;
const FLAG QUEENPROM = 0b1011;

/*
 * Piece move rules
 */

int pieceMv[6][8] = {
        //Since we don't use the section that the paws would use, we can use it to indicate if the given pieces can slide or not
        {false, false, true, true, true, false},
        //Knights
        {31, -31, 33, -33, 18, -18, 14, -14},
        //Bishops
        {15, 17, -15, -17, 0, 0, 0, 0},
        //Rooks
        {1, -1, 16, -16, 0, 0, 0, 0},
        //Queen
        {1, -1, 16, -16, 15, 17, -15, -17},
        //King
        {1, -1, 16, -16, 15, 17, -15, -17},
};

/*
 * Useful conversion functions
 */

BYTE squareIndex(BYTE file, BYTE rank) {
    return 16*rank + file;
}
BYTE fileIndex(BYTE square){
    return square & 7;
}
BYTE rankIndex(BYTE square){
    return square >> 4;
}
BYTE square8x8(BYTE square){
    return (square + (square & 7)) >> 1;
}
BYTE square0x88(BYTE square){
    return square + (square & ~7);
}
bool isInvalid(BYTE square){
    return (square & 0x88) != 0;
}

/*
 * Encoding/Decoding functions
 */

//Encodes a move on 18bits (7bits * 2 for from-to info, and 4 bits for flags
MOVEBITS encodeMove(BYTE from, BYTE to, BYTE flags){
    return (from << 11) + (to << 4) + (flags);
}

BYTE fromSq(MOVEBITS move){ return (move & 0b111111100000000000) >> 11; }
BYTE toSq(MOVEBITS move){ return (move & 0b000000011111110000) >> 4; }
BYTE flag(MOVEBITS move){ return (move & 0b0000000000001111); }

//Encodes a position on 64bits (18 bits for move made, 3bits for piece taken, 8bits for enPassant square, 4bits for castling and 7bits fifty move rule)
POSITIONBITS encodePosition(MOVEBITS move, BYTE pieceTaken, BYTE enPassant, BYTE castling, int fifty){
    return (move << 22) + (pieceTaken << 19) + (enPassant << 11) + (castling << 7) + fifty;
}

MOVEBITS movePlayed(POSITIONBITS position){ return (position & 0b1111111111111111110000000000000000000000) >> 22; }
BYTE pieceToWakeFromTheDead(POSITIONBITS position){ return (position & 0b1110000000000000000000) >> 19; }
BYTE enPassantLast(POSITIONBITS position){ return (position & 0b1111111100000000000) >> 11; }
BYTE castling(POSITIONBITS position){ return (position & 0b11110000000) >> 7; }
int fiftyClock(POSITIONBITS position){ return (position & 0b1111111); }

#endif //BAUB_CHESS_CONSTANTS_H
