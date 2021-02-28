//
// Created by bitterboyy on 2/20/21.
//

#include "board_representation.h"

void board_representation::gen(){
    m_moveStackIndex = 0;
    for(int pieceType = 0; pieceType < 6; pieceType++){
        for(sq adress : m_pieces[m_sideToMove][pieceType]){
            if(pieceType == PAWN){
                //We need two different pieces of code depending on which side the pawn belongs to since they can only
                //move forward
                if(m_sideToMove == WHITE){
                    /*
                     * Pawns can either :
                     * - push one square if no piece is on target
                     * - push two squares if they stand on their original rank and no piece is obstructing
                     * - one square diagonnaly forward if an opposite piece sits there
                     */
                    if(m_piecesBoard[adress + N] == EMPTY){
                        if(m_piecesBoard[adress + (2*N)] == EMPTY && rank(adress) == 1){ //Double push is available
                            m_moveStack[m_moveStackIndex] = encodeMove(adress, sq(adress+(2*N)), DPAWNPUSH);
                            m_moveStackIndex++;
                        }
                        if(rank(adress) == 6){ //That would be a promotion
                            for(int i = 0; i < 4; i++){
                                m_moveStack[m_moveStackIndex] = encodeMove(adress, sq(adress+N), flag(NPROM+i));
                                m_moveStackIndex++;
                            }
                        }
                        else{
                            m_moveStack[m_moveStackIndex] = encodeMove(adress, sq(adress+N), QUIET);
                            m_moveStackIndex++;
                        }
                    }

                    if(m_colorBoard[adress + NW] == BLACK || adress + NW == m_ep){ //Capture to the north west
                        if(rank(adress) == 6){ //promo capture case
                            for(int i = 0; i < 4; i++){
                                m_moveStack[m_moveStackIndex] = encodeMove(adress, sq(adress+NW), flag(NPROMCAP+i));
                                m_moveStackIndex++;
                            }
                        }
                        else if(adress + NW == m_ep){
                            m_moveStack[m_moveStackIndex] = encodeMove(adress, sq(adress+NW), EPCAP);
                            m_moveStackIndex++;
                        }
                        else {
                            m_moveStack[m_moveStackIndex] = encodeMove(adress, sq(adress + NW), CAP);
                            m_moveStackIndex++;
                        }
                    }
                    if(m_colorBoard[adress + NE] == BLACK || adress + NE == m_ep){ //Capture to the north east
                        if(rank(adress) == 6){ //promo capture case
                            for(int i = 0; i < 4; i++){
                                m_moveStack[m_moveStackIndex] = encodeMove(adress, sq(adress+NE), flag(NPROMCAP+i));
                                m_moveStackIndex++;
                            }
                        }
                        else if(adress + NE == m_ep){
                            m_moveStack[m_moveStackIndex] = encodeMove(adress, sq(adress+NE), EPCAP);
                            m_moveStackIndex++;
                        }
                        else{
                            m_moveStack[m_moveStackIndex] = encodeMove(adress, sq(adress+NE), CAP);
                            m_moveStackIndex++;
                        }
                    }
                }
                else{
                    //Same thing as the above code but mirrored
                    if(m_piecesBoard[adress + S] == EMPTY){
                        if(m_piecesBoard[adress + (2*S)] == EMPTY && rank(adress) == 6){ //Double push is available
                            m_moveStack[m_moveStackIndex] = encodeMove(adress, sq(adress+(2*S)), DPAWNPUSH);
                            m_moveStackIndex++;
                        }
                        if(rank(adress) == 1){ //That would be a promotion
                            for(int i = 0; i < 4; i++){
                                m_moveStack[m_moveStackIndex] = encodeMove(adress, sq(adress+S), flag(NPROM+i));
                                m_moveStackIndex++;
                            }
                        }
                        else{
                            m_moveStack[m_moveStackIndex] = encodeMove(adress, sq(adress+S), QUIET);
                            m_moveStackIndex++;
                        }
                    }

                    if(m_colorBoard[adress + SW] == WHITE || adress + SW == m_ep){ //Capture to the north west
                        if(rank(adress) == 1){ //promo capture case
                            for(int i = 0; i < 4; i++){
                                m_moveStack[m_moveStackIndex] = encodeMove(adress, sq(adress+SW), flag(NPROMCAP+i));
                                m_moveStackIndex++;
                            }
                        }
                        else if(adress + SW == m_ep){
                            m_moveStack[m_moveStackIndex] = encodeMove(adress, sq(adress+SW), EPCAP);
                            m_moveStackIndex++;
                        }
                        else{
                            m_moveStack[m_moveStackIndex] = encodeMove(adress, sq(adress+SW), CAP);
                            m_moveStackIndex++;
                        }
                    }
                    if(m_colorBoard[adress + SE] == WHITE || adress + SE == m_ep){ //Capture to the north east
                        if(rank(adress) == 1){ //promo capture case
                            for(int i = 0; i < 4; i++){
                                m_moveStack[m_moveStackIndex] = encodeMove(adress, sq(adress+SE), flag(NPROMCAP+i));
                                m_moveStackIndex++;
                            }
                        }
                        else if(adress + SE == m_ep){
                            m_moveStack[m_moveStackIndex] = encodeMove(adress, sq(adress+SE), EPCAP);
                            m_moveStackIndex++;
                        }
                        else{
                            m_moveStack[m_moveStackIndex] = encodeMove(adress, sq(adress+SE), CAP);
                            m_moveStackIndex++;
                        }
                    }
                }
            }

            //Next we can deal with sliding piece generation
            else{
                //Sliding pieces use rays to check when they sould stop being able to move in a certain direction
                //to make that happen, we make one step in every given direction until we encounter an obstacle
                //If said obstacle is one of our own pieces or out of the board shenanigans, we can't make the last step
                //Otherwise, the last step is a capture
                for(auto stepDirection : m_pieceMoves[pieceType]){
                    if(stepDirection == 0) continue;

                    sq currentSquare = adress;
                    while(true){
                        currentSquare = sq(currentSquare + stepDirection);
                        if(m_colorBoard[currentSquare] == m_sideToMove
                            || (currentSquare & 0x88)) break;
                        else if(m_colorBoard[currentSquare] == !m_sideToMove){
                            m_moveStack[m_moveStackIndex] = encodeMove(adress, currentSquare, CAP);
                            m_moveStackIndex++;
                            break;
                        }
                        else{
                            m_moveStack[m_moveStackIndex] = encodeMove(adress, currentSquare, QUIET);
                            m_moveStackIndex++;
                            if(!m_pieceMoves[0][pieceType]) break;
                        }
                    }
                }
            }
        }
    }

    //Lastly, we can deal with castling
    /*
     * The rules here are as follow :
     * - King must not be in check
     * - The squares between king/rook should not be occupied OR attacked
     * - castling rights must allow the move
     */
    if(m_sideToMove){
        if(m_castlingRights & WKCASTLE
           && !m_piecesBoard[f1] && !m_piecesBoard[g1] && !m_piecesBoard[e1]
           && !sqAttacked(f1, BLACK) && !sqAttacked(g1, BLACK) && !sqAttacked(e1, BLACK)){
            m_moveStack[m_moveStackIndex] = encodeMove(e1, g1, KCASTLE);
            m_moveStackIndex++;
        }
        if(m_castlingRights & WQCASTLE
           && !m_piecesBoard[d1] && !m_piecesBoard[c1] && !m_piecesBoard[b1] && !m_piecesBoard[e1]
           && !sqAttacked(d1, BLACK) && !sqAttacked(c1, BLACK) && !sqAttacked(b1, BLACK) && !sqAttacked(e1, BLACK)){
            m_moveStack[m_moveStackIndex] = encodeMove(e1, c1, QCASTLE);
            m_moveStackIndex++;
        }
    }
    else {
        if (m_castlingRights & BKCASTLE
            && !m_piecesBoard[f8] && !m_piecesBoard[g8] && !m_piecesBoard[e8]
            && !sqAttacked(f8, WHITE) && !sqAttacked(g8, WHITE) && !sqAttacked(e8, WHITE)) {
            m_moveStack[m_moveStackIndex] = encodeMove(e8, g8, KCASTLE);
            m_moveStackIndex++;
        }
        if (m_castlingRights & BQCASTLE
            && !m_piecesBoard[d8] && !m_piecesBoard[c8] && !m_piecesBoard[b8] && !m_piecesBoard[e8]
            && !sqAttacked(d8, WHITE) && !sqAttacked(c8, WHITE) && !sqAttacked(b8, WHITE) && !sqAttacked(e8, WHITE)) {
            m_moveStack[m_moveStackIndex] = encodeMove(e8, c8, QCASTLE);
            m_moveStackIndex++;
        }
    }
}

