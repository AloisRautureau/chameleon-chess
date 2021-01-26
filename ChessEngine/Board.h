//
// Created by bitterboyy on 1/16/21.
//

#ifndef BAUB_CHESS_BOARD_H
#define BAUB_CHESS_BOARD_H

#include "Constants.h"
#include <utility>
#include <cmath>
#include <chrono>

/*
 * Board class is used to represent the current position, generate and make moves, and generally speaking modify the board in any meaningful way
 * It does most of the heavy lifting, if you'd like
 *
 * It uses a 0x88 representation, as well as a piece-list, in order to store those informations
 * Other small variables are used to keep information such as side to move etc
 */

class Board{
private:
    /*
     * The main boards, in 0x88 representation
     */
    BYTE pieces[128] = {ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK,        INV, INV, INV, INV, INV, INV, INV, INV,
                        PAWN, PAWN,   PAWN,   PAWN,  PAWN, PAWN,   PAWN,   PAWN,        INV, INV, INV, INV, INV, INV, INV, INV,
                        EMPTY,EMPTY,  EMPTY,  EMPTY,EMPTY, EMPTY, EMPTY,   EMPTY,       INV, INV, INV, INV, INV, INV, INV, INV,
                        EMPTY,EMPTY,  EMPTY,  EMPTY,EMPTY, EMPTY, EMPTY,   EMPTY,       INV, INV, INV, INV, INV, INV, INV, INV,
                        EMPTY,EMPTY,  EMPTY,  EMPTY,EMPTY, EMPTY, EMPTY,   EMPTY,       INV, INV, INV, INV, INV, INV, INV, INV,
                        EMPTY,EMPTY,  EMPTY,  EMPTY,EMPTY, EMPTY, EMPTY,   EMPTY,       INV, INV, INV, INV, INV, INV, INV, INV,
                        PAWN, PAWN,   PAWN,   PAWN,  PAWN, PAWN,   PAWN,   PAWN,        INV, INV, INV, INV, INV, INV, INV, INV,
                        ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK,        INV, INV, INV, INV, INV, INV, INV, INV, };

    BYTE colors[128] = {WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE,         INV, INV, INV, INV, INV, INV, INV, INV,
                        WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE,         INV, INV, INV, INV, INV, INV, INV, INV,
                        EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,         INV, INV, INV, INV, INV, INV, INV, INV,
                        EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,         INV, INV, INV, INV, INV, INV, INV, INV,
                        EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,         INV, INV, INV, INV, INV, INV, INV, INV,
                        EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,         INV, INV, INV, INV, INV, INV, INV, INV,
                        BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK,         INV, INV, INV, INV, INV, INV, INV, INV,
                        BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK,         INV, INV, INV, INV, INV, INV, INV, INV, };

    /*
     * Piece list part
     */

    BYTE whitePieces[6][10] = {
            //Pawns
            {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, INV, INV},
            //Knights
            {0x01, 0x06, INV, INV, INV, INV, INV, INV, INV, INV},
            //Bishops
            {0x02, 0x05, INV, INV, INV, INV, INV, INV, INV, INV},
            //Rooks
            {0x00, 0x07, INV, INV, INV, INV, INV, INV, INV, INV},
            //Queens
            {0x03, INV, INV, INV, INV, INV, INV, INV, INV, INV},
            //King
            {0x04, INV, INV, INV, INV, INV, INV, INV, INV, INV},
    };

    BYTE blackPieces[6][10] = {
            //Pawns
            {0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, INV, INV},
            //Knights
            {0x71, 0x76, INV, INV, INV, INV, INV, INV, INV, INV},
            //Bishops
            {0x72, 0x75, INV, INV, INV, INV, INV, INV, INV, INV},
            //Rooks
            {0x70, 0x77, INV, INV, INV, INV, INV, INV, INV, INV},
            //Queens
            {0x73, INV, INV, INV, INV, INV, INV, INV, INV, INV},
            //King
            {0x74, INV, INV, INV, INV, INV, INV, INV, INV, INV},
    };

    /*
     * List of available moves, the generator uses it to store it's findings
     */
    MOVEBITS moveList[256];
    int moveListIndx = 0;

    /*
     * Side to move, fifty moves rule, etc
     */
    BYTE sideToMove = WHITE;
    int ply = 0;
    int fifty = 0;
    BYTE enPassant = INV;
    BYTE castlingRights = 0b1111;
    std::stack<POSITIONBITS> history;

    /*
     * Lets Evaluation.h and Search.h access Board class private members
     */
    friend class Evaluation;
    friend class Search;
    friend class Debug;



public:
    /*
     * Move generation functions
     * Generate all psuedolegal moves/only captures, in the current position
     */

