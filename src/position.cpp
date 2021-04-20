//
// Created by bitterboyy on 4/16/21.
//

#include <iostream>
#include <bitset>
#include "position.h"

namespace Chameleon {
    position::position() {
        //Here, we initialize every variable to be equivalent to the initial position of a chess game
        //Notably, we initialize arrays here

        for(int i = 0; i < 0x88; i++){
            m_board[i] = initialPosition[i];
            m_push[i] = true;
            m_capture[i] = true;
        }

        init_plist(m_plists[W][PAWN], {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17});
        init_plist(m_plists[B][PAWN], {0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67});
        init_plist(m_plists[W][KNIGHT], {0x01, 0x06});
        init_plist(m_plists[B][KNIGHT], {0x71, 0x76});
        init_plist(m_plists[W][BISHOP], {0x02, 0x05});
        init_plist(m_plists[B][BISHOP], {0x72, 0x75});
        init_plist(m_plists[W][ROOK], {0x00, 0x07});
        init_plist(m_plists[B][ROOK], {0x70, 0x77});
        init_plist(m_plists[W][QUEEN], {0x03});
        init_plist(m_plists[B][QUEEN], {0x73});
        init_plist(m_plists[W][KING], {0x04});
        init_plist(m_plists[B][KING], {0x74});

        initPin(m_pinned);
    }