bool board_representation::sqAttacked(int sq, bool side) {
    for(int pieceType = 0; pieceType < 6; pieceType++){
        for(auto& adress : m_pieces[side][pieceType]){
            if(pieceType == PAWN){
                //Just check if the given square is in the diagonal of any of the opponents pawns
                if(side && m_colorBoard[adress + NW] == sq || m_colorBoard[adress + NE] == sq) return true;
                if(!side && m_colorBoard[adress + SW] == sq || m_colorBoard[adress + SE] == sq) return true;
            }

            //Here we can optimize by noticing we don't actually care about directions which aren't going anywhere near
            //our given square.
            else{
                for(auto stepDirection : m_pieceMoves[pieceType]){
                    if(stepDirection == 0) break;
                    if((sq - adress < 0) == (stepDirection < 0) && (sq - adress)%stepDirection) continue;

                    int currentSquare  = adress;
                    while(true){
                        currentSquare += stepDirection;
                        if(m_colorBoard[currentSquare] != EMPTY || !m_pieceMoves[0][pieceType]) {
                            if(currentSquare == sq) return true;
                            break;
                        }
                        if(currentSquare & 0x88) break;
                        else if(currentSquare == sq) return true;
                    }
                }
            }
        }
    }
    return false;
}

bool board_representation::inCheck(bool side) {
    for(sq kingSquare : m_pieces[side][KING]){
        return sqAttacked(kingSquare, !side);
    }
    return false;
}

