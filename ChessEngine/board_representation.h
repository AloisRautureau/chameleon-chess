//
// Created by bitterboyy on 2/20/21.
//

#ifndef BAUB_CHESS_BOARD_REPRESENTATION_H
#define BAUB_CHESS_BOARD_REPRESENTATION_H

#include <iostream>
#include <stack>
#include <vector>
class display;
class debug;
class evaluation;
class search;

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

static const char PAWN = 0;
static const char KNIGHT = 1;
static const char BISHOP = 2;
static const char ROOK = 3;
static const char QUEEN = 4;
static const char KING = 5;
static const char EMPTY = 6;
static const char INV = 7;

static const bool WHITE = 0;
static const bool BLACK = 1;

static const int inv = 0x88;

static const char QUIET = 0b0000;
static const char DPAWNPUSH = 0b0001;
static const char KCASTLE = 0b0010;
static const char QCASTLE = 0b0011;
static const char CAP = 0b0100;
static const char EPCAP = 0b0101;
static const char NPROM = 0b1000;
static const char BPROM = 0b1001;
static const char RPROM = 0b1010;
static const char QPROM = 0b1011;
static const char NPROMCAP=0b1100;
static const char BPROMCAP=0b1101;
static const char RPROMCAP=0b1110;
static const char QPROMCAP=0b1111;

//Directionnal constants
enum directions{
    N = 0x10,
    S = -0x10,
    E = 0x01,
    W = -0x01,
    NE = 0x11,
    NW = 0x0F,
    SE = -0x0F,
    SW = -0x11
};

static const char WKCASTLE = 0b1000;
static const char WQCASTLE = 0b0100;
static const char BKCASTLE = 0b0010;
static const char BQCASTLE = 0b0001;

struct takebackInfo{
    movebits move{};
    char pieceTaken = EMPTY;
    char castling = 0;
    int halfmove = 0;
    int ep = inv;
};

static int file(int square){return square & 7;}
static int rank(int square){return square >> 4;}

class pieceList {
private:
    int indices[10]{inv}; //There can be a maximum of 10 of the same pieces (if all pawns promote to said pieceType
    int board[0x88]{EMPTY}; //This let's us search a piece with O(1) efficiency
    int m_size{0};

public:
    pieceList(std::vector<int> basePieces){
        for(int index : basePieces){
            board[index] = m_size;
            indices[m_size++] = index;
        }
    }

    void add(int adress){
        if(m_size < 10){
            board[adress] = m_size;
            indices[m_size++] = adress;
        }
    }

    void remove(int adress){
        m_size--;
        int index = board[adress];
        indices[index] = indices[m_size];
        board[indices[index]] = index;
        indices[m_size] = inv;
        board[adress] = EMPTY;
    }

    int get(int index){
        if(index < m_size) return indices[index];
        return inv;
    }