    void position::gen(movestack &stack, bool noisy) {
        //Reset the stack
        stack.size = 0;

        //Generate the evasion masks if we're in check
        if(m_checked) {
            genEvasionMasks();
        }
        else { //Otherwise reset them
            for(int i = 0; i < 0x88; i++){
                m_capture[i] = true;
                m_push[i] = true;
            }
        }

        int from;
        int raySq;
        int pinDelta;
        int piece_type;
        int i;
        //Traverse every piece list of the side to move, then for each piece, get it's type and generate corresponding moves
        for (piece_type = PAWN; piece_type < KING && !m_doublechecked; piece_type++) {
            for (i = 0; i < m_plists[m_side][piece_type].size; i++) {
                from = m_plists[m_side][piece_type].indexes[i];
                pinDelta = getPinDelta(m_pinned, from); //If this is equal to 0 the piece can move freely. Otherwise it is limited to its delta
                if (piece_type == PAWN) {
                    //Pawns can only move up for white, down for black, capture diagonally, and promote
                    //We deal with each of those in two branches, one for each color, to keep things clean
                    if (!m_side) {
                        if (!noisy && m_board[from + 0x10] == EMPTY && !(pinDelta%0x10)) {
                            if (from >= 0x60 && from <= 0x67 && m_push[from + 0x10])
                                storeMove(encode(from, from + 0x10, NPROM), stack); //Quiet promotion
                            else {
                                if(m_push[from + 0x10]) storeMove(encode(from, from + 0x10, QUIET), stack); //Pure quiet
                                if (m_board[from + 0x20] == EMPTY && from <= 0x17 && m_push[from + 0x20]) {
                                    storeMove(encode(from, from + 0x20, DPAWNPUSH), stack); //Double pawn push
                                }
                            }
                        }

                        if (m_board[from + 0x11] & BLACK && !(pinDelta%0x11) && m_capture[from + 0x11]) {
                            if (from >= 0x60 && from <= 0x67)
                                storeMove(encode(from, from + 0x11, NPROMCAP), stack); //Capture promotion
                            else storeMove(encode(from, from + 0x11, CAP), stack);
                        }
                        else if (from + 0x11 == m_ep && !(pinDelta%0x11) && !epGotcha(from, from+1) && (m_capture[from + 0x01] || m_push[from + 0x11])){
                            storeMove(encode(from, from + 0x11, EPCAP), stack);
                        }

                        if (m_board[from + 0x0F] & BLACK && !(pinDelta%0x0F) && m_capture[from + 0x0F]) {
                            if (from >= 0x60 && from <= 0x67)
                                storeMove(encode(from, from + 0x0F, NPROMCAP), stack); //Capture promotion
                            else storeMove(encode(from, from + 0x0F, CAP), stack);
                        }
                        else if (from + 0x0F == m_ep && !(pinDelta%0x0F) && !epGotcha(from, from-1) && (m_capture[from - 0x01] || m_push[from + 0x0F])){
                            storeMove(encode(from, from + 0x0F, EPCAP), stack);
                        }
                    }
                    else {
                        if (!noisy && m_board[from - 0x10] == EMPTY && !(pinDelta%0x10)) {
                            if (from >= 0x10 && from <= 0x17 && m_push[from - 0x10])
                                storeMove(encode(from, from - 0x10, NPROM), stack); //Quiet promotion
                            else {
                                if(m_push[from - 0x10]) storeMove(encode(from, from - 0x10, QUIET), stack); //Pure quiet
                                if (m_board[from - 0x20] == EMPTY && from >= 0x60 && m_push[from - 0x20]) {
                                    storeMove(encode(from, from - 0x20, DPAWNPUSH), stack); //Double pawn push
                                }
                            }
                        }

                        if (m_board[from - 0x11] & WHITE && !(pinDelta%0x11) && m_capture[from - 0x11]) {
                            if (from >= 0x10 && from <= 0x17)
                                storeMove(encode(from, from - 0x11, NPROMCAP), stack); //Capture promotion
                            else storeMove(encode(from, from - 0x11, CAP), stack);
                        }
                        else if (from - 0x11 == m_ep && !(pinDelta%0x11) && !epGotcha(from, from-1) && (m_capture[from - 0x01] || m_push[from - 0x11])){
                            storeMove(encode(from, from - 0x11, EPCAP), stack);
                        }

                        if (m_board[from - 0x0F] & WHITE && !(pinDelta%0x0F) && m_capture[from - 0x0F]) {
                            if (from >= 0x10 && from <= 0x17)
                                storeMove(encode(from, from - 0x0F, NPROMCAP), stack); //Capture promotion
                            else storeMove(encode(from, from - 0x0F, CAP), stack);
                        }
                        else if (from - 0x0F == m_ep && !(pinDelta%0x0F) && !epGotcha(from, from+1) && (m_capture[from + 0x01] || m_push[from - 0x0F])){
                            storeMove(encode(from, from - 0x0F, EPCAP), stack);
                        }
                    }
                }
                else {
                    //For other pieces, we iterate through each direction, and follow the ray
                    //A ray stops when :
                    // - it hits a piece (if it's of an opposite color, we add the move as a capture, else don't add it)
                    // - it hits an invalid square
                    // - it can't slide
                    for(int delta : piece_delta[piece_type]){
                        if(!delta) break;
                        if(pinDelta%delta || (abs(delta) == 1 && pinDelta && abs(pinDelta) != abs(delta))) continue;

                        raySq = from;
                        for(;;){
                            raySq += delta;
                            if(isInvalid(raySq) || m_board[raySq] & (m_side ? BLACK : WHITE)) break;
                            if(m_board[raySq] & (m_side ? WHITE : BLACK)) { //Current square is occupied by opposing piece
                                if(m_capture[raySq]) storeMove(encode(from, raySq, CAP), stack);
                                break;
                            }
                            else if(!noisy && m_push[raySq]) storeMove(encode(from, raySq, QUIET), stack);
                            if(!piece_delta[0][piece_type]) break;
                        }
                    }
                }
            }
        }

        //The king has to get particular attention when writing a legal move generator, as his moves
        //are the most likely to be illegal ones
        from = m_plists[m_side][KING].indexes[0];
        for(auto delta : piece_delta[KING]){
            raySq = from + delta;
            if(!isInvalid(raySq) && !(m_board[raySq] & (m_side ? BLACK : WHITE)) && !isAttacked(raySq, true)) {
                if(m_board[raySq] == EMPTY) {
                    if(!noisy) storeMove(encode(from, raySq, QUIET), stack);
                }
                else storeMove(encode(from, raySq, CAP), stack);
            }
        }

        // Generating castling moves
        // Rules of castling :
        // - Castling rights allow it (king and rook haven't been moved
        // - King not in check (but this generator shouldn't be called if the king is in check anyway)
        // - No squares between king from and to are attacked by the opponent
        if(!noisy && !m_side && !m_checked){
            if(m_castling&0b1000 && m_board[0x05] == EMPTY && m_board[0x06] == EMPTY && !isAttacked(0x05) && !isAttacked(0x06)) storeMove(encode(0x04, 0x06, KCASTLE), stack);
            if(m_castling&0b0100 && m_board[0x03] == EMPTY && m_board[0x02] == EMPTY && m_board[0x01] == EMPTY && !isAttacked(0x03) && !isAttacked(0x02)) storeMove(encode(0x04, 0x02, QCASTLE), stack);
        }
        else if(!noisy && m_side && !m_checked){
            if(m_castling&0b0010 && m_board[0x75] == EMPTY && m_board[0x76] == EMPTY && !isAttacked(0x75) && !isAttacked(0x76)) storeMove(encode(0x74, 0x76, KCASTLE), stack);
            if(m_castling&0b0001 && m_board[0x73] == EMPTY && m_board[0x72] == EMPTY && m_board[0x71] == EMPTY && !isAttacked(0x73) && !isAttacked(0x72)) storeMove(encode(0x74, 0x72, QCASTLE), stack);
        }

        /*
        std::cout << stack.size << std::endl;
        for(int i = 0; i < stack.size; i++){
            std::cout << std::hex << fromSq(stack.moves[i]) << " " << toSq(stack.moves[i]) << std::dec << std::endl;
        }
         */
    }