movebits board_representation::encodeMove(sq from, sq to, flag flag) {
    return (flag << 12) + (((to + (to & 7)) >> 1) << 6) + ((from + (from & 7)) >> 1);
}
sq board_representation::fromSq(movebits move) {
    return sq((move & 0x003F) + ((move & 0x003F) & ~7));
}
sq board_representation::toSq(movebits move){
    return sq(((move >> 6) & 0x003F) + (((move >> 6) & 0x003F) & ~7));
}
flag board_representation::getFlag(movebits move) {
    return flag((move >> 12) & 0x000F);
}

bool board_representation::make(movebits move) {
    //We need to store some info in the takeback stack to allow it
    takebackInfo info = {move, pieceType(m_piecesBoard[toSq(move)]), m_castlingRights, m_halfclock, m_ep};
    m_takebackInfo.push(info);

    //Store flag, from and to squares to avoid repeating the calculations
    sq from = fromSq(move), to = toSq(move);
    flag mvFlag = getFlag(move);

    //First we update state variables
    if(m_piecesBoard[from] == KING){
        m_castlingRights &= m_sideToMove ? 0b0011 : 0b1100;
    }
    if(m_piecesBoard[from] == ROOK){
        if(file(from) == 7){
            m_castlingRights &= m_sideToMove ? 0b0111 : 0b1101;
        }
        if(file(from) == 0){
            m_castlingRights &= m_sideToMove ? 0b1011 : 0b1110;
        }
    }

    if(!(mvFlag & CAP || m_piecesBoard[from] == PAWN)) m_halfclock++;
    else m_halfclock = 0;

    if(mvFlag == DPAWNPUSH) m_ep = sq(to + (m_sideToMove == WHITE ? S : N));
    else m_ep = a1;

    m_ply++;

    /*
     * Now we actually move pieces
     */
    //In case of a capture, we need to delete the captured piece in the oponent piece list
    if(mvFlag & CAP){
        if(mvFlag == EPCAP) m_pieces[!m_sideToMove][PAWN].remove(sq(to + (m_sideToMove == WHITE ? S : N)));
        else m_pieces[!m_sideToMove][m_piecesBoard[to]].remove(to);
    }
    m_piecesBoard[to] = m_piecesBoard[from];
    m_colorBoard[to] = m_colorBoard[from];
    //Update the piecelist
    m_pieces[m_sideToMove][m_piecesBoard[to]].remove(from);
    m_pieces[m_sideToMove][m_piecesBoard[to]].push_front(to);
    //Cleanup
    m_piecesBoard[from] = EMPTY;
    m_colorBoard[from] = EMPTY;

    //We can now take care of special flags, like castling and promotions
    if(mvFlag == KCASTLE) {
        sq rookAdress = sq(m_sideToMove ? 0x07 : 0x77);
        sq arrivalAdress = sq(to - 1);
        m_piecesBoard[rookAdress] = EMPTY;
        m_colorBoard[rookAdress] = EMPTY;
        m_piecesBoard[arrivalAdress] = ROOK;
        m_colorBoard[arrivalAdress] = m_sideToMove ? WHITE : BLACK;

        m_pieces[m_sideToMove][ROOK].remove(rookAdress);
        m_pieces[m_sideToMove][ROOK].push_front(arrivalAdress);
    }
    else if(mvFlag == QCASTLE){
        sq rookAdress = sq(m_sideToMove ? 0x00 : 0x70);
        sq arrivalAdress = sq(to + 1);
        m_piecesBoard[rookAdress] = EMPTY;
        m_colorBoard[rookAdress] = EMPTY;
        m_piecesBoard[arrivalAdress] = ROOK;
        m_colorBoard[arrivalAdress] = m_sideToMove ? WHITE : BLACK;

        m_pieces[m_sideToMove][ROOK].remove(rookAdress);
        m_pieces[m_sideToMove][ROOK].push_front(arrivalAdress);
    }

    //Remove the to square from the pawn pieceList, and add it to the target piece pieceList
    if(mvFlag & 0b1000){
        char targetPiece = QUEEN;
        switch(mvFlag & 0b1011){
            case NPROM:
                targetPiece = KNIGHT;
                break;
            case BPROM:
                targetPiece = BISHOP;
                break;
            case RPROM:
                targetPiece = ROOK;
                break;
            default:
                break;
        }
        m_pieces[m_sideToMove][targetPiece].push_front(to);
        m_pieces[m_sideToMove][PAWN].remove(to);
    }

    //Now that we're finished we can change the side to move, then check if the king is threatened
    m_sideToMove ^= 1;

    if(inCheck(!m_sideToMove)){
        takeback();
        return false;
    }

    return true;
}

