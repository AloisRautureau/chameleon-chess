//
// Created by bitterboyy on 1/16/21.
//

#ifndef BAUB_CHESS_BOARD_H
#define BAUB_CHESS_BOARD_H

#include "Constants.h"
#include <utility>

class Board{
private:
    /*
     * Color and piece board
     * Little-endian layout
     */
    int squareCoding[64] = {  0,  1,  2,  3,   4,   5,   6,   7,
                             16, 17, 18, 19,  20,  21,  22,  23,
                             32, 33, 34, 35,  36,  37,  38,  39,
                             48, 49, 50, 51,  52,  53,  54,  55,
                             64, 65, 66, 67,  68,  69,  70,  71,
                             80, 81, 82, 83,  84,  85,  86,  87,
                             96, 97, 98, 99, 100, 101, 102, 103,
                             112,113,114,115,116, 117, 118, 119};

    int pieces[128] = {ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK, 7, 7, 7, 7, 7, 7, 7, 7,
                       PAWN, PAWN,   PAWN,   PAWN,  PAWN, PAWN,   PAWN,   PAWN, 7, 7, 7, 7, 7, 7, 7, 7,
                       EMPTY,EMPTY,  EMPTY,  EMPTY, EMPTY,EMPTY,  EMPTY,  EMPTY,7, 7, 7, 7, 7, 7, 7, 7,
                       EMPTY,EMPTY,  EMPTY,  EMPTY, EMPTY,EMPTY,  EMPTY,  EMPTY,7, 7, 7, 7, 7, 7, 7, 7,
                       EMPTY,EMPTY,  EMPTY,  EMPTY, EMPTY,EMPTY,  EMPTY,  EMPTY,7, 7, 7, 7, 7, 7, 7, 7,
                       EMPTY,EMPTY,  EMPTY,  EMPTY, EMPTY,EMPTY,  EMPTY,  EMPTY,7, 7, 7, 7, 7, 7, 7, 7,
                       PAWN, PAWN,   PAWN,   PAWN,  PAWN, PAWN,   PAWN,   PAWN, 7, 7, 7, 7, 7, 7, 7, 7,
                       ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK, 7, 7, 7, 7, 7, 7, 7, 7};

    int color[128] = {WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, 7, 7, 7, 7, 7, 7, 7, 7,
                      WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, 7, 7, 7, 7, 7, 7, 7, 7,
                      EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, 7, 7, 7, 7, 7, 7, 7, 7,
                      EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, 7, 7, 7, 7, 7, 7, 7, 7,
                      EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, 7, 7, 7, 7, 7, 7, 7, 7,
                      EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, 7, 7, 7, 7, 7, 7, 7, 7,
                      BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, 7, 7, 7, 7, 7, 7, 7, 7,
                      BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, 7, 7, 7, 7, 7, 7, 7, 7};

    /*
     * Piece lists are kept to make move generation/evaluation faster
     * They are represented by two arrays, containing on array for each piece type
     * A value of -1 in an array indicates that there are no pieces after that, stopping any iteration
     */
    int whitePieces[6][8] = {
            {8, 9, 10, 11, 12, 13, 14, 15},
            {1, 6, -1, -1, -1, -1, -1, -1},
            {2, 5, -1, -1, -1, -1, -1, -1},
            {0, 7, -1, -1, -1, -1, -1, -1},
            {3, -1, -1, -1, -1, -1, -1, -1},
            {4, -1, -1, -1, -1, -1, -1, -1}
    };
    int blackPieces[6][8] = {
            {48, 49, 50, 51, 52, 53, 54, 55},
            {57, 62, -1, -1, -1, -1, -1, -1},
            {58, 61, -1, -1, -1, -1, -1, -1},
            {56, 63, -1, -1, -1, -1, -1, -1},
            {59, -1, -1, -1, -1, -1, -1, -1},
            {60, -1, -1, -1, -1, -1, -1, -1}
    };

    /*
     * Keeps track of the opposing side and the side that moves
     */

    int sideToMove = WHITE;
    int oppositeSide = BLACK;

    /*
     * Move stack keeps generated moves in memory, with the value of 0 indicating end of the stack
     */

    movebits moveStack[256];
    int moveStackIndex = 0;


    /*
     * Keeps track of informations like castling rights, enPassant square, fifty moves rule, current ply
     */
    int enPassant = 0;
    int castling = 0b1111;
    int fifty = 0;
    int ply = 0;

