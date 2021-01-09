//
// Created by bitterboyy on 12/11/20.
//

#include "BoardRepresentation.h"
#include "Engine.h"
#include <sstream>
#include <iomanip>
#include <utility>

int pieceCorrespondance(int piece, int color){
    int pieceTransf;
    switch(piece){
        case 1:
            pieceTransf = color == 1 ? 0 : 6;
            break;
        case 3:
            pieceTransf = color == 1 ? 1 : 7;
            break;
        case 4:
            pieceTransf = color == 1 ? 2 : 8;
            break;
        case 5:
            pieceTransf = color == 1 ? 3 : 9;
            break;
        case 9:
            pieceTransf = color == 1 ? 4 : 10;
            break;
        case 100:
            pieceTransf = color == 1 ? 5 : 11;
            break;

        default:
            pieceTransf = 0;
            break;
    }

    return pieceTransf;
}


std::vector<std::string> split(std::string toSplit, char splitter){
    std::vector<std::string> substrings;
    std::string substring;
    for(int i = 0; i <= toSplit.length(); i++){
        if(toSplit[i] == splitter || i == toSplit.length()){
            substrings.push_back(substring);
            substring = "";
        }

        else{
            substring += toSplit[i];
        }
    }
    return substrings;
}


/*
 * FEN PART :
 * This part takes care of parsing/generating a FEN to set the board to said FEN or
 * scan the board to generate the current FEN
 */