void board_representation::takeback() {
    //We start off by setting back whatever comes naturally, that is ply and sideToMove since those always change the same way
    m_ply--;
    m_sideToMove ^= 1;

    /*
     * After that we'll need :
     * - The actual move
     * - The piece that was taken in case of capture
     */
    movebits move = m_takebackInfo.top().move;
    pieceType pieceTaken = m_takebackInfo.top().pieceTaken;

    //Undo any promotion that was done
    if(getFlag(move) & 0b1000){
        char targetPiece = QUEEN;
        switch(getFlag(move) & 0b1011){
            case NPROM:
                targetPiece = KNIGHT;
                break;
            case BPROM:
                targetPiece = BISHOP;
                break;
            case RPROM:
                targetPiece = ROOK;
                break;
            default:
                break;
        }
        m_pieces[m_sideToMove][targetPiece].remove(toSq(move));
        m_pieces[m_sideToMove][PAWN].push_front(toSq(move));
    }

    //Undo any castling move
    if(getFlag(move) == KCASTLE){
        sq rookAdress = sq(m_sideToMove ? 0x05 : 0x75);
        sq arrivalAdress = sq(m_sideToMove ? 0x07 : 0x77);
        m_piecesBoard[rookAdress] = EMPTY;
        m_colorBoard[rookAdress] = EMPTY;
        m_piecesBoard[arrivalAdress] = ROOK;
        m_colorBoard[arrivalAdress] = m_sideToMove ? WHITE : BLACK;

        m_pieces[m_sideToMove][ROOK].remove(rookAdress);
        m_pieces[m_sideToMove][ROOK].push_front(arrivalAdress);
    }
    else if(getFlag(move) == QCASTLE){
        sq rookAdress = sq(m_sideToMove ? 0x03 : 0x73);
        sq arrivalAdress = sq(m_sideToMove ? 0x00 : 0x70);
        m_piecesBoard[rookAdress] = EMPTY;
        m_colorBoard[rookAdress] = EMPTY;
        m_piecesBoard[arrivalAdress] = ROOK;
        m_colorBoard[arrivalAdress] = m_sideToMove ? WHITE : BLACK;

        m_pieces[m_sideToMove][ROOK].remove(rookAdress);
        m_pieces[m_sideToMove][ROOK].push_front(arrivalAdress);
    }

    //Undo the actual move
    m_piecesBoard[fromSq(move)] = m_piecesBoard[toSq(move)];
    m_colorBoard[fromSq(move)] = m_colorBoard[toSq(move)];

    //Place back the piece if capture, otherwise clean the square
    if(getFlag(move) & CAP){
        if(getFlag(move) == EPCAP) {
            m_pieces[!m_sideToMove][PAWN].push_front(sq(toSq(move) + (m_sideToMove ? S : N)));
            m_piecesBoard[sq(toSq(move) + (m_sideToMove ? S : N))] = PAWN;
            m_colorBoard[sq(toSq(move) + (m_sideToMove ? S : N))] = m_sideToMove ? BLACK : WHITE;
        }
        else {
            m_pieces[!m_sideToMove][m_piecesBoard[pieceTaken]].push_front(toSq(move));
            m_piecesBoard[toSq(move)] = pieceTaken;
            m_colorBoard[toSq(move)] = m_sideToMove ? BLACK : WHITE;
        }
    }

    //Update the piecelist
    m_pieces[m_sideToMove][m_piecesBoard[fromSq(move)]].remove(toSq(move));
    m_pieces[m_sideToMove][m_piecesBoard[fromSq(move)]].push_front(fromSq(move));

    m_piecesBoard[toSq(move)] = EMPTY;
    m_colorBoard[toSq(move)] = EMPTY;

    //We also must set back the castling rights and halfmove clock
    m_castlingRights = m_takebackInfo.top().castling;
    m_halfclock = m_takebackInfo.top().halfmove;
    m_ep = m_takebackInfo.top().ep;

    //finally we can just pop the top of the takeback stack
    m_takebackInfo.pop();
}