    void position::genEvasionMasks() {
        //We must first get the checking pieces, to know whether we're in single check or not
        //To do so, we loop through every opponent piece and check
        int king = m_plists[m_side][KING].indexes[0];
        int raySq;
        std::vector<int> attackers;
        std::vector<int> deltas;
        getAttackers(m_plists[m_side][KING].indexes[0], attackers, deltas);

        //If we're in single check, we can also generate the push and capture mask
        //Otherwise, it isn't useful and we just return a true-only mask
        if(attackers.size() == 1) {
            for(int i = 0; i < 0x88; i++){
                m_push[i] = false;
                m_capture[i] = false;
            }

            m_capture[attackers[0]] = true;
            //Follow the ray towards the king to create the push mask
            raySq = attackers[0];
            for(;;) {
                raySq += deltas[0];
                if(king == raySq || isInvalid(raySq)) break;
                m_push[raySq] = true;
            }
        }
        else {
            m_doublechecked = true;
        }
    }

    void position::make(movebyte move) {
        //Get relevant information from the movebyte
        int from = fromSq(move);
        int to = toSq(move);
        int fl = flag(move);
        piece moved = m_board[from];
        piece captured = m_board[to];

        //Save the current position in the history so that we can unmake the move if necessary
        history_entry hist = {move, m_ep, m_castling, m_fifty, captured, m_pinned, m_checked};
        m_history.push(hist);

        //Move our piece
        plist_update(m_plists[m_side][moved&PTMASK], from, to);
        m_board[from] = EMPTY;
        m_board[to] = moved;

        //Update variables
        m_fifty++;
        m_ply++;
        m_ep = 0x88;

        //We now have to deal with special flags
        switch(fl){
            case DPAWNPUSH: //Update ep square, reset halfmove clock
                m_ep = to + (m_side ? 0x10 : -0x10);
                m_fifty = 0;
                break;

            case KCASTLE: //Move the rook, update the castling rights
                m_castling &= (m_side ? 0b1100 : 0b0011);
                m_board[(m_side ? 0x75 : 0x05)] = m_board[(m_side ? 0x77 : 0x07)];
                m_board[(m_side ? 0x77 : 0x07)] = EMPTY;
                plist_update(m_plists[m_side][ROOK], (m_side ? 0x77 : 0x07), (m_side ? 0x75 : 0x05));
                break;

            case QCASTLE: //Move the rook, update castling rights
                m_castling &= (m_side ? 0b1100 : 0b0011);
                m_board[(m_side ? 0x73 : 0x03)] = m_board[(m_side ? 0x70 : 0x00)];
                m_board[(m_side ? 0x70 : 0x00)] = EMPTY;
                plist_update(m_plists[m_side][ROOK], (m_side ? 0x70 : 0x00), (m_side ? 0x73 : 0x03));
                break;

            case CAP: //Remove the piece that has been captured from the piece list
                plist_remove(m_plists[!m_side][captured&PTMASK], to);
                m_fifty = 0;
                break;

            case EPCAP: //Remove captured pawn from the piece list and from the board
                m_fifty = 0;
                plist_remove(m_plists[!m_side][PAWN], to + (m_side ? 0x10 : -0x10));
                m_board[to + (m_side ? 0x10 : -0x10)] = EMPTY;
                break;

            default: break;
        }

        //Next we deal with promoting pawns if necessary, by setting the to square to the piece type of our color
        if(fl&NPROM){
            m_board[to] = (m_side ? BLACK : WHITE) | ((fl&0b0011) + 1);
            plist_remove(m_plists[m_side][PAWN], to);
            plist_add(m_plists[m_side][(fl&0b11) + 1], to);
            if(fl&CAP) {
                plist_remove(m_plists[!m_side][captured&PTMASK], to);
                m_fifty = 0;
            }
        }

        //If a rook or a king has been moved, update the castling rights accordingly
        if((moved&PTMASK) == KING) m_castling &= (m_side ? 0b1100 : 0b0011);
        if((moved&PTMASK) == ROOK) {
            if(from == (m_side ? 0x70 : 0x00)) m_castling &= (m_side ? 0b1110 : 0b1011);
            if(from == (m_side ? 0x77 : 0x07)) m_castling &= (m_side ? 0b1101 : 0b0111);
        }
        if((captured&PTMASK) == ROOK) {
            if(to == (m_side ? 0x00 : 0x70)) m_castling &= (m_side ? 0b1011 : 0b1110);
            if(to == (m_side ? 0x07 : 0x77)) m_castling &= (m_side ? 0b0111 : 0b1101);
        }

        //Finally, we can change side and check our pins/if we're checked
        m_side ^= 1;
        updatePins();
        m_checked = isAttacked(m_plists[m_side][KING].indexes[0]);
        m_doublechecked = false;
    }

