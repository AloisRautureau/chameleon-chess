#ifndef CHAMELEON_DEFS_H
#define CHAMELEON_DEFS_H

/*
 * This contains every variable used globally by Chameleon.
 * Think things like piece code, type definitions, etc
 */

#include <cstdint>

namespace Chameleon{

    /*
     * POSITIONAL INFORMATION
     */

    //Piece encoding (we or color value and piece type to get the full piece encoding)
    typedef uint8_t piece;
    static const piece EMPTY = 0;
    static const piece WHITE = 0b00000001;
    static const piece BLACK = 0b00000010;

    static const piece PAWN = 0b00000100;
    static const piece KNIGHT = 0b00001000;
    static const piece BISHOP = 0b00010000;
    static const piece ROOK = 0b00100000;
    static const piece QUEEN = 0b01000000;
    static const piece KING = 0b10000000;
    
    //Definition of initial chessboard position
    piece initialPosition[0x88] = {
            ROOK|WHITE, KNIGHT|WHITE, BISHOP|WHITE, QUEEN|WHITE, KING|WHITE, ROOK|WHITE, KNIGHT|WHITE, BISHOP|WHITE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
            PAWN|WHITE, PAWN|WHITE, PAWN|WHITE, PAWN|WHITE, PAWN|WHITE, PAWN|WHITE, PAWN|WHITE, PAWN|WHITE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
            EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
            EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
            EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
            EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
            PAWN|BLACK, PAWN|BLACK, PAWN|BLACK, PAWN|BLACK, PAWN|BLACK, PAWN|BLACK, PAWN|BLACK, PAWN|BLACK, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
            ROOK|BLACK, KNIGHT|BLACK, BISHOP|BLACK, QUEEN|BLACK, KING|BLACK, ROOK|BLACK, KNIGHT|BLACK, BISHOP|BLACK, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
    };

    //Coordinate transformation in 0x88 little-endian setup
    int getFile(int square) {
        return square & 7;
    }

    int getRank(int square) {
        return square >> 4;
    }

    int getSquare(int rank, int file) {
        return 16 * rank + file;
    }

    //Know if a square is invalid
    bool isInvalid(int square) {
        return square & 0x88;
    }

    struct plist {
        int indexes[10]{0x88};
        int size{0};
        int indexesBoard[0x88]{0x88};
    };


    /*
     * MOVE ENCODING/STORING
     */
    //Definition of movebits type, which will encode moves in 16 bits
    typedef uint16_t movebits;

    movebits encode(int from, int to, int flag){
        return (((from + (from & 7)) >> 1) << 10) + (((to + (to & 7)) >> 1) << 4) + flag;
    }

    int fromSq(movebits move) {
        return (move & 0b1111110000000000) >> 10;
    }

    int toSq(movebits move) {
        return (move & 0b1111110000) >> 4;
    }

    int flag(movebits move) {
        return (move & 0b1111);
    }

    //Definition of the movestack structure, which will hold generated moves
    struct movestack {
        movebits moves[256]{0};
        int size{0};
    };

    void storeMove(movebits move, movestack &stack){
        stack.moves[stack.size] = move;
        stack.size++;
    }
}


#endif //CHAMELEON_DEFS_H