    void gen(){
        moveListIndx = 0;
        //For each piece in each pieceType
        for(int pieceType = 0; pieceType < 6; pieceType++){
            for(int piece = 0; piece < 10; piece++){
                int adress = sideToMove == WHITE ? whitePieces[pieceType][piece] : blackPieces[pieceType][piece];
                //If the adress points to an invalid square, piece doesn't exist and we skip to the next pieceType
                if(adress == INV) break;

                if(pieceType == PAWN){
                    if(sideToMove == WHITE){
                        if(pieces[adress + 16] == EMPTY){
                            if(pieces[adress + 32] == EMPTY && adress >= 0x10 && adress <= 0x17){
                                moveList[moveListIndx] = encodeMove(adress, adress+32, DPAWNPUSH);
                                moveListIndx++;
                            }
                            if(adress >= 0x60 && adress <= 0x67){ //Generate promotions
                                for(int i = 0; i < 4; i++){
                                    moveList[moveListIndx] = encodeMove(adress, adress+16, KNIGHTPROM+i);
                                    moveListIndx++;
                                }
                            }
                            else{
                                moveList[moveListIndx] = encodeMove(adress, adress+16, QUIET);
                                moveListIndx++;
                            }
                        }

                        if(colors[adress + 15] == BLACK || adress+15 == enPassant){
                            if(adress+15 == enPassant && enPassant != INV){
                                moveList[moveListIndx] = encodeMove(adress, enPassant, EPCAP);
                                moveListIndx++;
                            }
                            else if(adress + 15 >= 0x70 && adress + 15 <= 0x77){ //Generate promotions
                                for(int i = 0; i < 4; i++){
                                    moveList[moveListIndx] = encodeMove(adress, adress+15, KNIGHTPROM+i | CAP);
                                    moveListIndx++;
                                }
                            }
                            else{
                                moveList[moveListIndx] = encodeMove(adress, adress+15, CAP);
                                moveListIndx++;
                            }
                        }

                        if(colors[adress + 17] == BLACK || adress+17 == enPassant){
                            if(adress+17 == enPassant && enPassant != INV){
                                moveList[moveListIndx] = encodeMove(adress, enPassant, EPCAP);
                                moveListIndx++;
                            }
                            else if(adress + 17 >= 0x70 && adress + 17 <= 0x77){ //Generate promotions
                                for(int i = 0; i < 4; i++){
                                    moveList[moveListIndx] = encodeMove(adress, adress+17, KNIGHTPROM+i | CAP);
                                    moveListIndx++;
                                }
                            }
                            else{
                                moveList[moveListIndx] = encodeMove(adress, adress+17, CAP);
                                moveListIndx++;
                            }
                        }
                    }

                    else{
                        if(pieces[adress - 16] == EMPTY){
                            if(pieces[adress - 32] == EMPTY && adress >= 0x60 && adress <= 0x67){
                                moveList[moveListIndx] = encodeMove(adress, adress-32, DPAWNPUSH);
                                moveListIndx++;
                            }
                            if(adress >= 0x10 && adress <= 0x17){ //Generate promotions
                                for(int i = 0; i < 4; i++){
                                    moveList[moveListIndx] = encodeMove(adress, adress-16, KNIGHTPROM+i);
                                    moveListIndx++;
                                }
                            }
                            else{
                                moveList[moveListIndx] = encodeMove(adress, adress-16, QUIET);
                                moveListIndx++;
                            }
                        }

                        if(colors[adress - 15] == WHITE || adress-15 == enPassant){
                            if(adress-15 == enPassant && enPassant != INV){
                                moveList[moveListIndx] = encodeMove(adress, enPassant, EPCAP);
                                moveListIndx++;
                            }
                            else if(adress - 15 >= 0x00 && adress - 15 <= 0x07){ //Generate promotions
                                for(int i = 0; i < 4; i++){
                                    moveList[moveListIndx] = encodeMove(adress, adress-15, KNIGHTPROM+i | CAP);
                                    moveListIndx++;
                                }
                            }
                            else{
                                moveList[moveListIndx] = encodeMove(adress, adress-15, CAP);
                                moveListIndx++;
                            }
                        }

                        if(colors[adress - 17] == WHITE || adress-17 == enPassant){
                            if(adress-17 == enPassant && enPassant != INV){
                                moveList[moveListIndx] = encodeMove(adress, enPassant, EPCAP);
                                moveListIndx++;
                            }
                            else if(adress - 17 >= 0x00 && adress - 17 <= 0x07){ //Generate promotions
                                for(int i = 0; i < 4; i++){
                                    moveList[moveListIndx] = encodeMove(adress, adress-17, KNIGHTPROM+i | CAP);
                                    moveListIndx++;
                                }
                            }
                            else{
                                moveList[moveListIndx] = encodeMove(adress, adress-17, CAP);
                                moveListIndx++;
                            }
                        }
                    }
                }

                else{
                    //For each move direction of the pieceType, we check if the move is valid or not
                    //In the case the piece can slide, we repeat the operation until we find an invalid move
                    for(int i = 0; i < 8; i++){
                        int direction = pieceMv[pieceType][i];
                        if(direction == 0) break;
                        BYTE currentSquare = adress + direction;
                        while(true){
                            if(isInvalid(currentSquare) || colors[currentSquare] == sideToMove || currentSquare > 0x7F) break;
                            else if(colors[currentSquare] == (sideToMove^1)){
                                moveList[moveListIndx] = encodeMove(adress, currentSquare, CAP);
                                moveListIndx++;
                                break;
                            }
                            else{
                                moveList[moveListIndx] = encodeMove(adress, currentSquare, QUIET);
                                moveListIndx++;
                                currentSquare += direction;
                                if(!pieceMv[0][pieceType]) break;
                            }
                        }
                    }
                }
            }
        }
        //Generate castling moves
        //To do this sort of effectively, we check conditions gradually by order of time complexity
        //Therefore, we end up by checking if the squares are attacked
        if(sideToMove == WHITE){
            if((castlingRights & 0b1000) == 0b1000){
                //White kingside castle
                if(colors[0x05] == EMPTY && colors[0x06] == EMPTY){
                    if(!(isUnderAttack(0x05, BLACK) || isUnderAttack(0x06, BLACK) || isUnderAttack(0x04, BLACK))){
                        moveList[moveListIndx] = encodeMove(0x04, 0x06, KCASTLE);
                        moveListIndx++;
                    }
                }
            }
            if((castlingRights & 0b0100) == 0b0100){
                //White queenside castle
                if(colors[0x03] == EMPTY && colors[0x02] == EMPTY && colors[0x01] == EMPTY){
                    if(!(isUnderAttack(0x02, BLACK) || isUnderAttack(0x03, BLACK) || isUnderAttack(0x04, BLACK) || isUnderAttack(0x01, BLACK))){
                        moveList[moveListIndx] = encodeMove(0x04, 0x02, QCASTLE);
                        moveListIndx++;
                    }
                }
            }
        }
        else{
            if((castlingRights & 0b0010) == 0b0010){
                //Black kingside castle
                if(colors[0x75] == EMPTY && colors[0x76] == EMPTY){
                    if(!(isUnderAttack(0x75, WHITE) || isUnderAttack(0x76, WHITE) || isUnderAttack(0x74, WHITE))){
                        moveList[moveListIndx] = encodeMove(0x74, 0x76, KCASTLE);
                        moveListIndx++;
                    }
                }
            }
            if((castlingRights & 0b0001) == 0b0001){
                //Black queenside castle
                if(colors[0x73] == EMPTY && colors[0x72] == EMPTY && colors[0x71] == EMPTY){
                    if(!(isUnderAttack(0x72, WHITE) || isUnderAttack(0x73, WHITE) || isUnderAttack(0x74, WHITE) || isUnderAttack(0x71, WHITE))){
                        moveList[moveListIndx] = encodeMove(0x74, 0x72, QCASTLE);
                        moveListIndx++;
                    }
                }
            }
        }

        /*
        for(int i = 0; i < moveListIndx; i++){
            std::cout << std::hex << (int)fromSq(moveList[i]) << " -> " << (int)toSq(moveList[i]) << std::dec << " " << moveList[i] << std::endl;
        }
        std::cout << std::dec << std::endl;
         */


    }