    void position::unmake() {
        //Extract the info from the history entry
        history_entry hist = m_history.top();
        m_checked = hist.checked;
        m_pinned = hist.pinned;
        m_castling = hist.castling;
        m_ep = hist.ep;
        m_fifty = hist.fifty;
        m_ply--;
        m_side ^= 1;

        int from = fromSq(hist.move);
        int to = toSq(hist.move);
        int fl = flag(hist.move);

        piece moved = m_board[to];
        piece captured = hist.captured;

        //First we unpromote any pawn if necessary
        if(fl&NPROM){
            plist_add(m_plists[m_side][PAWN], to);
            plist_remove(m_plists[m_side][moved&PTMASK], to);
            m_board[to] = (m_side ? BLACK : WHITE) | (PAWN);
            moved = m_board[to];
            if(fl&CAP){
                fl = CAP;
            }
        }

        //We started by moving the piece back to its original position
        plist_update(m_plists[m_side][moved&PTMASK], to, from);
        m_board[to] = EMPTY;
        m_board[from] = moved;

        //Deal with special flags
        switch(fl){
            case KCASTLE: //Move the rook
                m_board[(m_side ? 0x77 : 0x07)] = m_board[(m_side ? 0x75 : 0x05)];
                m_board[(m_side ? 0x75 : 0x05)] = EMPTY;
                plist_update(m_plists[m_side][ROOK], (m_side ? 0x75 : 0x05), (m_side ? 0x77 : 0x07));
                break;

            case QCASTLE: //Move the rook
                m_board[(m_side ? 0x70 : 0x00)] = m_board[(m_side ? 0x73 : 0x03)];
                m_board[(m_side ? 0x73 : 0x03)] = EMPTY;
                plist_update(m_plists[m_side][ROOK],  (m_side ? 0x73 : 0x03), (m_side ? 0x70 : 0x00));
                break;

            case CAP: //Read the piece that had been captured from the piece list
                plist_add(m_plists[!m_side][captured&PTMASK], to);
                m_board[to] = captured;
                break;

            case EPCAP: //Put back captured pawn in the piece list and on the board
                plist_add(m_plists[!m_side][PAWN], to + (m_side ? 0x10 : -0x10));
                m_board[to + (m_side ? 0x10 : -0x10)] = (m_side ? WHITE : BLACK) | PAWN;
                break;

            default: break;
        }
        m_history.pop();
        m_doublechecked = false;
    }

