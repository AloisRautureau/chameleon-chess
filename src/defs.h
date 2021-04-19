#ifndef CHAMELEON_DEFS_H
#define CHAMELEON_DEFS_H

/*
 * This contains every variable used globally by Chameleon.
 * Think things like piece code, type definitions, etc
 */

#include <cstdint>
#include <vector>
#include <stack>

namespace Chameleon{
    /*
        * POSITIONAL INFORMATION
        */

    //Let's us access arrays indexed by piece type/color easily
    static const int W = 0;
    static const int B = 1;

    //Piece encoding (we or color value and piece type to get the full piece encoding)
    typedef uint8_t piece;
    static const piece EMPTY = 7;
    static const piece WHITE = 0b01000;
    static const piece BLACK = 0b10000;

    static const piece PAWN = 0b0000;
    static const piece KNIGHT = 0b0001;
    static const piece BISHOP = 0b0010;
    static const piece ROOK = 0b0011;
    static const piece QUEEN = 0b0100;
    static const piece KING = 0b0101;
    static const piece PTMASK = 0b0111;

    //Definition of initial chessboard position
    static piece initialPosition[0x88] = {
            ROOK|WHITE, KNIGHT|WHITE, BISHOP|WHITE, QUEEN|WHITE, KING|WHITE, BISHOP|WHITE, KNIGHT|WHITE, ROOK|WHITE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
            PAWN|WHITE, PAWN|WHITE, PAWN|WHITE, PAWN|WHITE, PAWN|WHITE, PAWN|WHITE, PAWN|WHITE, PAWN|WHITE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
            EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
            EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
            EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
            EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
            PAWN|BLACK, PAWN|BLACK, PAWN|BLACK, PAWN|BLACK, PAWN|BLACK, PAWN|BLACK, PAWN|BLACK, PAWN|BLACK, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
            ROOK|BLACK, KNIGHT|BLACK, BISHOP|BLACK, QUEEN|BLACK, KING|BLACK, BISHOP|BLACK, KNIGHT|BLACK, ROOK|BLACK, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
    };

    //Coordinate transformation in 0x88 little-endian setup
    static int getFile(int square) {
        return square & 7;
    }

    static int getRank(int square) {
        return square >> 4;
    }

    static int getSquare(int rank, int file) {
        return 16 * rank + file;
    }

    //Know if a square is invalid
    static bool isInvalid(int square) {
        return square & 0x88;
    }

    struct plist {
        int indexes[32]{0};
        int size{0};
        int indexesBoard[0x88]{0};
    };

    static void init_plist(plist &list, const std::vector<int>& squares = {}) {
        for(int & i : list.indexesBoard){
            i = -1;
        }
        for(auto square : squares) {
            list.indexesBoard[square] = list.size;
            list.indexes[list.size] = square;
            list.size++;
        }
    }

    static void plist_update(plist &list, int from, int to) {
        int index = list.indexesBoard[from];
        list.indexesBoard[from] = -1;
        list.indexesBoard[to] = index;
        list.indexes[index] = to;
    }

    static void plist_remove(plist &list, int square) {
        list.size--;
        int index = list.indexesBoard[square];
        int sq = list.indexes[list.size];
        list.indexes[index] = sq;
        list.indexesBoard[sq] = index;
    }

    static void plist_add(plist &list, int square) {
        list.indexes[list.size] = square;
        list.indexesBoard[square] = list.size;
        list.size++;
    }


    /*
     * MOVE ENCODING/STORING
     */
    //Definition of movebyte type, which will encode moves in 16 bits
    typedef uint16_t movebyte;

    enum flags {
        QUIET,
        DPAWNPUSH,
        KCASTLE,
        QCASTLE,
        CAP,
        EPCAP,
        NPROM = 8,
        BPROM,
        RPROM,
        QPROM,
        NPROMCAP,
        BPROMCAP,
        RPROMCAP,
        QPROMCAP,
    };

    //Lookup tables for 0x88 to 8x8 conversion
    static const int sq8x8[0x88] = {
            0, 1, 2, 3, 4, 5, 6, 7, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
            8, 9, 10, 11, 12, 13, 14, 15, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
            16, 17, 18, 19, 20, 21, 22, 23, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
            24, 25, 26, 27, 28, 29, 30, 31, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
            32, 33, 34, 35, 36, 37, 38, 39, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
            40, 41, 42, 43, 44, 45, 46, 47, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
            48, 49, 50, 51, 52, 53, 54, 55, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
            56, 57, 58, 59, 60, 61, 62, 63, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
    };