    //Generates only captures for quiescence search
    void genCaps(){
        moveListIndx = 0;
        //For each piece in each pieceType
        for(int pieceType = 0; pieceType < 6; pieceType++){
            for(int piece = 0; piece < 10; piece++){
                int adress = sideToMove == WHITE ? whitePieces[pieceType][piece] : blackPieces[pieceType][piece];
                //If the adress points to an invalid square, piece doesn't exist and we skip to the next pieceType
                if(adress == INV) break;

                if(pieceType == PAWN){
                    if(sideToMove == WHITE){
                        if(colors[adress + 15] == BLACK || adress+15 == enPassant){
                            if(adress+15 == enPassant && enPassant != INV){
                                moveList[moveListIndx] = encodeMove(adress, enPassant, EPCAP);
                                moveListIndx++;
                            }
                            else if(adress + 15 >= 0x70 && adress + 15 <= 0x77){ //Generate promotions
                                for(int i = 0; i < 4; i++){
                                    moveList[moveListIndx] = encodeMove(adress, adress+15, KNIGHTPROM+i | CAP);
                                    moveListIndx++;
                                }
                            }
                            else{
                                moveList[moveListIndx] = encodeMove(adress, adress+15, CAP);
                                moveListIndx++;
                            }
                        }

                        if(colors[adress + 17] == BLACK || adress+17 == enPassant){
                            if(adress+17 == enPassant && enPassant != INV){
                                moveList[moveListIndx] = encodeMove(adress, enPassant, EPCAP);
                                moveListIndx++;
                            }
                            else if(adress + 17 >= 0x70 && adress + 17 <= 0x77){ //Generate promotions
                                for(int i = 0; i < 4; i++){
                                    moveList[moveListIndx] = encodeMove(adress, adress+17, KNIGHTPROM+i | CAP);
                                    moveListIndx++;
                                }
                            }
                            else{
                                moveList[moveListIndx] = encodeMove(adress, adress+17, CAP);
                                moveListIndx++;
                            }
                        }
                    }

                    else{
                        if(colors[adress - 15] == WHITE || adress-15 == enPassant){
                            if(adress-15 == enPassant && enPassant != INV){
                                moveList[moveListIndx] = encodeMove(adress, enPassant, EPCAP);
                                moveListIndx++;
                            }
                            else if(adress - 15 >= 0x00 && adress - 15 <= 0x07){ //Generate promotions
                                for(int i = 0; i < 4; i++){
                                    moveList[moveListIndx] = encodeMove(adress, adress-15, KNIGHTPROM+i | CAP);
                                    moveListIndx++;
                                }
                            }
                            else{
                                moveList[moveListIndx] = encodeMove(adress, adress-15, CAP);
                                moveListIndx++;
                            }
                        }

                        if(colors[adress - 17] == WHITE || adress-17 == enPassant){
                            if(adress-17 == enPassant && enPassant != INV){
                                moveList[moveListIndx] = encodeMove(adress, enPassant, EPCAP);
                                moveListIndx++;
                            }
                            else if(adress - 17 >= 0x00 && adress - 17 <= 0x07){ //Generate promotions
                                for(int i = 0; i < 4; i++){
                                    moveList[moveListIndx] = encodeMove(adress, adress-17, KNIGHTPROM+i | CAP);
                                    moveListIndx++;
                                }
                            }
                            else{
                                moveList[moveListIndx] = encodeMove(adress, adress-17, CAP);
                                moveListIndx++;
                            }
                        }
                    }
                }

                else{
                    //For each move direction of the pieceType, we check if the move is valid or not
                    //In the case the piece can slide, we repeat the operation until we find an invalid move
                    for(int i = 0; i < 8; i++){
                        int direction = pieceMv[pieceType][i];
                        if(direction == 0) break;
                        BYTE currentSquare = adress + direction;
                        while(true){
                            if(isInvalid(currentSquare) || colors[currentSquare] == sideToMove || currentSquare > 0x7F) break;
                            else if(colors[currentSquare] == (sideToMove^1)){
                                moveList[moveListIndx] = encodeMove(adress, currentSquare, CAP);
                                moveListIndx++;
                                break;
                            }
                            else{
                                currentSquare += direction;
                                if(!pieceMv[0][pieceType]) break;
                            }
                        }
                    }
                }
            }
        }
    }