    int size(){
        return m_size;
    }
};

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

    char m_pieces[0x88] = {
            ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK, INV, INV, INV, INV, INV, INV, INV, INV,
            PAWN, PAWN, PAWN, PAWN, PAWN, PAWN, PAWN, PAWN,          INV, INV, INV, INV, INV, INV, INV, INV,
            EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,  INV, INV, INV, INV, INV, INV, INV, INV,
            EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,  INV, INV, INV, INV, INV, INV, INV, INV,
            EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,  INV, INV, INV, INV, INV, INV, INV, INV,
            EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,  INV, INV, INV, INV, INV, INV, INV, INV,
            PAWN, PAWN, PAWN, PAWN, PAWN, PAWN, PAWN, PAWN,          INV, INV, INV, INV, INV, INV, INV, INV,
            ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK, INV, INV, INV, INV, INV, INV, INV, INV,
    };
    char m_color[0x88] = {
            WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE,  INV, INV, INV, INV, INV, INV, INV, INV,
            WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE,  INV, INV, INV, INV, INV, INV, INV, INV,
            EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,  INV, INV, INV, INV, INV, INV, INV, INV,
            EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,  INV, INV, INV, INV, INV, INV, INV, INV,
            EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,  INV, INV, INV, INV, INV, INV, INV, INV,
            EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,  INV, INV, INV, INV, INV, INV, INV, INV,
            BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK,  INV, INV, INV, INV, INV, INV, INV, INV,
            BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK,  INV, INV, INV, INV, INV, INV, INV, INV,
    };

    int m_directions[6][8] = {
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

    pieceList m_plist[2][6] = {
            { //WHITE PIECELISTS
                pieceList({0x10, 0X11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17}),
                pieceList({0x01, 0x06}),
                pieceList({0x02, 0x05}),
                pieceList({0x00, 0x07}),
                pieceList({0x03}),
                pieceList({0x04}),
            },
            { //BLACK PIECELISTS
                    pieceList({0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67}),
                    pieceList({0x71, 0x76}),
                    pieceList({0x72, 0x75}),
                    pieceList({0x70, 0x77}),
                    pieceList({0x73}),
                    pieceList({0x74}),
            }
    };

    //This is the attack array, taken from Mediocre Chess's blogpost about attacked squares
    //Thanks btw, really helped me understand the concept
    enum attacks{
        attNONE = 0,
        attKQR = 0b111000,
        attQR = 0b011000,
        attKQBwP = 0b110101,
        attKQBbP = 0b110100,
        attQB = 0b010100,
        attN = 0b000010,
    };

    int m_attackArray[257] =
            {0,0,0,0,0,0,0,0,0,0b010100,0,0,0,0,0,0,0b011000,0,0,0,     //0-19
             0,0,0,0b010100,0,0,0b010100,0,0,0,0,0,0b011000,0,0,0,0,0,0b010100,0,     //20-39
             0,0,0,0b010100,0,0,0,0,0b011000,0,0,0,0,0b010100,0,0,0,0,0,0,     //40-59
             0b010100,0,0,0,0b011000,0,0,0,0b010100,0,0,0,0,0,0,0,0,0b010100,0,0,     //60-79
             0b011000,0,0,0b010100,0,0,0,0,0,0,0,0,0,0,0b010100,0b000010,0b011000,0b000010,0b010100,0,     //80-99
             0,0,0,0,0,0,0,0,0,0,0b000010,0b110100,0b111000,0b110100,0b000010,0,0,0,0,0,     //100-119
             0,0b011000,0b011000,0b011000,0b011000,0b011000,0b011000,0b111000,0,0b111000,0b011000,0b011000,0b011000,0b011000,0b011000,0b011000,0,0,0,0,     //120-139
             0,0,0b000010,0b110101,0b111000,0b110101,0b000010,0,0,0,0,0,0,0,0,0,0,0,0b010100,0b000010,     //140-159
             0b011000,0b000010,0b010100,0,0,0,0,0,0,0,0,0,0,0b010100,0,0,0b011000,0,0,0b010100,     //160-179
             0,0,0,0,0,0,0,0,0b010100,0,0,0,0b011000,0,0,0,0b010100,0,0,0,     //180-199
             0,0,0,0b010100,0,0,0,0,0b011000,0,0,0,0,0b010100,0,0,0,0,0b010100,0,     //200-219
             0,0,0,0,0b011000,0,0,0,0,0,0b010100,0,0,0b010100,0,0,0,0,0,0,     //220-239
             0b011000,0,0,0,0,0,0,0b010100,0,0,0,0,0,0,0,0,0         }; //240-256

    //Same thing, except this stores the delta value to get to each square
    int m_deltaArray[257] =
            {  0,   0,   0,   0,   0,   0,   0,   0,   0, -17,   0,   0,   0,   0,   0,   0, -16,   0,   0,   0,
               0,   0,   0, -15,   0,   0, -17,   0,   0,   0,   0,   0, -16,   0,   0,   0,   0,   0, -15,   0,
               0,   0,   0, -17,   0,   0,   0,   0, -16,   0,   0,   0,   0, -15,   0,   0,   0,   0,   0,   0,
               -17,   0,   0,   0, -16,   0,   0,   0, -15,   0,   0,   0,   0,   0,   0,   0,   0, -17,   0,   0,
               -16,   0,   0, -15,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, -17, -33, -16, -31, -15,   0,
               0,   0,   0,   0,   0,   0,   0,   0,   0,   0, -18, -17, -16, -15, -14,   0,   0,   0,   0,   0,
               0,  -1,  -1,  -1,  -1,  -1,  -1,  -1,   0,   1,   1,   1,   1,   1,   1,   1,   0,   0,   0,   0,
               0,   0,  14,  15,  16,  17,  18,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  15,  31,
               16,  33,  17,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  15,   0,   0,  16,   0,   0,  17,
               0,   0,   0,   0,   0,   0,   0,   0,  15,   0,   0,   0,  16,   0,   0,   0,  17,   0,   0,   0,
               0,   0,   0,  15,   0,   0,   0,   0,  16,   0,   0,   0,   0,  17,   0,   0,   0,   0,  15,   0,
               0,   0,   0,   0,  16,   0,   0,   0,   0,   0,  17,   0,   0,  15,   0,   0,   0,   0,   0,   0,
               16,   0,   0,   0,   0,   0,   0,  17,   0,   0,   0,   0,   0,   0,   0,   0,   0};

    //Variables used to keep track of the game state
    bool m_side = WHITE;
    char m_castling = 0b1111;
    int m_halfclock = 0;
    int m_ply = 1;
    int m_ep = inv;
    std::stack<takebackInfo> m_takebackInfo;

    //Move list is a 256 entry array
    movebits m_moveStack[256] = {0};
    int m_moveStackIndex = 0;

    friend display;
    friend debug;
    friend evaluation;
    friend search;

public:
    /*
     * Generates all possible moves for the current side to move
     * TODO : Use table-driven generation to speed things up
     */
    void gen();
    //Generates only captures/checking moves, useful during quiescence search
    void geNoisy();

    //Checks if the given square is under attack by the given side
    bool sqAttacked(int sq, bool side);
    bool inCheck(bool side);

    //Encodes a move on 16bits
    static movebits encodeMove(int from, int to, char flag);

    //Decoding move functions
    static int fromSq(movebits move);
    static int toSq(movebits move);
    static char getFlag(movebits move);

    /*
     * Makes a move, as simple as that!
     * Careful tho, it doesn't really check whether or not the move is legal. If you tell the make function to move a bishop
     * from a1 to b1, it will make the move.
     */
    bool make(movebits move);

    //Takes back the last move made
    void takeback();

    //Sets up the current board position to match the given FEN notation
    void setFEN(std::string fen);

    bool sqAttackedMK2(int square, bool side);
};


#endif //BAUB_CHESS_BOARD_REPRESENTATION_H