    bool position::isAttacked(int square, bool kingxray) {
        int attacking;
        int delta;
        int raySq;
        //To find out if a square is attacked, loop through opponent's pieces and check if they have an attack delta towards given square
        //If an attack delta is found, follow it to check if it is obstructed or not
        for(int piece_type = PAWN; piece_type <= KING; piece_type++){
            for(int i = 0; i < m_plists[!m_side][piece_type].size; i++){
                attacking = m_plists[!m_side][piece_type].indexes[i];
                delta = get_attackingdelta(square, attacking, m_board[attacking]);
                if(delta){ //If we found an attacking delta, follow it (if knight, king or pawn, stop after one iteration)
                    if(piece_delta[0][piece_type]) { //Sliding piece
                        raySq = attacking;
                        for(;;){
                            raySq += delta;
                            if(raySq == square) return true;
                            if(isInvalid(raySq) || m_board[raySq] != EMPTY) {
                                if(kingxray && (m_board[raySq]) == (KING | (m_side ? BLACK : WHITE))) continue;
                                else break;
                            }
                        }
                    }
                    else if(attacking + delta == square) return true;
                }
            }
        }
        return false;
    }

    void position::getAttackers(int square, std::vector<int>& attackers, std::vector<int>& deltas) {
        int attacking;
        int delta;
        int raySq;
        //To find out if a square is attacked, loop through opponent's pieces and check if they have an attack delta towards given square
        //If an attack delta is found, follow it to check if it is obstructed or not
        for(int piece_type = PAWN; piece_type <= KING; piece_type++){
            for(int i = 0; i < m_plists[!m_side][piece_type].size; i++){
                attacking = m_plists[!m_side][piece_type].indexes[i];
                delta = get_attackingdelta(square, attacking, m_board[attacking]);
                if(delta){ //If we found an attacking delta, follow it (if knight, king or pawn, stop after one iteration)
                    if(piece_delta[0][piece_type]) { //Sliding piece
                        raySq = attacking;
                        for(;;){
                            raySq += delta;
                            if(raySq == square) {
                                attackers.push_back(attacking); deltas.push_back(delta);
                            }
                            if(isInvalid(raySq) || m_board[raySq] != EMPTY) {
                                break;
                            }
                        }
                    }
                    else if(attacking + delta == square) {
                        attackers.push_back(attacking); deltas.push_back(delta);
                    }
                }
            }
        }
    }

    void position::updatePins() {
        int attacker;
        int delta;
        int raySq;
        int king = m_plists[m_side][KING].indexes[0]; //Get our king's square
        int pinnedSq;
        pins pinned; initPin(pinned);
        //The idea is to find every opponent piece that can get to the king, then follow their rays
        //We xray to the first piece found if it is one of our pieces, but stop if we encounter a second one
        for(int piece_type = PAWN; piece_type <= KING; piece_type++){
            for(int i = 0; i < m_plists[!m_side][piece_type].size; i++){
                attacker = m_plists[!m_side][piece_type].indexes[i];
                delta = get_attackingdelta(king, attacker, piece_type);
                if(!delta) continue;
                pinnedSq = 0x88;
                //In case we're dealing with a sliding piece, follow the ray stopping at second friendly piece found
                //or any enemy piece
                if(piece_delta[0][piece_type]) {
                    raySq = attacker;
                    for(;;) {
                        raySq += delta;
                        if(raySq == king) { //If we get to the king, it means a piece got pinned
                            addPin(pinned, pinnedSq, delta);
                            break;
                        }
                        if(m_board[raySq] != EMPTY){
                            if(m_board[raySq] & (m_side ? BLACK : WHITE)){ //Friendly piece on square
                                if(pinnedSq == 0x88) pinnedSq = raySq;
                                else break;
                            }
                            else break; //Not friendly pieces
                        }
                    }
                }
            }
        }

        m_pinned = pinned;
    }