    //Checks if a given square is under attack by the given side
    //Since this function is quite heavy to calculate, we try to call it as rarely as possible
    bool isUnderAttack(BYTE square, int side){
        for(int pieceType = 0; pieceType < 6; pieceType++){
            for(int piece = 0; piece < 10; piece++){
                int adress = (side == WHITE ? whitePieces[pieceType][piece] : blackPieces[pieceType][piece]);
                //If the adress points to an invalid square, piece doesn't exist and we skip to the next pieceType
                if(adress == INV) break;

                if(pieceType == PAWN){
                    if(side == WHITE) {
                        if (adress + 15 == square || adress + 17 == square) return true;
                    }

                    else {
                        if (adress - 15 == square || adress - 17 == square) return true;
                    }
                }

                else{
                    //For each move direction of the pieceType, we check if the move is valid or not
                    //In the case the piece can slide, we repeat the operation until we find an invalid move
                    for(int i = 0; i < 8; i++){
                        int direction = pieceMv[pieceType][i];
                        if(direction == 0) break;
                        //If the signs are different, it means we're not going in the right direction and can ignore it
                        //if(std::signbit(direction) == std::signbit(square - adress)) continue;
                        BYTE currentSquare = adress + direction;

                        while(true){
                            if(currentSquare == square) return true;
                            if(isInvalid(currentSquare) || colors[currentSquare] != EMPTY || currentSquare>0x7F || !pieceMv[0][pieceType]) break;
                            currentSquare += direction;
                        }
                    }
                }
            }
        }
        return false;
    }

    /*
     * Make and unmake functions
     */

    //Updates the piecelist
    //Flags are 0 for changing adress, 1 to insert a new piece, 2 delete an entry
    void updatePieceList(int side, BYTE from, BYTE to, BYTE pieceType, int flag){
        //Checks once to make sure the given adress doesn't already exist
        for(int i = 0; i < 10; i++){
            if(to != INV && (side == WHITE ? whitePieces[pieceType][i] : blackPieces[pieceType][i]) == to){
                return;
            }
        }
        for(int i = 0; i < 10; i++){
            if((side == WHITE ? whitePieces[pieceType][i] : blackPieces[pieceType][i]) == from){
                switch(flag){
                    default: //Change adress
                    case 1: //Insert (changing from INV to given square
                        (side == WHITE ? whitePieces[pieceType][i] : blackPieces[pieceType][i]) = to;
                        break;

                    case 2: //delete an entry
                        for(int j = i; j < 10; j++){
                            if(side == WHITE){
                                if(whitePieces[pieceType][j] == INV){
                                    whitePieces[pieceType][i] = whitePieces[pieceType][j-1];
                                    whitePieces[pieceType][j-1] = to;
                                    break;
                                }
                            }
                            else{
                                if(blackPieces[pieceType][j] == INV){
                                    blackPieces[pieceType][i] = blackPieces[pieceType][j-1];
                                    blackPieces[pieceType][j-1] = to;
                                    break;
                                }
                            }
                        }
                        break;
                }
                break;
            }
        }
    }

