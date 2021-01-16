//
// Created by bitterboyy on 1/9/21.
//

#ifndef BAUB_CHESS_CONSTANTS_H
#define BAUB_CHESS_CONSTANTS_H

#include <utility>
#include <vector>
#include <bitset>
#include <stack>

//Couleurs
const int WHITE = 0;
const int BLACK = 1;
const int EMPTY = -1;

//Pieces
const int PAWN = 0;
const int KNIGHT = 1;
const int BISHOP = 2;
const int ROOK = 3;
const int QUEEN = 4;
const int KING = 5;

const int pieceValue[6] = {100, 350, 350, 525, 1000, 10000};

//Valeurs de coups spéciaux
const int PROMOTABLE = 0;
const int DOUBLEPAWNPUSH = -4;
const int ENPASSANT = -2;
const int CASTLE = -3;


//Definition des coups
class Move{
public:
    int start;
    int end;
    int special;

    Move(int start, int end, int special) : start(start), end(end), special(special){};
};

//Les array mailbox permettent de vérifier la validité des coups pour éviter à un cavalier de se téléporter à l'autre bout du plateau
int mailbox[120] = {
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1,  0,  1,  2,  3,  4,  5,  6,  7, -1,
        -1,  8,  9, 10, 11, 12, 13, 14, 15, -1,
        -1, 16, 17, 18, 19, 20, 21, 22, 23, -1,
        -1, 24, 25, 26, 27, 28, 29, 30, 31, -1,
        -1, 32, 33, 34, 35, 36, 37, 38, 39, -1,
        -1, 40, 41, 42, 43, 44, 45, 46, 47, -1,
        -1, 48, 49, 50, 51, 52, 53, 54, 55, -1,
        -1, 56, 57, 58, 59, 60, 61, 62, 63, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

int mailbox64[64] = {
        21, 22, 23, 24, 25, 26, 27, 28,
        31, 32, 33, 34, 35, 36, 37, 38,
        41, 42, 43, 44, 45, 46, 47, 48,
        51, 52, 53, 54, 55, 56, 57, 58,
        61, 62, 63, 64, 65, 66, 67, 68,
        71, 72, 73, 74, 75, 76, 77, 78,
        81, 82, 83, 84, 85, 86, 87, 88,
        91, 92, 93, 94, 95, 96, 97, 98
};

//Direction des pièces qui slide (fou, tour, dame)
int directionsFou[4] = {-7, 7, -9, 9};
int directionsTour[4] = {8, -8, 1, -1};
int directionsDame[8] = {8, -8, 1, -1, -7, 7, -9, 9};

//Permet de faire des takeback facilement
class Position{
public:
    int castling;
    int fiftyMoveRule;
    int pieces[64]{};
    int color[64]{};

    Position(int castling, int fifty, const int piecesp[64], const int colorp[64]) :
    castling(castling), fiftyMoveRule(fifty){
        for(int i = 0; i < 64; i++){
            pieces[i] = piecesp[i];
            color[i] = colorp[i];
        }
    };
};

//Permet de stocker facilement les coups et de leur donner un score pour le move ordering
class MoveStack{
private:
    std::vector<Move> m_moves;
    std::vector<int> m_scores;

public:
    void storeMove(Move move, int score){
        //If it is a pawn push on the backrank
        if(move.special == PROMOTABLE && ((move.end <= 63 && move.end >= 56) xor (move.end <= 7 && move.end >= 0))){
            //On génère chaque promotion possible
            for(int i = KNIGHT; i < KING; i++){
                m_moves.push_back(Move(move.start, move.end, i));
                m_scores.push_back(i*10);
            }
        }

        m_moves.push_back(move);
        m_scores.push_back(score);
    }

    void reset(){
        m_moves.clear();
        m_scores.clear();
    }

    bool empty(){
        return m_moves.empty() && m_scores.empty();
    }

    //Sert quand l'humain joue
    Move searchMove(int start, int end, int spe = -1){
        for(Move move : m_moves){
            if((spe != -1 && move.start == start && move.end == end && move.special == spe)
                || (move.start == start && move.end == end)) return move;
        }
        return {0, 0, 0};
    }

    //Sorts the stack with best moves first
    void sortStack(){

    }

    //returns the move stack
    std::vector<Move> getStack(){ return m_moves; }

    //Used for testing purposes
    void printStack(){
        for(Move move : m_moves){
            std::cout << move.start << " " << move.end << std::endl;
        }
    }
};


int pieceMovements[4][8]{
    //knight
    {10, -10, 17, -17, 15, -15, 6, -6},
    //bishop
    {-7, 7, -9, 9, 0, 0, 0, 0},

    //Rook
    {8, -8, 1, -1, 0, 0, 0, 0},

    //Queen/King
    {-7, 7, -9, 9, 8, -8, 1, -1},
};

//même chose mais sur la mailbox
int pieceMailboxMovements[4][8]{
    {12, -12, 21, -21, 19, -19, 8, -8},
    {-9, 9, -11, 11, 0, 0, 0, 0},
    {10, -10, 1, -1, 0, 0, 0, 0},
    {-9, 9, -11, 11, 10, -10, 1, -1},
};

//change des coordonnées en entier
int coordinateToInt(std::string coordinate){
    char colonne = coordinate.at(0);
    char ligne = coordinate.at(1);
    int col = colonne - 'a';
    int lig = ligne - '1';

    return lig*8 + col;
}


//Constantes utiles pour l'évaluation
int TBLPIONS[64] = {
        0,  0,  0,  0,  0,  0,  0,  0,
        50, 50, 50, 50, 50, 50, 50, 50,
        10, 10, 20, 30, 30, 20, 10, 10,
        5,  5, 10, 30, 30, 10,  5,  5,
        0,  0,  0, 20, 20,  0,  0,  0,
        5, -5,-10,  0,  0,-10, -5,  5,
        5, 10, 10,-20,-20, 10, 10,  5,
        0,  0,  0,  0,  0,  0,  0,  0
};

int TBLKNIGHT[64] = {
        -50,-40,-30,-30,-30,-30,-40,-50,
        -40,-20,  0,  0,  0,  0,-20,-40,
        -30,  0, 10, 15, 15, 10,  0,-30,
        -30,  5, 15, 20, 20, 15,  5,-30,
        -30,  0, 15, 20, 20, 15,  0,-30,
        -30,  5, 10, 15, 15, 10,  5,-30,
        -40,-20,  0,  5,  5,  0,-20,-40,
        -50,-40,-30,-30,-30,-30,-40,-50
};

int TBLBISHOP[64] = {
        -20,-10,-10,-10,-10,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5, 10, 10,  5,  0,-10,
        -10,  5,  5, 10, 10,  5,  5,-10,
        -10,  0, 10, 10, 10, 10,  0,-10,
        -10, 10, 10, 10, 10, 10, 10,-10,
        -10,  5,  0,  0,  0,  0,  5,-10,
        -20,-10,-10,-10,-10,-10,-10,-20
};

int TBLROOK[64] = {
        0,  0,  0,  0,  0,  0,  0,  0,
        5, 10, 10, 10, 10, 10, 10,  5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        0,  0,  0,  5,  5,  0,  0,  0

};

int TBLQUEEN[64] = {
        -20,-10,-10, -5, -5,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5,  5,  5,  5,  0,-10,
        -5,  0,  5,  5,  5,  5,  0, -5,
        0,  0,  5,  5,  5,  5,  0, -5,
        -10,  5,  5,  5,  5,  5,  0,-10,
        -10,  0,  5,  0,  0,  0,  0,-10,
        -20,-10,-10, -5, -5,-10,-10,-20
};

int TBLKING[64] = {
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -20,-30,-30,-40,-40,-30,-30,-20,
        -10,-20,-20,-20,-20,-20,-20,-10,
        20, 20,  0,  0,  0,  0, 20, 20,
        20, 30, 10,  0,  0, 10, 30, 20
};

int TBLKING_END[64] = {
        -50,-40,-30,-20,-20,-30,-40,-50,
        -30,-20,-10,  0,  0,-10,-20,-30,
        -30,-10, 20, 30, 30, 20,-10,-30,
        -30,-10, 30, 40, 40, 30,-10,-30,
        -30,-10, 30, 40, 40, 30,-10,-30,
        -30,-10, 20, 30, 30, 20,-10,-30,
        -30,-30,  0,  0,  0,  0,-30,-30,
        -50,-30,-30,-30,-30,-30,-30,-50
};

//Coordonnées inversées pour les tables
int INVERT[64] = {
        56, 57, 58, 59, 60, 61, 62, 63,
        48, 49, 50, 51, 52, 53, 54, 55,
        40, 41, 42, 43, 44, 45, 46, 47,
        32, 33, 34, 35, 36, 37, 38, 39,
        24, 25, 26, 27, 28, 29, 30, 31,
        16, 17, 18, 19, 20, 21, 22, 23,
         8,  9, 10, 11, 12, 13, 14, 15,
         0,  1,  2,  3,  4,  5,  6,  7
};


#endif //BAUB_CHESS_CONSTANTS_H