void BoardRepresentation::setFEN(std::string fen) {
    /*
     * La notation FEN est comme suit :
     * Chaque rang est séparé par un /
     * Les pièces noires sont en minuscules, les pièces blanches en majuscules
     * w/b indique le tour de jeu
     * On indique ensueEmptite les casle possibles
     * Puis les en passant possibles
     * Coups depuis la dernière prise
     * Numero du coup
     *
     * ON NOTE QUE FEN[x] = BOARD[
     */

    for(int i = 0; i < 64; i++){
        m_color[i] = 0;
        m_piece[i] = 0;
    }
    std::vector<std::string> subStrings;
    std::vector<std::string> subStringsSettings;
    subStrings = split(std::move(fen), '/');
    subStringsSettings = split(subStrings.at(subStrings.size() - 1), ' ');
    subStrings.at(subStrings.size() - 1) = subStringsSettings.at(0);

    //On itère 1 à 1 les différents champs de la notation donnée pour le board en lui-même
    for(int i = 0; i < subStrings.size(); i++){
        int k = 0; //Indice du charactère
        //Dans ces champs, on itère case par case
        //Sauf si l'on est au dernier substring, auquel cas on doit aussi prendre les derniers champs en compte
        if(i < 8){
            for(int j = 0; j < 8; j++){
                int caseCorrespondante = (7-i)*8 + j; //On retrouve la case correspondante à la notation
                //std::cout << "Pour la case " << caseCorrespondante << " on place ";

                //On check si le char donné est un digit
                if(isdigit(subStrings.at(i)[k])){
                    //Si c'est le cas, on marque les x prochaines cases comme vides
                    //Pour cela, on avance j à la prochaine valeur non vide
                    int aPasser = subStrings.at(i)[k] - 49;
                    j += aPasser;
                    //std::cout << subStrings.at(i)[k] << " " << j;
                }

                else{
                    //On switch la lettre donnée pour voir à quoi elle corresponds
                    switch(subStrings.at(i)[k]){
                        case 'p':
                            m_color[caseCorrespondante] = 2;
                            m_piece[caseCorrespondante] = 1;
                            //std::cout << "Un pion";
                            break;

                        case 'P':
                            m_color[caseCorrespondante] = 1;
                            m_piece[caseCorrespondante] = 1;
                            //std::cout << "Un pion";
                            break;

                        case 'n':
                            m_color[caseCorrespondante] = 2;
                            m_piece[caseCorrespondante] = 3;
                            //std::cout << "Un cavalier";
                            break;

                        case 'N':
                            m_color[caseCorrespondante] = 1;
                            m_piece[caseCorrespondante] = 3;
                            //std::cout << "Un cavalier";
                            break;

                        case 'b':
                            m_color[caseCorrespondante] = 2;
                            m_piece[caseCorrespondante] = 4;
                            //std::cout << "Un fou";
                            break;

                        case 'B':
                            m_color[caseCorrespondante] = 1;
                            m_piece[caseCorrespondante] = 4;
                            //std::cout << "Un fou";
                            break;

                        case 'r':
                            m_color[caseCorrespondante] = 2;
                            m_piece[caseCorrespondante] = 5;
                            //std::cout << "Une tour";
                            break;

                        case 'R':
                            m_color[caseCorrespondante] = 1;
                            m_piece[caseCorrespondante] = 5;
                            //std::cout << "Une tour";
                            break;

                        case 'q':
                            m_color[caseCorrespondante] = 2;
                            m_piece[caseCorrespondante] = 9;
                            //std::cout << "Une reine";
                            break;

                        case 'Q':
                            m_color[caseCorrespondante] = 1;
                            m_piece[caseCorrespondante] = 9;
                            //std::cout << "Une reine";
                            break;

                        case 'k':
                            m_color[caseCorrespondante] = 2;
                            m_piece[caseCorrespondante] = 100;
                            //std::cout << "Un roi";
                            break;

                        case 'K':
                            m_color[caseCorrespondante] = 1;
                            m_piece[caseCorrespondante] = 100;
                            //std::cout << "Un roi";
                            break;
                    }
                }
                k++;
                //std::cout << std::endl;
            }
        }

        else{
            for(int j = 0; j < 8; j++){
                int caseCorrespondante = (7-i)*8 + j; //On retrouve la case correspondante à la notation

                //On check si le char donné est un digit
                if(isdigit(subStrings.at(i)[k])){
                    //Si c'est le cas, on marque les x prochaines cases comme vides
                    //Pour cela, on avance j à la prochaine valeur non vide
                    j += (int) subStrings.at(i)[k];
                }

                else{
                    //On switch la lettre donnée pour voir à quoi elle corresponds
                    switch(subStrings.at(i)[k]){
                        case 'p':
                            m_color[caseCorrespondante] = 2;
                            m_piece[caseCorrespondante] = 1;
                            break;

                        case 'P':
                            m_color[caseCorrespondante] = 1;
                            m_piece[caseCorrespondante] = 1;
                            break;

                        case 'n':
                            m_color[caseCorrespondante] = 2;
                            m_piece[caseCorrespondante] = 3;
                            break;

                        case 'N':
                            m_color[caseCorrespondante] = 1;
                            m_piece[caseCorrespondante] = 3;
                            break;

                        case 'b':
                            m_color[caseCorrespondante] = 2;
                            m_piece[caseCorrespondante] = 4;
                            break;

                        case 'B':
                            m_color[caseCorrespondante] = 1;
                            m_piece[caseCorrespondante] = 4;
                            break;

                        case 'r':
                            m_color[caseCorrespondante] = 2;
                            m_piece[caseCorrespondante] = 5;
                            break;

                        case 'R':
                            m_color[caseCorrespondante] = 1;
                            m_piece[caseCorrespondante] = 5;
                            break;

                        case 'q':
                            m_color[caseCorrespondante] = 2;
                            m_piece[caseCorrespondante] = 9;
                            break;

                        case 'Q':
                            m_color[caseCorrespondante] = 1;
                            m_piece[caseCorrespondante] = 9;
                            break;

                        case 'k':
                            m_color[caseCorrespondante] = 2;
                            m_piece[caseCorrespondante] = 100;
                            break;

                        case 'K':
                            m_color[caseCorrespondante] = 1;
                            m_piece[caseCorrespondante] = 100;
                            break;
                    }
                }
                k++;
            }

            //On récupère ensuite le reste des informations de la notation FEN
        }
    }

    m_sideToMove = subStringsSettings.at(1)[0] == 'w' ? 1 : 2;

    m_castlingRights = 0x0000;
    for(int i = 0; i < subStringsSettings.at(2).size(); i++){
        switch(subStringsSettings.at(2)[i]){
            case 'K':
                m_castlingRights += 0x1000;
                break;

            case 'Q':
                m_castlingRights += 0x0100;
                break;

            case 'k':
                m_castlingRights += 0x0010;
                break;

            case 'q':
                m_castlingRights += 0x0001;
                break;

            default:
                break;
        }

        std::string enPassant = subStringsSettings.at(3);
        m_enPassant = enPassant[0] != '-' ? writtenMoveParser(enPassant) : -1;

        std::stringstream ss;
        ss << subStringsSettings.at(4);
        ss >> m_fifty;

        std::stringstream ss2;
        ss2 << subStringsSettings.at(5);
        ss2 >> m_coups;
    }
}