    static const int sq0x88[64] = {
            0, 1, 2, 3, 4, 5, 6, 7,
            16, 17, 18, 19, 20, 21, 22, 23,
            32, 33, 34, 35, 36, 37, 38, 39,
            48, 49, 50, 51, 52, 53, 54, 55,
            64, 65, 66, 67, 68, 69, 70, 71,
            80, 81, 82, 83, 84, 85, 86, 87,
            96, 97, 98, 99, 100, 101, 102, 103,
            112, 113, 114, 115, 116, 117, 118, 119,
    };

    static movebyte encode(int from, int to, int flag){
        return (sq8x8[from] << 10) + (sq8x8[to] << 4) + flag;
    }

    static int fromSq(movebyte move) {
        return sq0x88[((move & 0b1111110000000000) >> 10)];
    }

    static int toSq(movebyte move) {
        return sq0x88[((move & 0b1111110000) >> 4)];
    }

    static int flag(movebyte move) {
        return (move & 0b1111);
    }

    //Definition of the movestack structure, which will hold generated moves
    struct movestack {
        movebyte moves[256]{0};
        int size{0};
    };

    static void storeMove(int from, int to, int flag, movestack &stack){
        stack.moves[stack.size] = encode(from, to, flag);
        stack.size++;
        if(flag & NPROM) { //If we're promoting, add all types of promotions as well
            for(int i = flag + 1; i < flag + 4; i++){
                stack.moves[stack.size] = encode(from, to, i);
                stack.size++;
            }
        }
    }

    /*
     * STORING POSSIBLE MOVEMENTS FOR PIECES
     */
    static const int piece_delta[6][8] = {
            { //Pawns don't actually use deltas, we instead use their array to tell if a piece can slide or not
                0, 0, 1, 1, 1, 0, 0, 0
            },
            { //Knights
                0x21, 0x12, -0x21, -0x12, 0x1F, 0x0E, -0x1F, -0x0E
            },
            { //Bishops
                0x0F, 0x11, -0x0F, -0x11, 0, 0, 0, 0
            },
            { //Rooks
                0x10, -0x10, 1, -1, 0, 0, 0, 0
            },
            { //Queen
                0x10, -0x10, 1, -1, 0x0F, 0x11, -0x0F, -0x11,
            },
            { //King
                0x10, -0x10, 1, -1, 0x0F, 0x11, -0x0F, -0x11,
            },
    };

    /*
     * ATTACK ARRAY
     * This part was taken from Mediocre Chess by Jonatan Pettersson
     * He has a really cool blog explaining the process of making a chess engine in Java
     * That's the particular article that I'm referring to : http://mediocrechess.blogspot.com/2006/12/guide-attacked-squares.html
     */
    static const int ATTACK_NONE = 0;
    static const int ATTACK_KQR = 1;
    static const int ATTACK_QR = 2;
    static const int ATTACK_KQBwP = 3;
    static const int ATTACK_KQBbP = 4;
    static const int ATTACK_QB = 5;
    static const int ATTACK_N = 6;

    static const int ATTACK_ARRAY[257] =
            {0,0,0,0,0,0,0,0,0,5,0,0,0,0,0,0,2,0,0,0,     //0-19
             0,0,0,5,0,0,5,0,0,0,0,0,2,0,0,0,0,0,5,0,     //20-39
             0,0,0,5,0,0,0,0,2,0,0,0,0,5,0,0,0,0,0,0,     //40-59
             5,0,0,0,2,0,0,0,5,0,0,0,0,0,0,0,0,5,0,0,     //60-79
             2,0,0,5,0,0,0,0,0,0,0,0,0,0,5,6,2,6,5,0,     //80-99
             0,0,0,0,0,0,0,0,0,0,6,4,1,4,6,0,0,0,0,0,     //100-119
             0,2,2,2,2,2,2,1,0,1,2,2,2,2,2,2,0,0,0,0,     //120-139
             0,0,6,3,1,3,6,0,0,0,0,0,0,0,0,0,0,0,5,6,     //140-159
             2,6,5,0,0,0,0,0,0,0,0,0,0,5,0,0,2,0,0,5,     //160-179
             0,0,0,0,0,0,0,0,5,0,0,0,2,0,0,0,5,0,0,0,     //180-199
             0,0,0,5,0,0,0,0,2,0,0,0,0,5,0,0,0,0,5,0,     //200-219
             0,0,0,0,2,0,0,0,0,0,5,0,0,5,0,0,0,0,0,0,     //220-239
             2,0,0,0,0,0,0,5,0,0,0,0,0,0,0,0,0         }; //240-256

