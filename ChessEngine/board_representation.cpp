//
// Created by bitterboyy on 2/20/21.
//

#include "board_representation.h"

void board_representation::gen(){
    m_moveStackIndex = 0;
    for(int adress = 0; adress < 0x78; adress++){
        char pieceType = m_pieces[adress];
        if(adress & 0x88 || m_color[adress] != m_side) continue;
        if(pieceType == PAWN){
            //We need two different pieces of code depending on which side the pawn belongs to since they can only
            //move forward
            if(m_side == WHITE){
                /*
                * Pawns can either :
                * - push one square if no piece is on target
                * - push two squares if they stand on their original rank and no piece is obstructing
                * - one square diagonnaly forward if an opposite piece sits there
                */
                if(m_pieces[adress + N] == EMPTY){
                    if(m_pieces[adress + (2*N)] == EMPTY && rank(adress) == 1){ //Double push is available
                        m_moveStack[m_moveStackIndex++] = encodeMove(sq(adress), sq(adress+(2*N)), DPAWNPUSH);
                    }
                    if(rank(adress) == 6){ //That would be a promotion
                        for(int i = 0; i < 4; i++){
                            m_moveStack[m_moveStackIndex++] = encodeMove(sq(adress), sq(adress+N), flag(NPROM+i));
                        }
                    }
                    else{
                        m_moveStack[m_moveStackIndex++] = encodeMove(sq(adress), sq(adress+N), QUIET);
                    }
                }

                if((m_color[adress + NW] == BLACK || adress + NW == m_ep) && !(adress+NW & 0x88)){ //Capture to the north west
                    if(rank(adress) == 6){ //promo capture case
                        for(int i = 0; i < 4; i++){
                            m_moveStack[m_moveStackIndex++] = encodeMove(sq(adress), sq(adress+NW), flag(NPROMCAP+i));
                        }
                    }
                    else if(adress + NW == m_ep){
                        m_moveStack[m_moveStackIndex++] = encodeMove(sq(adress), sq(adress+NW), EPCAP);
                    }
                    else if(m_color[adress + NW] == BLACK) {
                        m_moveStack[m_moveStackIndex++] = encodeMove(sq(adress), sq(adress + NW), CAP);
                    }
                }
                if((m_color[adress + NE] == BLACK || adress + NE == m_ep) && !(adress+NE & 0x88)){ //Capture to the north east
                    if(rank(adress) == 6){ //promo capture case
                        for(int i = 0; i < 4; i++){
                            m_moveStack[m_moveStackIndex++] = encodeMove(sq(adress), sq(adress+NE), flag(NPROMCAP+i));
                        }
                    }
                    else if(adress + NE == m_ep){
                        m_moveStack[m_moveStackIndex++] = encodeMove(sq(adress), sq(adress+NE), EPCAP);
                    }
                    else if(m_color[adress + NE] == BLACK){
                        m_moveStack[m_moveStackIndex++] = encodeMove(sq(adress), sq(adress+NE), CAP);
                    }
                }
            }
            else{
                //Same thing as the above code but mirrored
                if(m_pieces[adress + S] == EMPTY && !(adress+S & 0x88)){
                    if(m_pieces[adress + (2*S)] == EMPTY && rank(adress) == 6){ //Double push is available
                        m_moveStack[m_moveStackIndex++] = encodeMove(sq(adress), sq(adress+(2*S)), DPAWNPUSH);
                    }
                    if(rank(adress) == 1){ //That would be a promotion
                        for(int i = 0; i < 4; i++){
                            m_moveStack[m_moveStackIndex++] = encodeMove(sq(adress), sq(adress+S), flag(NPROM+i));
                        }
                    }
                    else{
                        m_moveStack[m_moveStackIndex++] = encodeMove(sq(adress), sq(adress+S), QUIET);
                    }
                }

                if((m_color[adress + SW] == WHITE || adress + SW == m_ep) && !(adress+SW & 0x88)){ //Capture to the north west
                    if(rank(adress) == 1){ //promo capture case
                        for(int i = 0; i < 4; i++){
                            m_moveStack[m_moveStackIndex++] = encodeMove(sq(adress), sq(adress+SW), flag(NPROMCAP+i));
                        }
                    }
                    else if(adress + SW == m_ep){
                        m_moveStack[m_moveStackIndex++] = encodeMove(sq(adress), sq(adress+SW), EPCAP);
                    }
                    else if(m_color[adress + SW] == WHITE){
                        m_moveStack[m_moveStackIndex++] = encodeMove(sq(adress), sq(adress+SW), CAP);
                    }
                }

                if((m_color[adress + SE] == WHITE || adress + SE == m_ep) && !(adress+SE & 0x88)){ //Capture to the north east
                    if(rank(adress) == 1){ //promo capture case
                        for(int i = 0; i < 4; i++){
                            m_moveStack[m_moveStackIndex++] = encodeMove(sq(adress), sq(adress+SE), flag(NPROMCAP+i));
                                
                        }
                    }
                    else if(adress + SE == m_ep){
                        m_moveStack[m_moveStackIndex++] = encodeMove(sq(adress), sq(adress+SE), EPCAP);
                    }
                    else if(m_color[adress + SE] == WHITE){
                        m_moveStack[m_moveStackIndex++] = encodeMove(sq(adress), sq(adress+SE), CAP);
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
            for(auto stepDirection : m_directions[pieceType]){
                if(stepDirection == 0) continue;
                sq currentSquare = sq(adress);
                while(true){
                    currentSquare = sq(currentSquare + stepDirection);
                    if(m_color[currentSquare] == m_side
                    || (currentSquare & 0x88)) break;
                    else if(m_color[currentSquare] == !m_side && m_pieces[currentSquare] != EMPTY){
                        m_moveStack[m_moveStackIndex++] = encodeMove(sq(adress), currentSquare, CAP);
                        break;
                    }
                    else{
                        m_moveStack[m_moveStackIndex++] = encodeMove(sq(adress), currentSquare, QUIET);
                        if(!m_directions[0][pieceType]) break;
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
    if(m_side == WHITE){
        if(m_castling & WKCASTLE
           && m_pieces[f1] == EMPTY && m_pieces[g1] == EMPTY
           && !sqAttacked(f1, BLACK) && !sqAttacked(g1, BLACK) && !inCheck(WHITE)){
            m_moveStack[m_moveStackIndex++] = encodeMove(e1, g1, KCASTLE);
        }
        if(m_castling & WQCASTLE
           && m_pieces[d1] == EMPTY && m_pieces[c1] == EMPTY && m_pieces[b1] == EMPTY
           && !sqAttacked(d1, BLACK) && !sqAttacked(c1, BLACK) && !sqAttacked(b1, BLACK) && !inCheck(WHITE)){
            m_moveStack[m_moveStackIndex++] = encodeMove(e1, c1, QCASTLE);
        }
    }
    else {
        if (m_castling & BKCASTLE
            && m_pieces[f8] == EMPTY && m_pieces[g8] == EMPTY
            && !sqAttacked(f8, WHITE) && !sqAttacked(g8, WHITE) && !inCheck(BLACK)) {
            m_moveStack[m_moveStackIndex++] = encodeMove(e8, g8, KCASTLE);
        }
        if (m_castling & BQCASTLE
            && m_pieces[d8] == EMPTY && m_pieces[c8] == EMPTY && m_pieces[b8] == EMPTY
            && !sqAttacked(d8, WHITE) && !sqAttacked(c8, WHITE) && !sqAttacked(b8, WHITE) && !inCheck(BLACK)) {
            m_moveStack[m_moveStackIndex++] = encodeMove(e8, c8, QCASTLE);
        }
    }
}

bool board_representation::sqAttacked(int sq, bool side) {
    for(int adress = 0; adress < 0x78; adress++){
        char pieceType = m_pieces[adress];
        if(adress & 0x88 || m_color[adress] != side) continue;
            if(pieceType == PAWN){
                //Just check if the given square is in the diagonal of any of the opponents pawns
                if(side == WHITE && (adress + NW == sq || adress + NE == sq)) return true;
                if(side == BLACK && (adress + SW == sq || adress + SE == sq)) return true;
            }

            //Here we can optimize by noticing we don't actually care about directions which aren't going anywhere near
            //our given square.
            else{
                for(auto stepDirection : m_directions[pieceType]){
                    if(stepDirection == 0) break;
                    if((sq - adress < 0) == (stepDirection < 0) && (sq - adress)%stepDirection) continue;

                    int currentSquare = adress;
                    while(true){
                        currentSquare += stepDirection;
                        if(currentSquare & 0x88) break;
                        if(m_color[currentSquare] != EMPTY || !m_directions[0][pieceType]) {
                            if(currentSquare == sq) return true;
                            break;
                        }
                        else if(currentSquare == sq) return true;
                    }
                }
            }
    }
    return false;
}

bool board_representation::inCheck(bool side) {
    //Find the king
    sq kingSquare = inv;
    for(int i = 0; i < 0x78; i++){
        if(m_pieces[i] == KING && m_color[i] == side) kingSquare = sq(i);
    }

    return sqAttacked(kingSquare, !side);
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
    takebackInfo info = {move, pieceType(m_pieces[toSq(move)]), m_castling, m_halfclock, m_ep};
    m_takebackInfo.push(info);

    //Store flag, from and to squares to avoid repeating the calculations
    sq from = fromSq(move), to = toSq(move);
    flag mvFlag = getFlag(move);
    char pieceMoving = m_pieces[from];
    char pieceTaken = m_pieces[to];



    //First we update state variables
    if(pieceMoving == KING){
        m_castling &= m_side == WHITE? 0b0011 : 0b1100;
    }
    if(pieceMoving == ROOK){
        if(file(from) == 7){
            m_castling &= m_side == WHITE ? 0b0111 : 0b1101;
        }
        if(file(from) == 0){
            m_castling &= m_side == WHITE ? 0b1011 : 0b1110;
        }
    }

    if(!(mvFlag & CAP || pieceMoving == PAWN)) m_halfclock++;
    else m_halfclock = 0;

    if(mvFlag == DPAWNPUSH) m_ep = sq(to + (m_side == WHITE ? S : N));
    else m_ep = inv;

    m_ply++;

    /*
     * Now we actually move pieces
     */
    m_pieces[to] = pieceMoving;
    m_color[to] = m_side == WHITE ? WHITE : BLACK;
    //Cleanup
    m_pieces[from] = EMPTY;
    m_color[from] = EMPTY;

    //We can now take care of special flags, like castling and promotions
    if(mvFlag == EPCAP){
        m_pieces[to + (m_side ? N : S)] = EMPTY;
        m_color[to + (m_side ? N : S)] = EMPTY;
    }

    if(mvFlag == KCASTLE) {
        sq rookAdress = sq(m_side == WHITE ? 0x07 : 0x77);
        sq arrivalAdress = sq(to - 1);
        m_pieces[rookAdress] = EMPTY;
        m_color[rookAdress] = EMPTY;
        m_pieces[arrivalAdress] = ROOK;
        m_color[arrivalAdress] = m_side == WHITE ? WHITE : BLACK;
    }
    else if(mvFlag == QCASTLE){
        sq rookAdress = sq(m_side == WHITE ? 0x00 : 0x70);
        sq arrivalAdress = sq(to + 1);
        m_pieces[rookAdress] = EMPTY;
        m_color[rookAdress] = EMPTY;
        m_pieces[arrivalAdress] = ROOK;
        m_color[arrivalAdress] = m_side == WHITE ? WHITE : BLACK;
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
    }

    //Now that we're finished we can change the side to move, then check if the king is threatened
    m_side ^= 1;

    if(inCheck(!m_side)){
        takeback();
        return false;
    }

    return true;
}

void board_representation::takeback() {
    //We start off by setting back whatever comes naturally, that is ply and sideToMove since those always change the same way
    m_ply--;
    m_side ^= 1;

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
    }

    //Undo any castling move
    if(mvFlag == KCASTLE){
        sq rookAdress = sq(m_side == WHITE ? 0x05 : 0x75);
        sq arrivalAdress = sq(m_side == WHITE ? 0x07 : 0x77);
        m_pieces[rookAdress] = EMPTY;
        m_color[rookAdress] = EMPTY;
        m_pieces[arrivalAdress] = ROOK;
        m_color[arrivalAdress] = m_side == WHITE ? WHITE : BLACK;
    }
    else if(mvFlag == QCASTLE){
        sq rookAdress = sq(m_side == WHITE ? 0x03 : 0x73);
        sq arrivalAdress = sq(m_side == WHITE ? 0x00 : 0x70);
        m_pieces[rookAdress] = EMPTY;
        m_color[rookAdress] = EMPTY;
        m_pieces[arrivalAdress] = ROOK;
        m_color[arrivalAdress] = m_side == WHITE ? WHITE : BLACK;
    }

    //Undo the actual move
    m_pieces[from] = m_pieces[to];
    m_color[from] = m_side == WHITE ? WHITE : BLACK;
    m_pieces[to] = EMPTY;
    m_color[to] = EMPTY;

    //Place back the piece if capture, otherwise clean the square
    if(mvFlag & CAP){
        if(mvFlag == EPCAP) {
            m_pieces[sq(to + (m_side == WHITE ? S : N))] = PAWN;
            m_color[sq(to + (m_side == WHITE ? S : N))] = m_side == WHITE ? BLACK : WHITE;
        }
        else {
            m_pieces[to] = pieceTaken;
            m_color[to] = m_side == WHITE ? BLACK : WHITE;
        }
    }

    //We also must set back the castling rights and halfmove clock
    m_castling = m_takebackInfo.top().castling;
    m_halfclock = m_takebackInfo.top().halfmove;
    m_ep = m_takebackInfo.top().ep;

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
    int file = 0;
    for(int rank = 0; rank < 8; rank++){
        file = 0;
        for(char current : boardData[rank]){
            sq square = sq((7-rank)*0x10 + file);
            switch(current){
                case 'p':
                    m_pieces[square] = PAWN;
                    m_color[square] = BLACK;
                    break;
                case 'P':
                    m_pieces[square] = PAWN;
                    m_color[square] = WHITE;
                    break;
                case 'N':
                    m_pieces[square] = KNIGHT;
                    m_color[square] = WHITE;
                    break;
                case 'n':
                    m_pieces[square] = KNIGHT;
                    m_color[square] = BLACK;
                    break;
                case 'B':
                    m_pieces[square] = BISHOP;
                    m_color[square] = WHITE;
                    break;
                case 'b':
                    m_pieces[square] = BISHOP;
                    m_color[square] = BLACK;
                    break;
                case 'R':
                    m_pieces[square] = ROOK;
                    m_color[square] = WHITE;
                    break;
                case 'r':
                    m_pieces[square] = ROOK;
                    m_color[square] = BLACK;
                    break;
                case 'Q':
                    m_pieces[square] = QUEEN;
                    m_color[square] = WHITE;
                    break;
                case 'q':
                    m_pieces[square] = QUEEN;
                    m_color[square] = BLACK;
                    break;
                case 'K':
                    m_pieces[square] = KING;
                    m_color[square] = WHITE;
                    break;
                case 'k':
                    m_pieces[square] = KING;
                    m_color[square] = BLACK;
                    break;
                default:
                    for(int i = 0; i < current - '0'; i++){
                        m_color[square + i] = EMPTY;
                        m_pieces[square + i] = EMPTY;
                        if(i != current - '1') file++;
                    }
                    break;
            }
            file++;
        }
    }

    //Now we can set the options!
    m_side = variableData[0][0] == 'w' ? WHITE : BLACK;

    m_castling = 0;
    for(char current : variableData[1]){
        switch(current){
            case '-':
                m_castling = 0;
                break;
            case 'K':
                m_castling += WKCASTLE;
                break;
            case 'Q':
                m_castling += WQCASTLE;
                break;
            case 'k':
                m_castling += BKCASTLE;
                break;
            case 'q':
                m_castling +=  BQCASTLE;
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
}