std::string BoardRepresentation::getFEN() {
    std::string fen;
    int compteurVide = 0; //Compte les cases vides

    //On itère d'abord les deux array de pièces
    for(int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            int caseCorrespondante = (7-i)*8 + j;

            if (caseCorrespondante != 56 && (caseCorrespondante) % 8 == 0) {
                if (compteurVide != 0) {
                    fen += (std::to_string(compteurVide));
                    compteurVide = 0;
                }
                fen += "/";
            }
            switch (m_piece[caseCorrespondante]) {
                case 1:
                    if (compteurVide != 0) {
                        fen += (std::to_string(compteurVide));
                        compteurVide = 0;
                    }
                    if (m_color[caseCorrespondante] == 2) fen += "p";
                    else fen += "P";
                    break;

                case 3:
                    if (compteurVide != 0) {
                        fen += (std::to_string(compteurVide));
                        compteurVide = 0;
                    }
                    if (m_color[caseCorrespondante] == 2) fen += "n";
                    else fen += "N";
                    break;

                case 4:
                    if (compteurVide != 0) {
                        fen += (std::to_string(compteurVide));
                        compteurVide = 0;
                    }
                    if (m_color[caseCorrespondante] == 2) fen += "b";
                    else fen += "B";
                    break;

                case 5:
                    if (compteurVide != 0) {
                        fen += (std::to_string(compteurVide));
                        compteurVide = 0;
                    }
                    if (m_color[caseCorrespondante] == 2) fen += "r";
                    else fen += "R";
                    break;

                case 9:
                    if (compteurVide != 0) {
                        fen += (std::to_string(compteurVide));
                        compteurVide = 0;
                    }
                    if (m_color[caseCorrespondante] == 2) fen += "q";
                    else fen += "Q";
                    break;

                case 100:
                    if (compteurVide != 0) {
                        fen += (std::to_string(compteurVide));
                        compteurVide = 0;
                    }
                    if (m_color[caseCorrespondante] == 2) fen += "k";
                    else fen += "K";
                    break;

                case 0:
                    compteurVide++;
            }
        }
    }

    if(compteurVide != 0){
        fen += (std::to_string(compteurVide));
    }



    //On ajoute ensuite les options
    fen += m_sideToMove == 1 ? " w " : " b ";

    int castling = m_castlingRights;
    if(castling == 0) fen += '-';
    else{
        do{
            if(castling >= 0x1000) {
                fen += "K";
                castling -= 0x1000;
            }
            else if(castling >= 0x0100){
                fen += "Q";
                castling -= 0x0100;
            }
            else if(castling >= 0x0010) {
                fen += "k";
                castling -= 0x0010;
            }
            else if(castling >= 0x0001) {
                fen += "q";
                castling -= 0x0001;
            }
        }while(castling != 0);
    }


    fen += m_enPassant != -1 ? " " + adressToMoveParser(m_enPassant) + " " : " - ";
    fen += std::to_string(m_fifty);

    fen += " ";

    fen += std::to_string(m_coups);

    return fen;
}

unsigned long long BoardRepresentation::getHash() {
    unsigned long long hash = 0;
    unsigned long long toXor;
    int piece;

    //Hash pieces
    for(int i = 0; i < 64; i++){
        if(m_piece[i] != 0){
            piece = pieceCorrespondance(m_piece[i], m_color[i]);
            toXor = m_zobrist_hash[((i+1 * piece+1)) - 1];
            if(hash == 0) hash = toXor;
            else hash = hash xor toXor;
        }
    }

    //Hash side to move
    if(m_sideToMove == 2){
        hash = hash xor m_zobrist_hash[768];
    }

    //Hash castling rights
    int castling = m_castlingRights;
    while(castling > 0){
        if(castling >= 0x1000){
            hash = hash xor m_zobrist_hash[769];
            castling -= 0x1000;
        }
        else if(castling >= 0x0100){
            hash = hash xor m_zobrist_hash[770];
            castling -= 0x0100;
        }
        else if(castling >= 0x0010) {
            hash = hash xor m_zobrist_hash[771];
            castling -= 0x0010;
        }
        else if(castling >= 0x0001) {
            hash = hash xor m_zobrist_hash[772];
            castling -= 0x0001;
        }
    }

    //Hash enPassant file
    int enPassantFile = m_enPassant%8;
    if(m_enPassant != -1) hash = hash xor m_zobrist_hash[773 + enPassantFile];

    m_positionHash = hash;
    return hash;
}


/*
 * LEGALITE DES COUPS
 * Ces fonctions servent à vérifier ensembles la légalité des coups :
 * isPseudoLegal vérifie q'un coup respecte les règles de mouvement des échecs
 * inCheck vérifie si le roi est en échec ou non
 */