    /*
     * Stores informations about positions
     */

    std::stack<position> posHistory;
    std::stack<movebits> moveHistory;

    /*
     *  TEST VARIABLES
     */
    int lastWhiteKingPos;
    int lastBlackKingPos;


public:
    //Main move generation function, generates only pseudolegal moves
    void generate(){
        //We clear the moveStack array
        for(int i = 0; i < moveStackIndex+1; i++){
            moveStack[i] = 0;
        }
        moveStackIndex = 0;

        for(int pieceType = 0; pieceType < 6; pieceType++){
            for(int piece = 0; piece < 8; piece++){
                //Cuts the iteration if we encounter an empty piece as to not waste cycling through the rest of the piece-list
                if(sideToMove == WHITE ? whitePieces[pieceType][piece] == -1 : blackPieces[pieceType][piece] == -1) break;

                int currentPiece;
                if(sideToMove == WHITE) currentPiece = whitePieces[pieceType][piece];
                else currentPiece = blackPieces[pieceType][piece];

                if(pieceType == PAWN){
                    //Pawn movements depend on the playing side
                    if(sideToMove == WHITE) {
                        //Capture moves
                        if(isValid(currentPiece + 9) && (color[squareCoding[currentPiece + 9]] == BLACK || currentPiece + 9 == enPassant)) {
                            if(currentPiece + 9 == enPassant && ((squareCoding[currentPiece] + 17) & 0x88) == 0){
                                moveStack[moveStackIndex] = encodeMove(currentPiece, currentPiece+9, EPCAP);
                                moveStackIndex++;
                                //std::cout << "Added a pawn cap move" << std::endl;
                            }
                            else {
                                moveStack[moveStackIndex] = encodeMove(currentPiece, currentPiece+9, CAP);
                                moveStackIndex++;
                                //std::cout << "Added a pawn cap move" << std::endl;
                            }
                        }

                        if((isValid(currentPiece + 7)) && (color[squareCoding[currentPiece + 7]] == BLACK || currentPiece + 7 == enPassant)){
                            if(currentPiece + 7 == enPassant && ((squareCoding[currentPiece] + 15) & 0x88) == 0){
                                moveStack[moveStackIndex] = encodeMove(currentPiece, currentPiece+7, EPCAP);
                                moveStackIndex++;
                                //std::cout << "Added a pawn cap move" << std::endl;
                            }
                            else {
                                moveStack[moveStackIndex] = encodeMove(currentPiece, currentPiece+7, CAP);
                                moveStackIndex++;
                                //std::cout << "Added a pawn cap move" << std::endl;
                            }
                        }

                        //Quiet move
                        if(isValid(currentPiece+8) && pieces[squareCoding[currentPiece + 8]] == EMPTY){
                            //Double push
                            if(currentPiece < 16 && currentPiece > 7 && pieces[squareCoding[currentPiece + 16]] == EMPTY) {
                                moveStack[moveStackIndex] = encodeMove(currentPiece, currentPiece+16, DPAWNPUSH);
                                moveStackIndex++;
                                //std::cout << "Added a pawn push move" << std::endl;
                            }
                            moveStack[moveStackIndex] = encodeMove(currentPiece, currentPiece+8, QUIET);
                            moveStackIndex++;
                            //std::cout << "Added a pawn push move" << std::endl;
                        }
                    }

                    else {
                        //Capture moves
                        if(isValid(currentPiece - 9) && (color[squareCoding[currentPiece - 9]] == WHITE || currentPiece - 9 == enPassant)) {
                            if(currentPiece - 9 == enPassant && ((squareCoding[currentPiece] - 17) & 0x88) == 0){
                                moveStack[moveStackIndex] = encodeMove(currentPiece, currentPiece-9, EPCAP);
                                moveStackIndex++;
                                //std::cout << "Added a pawn cap move" << std::endl;
                            }
                            else {
                                moveStack[moveStackIndex] = encodeMove(currentPiece, currentPiece-9, CAP);
                                moveStackIndex++;
                                //std::cout << "Added a pawn cap move" << std::endl;
                            }
                        }

                        if(isValid(currentPiece - 7) && (color[squareCoding[currentPiece - 7]] == WHITE || currentPiece - 7 == enPassant)){
                            if(currentPiece - 7 == enPassant && ((squareCoding[currentPiece] - 15) & 0x88) == 0){
                                moveStack[moveStackIndex] = encodeMove(currentPiece, currentPiece-7, EPCAP);
                                moveStackIndex++;
                                //std::cout << "Added a pawn cap move" << std::endl;
                            }
                            else {
                                moveStack[moveStackIndex] = encodeMove(currentPiece, currentPiece-7, CAP);
                                moveStackIndex++;
                                //std::cout << "Added a pawn cap move" << std::endl;
                            }
                        }

                        //Quiet move
                        if(isValid(currentPiece - 8) && pieces[squareCoding[currentPiece - 8]] == EMPTY){
                            //Double push
                            if(currentPiece < 56 && currentPiece > 47 && pieces[squareCoding[currentPiece - 16]] == EMPTY) {
                                moveStack[moveStackIndex] = encodeMove(currentPiece, currentPiece-16, DPAWNPUSH);
                                moveStackIndex++;
                                //std::cout << "Added a pawn push move" << std::endl;
                            }
                            moveStack[moveStackIndex] = encodeMove(currentPiece, currentPiece-8, QUIET);
                            moveStackIndex++;
                            //std::cout << "Added a pawn push move" << std::endl;
                        }
                    }
                }


                else {
                    //For other types of pieces, we check :
                    /*
                     * Whether or not they slide
                     * The directions they can go towards
                     */

                    //The piece can slide
                    if(PIECEMV[0][pieceType] == 1){
                        for(int i = 0; i < 8; i++){
                            //Happends to cut off if piece only has 4 directions
                            if(PIECEMV[pieceType][i] == 0) break;

                            int currentSquare = currentPiece + PIECEMV[pieceType][i];
                            bool halt = false;
                            while(!halt){
                                if(!isValid(currentSquare) || color[squareCoding[currentSquare]] == sideToMove) {
                                    halt = true;
                                }
                                else if(color[squareCoding[currentSquare]] == oppositeSide || ((squareCoding[currentSquare] + PIECEMV88[pieceType][i]) & 0x88) != 0){
                                    moveStack[moveStackIndex] = encodeMove(currentPiece, currentSquare, CAP);
                                    moveStackIndex++;
                                    halt = true;
                                    //std::cout << "Added a sliding piece cap move" << std::endl;
                                }
                                else {
                                    moveStack[moveStackIndex] = encodeMove(currentPiece, currentSquare, QUIET);
                                    moveStackIndex++;
                                    currentSquare += PIECEMV[pieceType][i];
                                    //std::cout << "Added a sliding piece move" << std::endl;
                                }
                            }
                        }
                    }

                    else{
                        for(int i = 0; i < 8; i++){
                            int to = currentPiece + PIECEMV[pieceType][i];
                            if(!isValid(to) || ((squareCoding[currentPiece] + PIECEMV88[pieceType][i]) & 0x88) != 0 || color[squareCoding[to]] == sideToMove) {
                                continue;
                            }
                            else if(color[squareCoding[to]] == oppositeSide){
                                moveStack[moveStackIndex] = encodeMove(currentPiece, to, CAP);
                                moveStackIndex++;
                                //std::cout << "Added a piece cap move" << std::endl;
                            }
                            else {
                                moveStack[moveStackIndex] = encodeMove(currentPiece, to, QUIET);
                                moveStackIndex++;
                                //std::cout << "Added a piece move" << std::endl;
                            }
                        }
                    }
                }
            }
        }

        //Generate castling moves
        int castlingT = castling;
        while(castlingT > 0){
            if((castlingT & 0b1000) == 0b1000){
                if(pieces[squareCoding[6]] == EMPTY && pieces[squareCoding[5]] == EMPTY){
                    moveStack[moveStackIndex] = encodeMove(4, 6, KINGC);
                    moveStackIndex++;
                }
                castlingT -= 0b1000;
            }
            if((castlingT & 0b0100) == 0b0100){
                if(pieces[squareCoding[1]] == EMPTY && pieces[squareCoding[2]] == EMPTY && pieces[squareCoding[3]] == EMPTY){
                    moveStack[moveStackIndex] = encodeMove(4, 2, QUEENC);
                    moveStackIndex++;
                }
                castlingT -= 0b0100;
            }
            if((castlingT & 0b0010) == 0b0010){
                if(pieces[squareCoding[62]] == EMPTY && pieces[squareCoding[61]] == EMPTY){
                    moveStack[moveStackIndex] = encodeMove(60, 62, KINGC);
                    moveStackIndex++;
                }

                castlingT -= 0b0010;
            }
            if((castlingT & 0b0001) == 0b0001){
                if(pieces[squareCoding[57]] == EMPTY && pieces[squareCoding[58]] == EMPTY && pieces[squareCoding[59]] == EMPTY){
                    moveStack[moveStackIndex] = encodeMove(60, 58, QUEENC);
                    moveStackIndex++;
                }
                castlingT -= 0b0001;
            }
        }
    };