    static const int DELTA_ARRAY[257] =
            {0,0,0,0,0,0,0,0,0,-17,0,0,0,0,0,0,-16,0,0,0,     //0-19
             0,0,0,-15,0,0,-17,0,0,0,0,0,-16,0,0,0,0,0,-15,0,     //20-39
             0,0,0,-17,0,0,0,0,-16,0,0,0,0,-15,0,0,0,0,0,0,     //40-59
             -17,0,0,0,-16,0,0,0,-15,0,0,0,0,0,0,0,0,-17,0,0,     //60-79
             -16,0,0,-15,0,0,0,0,0,0,0,0,0,0,-17,-33,-16,-31,-15,0,     //80-99
             0,0,0,0,0,0,0,0,0,0,-18,-17,-16,-15,-14,0,0,0,0,0,     //100-119
             0,-1,-1,-1,-1,-1,-1,-1,0,1,1,1,1,1,1,1,0,0,0,0,     //120-139
             0,0,14,15,16,17,18,0,0,0,0,0,0,0,0,0,0,0,15,31,     //140-159
             16,33,17,0,0,0,0,0,0,0,0,0,0,15,0,0,16,0,0,17,     //160-179
             0,0,0,0,0,0,0,0,15,0,0,0,16,0,0,0,17,0,0,0,     //180-199
             0,0,0,15,0,0,0,0,16,0,0,0,0,17,0,0,0,0,15,0,     //200-219
             0,0,0,0,16,0,0,0,0,0,17,0,0,15,0,0,0,0,0,0,     //220-239
             16,0,0,0,0,0,0,17,0,0,0,0,0,0,0,0,0         }; //240-256

    static int get_attackingdelta(int attacked, int attacking, piece pieceType) {
        int attack = ATTACK_ARRAY[attacked - attacking + 128];
        int side = pieceType&0b10000;
        //We first check if the attack is even possible
        if(attack == ATTACK_NONE) return 0;
        switch(pieceType&PTMASK){
            case PAWN:
                if((!side && attack != ATTACK_KQBwP) || (side && attack != ATTACK_KQBbP)) return 0;
                break;
            case BISHOP:
                if(attack != ATTACK_QB && attack != ATTACK_KQBwP && attack != ATTACK_KQBbP) return 0;
                break;
            case KNIGHT:
                if(attack != ATTACK_N) return 0;
                break;
            case ROOK:
                if(attack != ATTACK_KQR && attack != ATTACK_QR) return 0;
                break;
            case QUEEN:
                if(attack == ATTACK_N) return 0;
                break;
            case KING:
                if(attack != ATTACK_KQR && attack != ATTACK_KQBbP && attack != ATTACK_KQBwP) return 0;
                break;
            default: break;
        }

        //If we haven't returned anything till now, the attack is possible and we'll return the corresponding delta
        return DELTA_ARRAY[attacked - attacking + 128];
    }

    /*
     * PINS CONTAINERS
     * This is a particular piece container that keeps in memory pinning information, that is the square that is pinning/pinned,
     * as well as the pinning delta
     */
    struct pins {
        int deltas[32]{0};
        int index_board[0x88]{0};
        int size{0};
    };

    static void initPin(pins &pin){
        for(int & i : pin.index_board){
            i = -1;
        }
    }

    static void addPin(pins &pin, int square, int delta) {
        pin.deltas[pin.size] = delta;
        pin.index_board[square] = pin.size;
        pin.size++;
    }

    static int getPinDelta(pins &pin, int square) {
        return pin.index_board[square] == -1 ? 0 : pin.deltas[pin.index_board[square]];
    }

    /*
     * GAME HISTORY CONTAINER
     */
    struct history_entry {
        movebyte move{0};
        int ep{0};
        int castling{0};
        int fifty{0};
        int captured{0};
        pins pinned{0};
        bool checked{false};
    };

    /*
     * DISPLAY COMMON INFO
     */
    static std::string intToSq(int square){
        return std::string({(char)(getFile(square) + 'a'), (char)(getRank(square) + '1')});
    }
}


#endif //CHAMELEON_DEFS_H