    bool makeMove(MOVEBITS move){
        //Save the current position in case we need to takeback
        history.push(encodePosition(move, pieces[toSq(move)], enPassant, castlingRights, fifty));

        //We take care of moving the designed rook first if we're dealing with a castling move
        if(flag(move) == KCASTLE){
            if(sideToMove == WHITE){
                pieces[0x07] = EMPTY; colors[0x07] = EMPTY;
                pieces[0x05] = ROOK; colors[0x05] = WHITE;
                castlingRights -= (0b1100 & castlingRights);
                updatePieceList(WHITE, 0x07, 0x05, ROOK, 0);
            }
            else{
                pieces[0x77] = EMPTY; colors[0x77] = EMPTY;
                pieces[0x75] = ROOK; colors[0x75] = BLACK;
                castlingRights -= (0b0011 & castlingRights);
                updatePieceList(BLACK, 0x77, 0x75, ROOK, 0);
            }
        }
        else if(flag(move) == QCASTLE){
            if(sideToMove == WHITE){
                pieces[0x00] = EMPTY; colors[0x00] = EMPTY;
                pieces[0x03] = ROOK; colors[0x03] = WHITE;
                castlingRights -= (0b1100 & castlingRights);
                updatePieceList(WHITE, 0x00, 0x03, ROOK, 0);
            }
            else{
                pieces[0x70] = EMPTY; colors[0x70] = EMPTY;
                pieces[0x73] = ROOK; colors[0x73] = BLACK;
                castlingRights -= (0b0011 & castlingRights);
                updatePieceList(BLACK, 0x70, 0x73, ROOK, 0);
            }
        }

        //Update all the "bonus" informations like castling rights, ply, fifty move rule, enPassant
        //Castling
        if(pieces[fromSq(move)] == KING) {
            if(sideToMove == WHITE) castlingRights -= (0b1100 & castlingRights);
            else castlingRights -= (0b0011 & castlingRights);
        }
        else if(pieces[fromSq(move)] == ROOK){
            if(sideToMove == WHITE){
                if(fromSq(move) == 0x00) castlingRights -= (0b0100 & castlingRights);
                if(fromSq(move) == 0x07) castlingRights -= (0b1000 & castlingRights);
            }
            else{
                if(fromSq(move) == 0x70) castlingRights -= (0b0001 & castlingRights);
                if(fromSq(move) == 0x77) castlingRights -= (0b0010 & castlingRights);
            }
        }
        else if(pieces[toSq(move)] == ROOK){
            if(sideToMove == BLACK){
                if(toSq(move) == 0x00) castlingRights -= (0b0100 & castlingRights);
                if(toSq(move) == 0x07) castlingRights -= (0b1000 & castlingRights);
            }
            else{
                if(toSq(move) == 0x70) castlingRights -= (0b0001 & castlingRights);
                if(toSq(move) == 0x77) castlingRights -= (0b0010 & castlingRights);
            }
        }
        //Ply/fifty
        ply++;
        if(pieces[toSq(move)] != EMPTY || pieces[fromSq(move)] == PAWN) fifty = 0;
        else fifty++;
        //En passant
        if(flag(move) == DPAWNPUSH) enPassant = sideToMove == WHITE ? toSq(move) - 16 : toSq(move) + 16;
        else enPassant = INV;

        //Actual move making
        if((flag(move) & CAP) == CAP){
            if(flag(move) == EPCAP){
                if(sideToMove == WHITE){
                    pieces[toSq(move) - 16] = EMPTY; colors[toSq(move) - 16] = EMPTY;
                    updatePieceList(BLACK, toSq(move) - 16, INV, PAWN, 2);
                }
                else{
                    pieces[toSq(move) + 16] = EMPTY; colors[toSq(move) + 16] = EMPTY;
                    updatePieceList(WHITE, toSq(move) + 16, INV, PAWN, 2);
                }
            }
            else{
                updatePieceList(sideToMove^1, toSq(move), INV, pieces[toSq(move)], 2);
            }
        }


        pieces[toSq(move)] = pieces[fromSq(move)];
        colors[toSq(move)] = colors[fromSq(move)];
        pieces[fromSq(move)] = EMPTY; colors[fromSq(move)] = EMPTY;
        updatePieceList(sideToMove, fromSq(move), toSq(move), pieces[toSq(move)], 0);

        //Take care of promotion if necessary
        if(((toSq(move) >= 0x70 && toSq(move) <= 0x77) || (toSq(move) <= 0x07 && toSq(move) >= 0x00)) && pieces[toSq(move)] == PAWN){

            switch(flag(move) & 0b1011){
                case KNIGHTPROM:
                    pieces[toSq(move)] = KNIGHT;
                    updatePieceList(sideToMove, INV, toSq(move), KNIGHT, 1);
                    updatePieceList(sideToMove, toSq(move), INV, PAWN, 2);
                    break;
                case BISHOPPROM:
                    pieces[toSq(move)] = BISHOP;
                    updatePieceList(sideToMove, INV, toSq(move), BISHOP, 1);
                    updatePieceList(sideToMove, toSq(move), INV, PAWN, 2);
                    break;
                case ROOKPROM:
                    pieces[toSq(move)] = ROOK;
                    updatePieceList(sideToMove, INV, toSq(move), ROOK, 1);
                    updatePieceList(sideToMove, toSq(move), INV, PAWN, 2);
                    break;
                case QUEENPROM:
                default:
                    pieces[toSq(move)] = QUEEN;
                    updatePieceList(sideToMove, INV, toSq(move), QUEEN, 1);
                    updatePieceList(sideToMove, toSq(move), INV, PAWN, 2);
                    break;
            }
        }

        //We check if the king is in check, if it's the case we takeback the move
        if(isUnderAttack((sideToMove == WHITE ? whitePieces[5][0] : blackPieces[5][0]), sideToMove^1)){
            sideToMove ^= 1;
            unmake();
            return false;
        }

        sideToMove ^= 1;
        return true;

    }

