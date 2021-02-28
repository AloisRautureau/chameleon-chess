//
// Created by bitterboyy on 2/20/21.
//

#ifndef BAUB_CHESS_BOARD_REPRESENTATION_H
#define BAUB_CHESS_BOARD_REPRESENTATION_H

#include <forward_list>
#include <iostream>
#include <stack>
class display;
class debug;

/*
 * This class is the representation of a board state.
 * It should be capable of :
 * - generating every legal move in a current position
 * - make and unmake moves
 * - tell if a position is checkmate or stalemate
 */

/* bitrange    info
    *  0 - 5      from
    *  6 -11       to
    *  12-15      flags
    */
typedef unsigned short movebits;

enum pieceType{
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING,
    EMPTY,
    INV
};

enum pieceColor{
    WHITE,
    BLACK,
};

enum sq{
    a1=0x00, b1=0x01, c1=0x02, d1=0x03, e1=0x04, f1=0x05, g1=0x06, h1=0x07,
    a2=0x10, b2=0x11, c2=0x12, d2=0x13, e2=0x14, f2=0x15, g2=0x16, h2=0x17,
    a3=0x20, b3=0x21, c3=0x22, d3=0x23, e3=0x24, f3=0x25, g3=0x26, h3=0x27,
    a4=0x30, b4=0x31, c4=0x32, d4=0x33, e4=0x34, f4=0x35, g4=0x36, h4=0x37,
    a5=0x40, b5=0x41, c5=0x42, d5=0x43, e5=0x44, f5=0x45, g5=0x46, h5=0x47,
    a6=0x50, b6=0x51, c6=0x52, d6=0x53, e6=0x54, f6=0x55, g6=0x56, h6=0x57,
    a7=0x60, b7=0x61, c7=0x62, d7=0x63, e7=0x64, f7=0x65, g7=0x66, h7=0x67,
    a8=0x70, b8=0x71, c8=0x72, d8=0x73, e8=0x74, f8=0x75, g8=0x76, h8=0x77,
    inv = 0x88
};

enum flag{
    QUIET = 0,
    DPAWNPUSH = 1,
    KCASTLE = 2,
    QCASTLE = 3,
    CAP = 4,
    EPCAP = 5,
    NPROM = 8,
    BPROM = 9,
    RPROM = 10,
    QPROM = 11,
    NPROMCAP=12,
    BPROMCAP=13,
    RPROMCAP=14,
    QPROMCAP=15
};

//Directionnal constants
enum directions{
    N = 0x10,
    S = -0x10,
    E = 0x01,
    W = -0x01,
    NE = 0x11,
    NW = 0x0F,
    SE = -0xF,
    SW = -0x11
};

enum castlingRights{
    WKCASTLE = 0b1000,
    WQCASTLE = 0b0100,
    BKCASTLE = 0b0010,
    BQCASTLE = 0b0001
};

struct takebackInfo{
    movebits move;
    pieceType pieceTaken = EMPTY;
    char castling = 0;
    int halfmove = 0;
    sq ep = inv;
};

static int file(int square){return square & 7;}
static int rank(int square){return square >> 4;}

class board_representation {
protected:
    /*
     * We use a comination of two 0x88 boards to keep track of colors, and piece type respectively
     * The boards use Little-Endian Rank-File mapping (LERF for short), a fancy way of saying a1 = 0, b1 = 1... and h8 = 63
     * The rose would look like this :
     *  noWe         nort         noEa
                +15    +16    +17
                    \  |  /
        west    -1 <-  0 -> +1    east
                    /  |  \
                -17    -16    -15
        soWe         sout         soEa
     */

