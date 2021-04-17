//
// Created by bitterboyy on 4/16/21.
//

#include <iostream>
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

    void position::gen(movestack &stack) {
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
        //Traverse every piece list of the side to move, then for each piece, get it's type and generate corresponding moves
        for (int piece_type = PAWN; piece_type < KING && !m_doublechecked; piece_type++) {
            for (int i = 0; i < m_plists[m_side][piece_type].size; i++) {
                from = m_plists[m_side][piece_type].indexes[i];
                pinDelta = getPinDelta(m_pinned, from); //If this is equal to 0 the piece can move freely. Otherwise it is limited to its delta
                if (piece_type == PAWN) {
                    //Pawns can only move up for white, down for black, capture diagonally, and promote
                    //We deal with each of those in two branches, one for each color, to keep things clean
                    if (!m_side) {
                        if (m_board[from + 0x10] == EMPTY && !(pinDelta%0x10)) {
                            if (from >= 0x60 && from <= 0x67 && m_push[from + 0x10])
                                storeMove(from, from + 0x10, NPROM, stack); //Quiet promotion
                            else {
                                if(m_push[from + 0x10]) storeMove(from, from + 0x10, QUIET, stack); //Pure quiet
                                if (m_board[from + 0x20] == EMPTY && from <= 0x17 && m_push[from + 0x20]) {
                                    storeMove(from, from + 0x20, DPAWNPUSH, stack); //Double pawn push
                                }
                            }
                        }

                        if (m_board[from + 0x11] & BLACK && !(pinDelta%0x11) && m_capture[from + 0x11]) {
                            if (from >= 0x60 && from <= 0x67)
                                storeMove(from, from + 0x11, NPROMCAP, stack); //Capture promotion
                            else storeMove(from, from + 0x11, CAP, stack);
                        }
                        else if (from + 0x11 == m_ep && !(pinDelta%0x11) && !epGotcha(from, from+1) && m_capture[from + 0x11])
                            storeMove(from, from + 0x11, EPCAP, stack);

                        if (m_board[from + 0x0F] & BLACK && !(pinDelta%0x0F) && m_capture[from + 0x0F]) {
                            if (from >= 0x60 && from <= 0x67)
                                storeMove(from, from + 0x0F, NPROMCAP, stack); //Capture promotion
                            else storeMove(from, from + 0x0F, CAP, stack);
                        }
                        else if (from + 0x0F == m_ep && !(pinDelta%0x0F) && !epGotcha(from, from-1) && m_capture[from + 0x0F])
                            storeMove(from, from + 0x0F, EPCAP, stack);
                    }
                    else {
                        if (m_board[from - 0x10] == EMPTY && !(pinDelta%0x10)) {
                            if (from >= 0x10 && from <= 0x17 && m_push[from - 0x10])
                                storeMove(from, from - 0x10, NPROM, stack); //Quiet promotion
                            else {
                                if(m_push[from - 0x10]) storeMove(from, from - 0x10, QUIET, stack); //Pure quiet
                                if (m_board[from - 0x20] == EMPTY && from >= 0x60 && m_push[from - 0x20]) {
                                    storeMove(from, from - 0x20, DPAWNPUSH, stack); //Double pawn push
                                }
                            }
                        }

                        if (m_board[from - 0x11] & WHITE && !(pinDelta%0x11) && m_capture[from - 0x11]) {
                            if (from >= 0x10 && from <= 0x17)
                                storeMove(from, from - 0x11, NPROMCAP, stack); //Capture promotion
                            else storeMove(from, from - 0x11, CAP, stack);
                        }
                        else if (from - 0x11 == m_ep && !(pinDelta%0x11) && !epGotcha(from, from-1) && m_capture[from - 0x11])
                            storeMove(from, from - 0x11, EPCAP, stack);

                        if (m_board[from - 0x0F] & WHITE && !(pinDelta%0x0F) && m_capture[from - 0x0F]) {
                            if (from >= 0x10 && from <= 0x17)
                                storeMove(from, from - 0x0F, NPROMCAP, stack); //Capture promotion
                            else storeMove(from, from - 0x0F, CAP, stack);
                        }
                        else if (from - 0x0F == m_ep && !(pinDelta%0x0F) && !epGotcha(from, from+1) && m_capture[from - 0x0F])
                            storeMove(from, from - 0x0F, EPCAP, stack);
                    }
                }
                else {
                    //For other pieces, we iterate through each direction, and follow the ray
                    //A ray stops when :
                    // - it hits a piece (if it's of an opposite color, we add the move as a capture, else don't add it)
                    // - it hits an invalid square
                    // - it can't slide
                    for(auto delta : piece_delta[piece_type]){
                        if(!delta) break;
                        if(pinDelta%delta) continue;
                        raySq = from;
                        for(;;){
                            raySq += delta;
                            if(isInvalid(raySq) || m_board[raySq] & (m_side ? BLACK : WHITE)) break;
                            if(m_board[raySq] & (m_side ? WHITE : BLACK) && m_capture[raySq]) { //Current square is occupied by opposing piece
                                storeMove(from, raySq, CAP, stack);
                                break;
                            }
                            else if(m_push[raySq]) storeMove(from, raySq, QUIET, stack);
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
                if(m_board[raySq]) storeMove(from, raySq, QUIET, stack);
                else storeMove(from, raySq, CAP, stack);
            }
        }

        // Generating castling moves
        // Rules of castling :
        // - Castling rights allow it (king and rook haven't been moved
        // - King not in check (but this generator shouldn't be called if the king is in check anyway)
        // - No squares between king from and to are attacked by the opponent
        if(!m_side && !m_checked){
            if(m_castling&0b1000 && !m_board[0x05] && !m_board[0x06] && !isAttacked(0x05) && !isAttacked(0x06)) storeMove(0x04, 0x06, KCASTLE, stack);
            if(m_castling&0b0100 && !m_board[0x03] && !m_board[0x02] && !m_board[0x01] && !isAttacked(0x03) && !isAttacked(0x02)) storeMove(0x04, 0x02, QCASTLE, stack);
        }
        else if(m_side && !m_checked){
            if(m_castling&0b0010 && !m_board[0x75] && !m_board[0x76] && !isAttacked(0x75) && !isAttacked(0x76)) storeMove(0x74, 0x76, KCASTLE, stack);
            if(m_castling&0b0001 && !m_board[0x73] && !m_board[0x72] && !m_board[0x71]&& !isAttacked(0x73) && !isAttacked(0x72)) storeMove(0x74, 0x72, QCASTLE, stack);
        }

        /*
        std::cout << stack.size << std::endl;
        for(int i = 0; i < stack.size; i++){
            std::cout << std::hex << fromSq(stack.moves[i]) << " " << toSq(stack.moves[i]) << std::dec << std::endl;
        }
         */
    }

    void position::genNoisy(movestack &stack) {

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
                break;

            case QCASTLE: //Move the rook, update castling rights
                m_castling &= (m_side ? 0b1100 : 0b0011);
                m_board[(m_side ? 0x73 : 0x03)] = m_board[(m_side ? 0x70 : 0x00)];
                m_board[(m_side ? 0x70 : 0x00)] = EMPTY;
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
            m_board[to] = (m_side ? BLACK : WHITE) | (fl&0b11 + 1);
            plist_remove(m_plists[m_side][PAWN], to);
            plist_add(m_plists[m_side][fl&0b11 + 1], to);
            if(fl&CAP) m_fifty = 0;
        }

        //If a rook or a king has been moved, update the castling rights accordingly
        if(moved == KING) m_castling &= (m_side ? 0b1100 : 0b0011);
        if(moved == ROOK) {
            if(from == (m_side ? 0x70 : 0x00)) m_castling &= (m_side ? 0b1101 : 0b0111);
            if(from == (m_side ? 0x77 : 0x07)) m_castling &= (m_side ? 0b0001 : 0b1011);
        }
        if(captured == ROOK) {
            if(to == (m_side ? 0x00 : 0x70)) m_castling &= (m_side ? 0b0111 : 0b1101);
            if(to == (m_side ? 0x07 : 0x77)) m_castling &= (m_side ? 0b1011 : 0b0001);
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

        //We started by moving the piece back to its original position
        plist_update(m_plists[m_side][moved&PTMASK], to, from);
        m_board[to] = EMPTY;
        m_board[from] = moved;

        //Deal with special flags
        switch(fl){
            case KCASTLE: //Move the rook
                m_board[(m_side ? 0x77 : 0x07)] = m_board[(m_side ? 0x75 : 0x05)];
                m_board[(m_side ? 0x75 : 0x05)] = EMPTY;
                break;

            case QCASTLE: //Move the rook
                m_board[(m_side ? 0x70 : 0x00)] = m_board[(m_side ? 0x73 : 0x03)];
                m_board[(m_side ? 0x73 : 0x03)] = EMPTY;
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

        //Next we deal with unpromoting pawns if necessary
        if(fl&NPROM){
            m_board[from] = (m_side ? BLACK : WHITE) | (fl&0b11 + 1);
            plist_add(m_plists[m_side][PAWN], from);
            plist_remove(m_plists[m_side][fl&0b11 + 1], from);
            if(fl&CAP){
                plist_add(m_plists[!m_side][captured&PTMASK], to);
            }
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
                delta = get_attackingdelta(square, attacking, piece_type);
                if(delta){ //If we found an attacking delta, follow it (if knight, king or pawn, stop after one iteration)
                    if(piece_delta[0][piece_type]) { //Sliding piece
                        raySq = attacking;
                        for(;;){
                            raySq += delta;
                            if(raySq == square) attackers.push_back(attacking); deltas.push_back(delta);
                            if(isInvalid(raySq) || m_board[raySq] != EMPTY) {
                                break;
                            }
                        }
                    }
                    else if(attacking + delta == square) attackers.push_back(attacking); deltas.push_back(delta);
                }
            }
        }
    }

    void position::updatePins() {
        int attacker;
        int delta;
        int raySq;
        int king = m_plists[m_side][KING].indexes[0]; //Get our king's square
        int pinnedSq = 0;
        pins pinned; initPin(pinned);
        //The idea is to find every opponent piece that can get to the king, then follow their rays
        //We xray to the first piece found if it is one of our pieces, but stop if we encounter a second one
        for(int piece_type = PAWN; piece_type <= KING; piece_type++){
            for(int i = 0; i < m_plists[!m_side][piece_type].size; i++){
                attacker = m_plists[!m_side][piece_type].indexes[i];
                delta = get_attackingdelta(king, attacker, piece_type);
                if(!delta) continue;
                pinnedSq = 0;
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
                                if(!pinnedSq) pinnedSq = raySq;
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
                if(isInvalid(raySq) || (m_board[raySq] & (m_side ? BLACK : WHITE) && raySq != epSquare && raySq != captured)){
                    break;
                }
                if(m_board[raySq] & (m_side ? WHITE : BLACK)){
                    if((m_board[raySq]&PTMASK) == QUEEN || (m_board[raySq]&PTMASK) == ROOK) return true;
                }
            }
        }
        return false;
    }

    void position::setFEN(std::string fen) {

    }

    void position::perft(int depth) {
        movestack stack;
        gen(stack);
        uint64_t nodes;
        uint64_t total{0};

        for(int i = 0; i < stack.size; i++) {
            std::cout << std::hex << fromSq(stack.moves[i]) << "->" << toSq(stack.moves[i]) << std::dec << " : ";
            make(stack.moves[i]);
            nodes = perftRecursive(depth-1);
            std::cout << nodes << std::endl;
            total += nodes;
            unmake();
        }

        std::cout << std::endl << "Total : " << total << std::endl;
    }

    uint64_t position::perftRecursive(int depth){
        movestack stack;
        uint64_t nodes = 0;

        gen(stack);
        if(depth <= 1) {
            return stack.size;
        }

        for(int i = 0; i < stack.size; i++) {
            make(stack.moves[i]);
            nodes += perftRecursive(depth - 1);
            unmake();
        }
        return nodes;
    }
}
