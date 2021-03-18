//
// Created by bitterboyy on 2/20/21.
//

#include "position.h"

position::position() {
    //ZOBRIST KEYS INIT
    std::mt19937_64 mersenneTwister(time(nullptr));
    //Loop over every entry in the zobrist key arrays
    for(int side = WHITE; side < BLACK + 1; side++){
        for(int pieceType = PAWN; pieceType < EMPTY; pieceType++){
            for(int sq = 0; sq < 120; sq++){
                piecesKey[side][pieceType][sq] = mersenneTwister();
            }
        }
    }
    for(int i = 0; i < 16; i++){
        whiteCastlingKeys[i] = mersenneTwister();
        blackCastlingKeys[i] = mersenneTwister();
    }
    for(unsigned long long & epKey : epKeys){
        epKey = mersenneTwister();
    }

    sideKey = mersenneTwister();

    //POSITION HASH INIT
    //Loop all squares of the board
    for(int sq = 0; sq < 120; sq++){
        char pieceType = m_pieces[sq];
        char side = m_color[sq];
        if(sq & 0x88 || pieceType == EMPTY) continue;
        positionHash ^= piecesKey[side][pieceType][sq];
    }
    positionHash ^= whiteCastlingKeys[(m_castling >> 2)];
    positionHash ^= blackCastlingKeys[(m_castling & 0b0011)];

    if(m_ep != 0x88) positionHash ^= epKeys[m_ep];
    if(m_side) positionHash ^= sideKey;
}