bool BoardRepresentation::inCheck(int side) {
    //On cherche le roi vite fait
    int kingPosition = -1;
    PieceList pieces = side == 1 ? m_whitePieces : m_blackPieces;
    for(int i = 0; i < pieces.getAdresses().size(); i++){
        if(m_piece[i] == 100) kingPosition = i;
    }

    //On itère ensuite toooous les moves disponibles partant de chaque case à celle du roi pour voir si ils sont légaux
    int opposingSide = side == 1 ? 2 : 1;
    return underAttack(kingPosition, opposingSide);
}

bool BoardRepresentation::isPseudoLegal(Move move, int side) {
    bool w_kingside, w_queenside, b_kingside, b_queenside;
    int checkedAdress;
    int lastChecked;
    int direction = 0;

    //Si la pièce de la case de départ n'est pas à nous, le move est forcément faux
    //Même si chose si la pièce sur la case d'arrivée est de notre couleur bien entendu
    if(m_color[move.start()] != side || m_color[move.end()] == side) return false;

    //On check maintenant les cas des différentes pièces :
    switch(m_piece[move.start()]){
        case 1:
            //Les moves des pions dépendent de leur couleur
            switch(m_color[move.start()]){
                //pion blanc
                case 1:
                    //Si la pièce de la case d'arrivée est noire et en diagonale d'un pas du pion, le coup est valable
                    //Marche également si cette case est écrite dans m_enPassant
                    if(((m_color[move.end()] == 2 && (move.end() - move.start() == 7 || move.end() - move.start() == 9))
                    || (move.end() == m_enPassant && (move.end() - move.start() == 7 || move.end() - move.start() == 9)))
                    && abs(move.start() % 8 - move.end() % 8) <= 1) return true;

                    //Si le pion est sur son rang de départ, il peut avancer de deux cases
                    //Dans ce cas de figure, on ajoute la case en passant
                    else if(move.start() < 16 && move.start() > 7 && move.end() - move.start() == 16 && m_piece[move.end()] == 0){
                        m_enPassant = move.start() + 8;
                        return true;
                    }

                    //Sinon, le pion n'avance que d'une case en avant, si eu seulement si il n'y a pas de pièce sur la case d'arrivée
                    else if(move.end() - move.start() != 8 || m_piece[move.end()] != 0) return false;
                    break;

                //pion noir
                case 2:
                    //Pareil mais inversé
                    if(((m_color[move.end()] == 1 && (move.end() - move.start() == -7 || move.end() - move.start() == -9))
                       || (move.end() == m_enPassant && (move.end() - move.start() == -7 || move.end() - move.start() == -9)))
                       && abs(move.start() % 8 - move.end() % 8) <= 1) return true;

                    else if(move.start() <= 55 && move.start() >= 48 && move.end() - move.start() == -16 && m_piece[move.end()] == 0){
                        m_enPassant = move.start() - 8;
                        return true;
                    }
                    else if(move.end() - move.start() != -8 || m_piece[move.end()] != 0) return false;
                    break;

                //Case vide ou invalide
                default:
                    break;
            }
            break;

        //Cavalier
        case 3:
            //El famoso L
            if(abs(move.end() - move.start()) != 6
                && abs(move.end() - move.start()) != 15
                && abs(move.end() - move.start()) != 17
                && abs(move.end() - move.start()) != 10
                ||(((move.end()%8) - (move.start()%8) > 2 || (move.end()%8) - (move.start()%8) < -2)
                || ((move.end()/8) - (move.start()/8)) > 2) || (((move.end()/8) - (move.start()/8)) < -2)) return false;
            break;

        //Fou
        case 4:
            //On check dans toutes les directions, pas à pas, si le move souhaité est disponible.
            //On arrête chaque branche si l'on tombe sur une pièce ou sur la fin de la board
            //Ne se déplace qu'en diagonale
            for(int i = 0; i < 4; i++){
                checkedAdress = move.start();
                lastChecked = move.start();
                switch(i){
                    case 0: direction = 9; break;
                    case 1: direction = 7; break;
                    case 2: direction = -7; break;
                    case 3: direction = -9; break;
                    default : std::cout << "Problème avec le fonction pseudo-legal"; break;
                }
                do{
                    checkedAdress += direction;
                    if((checkedAdress%8 == 7 && lastChecked%8 == 0)
                    ||(checkedAdress%8 == 0 && lastChecked%8 == 7)
                    || (checkedAdress/8 == 1 && lastChecked/8 == 7)
                    || (checkedAdress/8 == 7 && lastChecked/8 == 1)) break;
                    else if(checkedAdress == move.end()) return true;
                    else if(m_piece[checkedAdress]!=0) break;
                    lastChecked = checkedAdress;
                }while(true);
            }
            return false;

        //Rook
        case 5:
            //Diagonale haute gauche
            for(int i = 0; i < 4; i++){
                lastChecked = move.start();
                checkedAdress = move.start();
                switch(i){
                    case 0: direction = 1; break;
                    case 1: direction = -1; break;
                    case 2: direction = 8; break;
                    case 3: direction = -8; break;
                    default : std::cout << "Problème avec le fonction pseudo-legal"; break;
                }
                do{
                    checkedAdress += direction;
                    if((checkedAdress%8 == 7 && lastChecked%8 == 0)
                       ||(checkedAdress%8 == 0 && lastChecked%8 == 7)
                       || (checkedAdress/8 == 1 && lastChecked/8 == 7)
                       || (checkedAdress/8 == 7 && lastChecked/8 == 1)) break;
                    else if(checkedAdress == move.end()) return true;
                    else if(m_piece[checkedAdress]!=0) break;

                    lastChecked = checkedAdress;
                }while(true);
            }
            return false;

        //Queen
        case 9:
            for(int i = 0; i < 8; i++){
                checkedAdress = move.start();
                lastChecked = move.start();
                switch(i){
                    case 0: direction = 1; break;
                    case 1: direction = -1; break;
                    case 2: direction = 8; break;
                    case 3: direction = -8; break;
                    case 4: direction = 7; break;
                    case 5: direction = -7; break;
                    case 6: direction = 9; break;
                    case 7: direction = -9; break;
                    default : std::cout << "Problème avec le fonction pseudo-legal"; break;
                }
                do{
                    checkedAdress += direction;
                    if(checkedAdress < 0 || checkedAdress > 63) break;
                    if((checkedAdress%8 == 7 && lastChecked%8 == 0)
                       ||(checkedAdress%8 == 0 && lastChecked%8 == 7)
                       || (checkedAdress/8 == 1 && lastChecked/8 == 7)
                       || (checkedAdress/8 == 7 && lastChecked/8 == 1))break;
                    else if(checkedAdress == move.end()) return true;
                    else if(m_piece[checkedAdress]!=0)break;

                    lastChecked = checkedAdress;
                }while(true);
            }
            return false;

        //Roi
        case 100:
            //Toutes directions, mais d'un seul pas ou castle
            //Castling
            if((move.start() == 4 || move.start() == 60) && abs(move.end() - move.start()) == 2 && m_piece[move.start()] == 100) {
                w_queenside = m_castlingRights >= 0x0100 && !inCheck(1)
                              && isPseudoLegal(w_queenSideR, 1) && m_piece[0] == 5
                              && !underAttack(2, 2) && !underAttack(3, 2)
                              && move.start() == 4 && move.end() == 2;

                w_kingside = m_castlingRights >= 0x1000 && !inCheck(1)
                             && isPseudoLegal(w_kingSideR, 1) && m_piece[7] == 5
                             && !underAttack(6, 2) && !underAttack(5, 2)
                             && move.start() == 4 && move.end() == 6;

                b_queenside = m_castlingRights >= 0x0001 && !inCheck(2)
                              && isPseudoLegal(b_queenSideR, 2) && m_piece[56] == 5
                              && !underAttack(59, 1) && !underAttack(58, 1)
                              && move.start() == 60 && move.end() == 58;

                b_kingside = m_castlingRights >= 0x0010 && !inCheck(2)
                             && isPseudoLegal(b_kingSideR, 2) && m_piece[63] == 5
                             && !underAttack(61, 1) && !underAttack(62, 1)
                             && move.start() == 60 && move.end() == 62;

                if(w_queenside || w_kingside || b_queenside || b_kingside) return true;
            }

            if(abs(move.end() - move.start()) != 1
                && abs(move.end() - move.start()) != 9
                && abs(move.end() - move.start()) != 8
                && abs(move.end() - move.start()) != 7
                || abs(move.end()%8 - move.start()%8) > 1) return false;


            break;

        default:
            return false;

    }
    return true;
}

