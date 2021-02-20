//
// Created by bitterboyy on 1/16/21.
//

#ifndef BAUB_CHESS_UTILITY_H
#define BAUB_CHESS_UTILITY_H

#include <stack>
#include <iostream>
#include <stack>
#include <random>
#include <sstream>

/*
 * Definition of useful types
 */

typedef uint8_t BYTE;
typedef uint32_t MOVEBITS;
typedef uint64_t POSITIONBITS;
typedef uint8_t FLAG;

/*
 * Pieces constants
 */

const std::string pieceNames[6] = {"PAWN", "KNIGHT", "BISHOP", "ROOK", "QUEEN", "KING"};

const BYTE PAWN = 0;
const BYTE KNIGHT = 1;
const BYTE BISHOP = 2;
const BYTE ROOK = 3;
const BYTE QUEEN = 4;
const BYTE KING = 5;

const BYTE WHITE = 0;
const BYTE BLACK = 1;

const BYTE EMPTY = 6;
const BYTE INV = 8;

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
BYTE invSquare8x8(BYTE square){
    return ((square8x8(square)%8) + (7 - (int)(square8x8(square)/8))*8);
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
    POSITIONBITS moveB = move, pieceB = pieceTaken, enPassantB = enPassant, castlingB = castling, fiftyB = fifty;
    return (moveB << 21) + (pieceB << 18) + (enPassantB << 11) + (castlingB << 7) + fiftyB;
}

MOVEBITS movePlayed(POSITIONBITS position){ return (position >> 21); }
BYTE pieceToWakeFromTheDead(POSITIONBITS position){ return (position & 0b111000000000000000000) >> 18; }
BYTE enPassantLast(POSITIONBITS position){ return (position & 0b111111100000000000) >> 11; }
BYTE castling(POSITIONBITS position){ return (position & 0b11110000000) >> 7; }
int fiftyClock(POSITIONBITS position){ return (position & 0b1111111); }


/*
 * Useful for evaluation purposes
 */

int pieceValue[6] = {100, 325, 350, 525, 1000};

//Piece square tables come from Tomasz Michniewski's Simplified Evaluation Function
int pieceSquareTables[6][64] = {
        //PAWNS
        {0,  0,  0,  0,  0,  0,  0,  0,
        50, 50, 50, 50, 50, 50, 50, 50,
        10, 10, 20, 30, 30, 20, 10, 10,
        5,  5, 10, 25, 25, 10,  5,  5,
        0,  0,  0, 20, 20,  0,  0,  0,
        5, -5,-10,  0,  0,-10, -5,  5,
        5, 10, 10,-20,-20, 10, 10,  5,
        0,  0,  0,  0,  0,  0,  0,  0},

        //KNIGHTS
        {-50,-40,-30,-30,-30,-30,-40,-50,
         -40,-20,  0,  0,  0,  0,-20,-40,
         -30,  0, 10, 15, 15, 10,  0,-30,
         -30,  5, 15, 20, 20, 15,  5,-30,
         -30,  0, 15, 20, 20, 15,  0,-30,
         -30,  5, 10, 15, 15, 10,  5,-30,
         -40,-20,  0,  5,  5,  0,-20,-40,
         -50,-40,-30,-30,-30,-30,-40,-50,},

         //BISHOPS
        {-20,-10,-10,-10,-10,-10,-10,-20,
         -10,  0,  0,  0,  0,  0,  0,-10,
         -10,  0,  5, 10, 10,  5,  0,-10,
         -10,  5,  5, 10, 10,  5,  5,-10,
         -10,  0, 10, 10, 10, 10,  0,-10,
         -10, 10, 10, 10, 10, 10, 10,-10,
         -10,  5,  0,  0,  0,  0,  5,-10,
         -20,-10,-10,-10,-10,-10,-10,-20,},

         //ROOKS
        {  0,  0,  0,  0,  0,  0,  0,  0,
           5, 10, 10, 10, 10, 10, 10,  5,
           -5,  0,  0,  0,  0,  0,  0, -5,
           -5,  0,  0,  0,  0,  0,  0, -5,
           -5,  0,  0,  0,  0,  0,  0, -5,
           -5,  0,  0,  0,  0,  0,  0, -5,
           -5,  0,  0,  0,  0,  0,  0, -5,
           0,  0,  0,  5,  5,  0,  0,  0},

           //QUEENS
        {-20,-10,-10, -5, -5,-10,-10,-20,
         -10,  0,  0,  0,  0,  0,  0,-10,
         -10,  0,  5,  5,  5,  5,  0,-10,
         -5,  0,  5,  5,  5,  5,  0, -5,
         0,  0,  5,  5,  5,  5,  0, -5,
         -10,  5,  5,  5,  5,  5,  0,-10,
         -10,  0,  5,  0,  0,  0,  0,-10,
         -20,-10,-10, -5, -5,-10,-10,-20},

         //KINGS
        {-30,-40,-40,-50,-50,-40,-40,-30,
         -30,-40,-40,-50,-50,-40,-40,-30,
         -30,-40,-40,-50,-50,-40,-40,-30,
         -30,-40,-40,-50,-50,-40,-40,-30,
         -20,-30,-30,-40,-40,-30,-30,-20,
         -10,-20,-20,-20,-20,-20,-20,-10,
         20, 20,  0,  0,  0,  0, 20, 20,
         20, 30, 10,  0,  0, 10, 30, 20}
};