    bool position::epGotcha(int epSquare, int captured) {
        //We must check, when generating an en passant move, if removing the two pawns will put our king in check
        //To do that, go on horizontal deltas from king position, and stop on any piece which isn't any of the two pawns
        //If the piece found isn't friendly and can attack horizontally, the en passant move is illegal
        int king = m_plists[m_side][KING].indexes[0];
        int raySq;
        for(int delta = -1; delta < 2; delta += 2){
            raySq = king;
            for(;;){
                raySq += delta;
                if(isInvalid(raySq)) break;
                if(m_board[raySq] != EMPTY && raySq != captured && raySq != epSquare){
                    if(m_board[raySq] == (QUEEN|(m_side ? WHITE : BLACK)) || m_board[raySq] == (ROOK|(m_side ? WHITE : BLACK))) return true;
                    else break;
                }
            }
        }
        return false;
    }

    void position::setFEN(const std::string& fen) {
        plist pieces[2][6];
        for(auto & piece : pieces){
            for(auto & j : piece){
                init_plist(j);
            }
        }

        std::string ranks[8]; //Holds the string describing each of the ranks
        int rank_size{0};
        std::string options[5]; //Same but with the options
        int options_size{0};

        //Iterate one time over the string to split it correctly
        std::string swap;
        for(char c : fen){
            if(c == '/') {ranks[rank_size++] = swap; swap = "";}
            else if(c == ' ') {
                if(rank_size < 8) {ranks[rank_size++] = swap; swap = "";}
                else {options[options_size++] = swap; swap = "";}
            }
            else swap += c;
        }
        options[options_size++] = swap;

        //We can then check every rank, setting the board according to the string
        int file;
        int square;
        for(int rank = 0; rank < rank_size; rank++){
            file = 0;
            for(int i = 0; i < ranks[rank].length(); i++){
                square = getSquare(7-rank, file);
                switch(tolower(ranks[rank][i])){
                    case 'p':
                        m_board[square] = PAWN | (islower(ranks[rank][i]) ? BLACK : WHITE);
                        plist_add(pieces[(islower(ranks[rank][i]) ? 1 : 0)][PAWN], square);
                        file++;
                        break;

                    case 'n':
                        m_board[square] = KNIGHT | (islower(ranks[rank][i]) ? BLACK : WHITE);
                        plist_add(pieces[(islower(ranks[rank][i]) ? 1 : 0)][KNIGHT], square);
                        file++;
                        break;

                    case 'b':
                        m_board[square] = BISHOP | (islower(ranks[rank][i]) ? BLACK : WHITE);
                        plist_add(pieces[(islower(ranks[rank][i]) ? 1 : 0)][BISHOP], square);
                        file++;
                        break;

                    case 'r':
                        m_board[square] = ROOK | (islower(ranks[rank][i]) ? BLACK : WHITE);
                        plist_add(pieces[(islower(ranks[rank][i]) ? 1 : 0)][ROOK], square);
                        file++;
                        break;

                    case 'q':
                        m_board[square] = QUEEN | (islower(ranks[rank][i]) ? BLACK : WHITE);
                        plist_add(pieces[(islower(ranks[rank][i]) ? 1 : 0)][QUEEN], square);
                        file++;
                        break;

                    case 'k':
                        m_board[square] = KING | (islower(ranks[rank][i]) ? BLACK : WHITE);
                        plist_add(pieces[(islower(ranks[rank][i]) ? 1 : 0)][KING], square);
                        file++;
                        break;

                    default:
                        for(int j = 0; j < ranks[rank][i] - '0'; j++){
                            square = getSquare(7-rank, file);
                            m_board[square] = EMPTY;
                            file++;
                        }
                        break;
                }
            }
        }

        if(options_size > 0) m_side = (options[0] == "b");
        else m_side = W;

        if(options_size > 1) {
            m_castling = 0;
            for(auto c : options[1]){
                switch(c){
                    case 'K': m_castling |= 0b1000; break;
                    case 'Q': m_castling |= 0b0100; break;
                    case 'k': m_castling |= 0b0010; break;
                    case 'q': m_castling |= 0b0001; break;
                    default: break;
                }
            }
        } else m_castling = 0b1111;

        if(options_size > 2){
            if(options[2] == "-") m_ep = 0x88;
            else m_ep = getSquare(options[2][1] - '1', options[2][0] - 'a');
        }

        if(options_size > 3) m_fifty = stoi(options[3]);
        if(options_size > 4) m_ply = stoi(options[4]);

        for(int i = 0; i < 2; i++){
            for(int j = 0; j < 6; j++){
                m_plists[i][j] = pieces[i][j];
            }
        }

        updatePins();
        m_checked = isAttacked(m_plists[m_side][KING].indexes[0]);
        m_doublechecked = false;
    }