bool BoardRepresentation::underAttack(int square, int attackingSide){
    PieceList attackingPieces = attackingSide == 1 ? m_whitePieces : m_blackPieces;
    for(int i = 0; i < attackingPieces.getAdresses().size(); i++){
        Move move = Move(i, square);
        if(i != square && isPseudoLegal(move, attackingSide)) return true;
    }
    return false;
}



bool BoardRepresentation::makeMove(Move move) {
    m_nodeCount++;

    //Permet de checker les changements de m_enPassant
    int enPassantStart = m_enPassant;
    //Copies des piece-list dans le cas d'un takeback
    PieceList whitePieces = m_whitePieces, blackPieces = m_blackPieces;

    //Checks if the move is pseudoLegal to start, if not we don't bother
    if(!isPseudoLegal(move, m_sideToMove)) return false;

    //Checks if we want to take enPassant
    if(move.end() == m_enPassant && m_piece[move.start()] == 1){
        if(m_sideToMove == 1){
            m_piece[move.end() - 8] = 0; m_color[move.end() - 8] = 0;
        }
        else{
            m_piece[move.end() + 8] = 0; m_color[move.end() + 8] = 0;
        }
    }

    //Castling
    if((move.start() == 4 || move.start() == 60) && abs(move.end() - move.start()) == 2 && m_piece[move.start()] == 100){
        switch(move.end()){
            case 62:
                m_piece[63] = 0; m_color[63] = 0;
                m_piece[61] = 5; m_color[61] = 2;
                m_blackPieces.removeAdress(63);
                m_blackPieces.addAdress(61);
                m_castlingRights -= 0x0011;
                break;

            case 58:
                m_piece[56] = 0; m_color[56] = 0;
                m_piece[59] = 5; m_color[59] = 2;
                m_blackPieces.removeAdress(56);
                m_blackPieces.addAdress(59);
                m_castlingRights -= 0x0011;
                break;

            case 6:
                m_piece[7] = 0; m_color[7] = 0;
                m_piece[5] = 5; m_color[5] = 1;
                m_whitePieces.removeAdress(7);
                m_whitePieces.addAdress(5);
                m_castlingRights -= 0x1100;
                break;

            case 2:
                m_piece[0] = 0; m_color[0] = 0;
                m_piece[3] = 5; m_color[3] = 1;
                m_whitePieces.removeAdress(0);
                m_whitePieces.addAdress(3);
                m_castlingRights -= 0x1100;
                break;

            default : break;
        }
    }

    //Regle des 50 coups (qui apparemment sont en fait 100 coups wtf)
    if(m_piece[move.end()] == 0 || m_piece[move.start()] != 1) m_fifty++;
    else m_fifty = 0;

    bool isPromotable = m_piece[move.start()] == 1;

    m_piece[move.end()] = m_piece[move.start()];
    m_color[move.end()] = m_color[move.start()];
    m_piece[move.start()] = 0;
    m_color[move.start()] = 0;

    if(m_sideToMove == 1){
        m_whitePieces.removeAdress(move.start());
        m_whitePieces.addAdress(move.end());
        m_blackPieces.removeAdress(move.end());
    }

    else{
        m_blackPieces.removeAdress(move.start());
        m_blackPieces.addAdress(move.end());
        m_whitePieces.removeAdress(move.end());
    }



    if(isPromotable && ((move.end() <= 63 && move.end() >= 56) || (move.end() <= 7 && move.end() >= 0))){
        switch(move.special()){
            case 3:
                m_piece[move.end()] = 3;
                break;

            case 4:
                m_piece[move.end()] = 4;
                break;

            case 5:
                m_piece[move.end()] = 5;
                break;

            default:
                m_piece[move.end()] = 9;
                break;
        }
    }

    int currentSide = m_sideToMove;
    m_sideToMove = m_sideToMove == 1 ? 2: 1;

    //Si m_enPassant == enPAssantStart, on reset
    if(m_enPassant == enPassantStart && enPassantStart != -1)
        m_enPassant = -1;



    m_positionHistory.push(getFEN());

    if(inCheck(currentSide)){
        takeback(whitePieces, blackPieces);
        return false;
    }

    getHash();

    return true;
}