    //Same thing but only generating captures
    void generateCaps(){}

    //Checks if the given side is in check to avoid illegal moves when making them
    bool inCheck(int side){
        //Find the king
        int kingSquare = -1;
        if(side == WHITE) kingSquare = whitePieces[5][0];
        else kingSquare = blackPieces[5][0];

        //Case where the king cannot be found, shouldn't happen but it never hurts to prepare for the eventuality
        if(kingSquare == -1) {
            std::cout << "King not found !" << std::endl;
            return true;
        }

        //Generate possible moves and checks if any of them target the kingSquare
        generate();
        for(int i = 0; i < moveStackIndex; i++){
            if(to(moveStack[i]) == kingSquare) {
                return true;
            }
        }
        return false;
    }

    //Returns true if the given side is in chekmate
    bool checkmate(int side){
        if((sideToMove == side && moveStackIndex == 0 && inCheck(side)) || (inCheck(side) && (whitePieces[5][0] == -1 || blackPieces[5][0] == -1))) return true;
        return false;
    }

    //Makes maintaining the piecelists easier
    void updatePieceList(int from, int to, int pieceType, int piecelist){
        if(piecelist == WHITE){
            for(int i = 0; i < 8; i++){
                if(whitePieces[pieceType][i] == from){
                    whitePieces[pieceType][i] = to;
                    break;
                }
            }
        }
        else{
            for(int i = 0; i < 8; i++){
                if(blackPieces[pieceType][i] == from){
                    blackPieces[pieceType][i] = to;
                    break;
                }
            }
        }
    }