    //Unmakes the last move
    void unmake(){
        //Reset the basic stuff
        castlingRights = castling(history.top());
        fifty = fiftyClock(history.top());
        enPassant = enPassantLast(history.top());
        sideToMove ^= 1;
        ply--;
        MOVEBITS move = movePlayed(history.top());
        BYTE pieceTaken = pieceToWakeFromTheDead(history.top());

        //Unpromote...
        if((flag(move) & 0b1000) == 0b1000){
            updatePieceList(sideToMove, toSq(move), INV, pieces[toSq(move)], 2); //Delete the piece in the promoted piece list
            updatePieceList(sideToMove, INV, toSq(move), PAWN, 1); //Place the pawn in the pawn list
            pieces[toSq(move)] = PAWN;
        }

        //Then unmake the main move
        pieces[fromSq(move)] = pieces[toSq(move)];
        colors[fromSq(move)] = colors[toSq(move)];
        pieces[toSq(move)] = EMPTY; colors[toSq(move)] = EMPTY;
        //std::cout << std::hex << "Putting back " << (int)pieces[fromSq(move)] << " from " << (int)toSq(move) << " to " << (int)fromSq(move) << std::dec <<std::endl;
        updatePieceList(sideToMove, toSq(move), fromSq(move), pieces[fromSq(move)], 0);

        //Replace a pawn if it was taken en passant
        if(flag(move) == EPCAP){
            if(sideToMove == WHITE){
                pieces[toSq(move) - 16] = PAWN;
                colors[toSq(move) - 16] = BLACK;
                updatePieceList(BLACK, INV, toSq(move) - 16, PAWN, 1);
            }
            else{
                pieces[toSq(move) + 16] = PAWN;
                colors[toSq(move) + 16] = WHITE;
                updatePieceList(WHITE, INV, toSq(move) + 16, PAWN, 1);
            }
        }
        else if((flag(move) & CAP) == CAP){
            pieces[toSq(move)] = pieceTaken;
            colors[toSq(move)] = sideToMove^1;
            updatePieceList(sideToMove^1, INV, toSq(move), pieceTaken, 1);
        }


        //Reset stuff like castling...
        if(flag(move) == KCASTLE){
            if(sideToMove == WHITE){
                pieces[0x05] = EMPTY; colors[0x05] = EMPTY;
                pieces[0x07] = ROOK; colors[0x07] = WHITE;
                updatePieceList(WHITE, 0x05, 0x07, ROOK, 0);
            }
            else{
                pieces[0x75] = EMPTY; colors[0x75] = EMPTY;
                pieces[0x77] = ROOK; colors[0x77] = BLACK;
                updatePieceList(BLACK, 0x75, 0x77, ROOK, 0);
            }
        }
        else if(flag(move) == QCASTLE){
            if(sideToMove == WHITE){
                pieces[0x03] = EMPTY; colors[0x03] = EMPTY;
                pieces[0x00] = ROOK; colors[0x00] = WHITE;
                updatePieceList(WHITE, 0x03, 0x00, ROOK, 0);
            }
            else{
                pieces[0x73] = EMPTY; colors[0x73] = EMPTY;
                pieces[0x70] = ROOK; colors[0x70] = BLACK;
                updatePieceList(BLACK, 0x73, 0x70, ROOK, 0);
            }
        }

        history.pop();
    }