void BoardRepresentation::moveGenerator() {
    /*
     * Du brute forcing pur et dur, on itère chaque case d'arrivée pour chaque case de départ
     * puis on range les moves légaux selon différents critères
     * Attacking moves : le move mange une pièce
     * Quiet moves : le move ne mange pas de pièce
     * Promote : le move met un pion sur le rang opposé à son rang de départ
     */

    m_moves.clear();
    m_attackingMoves.clear();
    m_quietMoves.clear();

    PieceList pieceToCheck = m_sideToMove == 1 ? m_whitePieces : m_blackPieces;
    PieceList oppositePieces = m_sideToMove == 1 ? m_blackPieces : m_whitePieces;
    int sideToMove = m_sideToMove;

    for(int i = 0; i < pieceToCheck.getAdresses().size(); i++) {
        int start = pieceToCheck.getAdresses().at(i);
        for (int j = 0; j < 64; j++) {
            Move move = Move(start, j);
            if (makeMove(move)) {
                if(sideToMove == 1) takeback(pieceToCheck, oppositePieces);
                else takeback(oppositePieces, pieceToCheck);


                if (oppositePieces.exists(move.end())) m_attackingMoves.push_back(move);
                else m_quietMoves.push_back(move);

                if (m_piece[move.start()] == 1 && (move.end() <= 63 && move.end() >= 56) ||
                (move.end() <= 7 && move.end() >= 0)) {
                    for (int prom : m_possibleProm) {
                        move.setSpe(prom);
                        m_quietMoves.push_back(move);
                    }
                }
            }
        }
    }

    //Permet à la fonction alpha-beta de tester les captures en premieres
    for(auto & m_attackingMove : m_attackingMoves){
        m_moves.push_back(m_attackingMove);
    }
    for(auto & m_quietMove : m_quietMoves){
        m_moves.push_back(m_quietMove);
    }
}