    char m_piecesBoard[0x88] = {
            ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK, INV, INV, INV, INV, INV, INV, INV, INV,
            PAWN, PAWN, PAWN, PAWN, PAWN, PAWN, PAWN, PAWN,          INV, INV, INV, INV, INV, INV, INV, INV,
            EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,  INV, INV, INV, INV, INV, INV, INV, INV,
            EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,  INV, INV, INV, INV, INV, INV, INV, INV,
            EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,  INV, INV, INV, INV, INV, INV, INV, INV,
            EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,  INV, INV, INV, INV, INV, INV, INV, INV,
            PAWN, PAWN, PAWN, PAWN, PAWN, PAWN, PAWN, PAWN,          INV, INV, INV, INV, INV, INV, INV, INV,
            ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK, INV, INV, INV, INV, INV, INV, INV, INV,
    };
    char m_colorBoard[0x88] = {
            WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE,  INV, INV, INV, INV, INV, INV, INV, INV,
            WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE,  INV, INV, INV, INV, INV, INV, INV, INV,
            EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,  INV, INV, INV, INV, INV, INV, INV, INV,
            EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,  INV, INV, INV, INV, INV, INV, INV, INV,
            EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,  INV, INV, INV, INV, INV, INV, INV, INV,
            EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,  INV, INV, INV, INV, INV, INV, INV, INV,
            BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK,  INV, INV, INV, INV, INV, INV, INV, INV,
            BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK,  INV, INV, INV, INV, INV, INV, INV, INV,
    };

    /*
     * As we use table-driven move gen to speed up the process, we'll have to store said tables
     */

    /*
     * In order not to traverse the entirety of the board each time we scan it (16*8 times can get redundant quickly)
     * we keep piece-lists, so that we can just check where each piece is.
     * They are indexed by [color][pieceType]
     */
    std::forward_list<sq> m_pieces[2][6] = {
            //WHITE PIECES
            {
                //PAWNS
                {a2, b2, c2, d2, e2, f2, g2, h2,},
                //KNIGHTS
                {b1, g1},
                //BISHOPS
                {c1, f1},
                //ROOKS
                {a1, h1},
                //QUEEN
                {d1},
                //KING
                {e1},
            },
            //BLACK PIECES
            {
                //PAWNS
                {a7, b7, c7, d7, e7, f7, g7, h7,},
                //KNIGHTS
                {b8, g8},
                //BISHOPS
                {c8, f8},
                //ROOKS
                {a8, h8},
                //QUEEN
                {d8},
                //KING
                {e8},
            }
    };

    int m_pieceMoves[6][8] = {
            //Pawn moves aren't in here, so we use their space to signify if a piece can slide or not
            {false, false, true, true, true, false, false, false},
            //Knights
            {2*N + W, 2*N + E, 2*S + W, 2*S + E, 2*W + S, 2*W + N, 2*E + S, 2*E + N},
            //Bishops
            {NW, NE, SE, SW, 0, 0, 0, 0},
            //Rooks
            {E, W, S, N, 0, 0, 0, 0},
            //Queens
            {E, W, N, S, NW, SW, NE, SE},
            //King
            {E, W, N, S, NW, SW, NE, SE},
    };

    //Variables used to keep track of the game state
    bool m_sideToMove = WHITE;
    char m_castlingRights = 0b1111;
    int m_halfclock = 0;
    int m_ply = 0;
    sq m_ep = a1;
    std::stack<takebackInfo> m_takebackInfo;

    //Move list is a 256 entry array
    movebits m_moveStack[256] = {0};
    int m_moveStackIndex = 0;

    friend display;
    friend debug;

public:
    /*
     * Generates all possible moves for the current side to move
     * TODO : Use table-driven generation to speed things up
     */
    void gen();

    //Checks if the given square is under attack by the given side
    bool sqAttacked(int sq, bool side);
    //A small variant on the sqAttacked function
    bool inCheck(bool side);

    //Encodes a move on 16bits
    static movebits encodeMove(sq from, sq to, flag flag);

    //Decoding move functions
    static sq fromSq(movebits move);
    static sq toSq(movebits move);
    static flag getFlag(movebits move);

    /*
     * Makes a move, as simple as that!
     * Careful tho, it doesn't really check whether or not the move is legal. If you tell the make function to move a bishop
     * from a1 to b1, it will make the move.
     */
    bool make(movebits move);

    //Takes back the last move made
    void takeback();

};


#endif //BAUB_CHESS_BOARD_REPRESENTATION_H