void position::gen(movebits stack[], int &stackIndx) {
    //If we're in check, we muste use the checkEvasion move generator as it is specifically optimized for this
    if(check){
        checkEvasion(stack, stackIndx);
        return;
    }

    stackIndx = 0;
    int adress;
    int ranka;
    int availableDelta;
    for(int piece = 0; piece < 6; piece++){
        for(int index = 0; index < m_plist[m_side][piece].size(); index++){
            adress = m_plist[m_side][piece].get(index);
            availableDelta = isPinned(adress); //Checks if the current piece is pinned and if so, stores the delta it can move on
            ranka = rank(adress);
            if(piece == PAWN){
                /*
                * Pawns can either :
                * - push one square if no piece is on target
                * - push two squares if they stand on their original rank and no piece is obstructing
                * - one square diagonnaly forward if an opposite piece sits there
                */
                if(m_side == WHITE){
                    if(m_pieces[adress + N] == EMPTY && (!availableDelta || abs(availableDelta) == abs(N))){
                        if(m_pieces[adress + 2*N] == EMPTY && ranka == 1){ //Double pawn push
                            addToStack(stack, stackIndx, encodeMove(adress, adress + 2*N, DPAWNPUSH));
                        }
                        if(ranka == 6){ //Promotion
                            for(char i = 0; i < 4; i++){
                                addToStack(stack, stackIndx, encodeMove(adress, adress+N, (char)(NPROM+i)));
                            }
                        }
                        else{
                            addToStack(stack, stackIndx, encodeMove(adress, adress+N, QUIET));
                        }
                    }

                    if(!(adress + NW & 0x88) && (m_color[adress + NW] == !m_side || adress + NW == m_ep) && (!availableDelta || abs(availableDelta) == abs(NW))){
                        if(adress + NW == m_ep){
                            if(isLegalEp(adress, adress+NW, m_side)) addToStack(stack, stackIndx, encodeMove(adress, adress+NW, EPCAP));
                        }
                        if(ranka == 6){
                            for(char i = 0; i < 4; i++){
                                addToStack(stack, stackIndx, encodeMove(adress, adress+NW, (char)(NPROMCAP+i)));
                            }
                        }
                        else if(m_pieces[adress + NW] != EMPTY){
                            addToStack(stack, stackIndx, encodeMove(adress, adress + NW, CAP));
                        }
                    }

                    if(!(adress + NE & 0x88) && (m_color[adress + NE] == !m_side || adress + NE == m_ep) && (!availableDelta || abs(availableDelta) == abs(NE))){
                        if(adress + NE == m_ep){
                            if(isLegalEp(adress, adress+NE, m_side)) addToStack(stack, stackIndx, encodeMove(adress, adress+NE, EPCAP));
                        }
                        if(ranka == 6){
                            for(char i = 0; i < 4; i++){
                                addToStack(stack, stackIndx, encodeMove(adress, adress+NE, (char)(NPROMCAP+i)));
                            }
                        }
                        else if(m_pieces[adress + NE] != EMPTY){
                            addToStack(stack, stackIndx, encodeMove(adress, adress + NE, CAP));
                        }
                    }
                }
                else{
                    if(m_pieces[adress + S] == EMPTY && (!availableDelta || abs(availableDelta) == abs(S))){
                        if(m_pieces[adress + 2*S] == EMPTY && ranka == 6){ //Double pawn push
                            addToStack(stack, stackIndx, encodeMove(adress, adress + 2*S, DPAWNPUSH));
                        }
                        if(ranka == 1){ //Promotion
                            for(char i = 0; i < 4; i++){
                                addToStack(stack, stackIndx, encodeMove(adress, adress+S, (char)(NPROM+i)));
                            }
                        }
                        else{
                            addToStack(stack, stackIndx, encodeMove(adress, adress+S, QUIET));
                        }
                    }

                    if(!(adress + SW & 0x88) && (m_color[adress + SW] == !m_side || adress + SW == m_ep) && (!availableDelta || abs(availableDelta) == abs(SW))){
                        if(adress + SW == m_ep){
                            if(isLegalEp(adress, adress+SW, m_side)) addToStack(stack, stackIndx, encodeMove(adress, adress+SW, EPCAP));
                        }
                        if(ranka == 1){
                            for(char i = 0; i < 4; i++){
                                addToStack(stack, stackIndx, encodeMove(adress, adress+SW, (char)(NPROMCAP+i)));
                            }
                        }
                        else if(m_pieces[adress + SW] != EMPTY){
                            addToStack(stack, stackIndx, encodeMove(adress, adress + SW, CAP));
                        }
                    }

                    if(!(adress + SE & 0x88) && (m_color[adress + SE] == !m_side || adress + SE == m_ep) && (!availableDelta || abs(availableDelta) == abs(SE))){
                        if(adress + SE == m_ep){
                            if(isLegalEp(adress, adress+SE, m_side)) addToStack(stack, stackIndx, encodeMove(adress, adress+SE, EPCAP));
                        }
                        if(ranka == 1){
                            for(char i = 0; i < 4; i++){
                                addToStack(stack, stackIndx, encodeMove(adress, adress+SE, (char)(NPROMCAP+i)));
                            }
                        }
                        else if(m_pieces[adress + SE] != EMPTY){
                            addToStack(stack, stackIndx, encodeMove(adress, adress + SE, CAP));
                        }
                    }
                }
            }
            else if(piece == KING){
                //Kings are special since they cannot move in check.
                //To make sure we don't, let's check if the square we want to move to is attacked by the opposite side
                for(auto delta : m_pieceDelta[KING]){
                    if(!(adress + delta & 0x88) && m_color[adress + delta] != m_side && !sqAttackedMK2(adress + delta, !m_side, true)){
                        if(m_pieces[adress + delta] == EMPTY){
                            addToStack(stack, stackIndx, encodeMove(adress, adress + delta, QUIET));
                        }
                        else{
                            addToStack(stack, stackIndx, encodeMove(adress, adress + delta, CAP));
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
                int currentSquare;
                for(auto delta : m_pieceDelta[piece]){
                    if(delta == 0 || (availableDelta && abs(delta) != abs(availableDelta))) continue;
                    currentSquare = adress;
                    while(true){
                        currentSquare += delta;
                        if(m_color[currentSquare] == m_side
                           || (currentSquare & 0x88)) break;
                        else if(m_color[currentSquare] == !m_side && m_pieces[currentSquare] != EMPTY){
                            addToStack(stack, stackIndx, encodeMove(adress, currentSquare, CAP));
                            break;
                        }
                        else{
                            addToStack(stack, stackIndx, encodeMove(adress, currentSquare, QUIET));
                            if(!m_pieceDelta[0][piece]) break;
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
    if(!m_side){
        if(m_castling & WKCASTLE
           && m_pieces[0x05] == EMPTY && m_pieces[0x06] == EMPTY
           && !sqAttackedMK2(0x05, BLACK) && !sqAttackedMK2(0x06, BLACK)){
            addToStack(stack, stackIndx, encodeMove(0x04, 0x06, KCASTLE));
        }
        if(m_castling & WQCASTLE
           && m_pieces[0x03] == EMPTY && m_pieces[0x02] == EMPTY && m_pieces[0x01] == EMPTY
           && !sqAttackedMK2(0x03, BLACK) && !sqAttackedMK2(0x02, BLACK)){
            addToStack(stack, stackIndx, encodeMove(0x04, 0x02, QCASTLE));
        }
    }
    else {
        if (m_castling & BKCASTLE
            && m_pieces[0x75] == EMPTY && m_pieces[0x76] == EMPTY
            && !sqAttackedMK2(0x75, WHITE) && !sqAttackedMK2(0x76, WHITE)) {
            addToStack(stack, stackIndx, encodeMove(0x74, 0x76, KCASTLE));
        }
        if (m_castling & BQCASTLE
            && m_pieces[0x73] == EMPTY && m_pieces[0x72] == EMPTY && m_pieces[0x71] == EMPTY
            && !sqAttackedMK2(0x73, WHITE) && !sqAttackedMK2(0x72, WHITE)) {
            addToStack(stack, stackIndx, encodeMove(0x74, 0x72, QCASTLE));
        }
    }
}

void position::genNoisy(movebits stack[], int &stackIndx) {
    //We can use a neat tactic when generating checks, which is :
    //- treating the opposite king as a queen, and generate an array with 4 values:
    //- either the king can go there by moving diagonnaly (1)
    //- or by moving in a line (2)
    //- or by moving as a knight (3)
    //- or not at all (0)
    stackIndx = 0;
    int possibleChecksArray[0x88]{0};
    int enemyKing = m_plist[!m_side][KING].get(0);
    int currentSquare;
    int squareValue;
    int adress;
    int rankp;
    for(auto stepDirection : m_pieceDelta[QUEEN]){
        if(abs(stepDirection) == 7 || abs(stepDirection) == 9) squareValue = 1;
        else squareValue = 2;
        currentSquare = enemyKing; //Get the adress of the king
        while(true){
            currentSquare += stepDirection;
            if(m_color[currentSquare] == m_side
               || (currentSquare & 0x88) || m_pieces[currentSquare] != EMPTY) break;
            else{
                possibleChecksArray[currentSquare] = squareValue;
            }
        }
    }
    //Same for knight moves
    for(auto stepDirection : m_pieceDelta[KNIGHT]){
        if(stepDirection == 0) break;
        currentSquare = enemyKing; //Get the adress of the king
        currentSquare += stepDirection;
        if(m_color[currentSquare] == m_side
           || (currentSquare & 0x88) || m_pieces[currentSquare] != EMPTY) break;
        else{
            possibleChecksArray[currentSquare] = squareValue;
        }
    }

    //Now, we generate captures AND pawn pushes that would attack the enemy king,
    //as well as moves where a sliding piece/knight gets on a square that would put enemy king in check

    for(int piece = 0; piece < 6; piece++){
        for(int index = 0; index < m_plist[m_side][piece].size(); index++){
            adress = m_plist[m_side][piece].get(index);
            rankp = rank(adress);
            if(piece == PAWN){
                if(m_pieces[adress + (m_side ? S : N)] == EMPTY){
                    if(m_pieces[adress + (2*(m_side ? S : N))] == EMPTY && rankp == (m_side ? 6 : 1)){ //Double push is available
                        if(adress+(2*(m_side ? S : N))+NW == enemyKing || adress+(2*(m_side ? S : N))+NE == enemyKing){
                            addToStack(stack, stackIndx, encodeMove(adress, adress+(2*(m_side ? S : N)), DPAWNPUSH));
                        }
                    }
                    if(rankp == (m_side ? 1 : 6)){ //That would be a promotion
                        for(char i = 0; i < 4; i++){
                            addToStack(stack, stackIndx, encodeMove(adress, adress+(m_side ? S : N), (char)(NPROM+i)));
                        }
                    }
                    else{
                        if(adress+(m_side ? S : N)+NW == enemyKing || adress+(m_side ? S : N)+NE == enemyKing){
                            addToStack(stack, stackIndx, encodeMove(adress, adress+(m_side ? S : N), QUIET));
                        }
                    }
                }
                if(((m_color[adress + (m_side ? SW : NW)] == !m_side || adress + (m_side ? SW : NW) == m_ep)) && !(adress+(m_side ? SW : NW) & 0x88)){ //Capture to the north west
                    if(rankp == (m_side ? 1 : 6)){ //promo capture case
                        for(char i = 0; i < 4; i++){
                            addToStack(stack, stackIndx, encodeMove(adress, adress+(m_side ? SW : NW), (char)(NPROMCAP+i)));
                        }
                    }
                    else if(adress + (m_side ? SW : NW) == m_ep){
                        addToStack(stack, stackIndx, encodeMove(adress, adress+(m_side ? SW : NW), EPCAP));
                    }
                    else if(m_color[adress + (m_side ? SW : NW)] == !m_side) {
                        addToStack(stack, stackIndx, encodeMove(adress, adress + (m_side ? SW : NW), CAP));
                    }
                }
                if(((m_color[adress + (m_side ? SE : NE)] == !m_side || adress + (m_side ? SE : NE) == m_ep)) && !(adress+(m_side ? SE : NE) & 0x88)){ //Capture to the north east
                    if(rankp == (m_side ? 1 : 6)){ //promo capture case
                        for(char i = 0; i < 4; i++){
                            addToStack(stack, stackIndx, encodeMove(adress, adress+(m_side ? SE : NE), (char)(NPROMCAP+i)));
                        }
                    }
                    else if(adress + (m_side ? SE : NE) == m_ep){
                        addToStack(stack, stackIndx, encodeMove(adress, adress+(m_side ? SE : NE), EPCAP));
                    }
                    else if(m_color[adress + (m_side ? SE : NE)] == !m_side){
                        addToStack(stack, stackIndx, encodeMove(adress, adress+(m_side ? SE : NE), CAP));
                    }
                }
            }
                //Next we can deal with sliding piece generation
            else{
                for(auto stepDirection : m_pieceDelta[piece]){
                    if(stepDirection == 0) continue;
                    if(abs(stepDirection) == 7 || abs(stepDirection) == 9) squareValue = 1;
                    else if(piece == KNIGHT) squareValue = 3;
                    else if(piece == KING) squareValue = 4; //King can't check the other king
                    else squareValue = 2;
                    currentSquare = adress;
                    while(true){
                        currentSquare += stepDirection;
                        if(m_color[currentSquare] == m_side
                           || (currentSquare & 0x88)) break;
                        else if(m_color[currentSquare] == !m_side && m_pieces[currentSquare] != EMPTY){
                            addToStack(stack, stackIndx, encodeMove(adress, currentSquare, CAP));
                            break;
                        }
                        else{
                            //Case where the move would be a check
                            if(possibleChecksArray[currentSquare] == squareValue) addToStack(stack, stackIndx, encodeMove(adress, currentSquare, QUIET));
                            if(!m_pieceDelta[0][piece]) break;
                        }
                    }
                }
            }
        }
    }
}

void position::checkEvasion(movebits *stack, int &stackIndx) {
    //Generate king moves first
    int kingSquare = m_plist[m_side][KING].get(0);

    for(auto delta : m_pieceDelta[KING]){
        if(!(kingSquare + delta & 0x88) && m_color[kingSquare + delta] != m_side && !sqAttackedMK2(kingSquare + delta, !m_side, true)){
            if(m_pieces[kingSquare + delta] == EMPTY){
                addToStack(stack, stackIndx, encodeMove(kingSquare, kingSquare + delta, QUIET));
            }
            else{
                addToStack(stack, stackIndx, encodeMove(kingSquare, kingSquare + delta, CAP));
            }
        }
    }

    //We need to find what king of check we got ourselves into:
    //- a single check is when only one piece attacks the king, we can either take it, move the king out of the way,
    // or put a piece in between if it's a sliding piece
    //- a double check happens when two or more pieces are attacking the king, in this case we can skip
    // generating other moves as only king moves will be legal
    int attackerSq{inv};
    int attackerDelta{0};
    for(auto delta : m_pieceDelta[QUEEN]){
        //Go through all the possible delta as if the king was a sliding piece until we hit another piece
        for(int currSquare = kingSquare + delta; !(currSquare & 0x88); currSquare += delta){
            if(m_pieces[currSquare] != EMPTY){
                if(m_color[currSquare] == m_side) break; //This is one of our own pieces
                else{
                    int attack = m_attackArray[kingSquare - currSquare + 128];
                    switch (m_pieces[currSquare]) {
                        case PAWN:
                            if(!m_side == WHITE){
                                if(currSquare + NW == kingSquare || currSquare + NE == kingSquare){
                                    if(attackerSq == inv) attackerSq = currSquare;
                                    else return;
                                    attackerDelta = 0;
                                }
                            }
                            else{
                                if(currSquare + SW == kingSquare || currSquare + SE == kingSquare){
                                    if(attackerSq == inv) attackerSq = currSquare;
                                    else return;
                                    attackerDelta = 0;
                                }
                            }
                            break;
                        case BISHOP:
                            if(attack == attKQBbP || attack == attKQBwP || attack == attQB){
                                if(attackerSq == inv) attackerSq = currSquare;
                                else return;
                                attackerDelta = m_deltaArray[kingSquare - currSquare + 128];
                            }
                            break;
                        case ROOK:
                            if(attack == attKQR || attack == attQR){
                                if(attackerSq == inv) attackerSq = currSquare;
                                else return;
                                attackerDelta = m_deltaArray[kingSquare - currSquare + 128];
                            }
                            break;
                        case QUEEN:
                            if(attack != attNONE && attack != attN){
                                if(attackerSq == inv) attackerSq = currSquare;
                                else return;
                                attackerDelta = m_deltaArray[kingSquare - currSquare + 128];
                            }
                            break;
                        default: break;
                    }
                    break;
                }
            }
        }
    }
    //Now we do the same for knight moves
    for(auto delta : m_pieceDelta[KNIGHT]){
        //Go through all the possible delta as if the king was a sliding piece until we hit another piece
        int currSquare = kingSquare + delta;
        if(currSquare & 0x88) continue;
        if(m_pieces[currSquare] != EMPTY){
            if(m_color[currSquare] == m_side) continue; //This is one of our own pieces
            else{
                int attack = m_attackArray[kingSquare - currSquare + 128];
                if(attack == attN && m_pieces[currSquare] == KNIGHT) {
                    if(attackerSq == inv) attackerSq = currSquare;
                    else return;
                    attackerDelta = 0;
                }
            }
        }
    }

    //If we ever get there, we're in a single check situation, so we must generate moves that can cut the ray if a sliding
    //piece is attacking, or capture the attacker
    int adress;
    int availableDelta;
    int ranka;
    if(!m_pieceDelta[0][m_pieces[attackerSq]]){ //Non-sliding piece attacks
        //Just generate every move that can get our pieces to capture the attacker
        for(int pieceType = PAWN; pieceType < 6; pieceType++){
            if(pieceType == KING) continue;
            for(int index = 0; index < m_plist[m_side][pieceType].size(); index++){
                adress = m_plist[m_side][pieceType].get(index);
                availableDelta = isPinned(adress); //Checks if the current piece is pinned and if so, stores the delta it can move on
                ranka = rank(adress);

                if(pieceType == PAWN){ //Only pawn captures are of use to us here
                    if(m_side == WHITE){
                        if(!(adress + NW & 0x88) && (adress + NW == attackerSq || (adress + NW == m_ep && attackerSq == m_ep + S))
                        && (!availableDelta || abs(availableDelta) == abs(NW))){
                            if(adress + NW == m_ep && attackerSq == m_ep + S){
                                if(isLegalEp(adress, adress + NW, m_side)) addToStack(stack, stackIndx, encodeMove(adress, m_ep, EPCAP));
                            }
                            if(ranka == 6){
                                for(char i = 0; i < 4; i++){
                                    addToStack(stack, stackIndx, encodeMove(adress, attackerSq, (char)(NPROMCAP+i)));
                                }
                            }
                            else if(attackerSq == adress + NW){
                                addToStack(stack, stackIndx, encodeMove(adress, attackerSq, CAP));
                            }
                        }
                        if(!(adress + NE & 0x88) && (adress + NE == attackerSq || (adress + NE == m_ep && attackerSq == m_ep + S))
                           && (!availableDelta || abs(availableDelta) == abs(NE))){
                            if(adress + NE == m_ep && attackerSq == m_ep + S){
                                if(isLegalEp(adress, adress + NE, m_side)) addToStack(stack, stackIndx, encodeMove(adress, m_ep, EPCAP));
                            }
                            if(ranka == 6){
                                for(char i = 0; i < 4; i++){
                                    addToStack(stack, stackIndx, encodeMove(adress, attackerSq, (char)(NPROMCAP+i)));
                                }
                            }
                            else if(attackerSq == adress + NE){
                                addToStack(stack, stackIndx, encodeMove(adress, attackerSq, CAP));
                            }
                        }
                    }
                    else{
                        if(!(adress + SW & 0x88) && (adress + SW == attackerSq || (adress + SW == m_ep && attackerSq == m_ep + S))
                           && (!availableDelta || abs(availableDelta) == abs(SW))){
                            if(adress + SW == m_ep && attackerSq == m_ep + S){
                                if(isLegalEp(adress, adress + SW, m_side)) addToStack(stack, stackIndx, encodeMove(adress, m_ep, EPCAP));
                            }
                            if(ranka == 1){
                                for(char i = 0; i < 4; i++){
                                    addToStack(stack, stackIndx, encodeMove(adress, attackerSq, (char)(NPROMCAP+i)));
                                }
                            }
                            else if(attackerSq == adress + SW){
                                addToStack(stack, stackIndx, encodeMove(adress, attackerSq, CAP));
                            }
                        }
                        if(!(adress + SE & 0x88) && (adress + SE == attackerSq || (adress + SE == m_ep && attackerSq == m_ep + S))
                           && (!availableDelta || abs(availableDelta) == abs(SE))){
                            if(adress + SE == m_ep && attackerSq == m_ep + S){
                                if(isLegalEp(adress, adress + SE, m_side)) addToStack(stack, stackIndx, encodeMove(adress, m_ep, EPCAP));
                            }
                            if(ranka == 1){
                                for(char i = 0; i < 4; i++){
                                    addToStack(stack, stackIndx, encodeMove(adress, attackerSq, (char)(NPROMCAP+i)));
                                }
                            }
                            else if(attackerSq == adress + SE){
                                addToStack(stack, stackIndx, encodeMove(adress, attackerSq, CAP));
                            }
                        }
                    }
                }
                else{ //Now for the rest of the pieceTypes
                    int currentSquare;
                    for(auto delta : m_pieceDelta[pieceType]){
                        if(delta == 0 || (availableDelta && abs(delta) != abs(availableDelta))) continue;
                        currentSquare = adress;
                        while(true){
                            currentSquare += delta;
                            if(m_color[currentSquare] == m_side
                               || (currentSquare & 0x88)) break;
                            else if(currentSquare == attackerSq){
                                addToStack(stack, stackIndx, encodeMove(adress, currentSquare, CAP));
                                break;
                            }
                            else if(m_pieces[currentSquare] != EMPTY) break;
                            if(!m_pieceDelta[0][pieceType]) break;
                        }
                    }
                }
            }
        }
    }
    else{ //If the attacker is a sliding piece
        //First we generate every square in the attack ray
        int ray[8] = {inv, inv, inv, inv, inv, inv, inv, inv};
        int rayLength{0};
        for(int square = attackerSq; !(square & 0x88) && m_pieces[square] != KING; square += attackerDelta){
            ray[rayLength++] = square;
        }

        for(int pieceType = PAWN; pieceType < 6; pieceType++) {
            if(pieceType == KING) continue;
            for (int index = 0; index < m_plist[m_side][pieceType].size(); index++) {
                adress = m_plist[m_side][pieceType].get(index);
                availableDelta = isPinned(adress); //Checks if the current piece is pinned and if so, stores the delta it can move on
                ranka = rank(adress);

                if(pieceType == PAWN){ //This time, we're only interested in pushes and ep captures
                    if(m_side == WHITE){
                        if(m_pieces[adress + N] == EMPTY && (!availableDelta || abs(availableDelta) == abs(N))){
                            if(m_pieces[adress + 2*N] == EMPTY && ranka == 1){ //Double pawn push
                                for(auto i : ray){
                                    if(i == adress + 2*N) addToStack(stack, stackIndx, encodeMove(adress, adress + 2*N, DPAWNPUSH));
                                }
                            }
                            if(ranka == 6){ //Promotion
                                for(auto i : ray){
                                    if(i == adress + N){
                                        for(char j = 0; j < 4; j++){
                                            addToStack(stack, stackIndx, encodeMove(adress, adress+N, (char)(NPROM+i)));
                                        }
                                    }
                                }
                            }
                            else{
                                for(auto i : ray){
                                    if(i == adress + N) addToStack(stack, stackIndx, encodeMove(adress, adress + N, QUIET));
                                }
                            }
                        }

                        if(!(adress + NW & 0x88) && (adress + NW == attackerSq || adress + NW == m_ep) && (!availableDelta || abs(availableDelta) == abs(NW))){
                            if(adress + NW == m_ep){
                                for(auto i : ray) {
                                    if (i == m_ep) {
                                        if(isLegalEp(adress, adress+NW, m_side)) addToStack(stack, stackIndx, encodeMove(adress, adress+NW, EPCAP));
                                    }
                                }
                            }
                            if(ranka == 6){
                                if(attackerSq == adress + NW){
                                    for(char j = 0; j < 4; j++){
                                        addToStack(stack, stackIndx, encodeMove(adress, adress+NW, (char)(NPROMCAP+j)));
                                    }
                                }
                            }
                            else if(adress + NW == attackerSq){
                                addToStack(stack, stackIndx, encodeMove(adress, adress + NW, CAP));
                            }
                        }

                        if(!(adress + NE & 0x88) && (adress + NE == attackerSq || adress + NE == m_ep) && (!availableDelta || abs(availableDelta) == abs(NE))){
                            if(adress + NE == m_ep){
                                for(auto i : ray) {
                                    if (i == m_ep) {
                                        if(isLegalEp(adress, adress+NE, m_side)) addToStack(stack, stackIndx, encodeMove(adress, adress+NE, EPCAP));
                                    }
                                }
                            }
                            if(ranka == 6){
                                if(attackerSq == adress + NE){
                                    for(char j = 0; j < 4; j++){
                                        addToStack(stack, stackIndx, encodeMove(adress, adress+NE, (char)(NPROMCAP+j)));
                                    }
                                }
                            }
                            else if(adress + NE == attackerSq){
                                addToStack(stack, stackIndx, encodeMove(adress, adress + NE, CAP));
                            }
                        }
                    }
                    else{
                        if(m_pieces[adress + S] == EMPTY && (!availableDelta || abs(availableDelta) == abs(S))){
                            if(m_pieces[adress + 2*S] == EMPTY && ranka == 6){ //Double pawn push
                                for(auto i : ray){
                                    if(i == adress + 2*S) addToStack(stack, stackIndx, encodeMove(adress, adress + 2*S, DPAWNPUSH));
                                }
                            }
                            if(ranka == 1){ //Promotion
                                for(auto i : ray){
                                    if(i == adress + S){
                                        for(char j = 0; j < 4; j++){
                                            addToStack(stack, stackIndx, encodeMove(adress, adress+S, (char)(NPROM+i)));
                                        }
                                    }
                                }
                            }
                            else{
                                for(auto i : ray){
                                    if(i == adress + S) addToStack(stack, stackIndx, encodeMove(adress, adress + S, QUIET));
                                }
                            }
                        }

                        if(!(adress + SW & 0x88) && (adress + SW == attackerSq || adress + SW == m_ep) && (!availableDelta || abs(availableDelta) == abs(SW))){
                            if(adress + SW == m_ep){
                                for(auto i : ray) {
                                    if (i == m_ep) {
                                        if(isLegalEp(adress, adress+SW, m_side)) addToStack(stack, stackIndx, encodeMove(adress, adress+SW, EPCAP));
                                    }
                                }
                            }
                            if(ranka == 1){
                                if(attackerSq == adress + SW){
                                    for(char j = 0; j < 4; j++){
                                        addToStack(stack, stackIndx, encodeMove(adress, adress+SW, (char)(NPROMCAP+j)));
                                    }
                                }
                            }
                            else if(adress + SW == attackerSq){
                                addToStack(stack, stackIndx, encodeMove(adress, adress + SW, CAP));
                            }
                        }

                        if(!(adress + SE & 0x88) && (adress + SE == attackerSq || adress + SE == m_ep) && (!availableDelta || abs(availableDelta) == abs(SE))){
                            if(adress + SE == m_ep){
                                for(auto i : ray) {
                                    if (i == m_ep) {
                                        if(isLegalEp(adress, adress+SE, m_side)) addToStack(stack, stackIndx, encodeMove(adress, adress+SE, EPCAP));
                                    }
                                }
                            }
                            if(ranka == 1){
                                if(attackerSq == adress + SE){
                                    for(char j = 0; j < 4; j++){
                                        addToStack(stack, stackIndx, encodeMove(adress, adress+SE, (char)(NPROMCAP+j)));
                                    }
                                }
                            }
                            else if(adress + SE == attackerSq){
                                addToStack(stack, stackIndx, encodeMove(adress, adress + SE, CAP));
                            }
                        }
                    }
                }

                else{ //We'll do the same with sliding pieces
                    int currentSquare;
                    for(auto delta : m_pieceDelta[pieceType]){
                        if(delta == 0 || (availableDelta && abs(delta) != abs(availableDelta))) continue;
                        currentSquare = adress;
                        while(true){
                            currentSquare += delta;
                            if(m_color[currentSquare] == m_side
                               || (currentSquare & 0x88)) break;
                            else if(m_pieces[currentSquare] != EMPTY){
                                if(currentSquare == attackerSq) addToStack(stack, stackIndx, encodeMove(adress, currentSquare, CAP));
                                break;
                            }
                            else{
                                for(int i : ray){
                                    if(i == currentSquare) {
                                        addToStack(stack, stackIndx, encodeMove(adress, currentSquare, QUIET));
                                    }
                                }
                            }
                            if(!(m_pieceDelta[0][pieceType])) break;
                        }
                    }
                }
            }
        }
    }
}

int position::isPinned(int square) {
    //To find out if a piece is pinned, we need to check if the king can get to it if it were a sliding piece
    //If so, the piece might be indeed pinned, so we need to check, from this piece and using the same delta, if :
    //- we find another one of our pieces, then it isn't pinned
    //- we find a sliding enemy piece, then check if is has access to our delta

    //Finds the ray from our king to the given square
    int kingSquare = m_plist[m_color[square]][KING].get(0);
    int attack = m_attackArray[square - kingSquare + 128];
    int delta{0};
    int potentiallyPinned = false;
    if(attack != attNONE && attack != attN){ //Checks if the attack table says a queen could attack the square from our king's square
        delta = m_deltaArray[square - kingSquare + 128]; //Here's the delta of the attack
        for(int currSquare = kingSquare + delta; !(currSquare & 0x88); currSquare += delta){ //Follow it until we find our piece
            if(currSquare == square) {
                //Found our piece, and it can be accessed by the king
                //We now need to continue following the ray, until we find a sliding enemy piece or a friendly one
                potentiallyPinned = true;
            }
            else if(m_pieces[currSquare] != EMPTY) { //We found a piece that's not the one we want
                if(potentiallyPinned){ //If we hit a piece and we've already passed through our square of interest
                    if(m_color[currSquare] == m_side) return 0; //No pin!
                    else{ //We need to check if it can attack our piece
                        attack = m_attackArray[square - currSquare + 128];
                        switch (m_pieces[currSquare]) {
                            case BISHOP:
                                if(attack == attKQBbP || attack == attKQBwP || attack == attQB){
                                    return delta;
                                }
                                break;
                            case ROOK:
                                if(attack == attKQR || attack == attQR){
                                    return delta;
                                }
                                break;
                            case QUEEN:
                                if(attack != attNONE && attack != attN){
                                    return delta;
                                }
                                break;
                            default: return 0; //No pin if it's not one of those 3 pieces
                        }
                    }
                    return 0;
                }
                return 0; //No pin
            }
        }
    }
    return 0; //We haven't found a pin for the square we gave
}

bool position::isLegalEp(int from, int to, bool side){
    //We go from king square, in both horizontal directions, and if the first piece we encounter, ignoring
    //from and to+-N pawns, is a rook or a queen, return true
    int kingSquare = m_plist[side][KING].get(0);
    int currSquare;
    for(int delta = -1; delta <=1; delta+=2){ //Dumb, but working way to get -1 and 1 directions only
        for(currSquare = kingSquare + delta; !(currSquare & 0x88); currSquare += delta){
            if(currSquare == from || currSquare == to + (side ? N : S)) continue; //We ignore the pawns
            else if(m_pieces[currSquare] != EMPTY){
                return !((m_pieces[currSquare] == ROOK || m_pieces[currSquare] == QUEEN) && m_color[currSquare] == !side);
            }
        }
    }
    return true;
}

bool position::sqAttackedMK2(int square, bool side, bool kXray) {
    int adress;
    int listSize;
    int delta;
    int attack;
    for(int piece = 0; piece < 6; piece++){
        listSize = m_plist[side][piece].size();
        for(int index = 0; index < listSize; index++){
            adress = m_plist[side][piece].get(index);
            if(piece == PAWN){
                if(!(adress + (side ? SW : NW) & 0x88) && adress + (side ? SW : NW) == square) return true;
                if(!(adress + (side ? SE : NE) & 0x88) && adress + (side ? SE : NE) == square) return true;
            }
            else{
                attack = m_attackArray[square - adress + 128];
                switch (piece) {
                    case KNIGHT:
                        if(attack == attN) return true;
                        break;
                    case BISHOP:
                        if(attack == attKQBbP || attack == attKQBwP || attack == attQB){
                            delta = m_deltaArray[square - adress + 128];
                            for(int currSquare = adress + delta; !(currSquare & 0x88); currSquare += delta){
                                if(currSquare == square) return true;
                                if(m_pieces[currSquare] != EMPTY) {
                                    if(!(kXray && m_pieces[currSquare] == KING && m_color[currSquare] == !side)) break;
                                }
                            }
                        }
                        break;
                    case ROOK:
                        if(attack == attKQR || attack == attQR){
                            delta = m_deltaArray[square - adress + 128];
                            for(int currSquare = adress + delta; !(currSquare & 0x88); currSquare += delta){
                                if(currSquare == square) return true;
                                if(m_pieces[currSquare] != EMPTY) {
                                    if(!(kXray && m_pieces[currSquare] == KING && m_color[currSquare] == !side)) break;
                                }
                            }
                        }
                        break;
                    case QUEEN:
                        if(attack != attNONE && attack != attN){
                            delta = m_deltaArray[square - adress + 128];
                            for(int currSquare = adress + delta; !(currSquare & 0x88); currSquare += delta){
                                if(currSquare == square) return true;
                                if(m_pieces[currSquare] != EMPTY) {
                                    if(!(kXray && m_pieces[currSquare] == KING && m_color[currSquare] == !side)) break;
                                }
                            }
                        }
                        break;
                    case KING:
                        if(attack == attKQR || attack == attKQBwP || attack == attKQBbP) return true;
                        break;
                    default: break;
                }
            }
        }
    }
    return false;
}

bool position::inCheck(bool side) {
    return sqAttackedMK2(m_plist[side][KING].get(0), !side);
}

movebits position::encodeMove(int from, int to, char flag) {
    return (flag << 12) + (((to + (to & 7)) >> 1) << 6) + ((from + (from & 7)) >> 1);
}
int position::fromSq(movebits move) {
    return (move & 0x003F) + ((move & 0x003F) & ~7);
}
int position::toSq(movebits move){
    return ((move >> 6) & 0x003F) + (((move >> 6) & 0x003F) & ~7);
}
char position::getFlag(movebits move) {
    return (char)((move >> 12) & 0x000F);
}

void position::make(movebits move) {
    //Store flag, from and to squares to avoid repeating the calculations
    int from = fromSq(move), to = toSq(move);
    char mvFlag = getFlag(move);
    char pieceMoving = m_pieces[from];
    char pieceTaken = m_pieces[to];

    //We need to store some info in the takeback stack to allow it
    takebackInfo info;
    info.move = move;
    info.pieceTaken = pieceTaken;
    info.halfmove = m_halfclock;
    info.ep = m_ep;
    info.castling = m_castling;
    m_takebackInfo.push(info);

    //First we update state variables
    if(pieceMoving == KING){
        positionHash ^= m_side ? blackCastlingKeys[(m_castling & 0b0011)] : whiteCastlingKeys[m_castling >> 2];
        m_castling &= !m_side ? 0b0011 : 0b1100;
        positionHash ^= m_side ? blackCastlingKeys[(m_castling & 0b0011)] : whiteCastlingKeys[m_castling >> 2];
    }
    if(pieceMoving == ROOK){
        if(file(from) == 7){
            positionHash ^= m_side ? blackCastlingKeys[(m_castling & 0b0011)] : whiteCastlingKeys[m_castling >> 2];
            m_castling &= !m_side ? 0b0111 : 0b1101;
            positionHash ^= m_side ? blackCastlingKeys[(m_castling & 0b0011)] : whiteCastlingKeys[m_castling >> 2];
        }
        else if(file(from) == 0){
            positionHash ^= m_side ? blackCastlingKeys[(m_castling & 0b0011)] : whiteCastlingKeys[m_castling >> 2];
            m_castling &= !m_side ? 0b1011 : 0b1110;
            positionHash ^= m_side ? blackCastlingKeys[(m_castling & 0b0011)] : whiteCastlingKeys[m_castling >> 2];
        }
    }

    if(!(mvFlag & CAP || pieceMoving == PAWN)) m_halfclock++;
    else m_halfclock = 0;

    if(m_ep != inv) positionHash ^= epKeys[m_ep];
    if(mvFlag == DPAWNPUSH) {
        m_ep = to + (!m_side ? S : N);
        positionHash ^= epKeys[m_ep];
    }
    else m_ep = inv;

    m_ply++;

    /*
     * Now we actually move pieces
     */
    m_pieces[to] = pieceMoving;
    m_color[to] = (char)m_side;
    //Cleanup
    m_pieces[from] = EMPTY;
    m_color[from] = EMPTY;

    //Update zobrist hash
    positionHash ^= piecesKey[m_side][pieceMoving][from];
    positionHash ^= piecesKey[m_side][pieceMoving][to];

    //We also need to update the pieceLists
    m_plist[m_side][pieceMoving].remove(from);
    m_plist[m_side][pieceMoving].add(to);
    if(mvFlag & 0b0100){
        //We can now take care of special flags, like castling and promotions
        if(mvFlag == EPCAP){
            m_pieces[to + (m_side ? N : S)] = EMPTY;
            m_color[to + (m_side ? N : S)] = EMPTY;
            positionHash ^= piecesKey[!m_side][PAWN][to + (m_side ? N : S)];
            m_plist[!m_side][PAWN].remove(to + (m_side ? N : S));
        }
        else{
            positionHash ^= piecesKey[!m_side][pieceTaken][to];
            m_plist[!m_side][pieceTaken].remove(to);
        }
    }

    if(mvFlag == KCASTLE) {
        int rookAdress = !m_side ? 0x07 : 0x77;
        int arrivalAdress = to - 1;
        m_pieces[rookAdress] = EMPTY;
        m_color[rookAdress] = EMPTY;
        m_pieces[arrivalAdress] = ROOK;
        m_color[arrivalAdress] = (char)m_side;

        positionHash ^= piecesKey[m_side][ROOK][rookAdress];
        positionHash ^= piecesKey[m_side][ROOK][arrivalAdress];
        m_plist[m_side][ROOK].remove(rookAdress);
        m_plist[m_side][ROOK].add(arrivalAdress);
    }
    else if(mvFlag == QCASTLE){
        int rookAdress = !m_side ? 0x00 : 0x70;
        int arrivalAdress = to + 1;
        m_pieces[rookAdress] = EMPTY;
        m_color[rookAdress] = EMPTY;
        m_pieces[arrivalAdress] = ROOK;
        m_color[arrivalAdress] = (char)m_side;

        positionHash ^= piecesKey[m_side][ROOK][rookAdress];
        positionHash ^= piecesKey[m_side][ROOK][arrivalAdress];
        m_plist[m_side][ROOK].remove(rookAdress);
        m_plist[m_side][ROOK].add(arrivalAdress);
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

        m_pieces[to] = targetPiece;

        positionHash ^= piecesKey[m_side][PAWN][to];
        positionHash ^= piecesKey[m_side][targetPiece][to];

        m_plist[m_side][PAWN].remove(to);
        m_plist[m_side][targetPiece].add(to);
    }

    //Now that we're finished we can change the side to move
    m_side ^= 1;
    positionHash ^= sideKey;

    check = inCheck(m_side);
}

void position::takeback() {
    //We start off by setting back whatever comes naturally, that is ply and sideToMove since those always change the same way
    m_ply--;
    m_side ^= 1;
    positionHash ^= sideKey;

    /*
     * After that we'll need :
     * - The actual move
     * - The piece that was taken in case of capture
     */
    movebits move = m_takebackInfo.top().move;
    char pieceTaken = m_takebackInfo.top().pieceTaken;
    char mvFlag = getFlag(move);
    int from = fromSq(move);
    int to = toSq(move);
    char movedPiece = m_pieces[to];

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

        m_pieces[to] = PAWN;
        movedPiece = m_pieces[to];

        positionHash ^= piecesKey[m_side][targetPiece][to];
        positionHash ^= piecesKey[m_side][PAWN][to];

        m_plist[m_side][targetPiece].remove(to);
        m_plist[m_side][PAWN].add(to);
    }

    //Undo any castling move
    if(mvFlag == KCASTLE){
        int rookAdress = !m_side ? 0x05 : 0x75;
        int arrivalAdress = !m_side ? 0x07 : 0x77;
        m_pieces[rookAdress] = EMPTY;
        m_color[rookAdress] = EMPTY;
        m_pieces[arrivalAdress] = ROOK;
        m_color[arrivalAdress] = (char)m_side;

        positionHash ^= piecesKey[m_side][ROOK][rookAdress];
        positionHash ^= piecesKey[m_side][ROOK][arrivalAdress];
        m_plist[m_side][ROOK].remove(rookAdress);
        m_plist[m_side][ROOK].add(arrivalAdress);
    }
    else if(mvFlag == QCASTLE){
        int rookAdress = !m_side ? 0x03 : 0x73;
        int arrivalAdress = !m_side ? 0x00 : 0x70;
        m_pieces[rookAdress] = EMPTY;
        m_color[rookAdress] = EMPTY;
        m_pieces[arrivalAdress] = ROOK;
        m_color[arrivalAdress] = (char)m_side;

        positionHash ^= piecesKey[m_side][ROOK][rookAdress];
        positionHash ^= piecesKey[m_side][ROOK][arrivalAdress];
        m_plist[m_side][ROOK].remove(rookAdress);
        m_plist[m_side][ROOK].add(arrivalAdress);
    }

    //Update position hash BEFORE unmaking the move
    m_pieces[to] = EMPTY;
    m_color[to] = EMPTY;
    if(mvFlag & 0b0100){
        if(mvFlag == EPCAP){
            m_pieces[to + (m_side ? N : S)] = PAWN;
            m_color[to + (m_side ? N : S)] = (char)!m_side;
            positionHash ^= piecesKey[!m_side][PAWN][to + (m_side ? N : S)];
            m_plist[!m_side][PAWN].add(to + (m_side ? N : S));
        }
        else{
            m_pieces[to] = pieceTaken;
            m_color[to] = (char)!m_side;
            positionHash ^= piecesKey[!m_side][pieceTaken][to];
            m_plist[!m_side][pieceTaken].add(to);
        }
    }


    //Undo the actual move
    m_pieces[from] = movedPiece;
    m_color[from] = (char)m_side;

    m_plist[m_side][movedPiece].remove(to);
    m_plist[m_side][movedPiece].add(from);

    positionHash ^= piecesKey[m_side][movedPiece][to];
    positionHash ^= piecesKey[m_side][movedPiece][from];
    //We also must set back the castling rights and halfmove clock
    if(m_ep != inv) positionHash ^= epKeys[m_ep];
    m_ep = m_takebackInfo.top().ep;
    if(m_ep != inv) positionHash ^= epKeys[m_ep];

    positionHash ^= m_side ? blackCastlingKeys[(m_castling & 0b0011)] : whiteCastlingKeys[m_castling >> 2];
    m_castling = m_takebackInfo.top().castling;
    positionHash ^= m_side ? blackCastlingKeys[(m_castling & 0b0011)] : whiteCastlingKeys[m_castling >> 2];

    m_halfclock = m_takebackInfo.top().halfmove;

    //finally we can just pop the top of the takeback stack
    m_takebackInfo.pop();

    check = inCheck(m_side);
}

void position::setFEN(std::string fen) {
    //Changing the FEN string means resetting the move history to avoid bugs
    while(!m_takebackInfo.empty()){
        m_takebackInfo.pop();
    }

    //Split the FEN string into different sections
    std::string boardData[8]{"", "", "", "", "", "", "", ""};
    int boardDataIndex = 0;
    std::string variableData[5]{"", "", "", "", ""};
    bool variables = false;

    pieceList plist[2][6]{
            {pieceList({}), pieceList({}), pieceList({}), pieceList({}), pieceList({}), pieceList({})},
            {pieceList({}), pieceList({}), pieceList({}), pieceList({}), pieceList({}), pieceList({})}
    };

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
    for(int rank = 0; rank < 8; rank++){
        int file = 0;
        for(char current : boardData[rank]){
            int square = (7-rank)*0x10 + file;
            switch(current){
                case 'p':
                    m_pieces[square] = PAWN;
                    m_color[square] = BLACK;
                    plist[BLACK][PAWN].add(square);
                    break;
                case 'P':
                    m_pieces[square] = PAWN;
                    m_color[square] = WHITE;
                    plist[WHITE][PAWN].add(square);
                    break;
                case 'N':
                    m_pieces[square] = KNIGHT;
                    m_color[square] = WHITE;
                    plist[WHITE][KNIGHT].add(square);
                    break;
                case 'n':
                    m_pieces[square] = KNIGHT;
                    m_color[square] = BLACK;
                    plist[BLACK][KNIGHT].add(square);
                    break;
                case 'B':
                    m_pieces[square] = BISHOP;
                    m_color[square] = WHITE;
                    plist[WHITE][BISHOP].add(square);
                    break;
                case 'b':
                    m_pieces[square] = BISHOP;
                    m_color[square] = BLACK;
                    plist[BLACK][BISHOP].add(square);
                    break;
                case 'R':
                    m_pieces[square] = ROOK;
                    m_color[square] = WHITE;
                    plist[WHITE][ROOK].add(square);
                    break;
                case 'r':
                    m_pieces[square] = ROOK;
                    m_color[square] = BLACK;
                    plist[BLACK][ROOK].add(square);
                    break;
                case 'Q':
                    m_pieces[square] = QUEEN;
                    m_color[square] = WHITE;
                    plist[WHITE][QUEEN].add(square);
                    break;
                case 'q':
                    m_pieces[square] = QUEEN;
                    m_color[square] = BLACK;
                    plist[BLACK][QUEEN].add(square);
                    break;
                case 'K':
                    m_pieces[square] = KING;
                    m_color[square] = WHITE;
                    plist[WHITE][KING].add(square);
                    break;
                case 'k':
                    m_pieces[square] = KING;
                    m_color[square] = BLACK;
                    plist[BLACK][KING].add(square);
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
    //Copy the new pieceLists in place of the old ones
    for(int i = 0; i < 2; i++){
        for(int j = 0; j < 6; j++){
            m_plist[i][j] = plist[i][j];
        }
    }

    //Now we can set the options!
    m_side = variableData[0][0] != 'w';

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
        m_ep = variableData[2][0] - 'a' + variableData[2][1] - '0';
    }

    m_halfclock = std::stoi(variableData[3]);

    m_ply = std::stoi(variableData[4]);

    //Update the check variable
    check = inCheck(m_side);
}

void position::addToStack(movebits stack[], int &stackIndx, movebits move) {
    stack[stackIndx++] = move;
}