void BoardRepresentation::generateCaptures() {
    //Pareil que moveGenerator(), mais ne s'occupe que des captures pour améliorer la vitesse de la recherche quiescence

    m_attackingMoves.clear();

    PieceList pieceToCheck = m_sideToMove == 1 ? m_whitePieces : m_blackPieces;
    PieceList enemyPiece = m_sideToMove == 1 ? m_blackPieces : m_whitePieces;
    int sideToMove = m_sideToMove;

    for(int i = 0; i < pieceToCheck.getAdresses().size(); i++){
        int start = pieceToCheck.getAdresses().at(i);
        for(int j = 0; j < enemyPiece.getAdresses().size(); j++){
            int end = enemyPiece.getAdresses().at(j);
            Move move = Move(start, end);
            if(makeMove(move)){
                if(sideToMove == 1) takeback(pieceToCheck, enemyPiece);
                else takeback(enemyPiece, pieceToCheck);

                if(m_piece[end] != 0) m_attackingMoves.push_back(move);
            }
        }
    }
}



void BoardRepresentation::takeback(PieceList whitePieces, PieceList blackPieces) {
    m_whitePieces = std::move(whitePieces);
    m_blackPieces = std::move(blackPieces);

    if(m_positionHistory.size() > 1) m_positionHistory.pop();
    setFEN(m_positionHistory.top());
    m_positionHash = getHash();
}

void BoardRepresentation::showCurrentPosition() {
    std::string fen = getFEN();
    int compteurLigne = 8;

    std::cout << (m_sideToMove == 1 ? "White" : "Black") << " to move !" << std::endl;

    std::cout << compteurLigne << " | ";

    for(char i : fen){
        if(i == ' ') break;
        if(i == '/'){
            compteurLigne--;
            std::cout << std::endl << compteurLigne << " | ";
        }
        else if(isdigit(i)){
            for(int j = 0; j < (i -48); j++){
                std::cout <<"  | ";
            }
        }
        else std::cout << i << " | ";
    }

    std::cout << std::endl << "  | A | B | C | D | E | F | G | H |" << std::endl;

    std::cout << "Evaluation : " << evalutation() << std::endl << std::endl;

    std::cout << "Move : " << m_coups << std::endl;
}



int BoardRepresentation::writtenMoveParser(std::string move) {
    //Si l'indicateur de file n'est pas une lettre, poubelle
    if((((int) move[0]) < ((int) 'a') || ((int) move[0]) > ((int) 'z')) ||
            ((int) move[1] < (int) '1' || (int) move[1] > (int) '8')) return -1;

    //Chaque file ajoute 1 à l'adresse, chaque rang ajoute 8.
    int adresse = 8*(((int) move[1] - (int) '0') - 1) + (int)move[0] - (int)'a';

    return adresse;
}


std::string BoardRepresentation::adressToMoveParser(int adress) {
    if(adress < 0 || adress > 63) return "invalid";

    char file =(char) (adress%8 +'a');
    char rang = (char) (adress/8 + '1');

    std::string move;
    move.append(1, file);
    move.append(1, rang);

    return move;
}

int BoardRepresentation::play(int engineSide){
    Engine engine = Engine(3, this);
    Engine* ptr_engine = &engine;

    playMove(engineSide, ptr_engine, true);
    return 0;
}