    void showBoard(bool flip = false, bool verbose = false){
        std::cout << (sideToMove == WHITE ? "White " : "Black ") << "to move" << std::endl;
        for(int i = 0; i < 0x80; i++){
            int adress = fileIndex(i) + (7-rankIndex(i))*16;
            if(flip) adress = i;

            if((i&0x88) != 0) continue;
            if(fileIndex(adress) == 0){
                std::cout << std::endl << "   +---+---+---+---+---+---+---+---+" << std::endl << " " << (int)rankIndex(adress) + 1 << " | ";
            }

            switch(pieces[adress]){
                case EMPTY:
                    std::cout << "  | ";
                    break;
                case PAWN:
                    std::cout << (colors[adress] == WHITE ? "P | " : "p | ");
                    break;
                case KNIGHT:
                    std::cout << (colors[adress] == WHITE ? "N | " : "n | ");
                    break;
                case BISHOP:
                    std::cout << (colors[adress] == WHITE ? "B | " : "b | ");
                    break;
                case ROOK:
                    std::cout << (colors[adress] == WHITE ? "R | " : "r | ");
                    break;
                case QUEEN:
                    std::cout << (colors[adress] == WHITE ? "Q | " : "q | ");
                    break;
                case KING:
                    std::cout << (colors[adress] == WHITE ? "K | " : "k | ");
                    break;
                default: break;
            }
        }

        std::cout << std::endl << "   +---+---+---+---+---+---+---+---+" << std::endl << "     ";
        for(int i = 0; i < 8; i++){
            std::cout << (char)('A' + i) << "   ";
        }
        std::cout << std::endl << std::endl << "Current ply : " << ply << std::endl;

        if(verbose){
            std::cout << "Piece list situation :" << std::endl;
            std::cout << "White : ";
            for(int i = 0; i < 6; i++){
                std::cout << std::endl << pieceNames[i] << " : ";
                for(int j = 0; j < 10; j++){
                    if(whitePieces[i][j] == INV) break;
                    std::cout <<std::hex << (int)whitePieces[i][j] << ", ";
                }
            }
            std::cout << std::endl << "Black : ";
            for(int i = 0; i < 6; i++){
                std::cout << std::endl << pieceNames[i] << " : ";
                for(int j = 0; j < 10; j++){
                    if(blackPieces[i][j] == INV) break;
                    std::cout <<std::hex << (int)blackPieces[i][j] << ", ";
                }
            }
            std::cout << "En passant square : " << (int)enPassant << std::endl;
            std::cout << std::dec;
        }
    }


    /*
    * FEN notation functions
    * Those functions are made to set a board state to a given FEN string, or output a FEN from a position
    */
    void setFEN(const std::string& fen){
        //Piece list variables for counting how many pieces we got
        int blackCounter[6] = {0, 0, 0, 0, 0, 0};
        int whiteCounter[6] = {0, 0, 0, 0, 0, 0};

        //FEN notation reads from A8 to H1, so we start at A8
        int charIndex = 0;
        int squareIndex = 0x70;
        bool settings = false;
        for(charIndex; charIndex < fen.size(); charIndex++){
            if(settings) break;

            switch(fen[charIndex]){
                case ' ': //Makes the cut to the settings part
                    settings = true;
                    break;

                case '/': //In that case, we get to the next line
                    squareIndex -= 0x18;
                    break;

                case 'p':
                    pieces[squareIndex] = PAWN;
                    colors[squareIndex] = BLACK;
                    blackPieces[PAWN][blackCounter[PAWN]] = squareIndex;
                    blackCounter[PAWN]++;
                    squareIndex++;
                    break;

                case 'P':
                    pieces[squareIndex] = PAWN;
                    colors[squareIndex] = WHITE;
                    whitePieces[PAWN][whiteCounter[PAWN]] = squareIndex;
                    whiteCounter[PAWN]++;
                    squareIndex++;
                    break;

                case 'n':
                    pieces[squareIndex] = KNIGHT;
                    colors[squareIndex] = BLACK;
                    blackPieces[KNIGHT][blackCounter[KNIGHT]] = squareIndex;
                    blackCounter[KNIGHT]++;
                    squareIndex++;
                    break;

                case 'N':
                    pieces[squareIndex] = KNIGHT;
                    colors[squareIndex] = WHITE;
                    whitePieces[KNIGHT][whiteCounter[KNIGHT]] = squareIndex;
                    whiteCounter[KNIGHT]++;
                    squareIndex++;
                    break;

                case 'b':
                    pieces[squareIndex] = BISHOP;
                    colors[squareIndex] = BLACK;
                    blackPieces[BISHOP][blackCounter[BISHOP]] = squareIndex;
                    blackCounter[BISHOP]++;
                    squareIndex++;
                    break;

                case 'B':
                    pieces[squareIndex] = BISHOP;
                    colors[squareIndex] = WHITE;
                    whitePieces[BISHOP][whiteCounter[BISHOP]] = squareIndex;
                    whiteCounter[BISHOP]++;
                    squareIndex++;
                    break;

                case 'r':
                    pieces[squareIndex] = ROOK;
                    colors[squareIndex] = BLACK;
                    blackPieces[ROOK][blackCounter[ROOK]] = squareIndex;
                    blackCounter[ROOK]++;
                    squareIndex++;
                    break;

                case 'R':
                    pieces[squareIndex] = ROOK;
                    colors[squareIndex] = WHITE;
                    whitePieces[ROOK][whiteCounter[ROOK]] = squareIndex;
                    whiteCounter[ROOK]++;
                    squareIndex++;
                    break;

                case 'q':
                    pieces[squareIndex] = QUEEN;
                    colors[squareIndex] = BLACK;
                    blackPieces[QUEEN][blackCounter[QUEEN]] = squareIndex;
                    blackCounter[QUEEN]++;
                    squareIndex++;
                    break;

                case 'Q':
                    pieces[squareIndex] = QUEEN;
                    colors[squareIndex] = WHITE;
                    whitePieces[QUEEN][whiteCounter[QUEEN]] = squareIndex;
                    whiteCounter[QUEEN]++;
                    squareIndex++;
                    break;

                case 'k':
                    pieces[squareIndex] = KING;
                    colors[squareIndex] = BLACK;
                    blackPieces[KING][blackCounter[KING]] = squareIndex;
                    blackCounter[KING]++;
                    squareIndex++;
                    break;

                case 'K':
                    pieces[squareIndex] = KING;
                    colors[squareIndex] = WHITE;
                    whitePieces[KING][whiteCounter[KING]] = squareIndex;
                    whiteCounter[KING]++;
                    squareIndex++;
                    break;

                default: //We get to this branch if we hit a numeral
                    for(int i = 0; i < fen[charIndex] - '0'; i++){
                        pieces[squareIndex] = EMPTY;
                        colors[squareIndex] = EMPTY;
                        squareIndex++;
                    }
                    break;
            }
        }
        //Once we're out, we clear the rest of the piece lists
        for(int i = 0; i < 6; i++){
            while(blackCounter[i] < 10){
                blackPieces[i][blackCounter[i]] = INV;
                blackCounter[i]++;
            }
            while(whiteCounter[i] < 10){
                whitePieces[i][whiteCounter[i]] = INV;
                whiteCounter[i]++;
            }
        }

        //Then we set the rest
        sideToMove = fen[charIndex] == 'w' ? WHITE : BLACK;
        charIndex+=2;

        castlingRights = 0;
        if(fen[charIndex] !=  '-'){
            while(fen[charIndex] != ' '){
                switch(fen[charIndex]){
                    case 'K':
                        castlingRights += 0b1000;
                        break;
                    case 'Q':
                        castlingRights += 0b0100;
                        break;
                    case 'k':
                        castlingRights += 0b0010;
                        break;
                    case 'q':
                        castlingRights += 0b0001;
                        break;
                    default:
                        break;
                }
                charIndex++;
            }
        }
        else charIndex++;
        charIndex++;

        if(fen[charIndex] == '-') {
            enPassant = INV;
            charIndex++;
        }
        else{
            enPassant = fileIndex(fen[charIndex] - 'A') + rankIndex(fen[charIndex + 1] - '0')*0x10;
            charIndex+=2;
        }
        charIndex++;

        if(charIndex == fen.size() - 4){
            ply = fen[charIndex] - '0';
            charIndex+=2;

            fifty = fen[charIndex] - '0';
        }
    }