/*
 * Zobrist hashing is used for a wide range of purposes : storing positions, evaluating pawn structure, etc
 */
typedef unsigned long long ZOBKEY;
typedef unsigned long long ZOBHASH;
ZOBKEY zobristKeys[781];

const int WPAWNZOB = 0;
const int WKNIGHTZOB = 64;
const int WBISHOPZOB = 128;
const int WROOKZOB = 192;
const int WQUEENZOB = 256;
const int WKINGZOB = 320;
const int BPAWNZOB = 384;
const int BKNIGHTZOB = 448;
const int BBISHOPZOB = 512;
const int BROOKZOB = 576;
const int BQUEENZOB = 640;
const int BKINGZOB = 704;
const int SIDEZOB = 768;
const int WKCASTLEZOB = 769;
const int WQCASTLEZOB = 770;
const int BKCASTLEZOB = 771;
const int BQCASTLEZOB = 772;
const int AEPZOB = 773;

//We use a Mersenne Twister to generate the keys
void initHash(){
    std::mt19937_64 twister(time(nullptr));
    for(int i = 0; i < 781; i++){
        zobristKeys[i] = twister.operator()();
    }
}

class transpositionTable{
public:
    ZOBHASH hashTable[1024] = {0};
    MOVEBITS bestMoveTable[1024] = {0};
    int depthTable[1024] = {0};
    int scoreTable[1024] = {0};
    char nodeTypeTable[1024] = {0};
    const char EXACT = 0;
    const char UPPER = 1;
    const char LOWER = 2;

    int getTableIndex(ZOBHASH hash){
        return hash%1024;
    }

    int storePosition(ZOBHASH hash, MOVEBITS bestMove, int depth, int score, char nodeType){
        int index = getTableIndex(hash);

        if(depthTable[index] <= depth || nodeTypeTable[index] >= nodeType){
            hashTable[index] = hash;
            bestMoveTable[index] = bestMove;
            depthTable[index] = depth;
            scoreTable[index] = score;
            nodeTypeTable[index] = nodeType;
            return index;
        }

        else return -1;
    }

    MOVEBITS getBestMove(ZOBHASH hash){
        int index = getTableIndex(hash);
        if(hashTable[index] == hash){
            return bestMoveTable[index];
        }
        else return 0;
    }

    void printTable(int index){
        std::cout << std::endl << "index  hash      nodeType     bestMove" << std::endl;
        if(hashTable[index] == 0) std::cout << "  " << index << "  empty" << std::endl;
        else{
            std::cout << "  " << index << "  " << hashTable[index] << " " << (int)nodeTypeTable[index] << " " << (int)bestMoveTable[index] << std::endl;
        }
    }
};

/*
 * Utility functions
 */
std::vector<std::string> split(const std::string& s, char delimiter)
{
    std::vector<std::string> substrings;
    std::string substring;
    std::istringstream ss(s);
    while (std::getline(ss, substring, delimiter))
    {
        substrings.push_back(substring);
    }
    return substrings;
}

#endif //BAUB_CHESS_UTILITY_H
