//
// Created by bitterboyy on 2/20/21.
//

#include "board_representation.h"

void board_representation::gen(){
    m_moveStackIndex = 0;
    for(int pieceType = 0; pieceType < 6; pieceType++){
        for(const sq adress : m_pieces[m_sideToMove][pieceType]){
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
                    if(m_piecesBoard[adress + N] == EMPTY && !(adress+N & 0x88)){
                        if(m_piecesBoard[adress + (2*N)] == EMPTY && rank(adress) == 1){ //Double push is available
                            m_moveStack[m_moveStackIndex++] = encodeMove(adress, sq(adress+(2*N)), DPAWNPUSH);
                            
                        }
                        if(rank(adress) == 6){ //That would be a promotion
                            for(int i = 0; i < 4; i++){
                                m_moveStack[m_moveStackIndex++] = encodeMove(adress, sq(adress+N), flag(NPROM+i));
                                
                            }
                        }
                        else{
                            m_moveStack[m_moveStackIndex++] = encodeMove(adress, sq(adress+N), QUIET);
                            
                        }
                    }

                    if((m_colorBoard[adress + NW] == BLACK || adress + NW == m_ep) && !(adress+NW & 0x88)){ //Capture to the north west
                        if(rank(adress) == 6){ //promo capture case
                            for(int i = 0; i < 4; i++){
                                m_moveStack[m_moveStackIndex++] = encodeMove(adress, sq(adress+NW), flag(NPROMCAP+i));
                                
                            }
                        }
                        else if(adress + NW == m_ep){
                            m_moveStack[m_moveStackIndex++] = encodeMove(adress, sq(adress+NW), EPCAP);
                            
                        }
                        else if(m_colorBoard[adress + NW] == BLACK) {
                            m_moveStack[m_moveStackIndex++] = encodeMove(adress, sq(adress + NW), CAP);
                            
                        }
                    }
                    if((m_colorBoard[adress + NE] == BLACK || adress + NE == m_ep) && !(adress+NE & 0x88)){ //Capture to the north east
                        if(rank(adress) == 6){ //promo capture case
                            for(int i = 0; i < 4; i++){
                                m_moveStack[m_moveStackIndex++] = encodeMove(adress, sq(adress+NE), flag(NPROMCAP+i));
                                
                            }
                        }
                        else if(adress + NE == m_ep){
                            m_moveStack[m_moveStackIndex++] = encodeMove(adress, sq(adress+NE), EPCAP);
                            
                        }
                        else if(m_colorBoard[adress + NE] == BLACK){
                            m_moveStack[m_moveStackIndex++] = encodeMove(adress, sq(adress+NE), CAP);
                            
                        }
                    }
                }
                else{
                    //Same thing as the above code but mirrored
                    if(m_piecesBoard[adress + S] == EMPTY && !(adress+S & 0x88)){
                        if(m_piecesBoard[adress + (2*S)] == EMPTY && rank(adress) == 6){ //Double push is available
                            m_moveStack[m_moveStackIndex++] = encodeMove(adress, sq(adress+(2*S)), DPAWNPUSH);
                            
                        }
                        if(rank(adress) == 1){ //That would be a promotion
                            for(int i = 0; i < 4; i++){
                                m_moveStack[m_moveStackIndex++] = encodeMove(adress, sq(adress+S), flag(NPROM+i));
                                
                            }
                        }
                        else{
                            m_moveStack[m_moveStackIndex++] = encodeMove(adress, sq(adress+S), QUIET);
                            
                        }
                    }

                    if((m_colorBoard[adress + SW] == WHITE || adress + SW == m_ep) && !(adress+SW & 0x88)){ //Capture to the north west
                        if(rank(adress) == 1){ //promo capture case
                            for(int i = 0; i < 4; i++){
                                m_moveStack[m_moveStackIndex++] = encodeMove(adress, sq(adress+SW), flag(NPROMCAP+i));
                                
                            }
                        }
                        else if(adress + SW == m_ep){
                            m_moveStack[m_moveStackIndex++] = encodeMove(adress, sq(adress+SW), EPCAP);
                            
                        }
                        else if(m_colorBoard[adress + SW] == WHITE){
                            m_moveStack[m_moveStackIndex++] = encodeMove(adress, sq(adress+SW), CAP);
                            
                        }
                    }

                    if((m_colorBoard[adress + SE] == WHITE || adress + SE == m_ep) && !(adress+SE & 0x88)){ //Capture to the north east
                        if(rank(adress) == 1){ //promo capture case
                            for(int i = 0; i < 4; i++){
                                m_moveStack[m_moveStackIndex++] = encodeMove(adress, sq(adress+SE), flag(NPROMCAP+i));
                                
                            }
                        }
                        else if(adress + SE == m_ep){
                            m_moveStack[m_moveStackIndex++] = encodeMove(adress, sq(adress+SE), EPCAP);
                        }
                        else if(m_colorBoard[adress + SE] == WHITE){
                            m_moveStack[m_moveStackIndex++] = encodeMove(adress, sq(adress+SE), CAP);
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
                        else if(m_colorBoard[currentSquare] == !m_sideToMove && m_piecesBoard[currentSquare] != EMPTY){
                            m_moveStack[m_moveStackIndex++] = encodeMove(adress, currentSquare, CAP);
                            break;
                        }
                        else{
                            m_moveStack[m_moveStackIndex++] = encodeMove(adress, currentSquare, QUIET);
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
    if(m_sideToMove == WHITE){
        if(m_castlingRights & WKCASTLE
           && m_piecesBoard[f1] == EMPTY && m_piecesBoard[g1] == EMPTY
           && !sqAttacked(f1, BLACK) && !sqAttacked(g1, BLACK) && (m_check & 0b1001) != WHITE){
            m_moveStack[m_moveStackIndex++] = encodeMove(e1, g1, KCASTLE);
        }
        if(m_castlingRights & WQCASTLE
           && m_piecesBoard[d1] == EMPTY && m_piecesBoard[c1] == EMPTY && m_piecesBoard[b1] == EMPTY
           && !sqAttacked(d1, BLACK) && !sqAttacked(c1, BLACK) && !sqAttacked(b1, BLACK) && (m_check & 0b1001) != WHITE){
            m_moveStack[m_moveStackIndex++] = encodeMove(e1, c1, QCASTLE);
        }
    }
    else {
        if (m_castlingRights & BKCASTLE
            && m_piecesBoard[f8] == EMPTY && m_piecesBoard[g8] == EMPTY
            && !sqAttacked(f8, WHITE) && !sqAttacked(g8, WHITE) && (m_check & 0b1001) != BLACK) {
            m_moveStack[m_moveStackIndex++] = encodeMove(e8, g8, KCASTLE);
        }
        if (m_castlingRights & BQCASTLE
            && m_piecesBoard[d8] == EMPTY && m_piecesBoard[c8] == EMPTY && m_piecesBoard[b8] == EMPTY
            && !sqAttacked(d8, WHITE) && !sqAttacked(c8, WHITE) && !sqAttacked(b8, WHITE) && (m_check & 0b1001) != BLACK) {
            m_moveStack[m_moveStackIndex++] = encodeMove(e8, c8, QCASTLE);
        }
    }
}

bool board_representation::genCheckEscape() {
    m_moveStackIndex = 0;
    sq kingSquare = m_pieces[m_sideToMove][KING].front();

    //Stores every available king squares
    for(int direction : m_pieceMoves[KING]){
        if(!sqAttacked(kingSquare + direction, !m_sideToMove) && !((kingSquare + direction)&0x88)){
            if(m_piecesBoard[kingSquare + direction] == EMPTY){
                m_moveStack[m_moveStackIndex++] = encodeMove(kingSquare, sq(kingSquare + direction), QUIET);
            }
            else if(m_colorBoard[kingSquare + direction] == !m_sideToMove){
                m_moveStack[m_moveStackIndex++] = encodeMove(kingSquare, sq(kingSquare + direction), CAP);
            }
        }
    }

    //We actually don't need to generate any other type of move if the king is double checked, since only king moves can be legal.

    if(!(m_check & 0b0010)){
        //Generate moves that either capture an attacking piece or cut the ray between the king and the attacking piece
        sq attacks[256];
        int attacksIndex = 0;

        for(auto adress : m_checkingPieces){
            attacks[attacksIndex++] = adress;

            auto piece = pieceType(m_piecesBoard[adress]);
            //For each checking piece, we generate their ray of attack towards the king and put it in the attacks array
            if(piece == PAWN || piece == KNIGHT || piece == KING) continue; //Pawns/Knights/King don't generate rays, so we don't need to care about those

            for(auto stepDirection : m_pieceMoves[piece]){
                if(stepDirection == 0) continue;
                if((kingSquare - adress < 0) != (stepDirection < 0) || (kingSquare - adress)%stepDirection ||
                (rank(kingSquare) != rank(adress) && abs(stepDirection) == 1)) continue;

                sq currentSquare = adress;
                while(true){
                    currentSquare = sq(currentSquare + stepDirection);
                    if(m_colorBoard[currentSquare] == !m_sideToMove
                       || (currentSquare & 0x88)) break;
                    else if(m_colorBoard[currentSquare] == m_sideToMove){
                        attacks[attacksIndex++] = currentSquare;
                        break;
                    }
                    else{
                        attacks[attacksIndex++] = currentSquare;
                    }
                }
            }
        }

        //Now we just need to check which squares our pieces can get to
        for(int i = 0; i < attacksIndex; i++){
            sq square = attacks[i];

            for(int pieceType = 0; pieceType < 5; pieceType++){
                for(auto& adress : m_pieces[m_sideToMove][pieceType]){
                    if(pieceType == PAWN){
                        //Just check if the given square is in the diagonal of any of the opponents pawns
                        if(m_sideToMove == WHITE){
                            if(m_piecesBoard[adress + N] == EMPTY){
                                if(adress + N == square) m_moveStack[m_moveStackIndex++] = encodeMove(adress, square, QUIET);
                                else if(adress + 2*N == square && m_piecesBoard[square] == EMPTY && rank(adress) == 1){
                                    m_moveStack[m_moveStackIndex++] = encodeMove(adress, square, DPAWNPUSH);
                                }
                            }

                            else if((adress + NW == square && (m_colorBoard[square] == !m_sideToMove || adress + NW == m_ep)) ||
                                (adress + NE == square && (m_colorBoard[square] == !m_sideToMove || adress + NE == m_ep))){
                                    m_moveStack[m_moveStackIndex++] = encodeMove(adress, square, m_ep == square ? EPCAP : CAP);
                            }
                            //Case where the piece is a pawn and can be captured en passant
                            else if(square + N == m_ep && (adress + NW == m_ep || adress + NE == m_ep)) m_moveStack[m_moveStackIndex++] = encodeMove(adress, square, EPCAP);
                        }

                        else{
                            if(m_piecesBoard[adress + S] == EMPTY){
                                if(adress + S == square){
                                    m_moveStack[m_moveStackIndex++] = encodeMove(adress, square, QUIET);
                                }
                                else if(adress + 2*S == square && m_piecesBoard[square] == EMPTY && rank(adress) == 6) {
                                    m_moveStack[m_moveStackIndex++] = encodeMove(adress, square, DPAWNPUSH);
                                }
                            }

                            else if((adress + SW == square && (m_colorBoard[square] == !m_sideToMove || adress + SW == m_ep)) ||
                            (adress + SE == square && (m_colorBoard[square] == !m_sideToMove || adress + SE == m_ep))){
                                    m_moveStack[m_moveStackIndex++] = encodeMove(adress, square, m_ep == square ? EPCAP : CAP);
                            }
                            //Case where the piece is a pawn and can be captured en passant
                            else if(square + S == m_ep && (adress + SW == m_ep || adress + SE == m_ep)) m_moveStack[m_moveStackIndex++] = encodeMove(adress, square, EPCAP);
                        }
                    }

                    else{
                        for(auto stepDirection : m_pieceMoves[pieceType]){
                            if(stepDirection == 0) break;
                            //Here we can optimize by noticing we don't actually care about directions which aren't going anywhere near
                            //our given square.
                            if((square - adress < 0) != (stepDirection < 0) || (square - adress)%stepDirection) continue;

                            sq currentSquare = adress;
                            while(true){
                                currentSquare = sq(currentSquare + stepDirection);
                                if(currentSquare & 0x88 || m_colorBoard[currentSquare] == m_sideToMove) break;
                                if(m_colorBoard[currentSquare] == !m_sideToMove) {
                                    if(currentSquare == square) m_moveStack[m_moveStackIndex++] = encodeMove(adress, currentSquare, CAP);
                                    break;
                                }
                                else if(currentSquare == square){
                                    m_moveStack[m_moveStackIndex++] = encodeMove(adress, currentSquare, QUIET);
                                    break;
                                }
                                if(!m_pieceMoves[0][pieceType]) break;
                            }
                        }
                    }
                }
            }
        }
    }
    return m_moveStackIndex != 0;
}


bool board_representation::sqAttacked(int sq, bool side) {
    for(int pieceType = 0; pieceType < 6; pieceType++){
        for(auto& adress : m_pieces[side][pieceType]){
            if(pieceType == PAWN){
                //Just check if the given square is in the diagonal of any of the opponents pawns
                if(side == WHITE && (adress + NW == sq || adress + NE == sq)) return true;
                if(side == BLACK && (adress + SW == sq || adress + SE == sq)) return true;
            }

            //Here we can optimize by noticing we don't actually care about directions which aren't going anywhere near
            //our given square.
            else{
                for(auto stepDirection : m_pieceMoves[pieceType]){
                    if(stepDirection == 0) break;
                    if((sq - adress < 0) == (stepDirection < 0) && (sq - adress)%stepDirection) continue;

                    int currentSquare = adress;
                    while(true){
                        currentSquare += stepDirection;
                        if(currentSquare & 0x88) break;
                        if(m_colorBoard[currentSquare] != EMPTY || !m_pieceMoves[0][pieceType]) {
                            if(currentSquare == sq) return true;
                            break;
                        }
                        else if(currentSquare == sq) return true;
                    }
                }
            }
        }
    }
    return false;
}

std::vector<sq> board_representation::attackingPieces(sq square, bool side){
    std::vector<sq> attackingPieces = {};

    for(int pieceType = 0; pieceType < 6; pieceType++){
        for(auto& adress : m_pieces[side][pieceType]){
            if(pieceType == PAWN){
                //Just check if the given square is in the diagonal of any of the opponents pawns
                if(side == WHITE && (adress + NW == square || adress + NE == square)) attackingPieces.push_back(adress);
                if(side == BLACK && (adress + SW == square || adress + SE == square)) attackingPieces.push_back(adress);
            }

                //Here we can optimize by noticing we don't actually care about directions which aren't going anywhere near
                //our given square.
            else{
                for(auto stepDirection : m_pieceMoves[pieceType]){
                    if(stepDirection == 0) break;
                    if((square - adress < 0) == (stepDirection < 0) && (square - adress)%stepDirection) continue;

                    int currentSquare  = adress;
                    while(true){
                        currentSquare += stepDirection;
                        if(currentSquare & 0x88) break;
                        if(m_colorBoard[currentSquare] != EMPTY || !m_pieceMoves[0][pieceType]) {
                            if(currentSquare == square) attackingPieces.push_back(adress);
                            break;
                        }
                        else if(currentSquare == square){
                            attackingPieces.push_back(adress);
                            break;
                        }
                    }
                }
            }
        }
    }
    return attackingPieces;
}

void board_representation::updateCheck() {
    m_check = NONE;
    for(int side = 0; side < 2; side++){
        for(sq kingSquare : m_pieces[side][KING]){
            if(m_check == NONE){
                m_checkingPieces = attackingPieces(kingSquare, !side);
                if(m_checkingPieces.size() >= 2) m_check = checkType(DOUBLEW + side); //If two or more pieces are attacking the king, it's a double check
                else if(m_checkingPieces.size() == 1) m_check = checkType(SINGLEW + side); //Single check case
                else m_check = NONE; //No check at all
            }
        }
    }
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
    char pieceMoving = m_piecesBoard[from];
    char pieceTaken = m_piecesBoard[to];



    //First we update state variables
    if(pieceMoving == KING){
        m_castlingRights &= m_sideToMove == WHITE? 0b0011 : 0b1100;
    }
    if(pieceMoving == ROOK){
        if(file(from) == 7){
            m_castlingRights &= m_sideToMove == WHITE ? 0b0111 : 0b1101;
        }
        if(file(from) == 0){
            m_castlingRights &= m_sideToMove == WHITE ? 0b1011 : 0b1110;
        }
    }

    if(!(mvFlag & CAP || pieceMoving == PAWN)) m_halfclock++;
    else m_halfclock = 0;

    if(mvFlag == DPAWNPUSH) m_ep = sq(to + (m_sideToMove == WHITE ? S : N));
    else m_ep = inv;

    m_ply++;

    /*
     * Now we actually move pieces
     */
    //In case of a capture, we need to delete the captured piece in the oponent piece list
    if(mvFlag & CAP){
        if(mvFlag == EPCAP) {
            m_pieces[!m_sideToMove][PAWN].remove(sq(to + (m_sideToMove == WHITE ? S : N)));
        }
        else m_pieces[!m_sideToMove][pieceTaken].remove(to);
    }
    m_piecesBoard[to] = pieceMoving;
    m_colorBoard[to] = m_sideToMove == WHITE ? WHITE : BLACK;
    //Update the piecelist
    m_pieces[m_sideToMove][pieceMoving].remove(from);
    m_pieces[m_sideToMove][pieceMoving].push_front(to);
    //Cleanup
    m_piecesBoard[from] = EMPTY;
    m_colorBoard[from] = EMPTY;

    //We can now take care of special flags, like castling and promotions
    if(mvFlag == KCASTLE) {
        sq rookAdress = sq(m_sideToMove == WHITE ? 0x07 : 0x77);
        sq arrivalAdress = sq(to - 1);
        m_piecesBoard[rookAdress] = EMPTY;
        m_colorBoard[rookAdress] = EMPTY;
        m_piecesBoard[arrivalAdress] = ROOK;
        m_colorBoard[arrivalAdress] = m_sideToMove == WHITE ? WHITE : BLACK;

        m_pieces[m_sideToMove][ROOK].remove(rookAdress);
        m_pieces[m_sideToMove][ROOK].push_front(arrivalAdress);
    }
    else if(mvFlag == QCASTLE){
        sq rookAdress = sq(m_sideToMove == WHITE ? 0x00 : 0x70);
        sq arrivalAdress = sq(to + 1);
        m_piecesBoard[rookAdress] = EMPTY;
        m_colorBoard[rookAdress] = EMPTY;
        m_piecesBoard[arrivalAdress] = ROOK;
        m_colorBoard[arrivalAdress] = m_sideToMove == WHITE ? WHITE : BLACK;

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

    //Update the check flag
    updateCheck();

    if((m_check & 0b1001) == !m_sideToMove){
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
    flag mvFlag = getFlag(move);
    sq from = fromSq(move);
    sq to = toSq(move);

    //Undo any promotion that was done
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
        m_pieces[m_sideToMove][targetPiece].remove(to);
        m_pieces[m_sideToMove][PAWN].push_front(to);
    }

    //Undo any castling move
    if(mvFlag == KCASTLE){
        sq rookAdress = sq(m_sideToMove == WHITE ? 0x05 : 0x75);
        sq arrivalAdress = sq(m_sideToMove == WHITE ? 0x07 : 0x77);
        m_piecesBoard[rookAdress] = EMPTY;
        m_colorBoard[rookAdress] = EMPTY;
        m_piecesBoard[arrivalAdress] = ROOK;
        m_colorBoard[arrivalAdress] = m_sideToMove == WHITE ? WHITE : BLACK;

        m_pieces[m_sideToMove][ROOK].remove(rookAdress);
        m_pieces[m_sideToMove][ROOK].push_front(arrivalAdress);
    }
    else if(mvFlag == QCASTLE){
        sq rookAdress = sq(m_sideToMove == WHITE ? 0x03 : 0x73);
        sq arrivalAdress = sq(m_sideToMove == WHITE ? 0x00 : 0x70);
        m_piecesBoard[rookAdress] = EMPTY;
        m_colorBoard[rookAdress] = EMPTY;
        m_piecesBoard[arrivalAdress] = ROOK;
        m_colorBoard[arrivalAdress] = m_sideToMove == WHITE ? WHITE : BLACK;

        m_pieces[m_sideToMove][ROOK].remove(rookAdress);
        m_pieces[m_sideToMove][ROOK].push_front(arrivalAdress);
    }

    //Undo the actual move
    m_piecesBoard[from] = m_piecesBoard[to];
    m_colorBoard[from] = m_sideToMove;
    m_piecesBoard[to] = EMPTY;
    m_colorBoard[to] = EMPTY;

    //Place back the piece if capture, otherwise clean the square
    if(mvFlag & CAP){
        if(mvFlag == EPCAP) {
            m_pieces[!m_sideToMove][PAWN].push_front(sq(to + (m_sideToMove == WHITE ? S : N)));
            m_piecesBoard[sq(to + (m_sideToMove == WHITE ? S : N))] = PAWN;
            m_colorBoard[sq(to + (m_sideToMove == WHITE ? S : N))] = m_sideToMove == WHITE ? BLACK : WHITE;
        }
        else {
            m_pieces[!m_sideToMove][pieceTaken].push_front(to);
            m_piecesBoard[to] = pieceTaken;
            m_colorBoard[to] = m_sideToMove == WHITE ? BLACK : WHITE;
        }
    }

    //Update the piecelist
    m_pieces[m_sideToMove][m_piecesBoard[from]].remove(to);
    m_pieces[m_sideToMove][m_piecesBoard[from]].push_front(from);

    //We also must set back the castling rights and halfmove clock
    m_castlingRights = m_takebackInfo.top().castling;
    m_halfclock = m_takebackInfo.top().halfmove;
    m_ep = m_takebackInfo.top().ep;

    //Update the check status
    updateCheck();

    //finally we can just pop the top of the takeback stack
    m_takebackInfo.pop();
}

void board_representation::setFEN(std::string fen) {
    //Split the FEN string into different sections
    std::string boardData[8]{"", "", "", "", "", "", "", ""};
    int boardDataIndex = 0;
    std::string variableData[5]{"", "", "", "", ""};

    bool variables = false;

    for(int i = 0; i < fen.size() && boardDataIndex < 13; i++){
        while(fen[i] != '/' && fen[i] != ' ' && i < fen.size()){
            if(variables){
                variableData[boardDataIndex - 8] += fen[i];
            }
            else{
                boardData[boardDataIndex] += fen[i];
            }
            i++;
        }
        boardDataIndex++;

        if(fen[i] == ' ') {
            variables = true;
        }
    }

    //Now we can act on each section.
    //To each section in boardData corresponds a rank
    std::forward_list<sq> pieceList[2][6]{
            {{}, {}, {}, {}, {}, {}},
            {{}, {}, {}, {}, {}, {}}
    };
    int file = 0;
    for(int rank = 0; rank < 8; rank++){
        file = 0;
        for(char current : boardData[rank]){
            sq square = sq((7-rank)*0x10 + file);
            switch(current){
                case 'p':
                    m_piecesBoard[square] = PAWN;
                    m_colorBoard[square] = BLACK;
                    pieceList[BLACK][PAWN].push_front(square);
                    break;
                case 'P':
                    m_piecesBoard[square] = PAWN;
                    m_colorBoard[square] = WHITE;
                    pieceList[WHITE][PAWN].push_front(square);
                    break;
                case 'N':
                    m_piecesBoard[square] = KNIGHT;
                    m_colorBoard[square] = WHITE;
                    pieceList[WHITE][KNIGHT].push_front(square);
                    break;
                case 'n':
                    m_piecesBoard[square] = KNIGHT;
                    m_colorBoard[square] = BLACK;
                    pieceList[BLACK][KNIGHT].push_front(square);
                    break;
                case 'B':
                    m_piecesBoard[square] = BISHOP;
                    m_colorBoard[square] = WHITE;
                    pieceList[WHITE][BISHOP].push_front(square);
                    break;
                case 'b':
                    m_piecesBoard[square] = BISHOP;
                    m_colorBoard[square] = BLACK;
                    pieceList[BLACK][BISHOP].push_front(square);
                    break;
                case 'R':
                    m_piecesBoard[square] = ROOK;
                    m_colorBoard[square] = WHITE;
                    pieceList[WHITE][ROOK].push_front(square);
                    break;
                case 'r':
                    m_piecesBoard[square] = ROOK;
                    m_colorBoard[square] = BLACK;
                    pieceList[BLACK][ROOK].push_front(square);
                    break;
                case 'Q':
                    m_piecesBoard[square] = QUEEN;
                    m_colorBoard[square] = WHITE;
                    pieceList[WHITE][QUEEN].push_front(square);
                    break;
                case 'q':
                    m_piecesBoard[square] = QUEEN;
                    m_colorBoard[square] = BLACK;
                    pieceList[BLACK][QUEEN].push_front(square);
                    break;
                case 'K':
                    m_piecesBoard[square] = KING;
                    m_colorBoard[square] = WHITE;
                    pieceList[WHITE][KING].push_front(square);
                    break;
                case 'k':
                    m_piecesBoard[square] = KING;
                    m_colorBoard[square] = BLACK;
                    pieceList[BLACK][KING].push_front(square);
                    break;
                default:
                    for(int i = 0; i < current - '0'; i++){
                        m_colorBoard[square + i] = EMPTY;
                        m_piecesBoard[square + i] = EMPTY;
                        if(i != current - '1') file++;
                    }
                    break;
            }
            file++;
        }
    }
    for(int i = 0; i < 2; i++){
        for(int j = 0; j < 6; j++){
            m_pieces[i][j].swap(pieceList[i][j]);
        }
    }

    //Now we can set the options!
    m_sideToMove = variableData[0][0] == 'w' ? WHITE : BLACK;

    m_castlingRights = 0;
    for(char current : variableData[1]){
        switch(current){
            case '-':
                m_castlingRights = 0;
                break;
            case 'K':
                m_castlingRights += WKCASTLE;
                break;
            case 'Q':
                m_castlingRights += WQCASTLE;
                break;
            case 'k':
                m_castlingRights += BKCASTLE;
                break;
            case 'q':
                m_castlingRights +=  BQCASTLE;
                break;

            default: break;
        }
    }

    if(variableData[2][0] == '-') m_ep = inv;
    else{
        m_ep = sq(variableData[2][0] - 'a' + variableData[2][1] - '0');
    }

    m_halfclock = std::stoi(variableData[3]);

    m_ply = std::stoi(variableData[4]);

    updateCheck();
}


