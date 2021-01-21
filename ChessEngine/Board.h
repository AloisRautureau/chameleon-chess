//
// Created by bitterboyy on 1/16/21.
//

#ifndef BAUB_CHESS_BOARD_H
#define BAUB_CHESS_BOARD_H

#include "Constants.h"
#include <utility>
#include <cmath>

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
    BYTE whiteIndexBoard[64] = {
            0, 0, 0, 0, 0, 1, 1, 1,
            0, 1, 2, 3, 4, 5, 6, 7,
            INV, INV, INV, INV, INV, INV, INV, INV,
            INV, INV, INV, INV, INV, INV, INV, INV,
            INV, INV, INV, INV, INV, INV, INV, INV, 
            INV, INV, INV, INV, INV, INV, INV, INV,
            INV, INV, INV, INV, INV, INV, INV, INV,
            INV, INV, INV, INV, INV, INV, INV, INV
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
    BYTE blackIndexBoard[64] = {
            INV, INV, INV, INV, INV, INV, INV, INV,
            INV, INV, INV, INV, INV, INV, INV, INV,
            INV, INV, INV, INV, INV, INV, INV, INV,
            INV, INV, INV, INV, INV, INV, INV, INV,
            INV, INV, INV, INV, INV, INV, INV, INV,
            INV, INV, INV, INV, INV, INV, INV, INV,
            0, 1, 2, 3, 4, 5, 6, 7,
            0, 0, 0, 0, 0, 1, 1, 1
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



public:
    /*
     * Move generation functions
     * Generate all psuedolegal moves/only captures, in the current position
     */
    void gen(){
        //For each piece in each pieceType
        for(int pieceType = 0; pieceType < 6; pieceType++){
            for(int piece = 0; piece < 10; piece++){
                int adress = sideToMove == WHITE ? whitePieces[pieceType][piece] : blackPieces[pieceType][piece];
                //If the adress points to an invalid square, piece doesn't exist and we skip to the next pieceType
                if(adress == INV) break;

                if(pieceType == PAWN){
                    if(colors[adress] == WHITE){
                        if(pieces[adress + 16] == EMPTY){
                            if(pieces[adress + 32] == EMPTY){
                                moveList[moveListIndx] = encodeMove(adress, adress+32, DPAWNPUSH);
                                moveListIndx++;
                            }
                            moveList[moveListIndx] = encodeMove(adress, adress+16, QUIET);
                            moveListIndx++;
                        }

                        if(colors[adress + 15] == BLACK || adress+15 == enPassant){
                            if(adress+15 == enPassant){
                                moveList[moveListIndx] = encodeMove(adress, enPassant, EPCAP);
                                moveListIndx++;
                            }
                            moveList[moveListIndx] = encodeMove(adress, adress+15, CAP);
                            moveListIndx++;
                        }

                        if(colors[adress + 17] == BLACK || adress+17 == enPassant){
                            if(adress+17 == enPassant){
                                moveList[moveListIndx] = encodeMove(adress, enPassant, EPCAP);
                                moveListIndx++;
                            }
                            moveList[moveListIndx] = encodeMove(adress, adress+17, CAP);
                            moveListIndx++;
                        }
                    }

                    else{
                        if(pieces[adress - 16] == EMPTY){
                            if(pieces[adress - 32] == EMPTY){
                                moveList[moveListIndx] = encodeMove(adress, adress-32, DPAWNPUSH);
                                moveListIndx++;
                            }
                            moveList[moveListIndx] = encodeMove(adress, adress-16, QUIET);
                            moveListIndx++;
                        }

                        if(colors[adress - 15] == WHITE || adress-15 == enPassant){
                            if(adress-15 == enPassant){
                                moveList[moveListIndx] = encodeMove(adress, enPassant, EPCAP);
                                moveListIndx++;
                            }
                            moveList[moveListIndx] = encodeMove(adress, adress-15, CAP);
                            moveListIndx++;
                        }

                        if(colors[adress - 17] == WHITE || adress-17 == enPassant){
                            if(adress-17 == enPassant){
                                moveList[moveListIndx] = encodeMove(adress, enPassant, EPCAP);
                                moveListIndx++;
                            }
                            moveList[moveListIndx] = encodeMove(adress, adress-17, CAP);
                            moveListIndx++;
                        }
                    }
                }

                else{
                    //For each move direction of the pieceType, we check if the move is valid or not
                    //In the case the piece can slide, we repeat the operation until we find an invalid move
                    for(int i = 0; i < 8; i++){
                        int direction = pieceMv[pieceType][i];
                        if(direction == 0) break;
                        bool done = false;
                        BYTE currentSquare = adress + direction;
                        while(!done){
                            if(isInvalid(currentSquare) || colors[currentSquare] == sideToMove || currentSquare>0x7F) done = true;
                            else if(colors[currentSquare] == (sideToMove^1)){
                                moveList[moveListIndx] = encodeMove(adress, currentSquare, CAP);
                                moveListIndx++;
                                done = true;
                            }
                            else{
                                moveList[moveListIndx] = encodeMove(adress, currentSquare, QUIET);
                                moveListIndx++;
                                currentSquare += direction;
                                done = pieceMv[0][pieceType] == false;
                            }
                        }
                    }
                }
            }
        }
        //Generate castling moves
        //To do this sort of effectively, we check conditions gradually by order of time complexity
        //Therefore, we end up by checking if the squares are attacked
        if((castlingRights & 0b1000) == 0b1000){
            //White kingside castle
            if(pieces[0x05] == EMPTY && pieces[0x06] == EMPTY && sideToMove == WHITE){
                if(!(isUnderAttack(0x05, BLACK) || isUnderAttack(0x06, BLACK) || isUnderAttack(0x04, BLACK))){
                    moveList[moveListIndx] = encodeMove(0x04, 0x06, KCASTLE);
                    moveListIndx++;
                }
            }
        }
        if((castlingRights & 0b0100) == 0b0100){
            //White queenside castle
            if(pieces[0x03] == EMPTY && pieces[0x02] == EMPTY && pieces[0x01] == EMPTY && sideToMove == WHITE){
                if(!(isUnderAttack(0x02, BLACK) || isUnderAttack(0x03, BLACK) || isUnderAttack(0x04, BLACK) || isUnderAttack(0x01, BLACK))){
                    moveList[moveListIndx] = encodeMove(0x04, 0x02, QCASTLE);
                    moveListIndx++;
                }
            }
        }
        if((castlingRights & 0b0010) == 0b0010){
            //Black kingside castle
            if(pieces[0x75] == EMPTY && pieces[0x76] == EMPTY && sideToMove == BLACK){
                if(!(isUnderAttack(0x75, WHITE) || isUnderAttack(0x76, WHITE) || isUnderAttack(0x74, WHITE))){
                    moveList[moveListIndx] = encodeMove(0x74, 0x76, KCASTLE);
                    moveListIndx++;
                }
            }
        }
        if((castlingRights & 0b0001) == 0b0001){
            //Black queenside castle
            if(pieces[0x73] == EMPTY && pieces[0x72] == EMPTY && pieces[0x71] == EMPTY && sideToMove == BLACK){
                if(!(isUnderAttack(0x72, WHITE) || isUnderAttack(0x73, WHITE) || isUnderAttack(0x74, WHITE) || isUnderAttack(0x71, WHITE))){
                    moveList[moveListIndx] = encodeMove(0x74, 0x72, QCASTLE);
                    moveListIndx++;
                }
            }
        }
    }

    //Checks if a given square is under attack by the given side
    //Since this function is quite heavy to calculate, we try to call it as rarely as possible
    bool isUnderAttack(BYTE square, int side){
        for(int pieceType = 0; pieceType < 6; pieceType++){
            for(int piece = 0; piece < 10; piece++){
                int adress = side == WHITE ? blackPieces[pieceType][piece] : whitePieces[pieceType][piece];
                //If the adress points to an invalid square, piece doesn't exist and we skip to the next pieceType
                if(adress == INV) break;

                if(pieceType == PAWN){
                    if(colors[adress] == WHITE) {
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
                        if(std::signbit(direction) == std::signbit(square - adress)) continue;

                        bool done = false;
                        BYTE currentSquare = adress + direction;

                        while(!done){
                            if(isInvalid(currentSquare) || colors[currentSquare] != EMPTY || currentSquare>0x7F || pieceMv[0][pieceType] == false) done = true;
                            else if(currentSquare == square) return true;
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
    void updatePieceList(int side, BYTE from, BYTE to, BYTE pieceType, int flag){
        bool deleted = false;
        for(int i = 0; i < 10; i++){
            switch(flag){
                case 0:
                default:
                    if(side == WHITE){
                        if(whitePieces[pieceType][i] == INV) break;
                        if(whitePieces[pieceType][i] == from) whitePieces[pieceType][i] = to;
                    }
                    else{
                        if(blackPieces[pieceType][i] == INV) break;
                        if(blackPieces[pieceType][i] == from) blackPieces[pieceType][i] = to;
                    }
                    break;

                case 1: //Insert
                    if(side == WHITE){
                        if(whitePieces[pieceType][i] == INV) whitePieces[pieceType][i] = to;
                    }
                    else{
                        if(blackPieces[pieceType][i] == INV) blackPieces[pieceType][i] = to;
                    }
                    break;

                case 2: //PROMTION
                    if(side == WHITE){
                        if(whitePieces[PAWN][i] == INV) {
                            for(int j = 0; j < 10; i++){
                                if(whitePieces[pieceType][j] == from) {
                                    whitePieces[pieceType][j] = whitePieces[pieceType][i-1];
                                    whitePieces[pieceType][i-1] = INV;
                                    break;
                                }
                            }
                            for(int j = 0; j < 10; j++){
                                if(whitePieces[pieceType][i] == INV) whitePieces[pieceType][i] = to;
                            }
                        }
                    }
                    else{
                        if(blackPieces[PAWN][i] == INV) {
                            for(int j = 0; j < 10; i++){
                                if(blackPieces[pieceType][j] == from) {
                                    blackPieces[pieceType][j] = blackPieces[pieceType][i-1];
                                    blackPieces[pieceType][i-1] = INV;
                                    break;
                                }
                            }
                            for(int j = 0; j < 10; j++){
                                if(blackPieces[pieceType][i] == INV) blackPieces[pieceType][i] = to;
                            }
                        }
                    }
                    break;

                case 3: //Delete
                    if(side == WHITE){
                        if(whitePieces[pieceType][i] == INV){
                            for(int j = 0; j < 10; i++){
                                if(whitePieces[pieceType][j] == from) {
                                    whitePieces[pieceType][j] = whitePieces[pieceType][i-1];
                                    whitePieces[pieceType][i-1] = INV;
                                    break;
                                }
                            }
                        }
                    }
                    else{
                        if(blackPieces[pieceType][i] == INV){
                            for(int j = 0; j < 10; i++){
                                if(blackPieces[pieceType][j] == from) {
                                    blackPieces[pieceType][j] = blackPieces[pieceType][i-1];
                                    blackPieces[pieceType][i-1] = INV;
                                    break;
                                }
                            }
                        }
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
                castlingRights -= 0b1100;
                updatePieceList(WHITE, 0x07, 0x05, ROOK, 0);
            }
            else{
                pieces[0x77] = EMPTY; colors[0x77] = EMPTY;
                pieces[0x75] = ROOK; colors[0x75] = BLACK;
                castlingRights -= 0b00111;
                updatePieceList(BLACK, 0x77, 0x75, ROOK, 0);
            }
        }
        else if(flag(move) == QCASTLE){
            if(sideToMove == WHITE){
                pieces[0x00] = EMPTY; colors[0x00] = EMPTY;
                pieces[0x03] = ROOK; colors[0x03] = WHITE;
                castlingRights -= 0b1100;
                updatePieceList(WHITE, 0x00, 0x03, ROOK, 0);
            }
            else{
                pieces[0x70] = EMPTY; colors[0x70] = EMPTY;
                pieces[0x73] = ROOK; colors[0x73] = BLACK;
                castlingRights -= 0b00111;
                updatePieceList(BLACK, 0X70, 0x73, ROOK, 0);
            }
        }

        //Update all the "bonus" informations like castling rights, ply, fifty move rule, enPassant
        //Castling
        if(pieces[fromSq(move)] == KING) castlingRights -= sideToMove == WHITE ? 0b1100 : 0b0011;
        else if(pieces[fromSq(move)] == ROOK){
            if(sideToMove == WHITE){
                if(fromSq(move) == 0x00) castlingRights -= 0b0100;
                if(fromSq(move) == 0x07) castlingRights -= 0b1000;
            }
            else{
                if(fromSq(move) == 0x70) castlingRights -= 0b0001;
                if(fromSq(move) == 0x77) castlingRights -= 0b0010;
            }
        }
        else if(pieces[toSq(move)] == ROOK){
            if(sideToMove == BLACK){
                if(toSq(move) == 0x00) castlingRights -= 0b0100;
                if(toSq(move) == 0x07) castlingRights -= 0b1000;
            }
            else{
                if(toSq(move) == 0x70) castlingRights -= 0b0001;
                if(toSq(move) == 0x77) castlingRights -= 0b0010;
            }
        }
        //Ply/fifty
        ply++;
        if(pieces[toSq(move)] != EMPTY || pieces[fromSq(move)] == PAWN) fifty = 0;
        else fifty++;
        //En passant
        if(flag(move) == DPAWNPUSH) enPassant = sideToMove == WHITE ? toSq(move) - 16 : toSq(move) + 16;

        //Actual move making
        if((flag(move) & CAP) == CAP){
            if(flag(move) == EPCAP){
                if(sideToMove == WHITE){
                    pieces[toSq(move) - 16] = EMPTY; colors[toSq(move) - 16] = EMPTY;
                    updatePieceList(BLACK, toSq(move) - 16, INV, PAWN, 3);
                }
                else{
                    pieces[toSq(move) + 16] = EMPTY; colors[toSq(move) + 16] = EMPTY;
                    updatePieceList(WHITE, toSq(move) + 16, INV, PAWN, 3);
                }
            }
            else{
                updatePieceList(sideToMove^1, toSq(move), INV, pieces[toSq(move)], 3);
            }
        }


        pieces[toSq(move)] = pieces[fromSq(move)];
        colors[toSq(move)] = colors[fromSq(move)];
        pieces[fromSq(move)] = EMPTY; colors[fromSq(move)] = EMPTY;
        updatePieceList(sideToMove, fromSq(move), toSq(move), pieces[toSq(move)], 0);

        //Take care of promotion if necessary
        if((toSq(move) > 0x70 || toSq(move) < 0x08) && pieces[toSq(move)] == PAWN){
            switch(flag(move) & 0b1011){
                case KNIGHTPROM:
                    pieces[toSq(move)] = KNIGHT;
                    updatePieceList(sideToMove, toSq(move), toSq(move), KNIGHT, 2);
                    break;
                case BISHOPPROM:
                    pieces[toSq(move)] = BISHOP;
                    updatePieceList(sideToMove, toSq(move), toSq(move), BISHOP, 2);
                    break;
                case ROOKPROM:
                    pieces[toSq(move)] = ROOK;
                    updatePieceList(sideToMove, toSq(move), toSq(move), ROOK, 2);
                    break;
                case QUEENPROM:
                    pieces[toSq(move)] = QUEEN;
                    updatePieceList(sideToMove, toSq(move), toSq(move), QUEEN, 2);
                    break;
                default:
                    std::cout << "Error when promoting" << std::endl;
                    break;
            }
        }

        //We check if the king is in check, if it's the case we takeback the move
        if(isUnderAttack(sideToMove == WHITE ? whitePieces[5][0] : blackPieces[5][0], sideToMove^1)){
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

        //Then unmake the main move
        MOVEBITS move = movePlayed(history.top());
        BYTE pieceTaken = pieceToWakeFromTheDead(history.top());

        pieces[fromSq(move)] = pieces[toSq(move)];
        colors[fromSq(move)] = colors[toSq(move)];
        updatePieceList(sideToMove, toSq(move), fromSq(move), pieces[fromSq(move)], 0);
        
        if(flag(move) == EPCAP){
            if(sideToMove == WHITE){
                pieces[toSq(move) - 16] = pieceTaken;
                colors[toSq(move) - 16] = sideToMove^1;
                updatePieceList(BLACK, INV, toSq(move) - 16, PAWN, 1);
            }
            else{
                pieces[toSq(move) + 16] = pieceTaken;
                colors[toSq(move) + 16] = sideToMove^1;
                updatePieceList(WHITE, INV, toSq(move) + 16, PAWN, 1);
            }
        }
        else{
            pieces[toSq(move)] = pieceTaken;
            if(pieceTaken != EMPTY){
                colors[toSq(move)] = sideToMove^1;
                updatePieceList(sideToMove^1, INV, toSq(move), pieceTaken, 1);
            }
            else colors[toSq(move)] = EMPTY;
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

        //Promotion...
        if((flag(move) & 0b1000) == 0b1000){
            updatePieceList(sideToMove, fromSq(move), INV, pieces[fromSq(move)], 3);
            updatePieceList(sideToMove, INV, fromSq(move), PAWN, 1);
            pieces[fromSq(move)] = PAWN;
        }
    }

    long long perft(int depth)
    {
        long long nodes = 0;

        if (depth == 0)
            return 1ULL;

        //Generate and copy moves
        gen();
        MOVEBITS stack[256];
        int moves = 0;
        for(int i = 0; i < moveListIndx; i++){
            moves++;
            stack[i] = moveList[i];
        }

        for (int i = 0; i < moves; i++) {
            if(makeMove(stack[i])){
                nodes += perft(depth - 1);
                unmake();
            }
        }
        return nodes;
    }

    //Calls perft for each depth layer and tests whether the values are right or not with display
    void perftTest(){
        std::cout << "##### PERFT TIME YAY !!! #####" << std::endl << std::endl;
        for(int i = 0; i < 10; i++){
            std::cout << "Number of nodes for depth " << i << " : " << perft(i) << std::endl;
        }
    }

};

#endif //BAUB_CHESS_BOARD_H