    void position::perft(int depth) {
        movestack stack;
        gen(stack);
        uint64_t nodes;
        uint64_t total{0};

        for(int i = 0; i < stack.size; i++) {
            std::cout << intToSq(fromSq(stack.moves[i])) << intToSq(toSq(stack.moves[i])) <<  " :  ";
            make(stack.moves[i]);
            nodes = depth == 1 ? 1 : perftRecursive(depth-1);
            std::cout << nodes << std::endl;
            total += nodes;
            unmake();
        }

        std::cout << std::endl << "depth " << depth << " : " << total << std::endl;
    }

    uint64_t position::perftRecursive(int depth){
        movestack stack;
        uint64_t nodes = 0;

        gen(stack);
        if(depth == 1) {
            return stack.size;
        }

        for(int i = 0; i < stack.size; i++) {
            make(stack.moves[i]);
            nodes += perftRecursive(depth - 1);
            unmake();
        }
        return nodes;
    }

    void position::show() {
        int square;
        std::cout << "- - - - - - - - - - - -\n- - - - - - - - - - - -" << std::endl;
        for(int rank = 7; rank >= 0; rank--){
            std::cout << "- - ";
            for(int file = 0; file < 8; file++){
                square = getSquare(rank, file);
                switch (m_board[square]&PTMASK) {
                    case PAWN:
                        std::cout << (m_board[square]&WHITE ? "P " : "p ");
                        break;
                    case KNIGHT:
                        std::cout << (m_board[square]&WHITE ? "N " : "n ");
                        break;
                    case BISHOP:
                        std::cout << (m_board[square]&WHITE ? "B " : "b ");
                        break;
                    case ROOK:
                        std::cout << (m_board[square]&WHITE ? "R " : "r ");
                        break;
                    case QUEEN:
                        std::cout << (m_board[square]&WHITE ? "Q " : "q ");
                        break;
                    case KING:
                        std::cout << (m_board[square]&WHITE ? "K " : "k ");
                        break;
                    default:
                        std::cout << (". ");
                        break;
                }
            }
            std::cout << "- -" << std::endl;
        }
        std::cout << "- - - - - - - - - - - -\n- - - - - - - - - - - -" << std::endl;

        std::cout << "ply: " << m_ply << "     " << (m_side ? "black's move" : "white's move") << std::endl;
        std::cout << "cast: " << (m_castling & 0b1000 ? "K" : ".")
                                  << (m_castling & 0b0100 ? "Q" : ".")
                                  << (m_castling & 0b0010 ? "k" : ".")
                                  << (m_castling & 0b0001 ? "q" : ".");
        std::cout << "     ep: " << (m_ep == 0x88 ? "-" : intToSq(m_ep)) << std::endl << std::endl;
    }
}