    //Function used to make a move
    bool makeMove(movebits move){

        //Stores the position in case we need to takeback
        posHistory.push(encodePosition(enPassant, castling, fifty, pieces[squareCoding[to(move)]]));
        moveHistory.push(move);
        
        //Moves the rook if this is a castling move
        if((move & KINGC) == KINGC){
            if(sideToMove == WHITE){
                pieces[7] = EMPTY; color[7] = EMPTY;
                pieces[5] = ROOK; color[5] = WHITE;
                updatePieceList(7, 5, ROOK, WHITE);
                castling -= 0b1100;
            }
            else{
                pieces[119] = EMPTY; color[119] = EMPTY;
                pieces[117] = ROOK; color[117] = BLACK;
                updatePieceList(119, 117, ROOK, BLACK);
                castling -= 0b0011;
            }
        }
        else if((move & QUEENC) == QUEENC){
            if(sideToMove == WHITE){
                pieces[0] = EMPTY; color[0] = EMPTY;
                pieces[3] = ROOK; color[3] = WHITE;
                updatePieceList(0, 3, ROOK, WHITE);
                castling -= 0b1100;
            }
            else{
                pieces[112] = EMPTY; color[112] = EMPTY;
                pieces[115] = ROOK; color[115] = BLACK;
                updatePieceList(112, 115, ROOK, BLACK);
                castling -= 0b0011;
            }
        }

        //Update castling rights, enPassant square, move, fifty-moves rule
        if(pieces[from(move)] == KING){
            castling -= sideToMove == WHITE ? 0b1100 : 0b0011;
        }
        if(pieces[from(move)] == ROOK){
            switch(from(move)){
                case 0 : if((castling & 0b0100) == 0b0100) castling -= 0b0100; break;
                case 7 : if((castling & 0b1000) == 0b1000) castling -= 0b1000; break;
                case 112 : if((castling & 0b0001) == 0b0001) castling -= 0b0001; break;
                case 119 : if((castling & 0b0010) == 0b0010) castling -= 0b0010; break;
                default : break;
            }
        }

        if((spe(move) & DPAWNPUSH) == DPAWNPUSH) enPassant = sideToMove == WHITE ? to(move) - 8 : to(move) + 8;
        else enPassant = -1;

        if((spe(move) & CAP) == CAP || pieces[squareCoding[from(move)]] == PAWN) fifty = 0;
        else fifty++;
        ply++;

        //Make the actual move
        if((spe(move) & CAP) == CAP)
            updatePieceList(to(move), -1, pieces[squareCoding[to(move)]], oppositeSide);

        pieces[squareCoding[to(move)]] = pieces[squareCoding[from(move)]];
        pieces[squareCoding[from(move)]] = EMPTY;

        color[squareCoding[to(move)]] = sideToMove;
        color[squareCoding[from(move)]] = EMPTY;

        updatePieceList(from(move), to(move), pieces[squareCoding[to(move)]], sideToMove);


        //Delete pawn if enPassant
        if((spe(move) & EPCAP) == EPCAP){
            if(sideToMove == WHITE){
                pieces[squareCoding[to(move) - 8]] = EMPTY;
                color[squareCoding[to(move) - 8]] = EMPTY;
                updatePieceList(to(move) - 8, -1, PAWN, BLACK);
            }
            else{
                pieces[squareCoding[to(move) + 8]] = EMPTY;
                color[squareCoding[to(move) + 8]] = EMPTY;
                updatePieceList(to(move) + 8, -1, PAWN, WHITE);
            }
        }

        //Promotion
        if((spe(move) & 0b1000) == 0b1000){
            switch(spe(move)){
                case 0b1000:
                    pieces[squareCoding[to(move)]] = KNIGHT;
                    break;

                case 0b1001:
                    pieces[squareCoding[to(move)]] = BISHOP;
                    break;

                case 0b1010:
                    pieces[squareCoding[to(move)]] = ROOK;
                    break;

                case 0b1011:
                    pieces[squareCoding[to(move)]] = QUEEN;
                    break;

                default:
                    break;
            }
        }


        sideToMove ^= 1;
        oppositeSide ^= 1;

        //If king is in check, the move isn't legal and we need to takeback
        if(inCheck(oppositeSide)){
            takeback();
            return false;
        }

        return true;
    }
    
