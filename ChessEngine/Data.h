//
// Created by bitterboyy on 12/11/20.
//

#ifndef BAUB_CHESS_DATA_H
#define BAUB_CHESS_DATA_H

#define EMPTY           0;

//Couleurs
#define BLACK           2;
#define WHITE           1;

//Pieces
#define PAWN            1;
#define KNIGHT          3;
#define BISHOP          4;
#define ROOK            5;
#define QUEEN           9;
#define KING            100;


//Cases importantes
#define A1				0
#define B1				1
#define C1				2
#define D1				3
#define E1				4
#define F1				5
#define G1				6
#define H1				7
#define A8				56
#define B8				57
#define C8				58
#define D8				59
#define E8				60
#define F8				61
#define G8				62
#define H8				63

#define FILE(x)			(x >> 3)
#define RANG(x)			(x & 7)

//Board vide
#define EMPTY_BOARD     {0, 0, 0, 0, 0, 0, 0, 0, \
                        0, 0, 0, 0, 0, 0, 0, 0,  \
                        0, 0, 0, 0, 0, 0, 0, 0,  \
                        0, 0, 0, 0, 0, 0, 0, 0,  \
                        0, 0, 0, 0, 0, 0, 0, 0,  \
                        0, 0, 0, 0, 0, 0, 0, 0,  \
                        0, 0, 0, 0, 0, 0, 0, 0,  \
                        0, 0, 0, 0, 0, 0, 0, 0}


#endif //BAUB_CHESS_DATA_H