    //Returns the current position's hash value
    ZOBHASH getPositionHash() const{
        ZOBHASH hash = 0xffffffffffffffff;
        for(int side = 0; side < 2; side++){
            for(int pieceType = 0; pieceType < 6; pieceType++){
                for(int piece = 0; piece < 10; piece++){
                    switch(pieceType){
                        case PAWN:
                            hash ^= (side == WHITE ? zobristKeys[WPAWNZOB + square8x8(piece)] : zobristKeys[BPAWNZOB + square8x8(piece)]);
                            break;

                        case KNIGHT:
                            hash ^= (side == WHITE ? zobristKeys[WKNIGHTZOB + square8x8(piece)] : zobristKeys[BKNIGHTZOB + square8x8(piece)]);
                            break;

                        case BISHOP:
                            hash ^= (side == WHITE ? zobristKeys[WBISHOPZOB + square8x8(piece)] : zobristKeys[BBISHOPZOB + square8x8(piece)]);
                            break;

                        case ROOK:
                            hash ^= (side == WHITE ? zobristKeys[WROOKZOB + square8x8(piece)] : zobristKeys[BROOKZOB + square8x8(piece)]);
                            break;

                        case QUEEN:
                            hash ^= (side == WHITE ? zobristKeys[WQUEENZOB + square8x8(piece)] : zobristKeys[BQUEENZOB + square8x8(piece)]);
                            break;

                        case KING:
                            hash ^= (side == WHITE ? zobristKeys[WKINGZOB + square8x8(piece)] : zobristKeys[BKINGZOB + square8x8(piece)]);
                            break;

                        default:
                            break;
                    }
                }
            }
        }
        if(sideToMove == BLACK) hash ^= zobristKeys[SIDEZOB];

        if(castlingRights & 0b1000) hash ^= zobristKeys[WKCASTLEZOB];
        if(castlingRights & 0b0100) hash ^= zobristKeys[WQCASTLEZOB];
        if(castlingRights & 0b0010) hash ^= zobristKeys[BKCASTLEZOB];
        if(castlingRights & 0b0001) hash ^= zobristKeys[BQCASTLEZOB];

        if(enPassant != INV){
            hash ^= zobristKeys[AEPZOB + fileIndex(enPassant)];
        }

        return hash;
    }
};

#endif //BAUB_CHESS_BOARD_H