    //Simply takeback last move
    void takeback(){
        if(moveHistory.empty() || posHistory.empty()) return;

        //Gets back positionnal informations
        enPassant = ep(posHistory.top());
        castling = castle(posHistory.top());
        fifty = fiftymoves(posHistory.top());
        ply--;
        sideToMove = oppositeSide;
        oppositeSide = sideToMove ^ 1;
        
        //Undo the actual move
        //starting whith moving back our piece
        updatePieceList(to(moveHistory.top()), from(moveHistory.top()), pieces[squareCoding[to(moveHistory.top())]], sideToMove);
        pieces[squareCoding[from(moveHistory.top())]] = pieces[squareCoding[to(moveHistory.top())]];
        color[squareCoding[from(moveHistory.top())]] = color[squareCoding[to(moveHistory.top())]];
        
        //Then, if a piece was captured, placing it back on the destination square
        if(cap(posHistory.top()) != 0b111){
            pieces[squareCoding[to(moveHistory.top())]] = cap(posHistory.top());
            color[squareCoding[to(moveHistory.top())]] = color[squareCoding[to(moveHistory.top())]] ^ 1;
            updatePieceList(-1, to(moveHistory.top()), cap(posHistory.top()), oppositeSide);
        }
        else{
            pieces[squareCoding[to(moveHistory.top())]] = EMPTY;
            color[squareCoding[to(moveHistory.top())]] = EMPTY;
        }

        
        //Unmaking special moves like enPassant, promotions and castling
        if(spe(moveHistory.top()) == EPCAP){
            if(sideToMove == WHITE) {
                pieces[squareCoding[to(moveHistory.top()) - 8]] = PAWN;
                color[squareCoding[to(moveHistory.top()) - 8]] = BLACK;
                updatePieceList(-1, to(moveHistory.top()) - 8, PAWN, BLACK);
            }

            else {
                pieces[squareCoding[to(moveHistory.top()) + 8]] = PAWN;
                color[squareCoding[to(moveHistory.top()) + 8]] = WHITE;
                updatePieceList(-1, to(moveHistory.top()) + 8, PAWN, WHITE);
            }
        }

        if((spe(moveHistory.top()) & 0b1000) == 0b1000){
            pieces[squareCoding[from(moveHistory.top())]] = PAWN;
        }

        if((spe(moveHistory.top()) & 0b0010) == 0b0010){
            if((spe(moveHistory.top()) & KINGC) == KINGC){
                if(sideToMove == WHITE){
                    pieces[5] = EMPTY; color[5] = EMPTY;
                    pieces[7] = ROOK; color[7] = WHITE;
                    updatePieceList(5, 7, ROOK, WHITE);
                }
                else{
                    pieces[117] = EMPTY; color[117] = EMPTY;
                    pieces[119] = ROOK; color[119] = BLACK;
                    updatePieceList(117, 119, ROOK, BLACK);
                }
            }
            else if((spe(moveHistory.top()) & QUEENC) == QUEENC){
                if(sideToMove == WHITE){
                    pieces[3] = EMPTY; color[3] = EMPTY;
                    pieces[0] = ROOK; color[0] = WHITE;
                    updatePieceList(3, 0, ROOK, WHITE);
                }
                else{
                    pieces[115] = EMPTY; color[115] = EMPTY;
                    pieces[112] = ROOK; color[112] = BLACK;
                    updatePieceList(115, 112, ROOK, BLACK);
                }
            }
        }

        //End with removing top element of the stack
        posHistory.pop();
        moveHistory.pop();
    }


