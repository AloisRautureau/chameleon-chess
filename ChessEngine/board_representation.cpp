//
// Created by bitterboyy on 2/20/21.
//

#include "board_representation.h"

board_representation::board_representation() {
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

void board_representation::gen(movebits stack[], int &stackIndx){
    m_moveStackIndex = 0;
    int adress;
    int ranka;
    for(int piece = 0; piece < 6; piece++){
        for(int index = 0; index < m_plist[m_side][piece].size(); index++){
            adress = m_plist[m_side][piece].get(index);
            ranka = rank(adress);
            if(piece == PAWN){
                /*
                * Pawns can either :
                * - push one square if no piece is on target
                * - push two squares if they stand on their original rank and no piece is obstructing
                * - one square diagonnaly forward if an opposite piece sits there
                */
                if(m_pieces[adress + (m_side ? S : N)] == EMPTY){
                    if(m_pieces[adress + (2*(m_side ? S : N))] == EMPTY && ranka == (m_side ? 6 : 1)){ //Double push is available
                        addToStack(stack, stackIndx, encodeMove(adress, adress+(2*(m_side ? S : N)), DPAWNPUSH));
                    }
                    if(ranka == (m_side ? 1 : 6)){ //That would be a promotion
                        for(char i = 0; i < 4; i++){
                            addToStack(stack, stackIndx, encodeMove(adress, adress+(m_side ? S : N), NPROM+i));
                        }
                    }
                    else{
                        addToStack(stack, stackIndx, encodeMove(adress, adress+(m_side ? S : N), QUIET));
                    }
                }
                if((m_color[adress + (m_side ? SW : NW)] == !m_side || adress + (m_side ? SW : NW) == m_ep) && !(adress+(m_side ? SW : NW) & 0x88)){ //Capture to the north west
                    if(ranka == (m_side ? 1 : 6)){ //promo capture case
                        for(char i = 0; i < 4; i++){
                            addToStack(stack, stackIndx, encodeMove(adress, adress+(m_side ? SW : NW), NPROMCAP+i));
                        }
                    }
                    else if(adress + (m_side ? SW : NW) == m_ep){
                        addToStack(stack, stackIndx, encodeMove(adress, adress+(m_side ? SW : NW), EPCAP));
                    }
                    else if(m_color[adress + (m_side ? SW : NW)] == !m_side) {
                        addToStack(stack, stackIndx, encodeMove(adress, adress + (m_side ? SW : NW), CAP));
                    }
                }
                if((m_color[adress + (m_side ? SE : NE)] == !m_side || adress + (m_side ? SE : NE) == m_ep) && !(adress+(m_side ? SE : NE) & 0x88)){ //Capture to the north east
                    if(ranka == (m_side ? 1 : 6)){ //promo capture case
                        for(char i = 0; i < 4; i++){
                            addToStack(stack, stackIndx, encodeMove(adress, adress+(m_side ? SE : NE), NPROMCAP+i));
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
                //Sliding pieces use rays to check when they sould stop being able to move in a certain direction
                //to make that happen, we make one step in every given direction until we encounter an obstacle
                //If said obstacle is one of our own pieces or out of the board shenanigans, we can't make the last step
                //Otherwise, the last step is a capture
                int currentSquare;
                for(auto stepDirection : m_directions[piece]){
                    if(stepDirection == 0) continue;
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
                            addToStack(stack, stackIndx, encodeMove(adress, currentSquare, QUIET));
                            if(!m_directions[0][piece]) break;
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
           && !sqAttackedMK2(0x05, BLACK) && !sqAttackedMK2(0x06, BLACK) && !inCheck(WHITE)){
            addToStack(stack, stackIndx, encodeMove(0x04, 0x06, KCASTLE));
        }
        if(m_castling & WQCASTLE
           && m_pieces[0x03] == EMPTY && m_pieces[0x02] == EMPTY && m_pieces[0x01] == EMPTY
           && !sqAttackedMK2(0x03, BLACK) && !sqAttackedMK2(0x02, BLACK) && !sqAttackedMK2(0x01, BLACK) && !inCheck(WHITE)){
            addToStack(stack, stackIndx, encodeMove(0x04, 0x02, QCASTLE));
        }
    }
    else {
        if (m_castling & BKCASTLE
            && m_pieces[0x75] == EMPTY && m_pieces[0x76] == EMPTY
            && !sqAttackedMK2(0x75, WHITE) && !sqAttackedMK2(0x76, WHITE) && !inCheck(BLACK)) {
            addToStack(stack, stackIndx, encodeMove(0x74, 0x76, KCASTLE));
        }
        if (m_castling & BQCASTLE
            && m_pieces[0x73] == EMPTY && m_pieces[0x72] == EMPTY && m_pieces[0x71] == EMPTY
            && !sqAttackedMK2(0x73, WHITE) && !sqAttackedMK2(0x72, WHITE) && !sqAttackedMK2(0x71, WHITE) && !inCheck(BLACK)) {
            addToStack(stack, stackIndx, encodeMove(0x74, 0x72, QCASTLE));
        }
    }
}

void board_representation::genNoisy(movebits stack[], int &stackIndx) {
    //We can use a neat tactic when generating checks, which is :
    //- treating the opposite king as a queen, and generate an array with 4 values:
    //- either the king can go there by moving diagonnaly (1)
    //- or by moving in a line (2)
    //- or by moving as a knight (3)
    //- or not at all (0)
    m_moveStackIndex = 0;
    int possibleChecksArray[0x88]{0};
    int enemyKing = m_plist[!m_side][KING].get(0);
    int currentSquare;
    int squareValue;
    int adress;
    int rankp;
    for(auto stepDirection : m_directions[QUEEN]){
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
    for(auto stepDirection : m_directions[KNIGHT]){
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
                            addToStack(stack, stackIndx, encodeMove(adress, adress+(m_side ? S : N), NPROM+i));
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
                            addToStack(stack, stackIndx, encodeMove(adress, adress+(m_side ? SW : NW), NPROMCAP+i));
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
                            addToStack(stack, stackIndx, encodeMove(adress, adress+(m_side ? SE : NE), NPROMCAP+i));
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
                for(auto stepDirection : m_directions[piece]){
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
                            if(!m_directions[0][piece]) break;
                        }
                    }
                }
            }
        }
    }
}

void board_representation::genCheckEvasion(movebits *stack, int &stackIndx) {
}


bool board_representation::sqAttackedMK2(int square, bool side) {
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
                                if(m_pieces[currSquare] != EMPTY) break;
                            }
                        }
                        break;
                    case ROOK:
                        if(attack == attKQR || attack == attQR){
                            delta = m_deltaArray[square - adress + 128];
                            for(int currSquare = adress + delta; !(currSquare & 0x88); currSquare += delta){
                                if(currSquare == square) return true;
                                if(m_pieces[currSquare] != EMPTY) break;
                            }
                        }
                        break;
                    case QUEEN:
                        if(attack != attNONE && attack != attN){
                            delta = m_deltaArray[square - adress + 128];
                            for(int currSquare = adress + delta; !(currSquare & 0x88); currSquare += delta){
                                if(currSquare == square) return true;
                                if(m_pieces[currSquare] != EMPTY) break;
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

bool board_representation::inCheck(bool side) {
    return sqAttackedMK2(m_plist[side][KING].get(0), !side);
}

movebits board_representation::encodeMove(int from, int to, char flag) {
    return (flag << 12) + (((to + (to & 7)) >> 1) << 6) + ((from + (from & 7)) >> 1);
}
int board_representation::fromSq(movebits move) {
    return (move & 0x003F) + ((move & 0x003F) & ~7);
}
int board_representation::toSq(movebits move){
    return ((move >> 6) & 0x003F) + (((move >> 6) & 0x003F) & ~7);
}
char board_representation::getFlag(movebits move) {
    return (move >> 12) & 0x000F;
}

bool board_representation::make(movebits move) {
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
    m_color[to] = m_side;
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
        m_color[arrivalAdress] = m_side;

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
        m_color[arrivalAdress] = m_side;

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
        m_color[arrivalAdress] = m_side;

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
        m_color[arrivalAdress] = m_side;

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
            m_color[to + (m_side ? N : S)] = !m_side;
            positionHash ^= piecesKey[!m_side][PAWN][to + (m_side ? N : S)];
            m_plist[!m_side][PAWN].add(to + (m_side ? N : S));
        }
        else{
            m_pieces[to] = pieceTaken;
            m_color[to] = !m_side;
            positionHash ^= piecesKey[!m_side][pieceTaken][to];
            m_plist[!m_side][pieceTaken].add(to);
        }
    }


    //Undo the actual move
    m_pieces[from] = movedPiece;
    m_color[from] = m_side;

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
}

void board_representation::setFEN(std::string fen) {
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
}

bool board_representation::checkmate() {
    return inCheck(m_side) && !m_moveStackIndex;
}

bool board_representation::stalemate() {
    return !m_moveStackIndex;
}

void board_representation::addToStack(movebits stack[], int &stackIndx, movebits move) {
    stack[stackIndx++] = move;
}