int BoardRepresentation::playMove(int engineSide, Engine* engine, bool perft = false) {

    //Si on utilise perft (calcul de performances en node/sec), on start le compteur ici
    m_nodeCount = 0;
    time_t moveStartTime;
    time_t moveEndTime;

    if(perft){
        moveStartTime = time(nullptr);
    }

    if(m_sideToMove == 1) m_coups++;
    int start;
    int end;
    int promotion = -1;

    moveGenerator();

    if(checkmated() == 1) return m_sideToMove;

    if(engineSide){
        Move engineMove = engine->search();
        start = engineMove.start();
        end = engineMove.end();
        promotion = engineMove.special();
        std::cout << "The engine plays " << start << " " << end << std::endl;
    }

    else{
        std::string humanMove;
        std::string humanStart;
        std::string humanEnd;
        std::cout << "Enter the move you want to play : (forme <depart><arrivee><promotion>) " << std::endl;
        std::cin >> humanMove;
        if(humanMove.size() > 4) humanStart = humanMove.substr(0, 2), humanEnd = humanMove.substr(2, 2), promotion = (int) humanMove.substr(4, 1)[0] - (int)'0';
        else humanStart = humanMove.substr(0, 2), humanEnd = humanMove.substr(2, 2);

        start = writtenMoveParser(humanStart); end = writtenMoveParser(humanEnd);
    }

    Move move = Move(start, end, promotion);

    if(!makeMove(move)){
        std::cout << "This move isn't legal ! Please enter a legal move" << std::endl << std::endl;
        playMove(engineSide, engine, perft);
    }

    else{
        showCurrentPosition();

        if(perft){
            moveEndTime = time(nullptr);
            double elapsed = difftime(moveEndTime, moveStartTime);

            std::cout << "Calculated " << m_nodeCount << " nodes in " << elapsed << " seconds. (" << (int)(m_nodeCount/elapsed) << " node/sec)" << std::endl;
        }

        playMove(engineSide, engine, perft);
    }

    if(checkmated() == -1) return -1;
    return m_sideToMove;
}

int BoardRepresentation::checkmated() {
    if(m_moves.empty() && inCheck(m_sideToMove)) return 1;
    else if(m_moves.empty() || m_fifty >= 100) return -1;
    return 0;
}

std::vector<Move> BoardRepresentation::getMoves() {
    return m_moves;
}

int BoardRepresentation::evalutation() {
    int whiteEval = 0;
    int wbishopPair = 0, wrookPair = 0, wknightPair = 0;
    for(int i = 0; i < m_whitePieces.getAdresses().size(); i++){
        int j = m_whitePieces.getAdresses().at(i);
        int jA = (7 - (int)(j/8))*8 + j%8;
        switch(m_piece[j]){
            case 1 : whiteEval += 100 + m_pieceSquare[0][jA]; break;
            case 3 : whiteEval += 350 + m_pieceSquare[1][jA]; wknightPair++; break;
            case 4 : whiteEval += 350 + m_pieceSquare[2][jA]; wbishopPair++; break;
            case 5 : whiteEval += 525 + m_pieceSquare[3][jA]; wrookPair++; break;
            case 9 : whiteEval += 1000 + m_pieceSquare[4][jA]; break;
            case 100 : whiteEval += 10000 + m_pieceSquare[5][jA]; break;
            default: break;
        }
    }

    int blackEval = 0;
    int bbishopPair = 0, brookPair = 0, bknightPair = 0;
    for(int i = 0; i < m_blackPieces.getAdresses().size(); i++){
        int j = m_blackPieces.getAdresses().at(i);
        switch(m_piece[j]){
            case 1 : blackEval += 100 + m_pieceSquare[0][j]; break;
            case 3 : blackEval += 350 + m_pieceSquare[1][j]; bknightPair++; break;
            case 4 : blackEval += 350 + m_pieceSquare[2][j]; bbishopPair++; break;
            case 5 : blackEval += 525 + m_pieceSquare[3][j]; brookPair++; break;
            case 9 : blackEval += 1000 + m_pieceSquare[4][j]; break;
            case 100 : blackEval += 10000 + m_pieceSquare[5][j]; break;
            default: break;
        }
    }

    //Checks bishop pairs
    if(bbishopPair >= 2) blackEval += 50;
    if(wbishopPair >= 2) whiteEval += 50;
    if(bknightPair >= 2) blackEval -= 50;
    if(wknightPair >= 2) whiteEval -= 50;
    if(brookPair >= 2) blackEval -= 50;
    if(wrookPair >= 2) whiteEval -= 50;

    return whiteEval - blackEval;
}