    //Function purposed with testing the performance and exactitude of the generation function
    long perft(int depth){
        long nodes = 0;
        int captures = 0;

        if(depth == 0 || checkmate(sideToMove)) return 1ULL;

        generate();
        int stack[256];
        for(int i = 0; i < moveStackIndex; i++){
            stack[i] = moveStack[i];
        }
        int moves = moveStackIndex;
        for(int i = 0; i < moves; i++){
            if(makeMove(stack[i])) {
                nodes += perft(depth - 1);
                takeback();
            }
        }

        return nodes;
    }

    //Same thing, but with in console display
    void perftTest(){
        //Useful for testing accuracy
        long nodesExpected[15] = {
                1, 20, 400, 8902, 197281, 4865609, 119060324, 3195901860, 8498978956, 2439530234167, 69352859712417
        };

        std::cout << "##### PERFT TIME!!! #####" << std::endl << std::endl;
        std::cout << " _________________________________________________" << std::endl;
        std::cout << "| depth  | nodes         | time    | nodes/sec    |" << std::endl;

        for(int depth = 0; depth < 10; depth++){
            std::cout << " _________________________________________________" << std::endl;
            time_t startTime = time(nullptr);
            long nodes = perft(depth);
            time_t endTime = time(nullptr);
            double diff = difftime(endTime, startTime);



            std::cout << "| " << depth;
            for(int i = std::to_string(depth).length() + 2; i < 9; i++) std::cout << " ";
            std::cout << "| " << nodes;
            for(int i = std::to_string(nodes).length() + 2; i < 16; i++) std::cout << " ";
            std::cout.precision(2);
            std::cout << "| " << std::fixed << diff;
            for(int i = std::to_string((int)diff).length() + 5; i < 10; i++) std::cout << " ";
            std::cout << "| " << std::fixed << nodes/diff;
            for(int i = std::to_string((int)nodes/diff).length() + 5; i < 18; i++) std::cout << " ";
            std::cout << "|";
            if(depth < 11 && nodes != nodesExpected[depth]) std::cout << (nodes > nodesExpected[depth] ? " !!! too many moves !!! " : " !!! missing moves !!! ") << std::endl;
            else std::cout << std::endl;
        }
    }
    
    //Shows the current position in console
    void showPosition(bool flip = false, bool debug = false){
        for(int i = 0; i < 64; i++){
            int square = i;
            if(!flip){
                square = (7 - (int)(i/8))*8 + i%8;
            }
            //Next rank
            if(square%8 == 0){
                std::cout << std::endl << "    +-----+-----+-----+-----+-----+-----+-----+-----+" << std::endl << " " << (int)(square/8) + 1 << "  |  ";
            }

            switch(pieces[squareCoding[square]]){
                case EMPTY:
                default:
                    std::cout << "   |  ";
                    break;

                case PAWN:
                    std::cout << (color[squareCoding[square]] == WHITE ? "P  |  " : "p  |  ");
                    break;

                case BISHOP:
                    std::cout << (color[squareCoding[square]] == WHITE ? "B  |  " : "b  |  ");
                    break;

                case KNIGHT:
                    std::cout << (color[squareCoding[square]] == WHITE ? "N  |  " : "n  |  ");
                    break;

                case ROOK:
                    std::cout << (color[squareCoding[square]] == WHITE ? "R  |  " : "r  |  ");
                    break;

                case QUEEN:
                    std::cout << (color[squareCoding[square]] == WHITE ? "Q  |  " : "q  |  ");
                    break;

                case KING:
                    std::cout << (color[squareCoding[square]] == WHITE ? "K  |  " : "k  |  ");
                    break;
            }
        }

        std::cout << std::endl << "    +-----+-----+-----+-----+-----+-----+-----+-----+" << std::endl << "       ";

        for(int i = 0; i < 8; i++){
            std::cout << (char)('A' + i) << "     ";
        }

        std::cout << std::endl << std::endl << (sideToMove == WHITE ? "White " : "Black ") << "to move!" << std::endl;
        std::cout << "Ply " << ply << std::endl << std::endl;

        std::cout << "Write your move down here :" << std::endl << "<from><to><promotion>" << std::endl;

        std::cout << "List of possible moves : " << std::endl;
        for(int i = 0; i < moveStackIndex; i++){
            std::cout << from(moveStack[i]) << "->" << to(moveStack[i]) << std::endl;
        }

        if(debug){
            //Less pretty way, but certainely working
            for(int i = 0; i < 64; i++){
                if(i%8 == 0) std::cout << std::endl;
                std::cout << pieces[squareCoding[i]] << " ";
            }
        }
    }

    //Given a move, returns the corresponding move from the stack (with special bits set)
    movebits isLegal(movebits move){
        for(int i = 0; i < moveStackIndex; i++){
            if(from(move) == from(moveStack[i]) && to(move) == to(moveStack[i])){
                return moveStack[i];
            }
        }
        return 0;
    }

    //The evaluation function returns the current "score" of the position
    int evaluation(){
        /*
         * This must take into account :
         * Material values
         * Pawn structure
         * Piece to square
         * Mobility
         *
         * Must invert the result when black plays for negamax
         */
        //TODO Piece-square evaluation in endgame
        int scores[2] = {0, 0};
        int material[2] = {0, 0};
        int pSqBonuses[2] = {0, 0};
        int pieceCounterWhite[6] = {0, 0, 0, 0, 0, 0};
        int pieceCounterBlack[6] = {0, 0, 0, 0, 0, 0};

        //We iterate the picelists piecetype by piecetype
        for(int pieceType = 0; pieceType < 6; pieceType++){
            for(int i = 0; i < 8; i++){
                if(whitePieces[pieceType][i] != -1){
                    material[WHITE] += materialValue[pieceType];
                    pSqBonuses[WHITE] += pSquare[pieceType][whitePieces[pieceType][i]%8 + (7 - whitePieces[pieceType][i]/8)*8];
                    pieceCounterWhite[pieceType]++;
                }
                if(blackPieces[pieceType][i] != -1){
                    material[BLACK] += materialValue[pieceType];
                    pSqBonuses[BLACK] += pSquare[pieceType][blackPieces[pieceType][i]];
                    pieceCounterBlack[pieceType]++;
                }
            }
        }

        std::cout << "Material balance : " << material[WHITE] - material[BLACK] << std::endl;
        std::cout << "Piece placement bonuses : " << pSqBonuses[WHITE] - pSqBonuses[BLACK] << std::endl;
        for(int i = 0; i < 6; i++){
            std::cout << "White has " << pieceCounterWhite[i] << " pieceType" << i << std::endl;
            std::cout << "Black has " << pieceCounterBlack[i] << " pieceType" << i << std::endl;
        }


        //TODO Pawn structure using hash tables

        //TODO Piece value eval :
        //Knights decrease in value as less pawns are here
        //Bishops surrounded by pawns lose a lot of mobility, bishop pair gives a bonus
        //Rooks increase in value as pawns disappear, are better on an open file and on the seventh rank, in endgames they are good if on the same file as a pawn

    }
};

#endif //BAUB_CHESS_BOARD_H
