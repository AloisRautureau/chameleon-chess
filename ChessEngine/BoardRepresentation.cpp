//
// Created by bitterboyy on 12/11/20.
//

#include "BoardRepresentation.h"
#include "Engine.h"
#include "Utility.h"
#include <sstream>

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

/*
 * LEGALITE DES COUPS
 * Ces fonctions servent à vérifier ensembles la légalité des coups :
 * isPseudoLegal vérifie q'un coup respecte les règles de mouvement des échecs
 * inCheck vérifie si le roi est en échec ou non
 */

bool BoardRepresentation::inCheck(int side) {
    //On cherche le roi vite fait
    int kingPosition = -1;
    for(int i = 0; i < 64; i++){
        if(m_piece[i] == 100 && m_color[i] == side) kingPosition = i;
    }

    //On itère ensuite toooous les moves disponibles partant de chaque case à celle du roi pour voir si ils sont légaux
    int opposingSide = side == 1 ? 2 : 1;
    for(int i = 0; i < 64; i++){
        if(i != kingPosition && isPseudoLegal(i, kingPosition, opposingSide)){
            return true;
        }
    }
    return false;
}

bool BoardRepresentation::isPseudoLegal(int startAdress, int endAdress, int side) {
    bool w_kingside, w_queenside, b_kingside, b_queenside;
    int checkedAdress;
    int lastChecked;
    int direction = 0;

    //Si la pièce de la case de départ n'est pas à nous, le move est forcément faux
    //Même si chose si la pièce sur la case d'arrivée est de notre couleur bien entendu
    if(m_color[startAdress] != side || m_color[endAdress] == side) return false;

    //On check maintenant les cas des différentes pièces :
    switch(m_piece[startAdress]){
        case 1:
            //Les moves des pions dépendent de leur couleur
            switch(m_color[startAdress]){
                //pion blanc
                case 1:
                    //Si la pièce de la case d'arrivée est noire et en diagonale d'un pas du pion, le coup est valable
                    //Marche également si cette case est écrite dans m_enPassant
                    if(((m_color[endAdress] == 2 && (endAdress - startAdress == 7 || endAdress - startAdress == 9))
                    || (endAdress == m_enPassant && (endAdress - startAdress == 7 || endAdress - startAdress == 9)))
                    && abs(startAdress % 8 - endAdress % 8) <= 1) return true;

                    //Si le pion est sur son rang de départ, il peut avancer de deux cases
                    //Dans ce cas de figure, on ajoute la case en passant
                    else if(startAdress < 16 && startAdress > 7 && endAdress - startAdress == 16 && m_piece[endAdress] == 0){
                        m_enPassant = startAdress + 8;
                        return true;
                    }

                    //Sinon, le pion n'avance que d'une case en avant, si eu seulement si il n'y a pas de pièce sur la case d'arrivée
                    else if(endAdress - startAdress != 8 || m_piece[endAdress] != 0) return false;
                    break;

                //pion noir
                case 2:
                    //Pareil mais inversé
                    if(((m_color[endAdress] == 1 && (endAdress - startAdress == -7 || endAdress - startAdress == -9))
                       || (endAdress == m_enPassant && (endAdress - startAdress == -7 || endAdress - startAdress == -9)))
                       && abs(startAdress % 8 - endAdress % 8) <= 1) return true;

                    else if(startAdress <= 55 && startAdress >= 48 && endAdress - startAdress == -16 && m_piece[endAdress] == 0){
                        m_enPassant = startAdress - 8;
                        return true;
                    }
                    else if(endAdress - startAdress != -8 || m_piece[endAdress] != 0) return false;
                    break;

                //Case vide ou invalide
                default:
                    break;
            }
            break;

        //Cavalier
        case 3:
            //El famoso L
            if(abs(endAdress - startAdress) != 6
                && abs(endAdress - startAdress) != 15
                && abs(endAdress - startAdress) != 17
                && abs(endAdress - startAdress) != 10
                ||(((endAdress%8) - (startAdress%8) > 2 || (endAdress%8) - (startAdress%8) < -2)
                || ((endAdress/8) - (startAdress/8)) > 2) || (((endAdress/8) - (startAdress/8)) < -2)) return false;
            break;

        //Fou
        case 4:
            //On check dans toutes les directions, pas à pas, si le move souhaité est disponible.
            //On arrête chaque branche si l'on tombe sur une pièce ou sur la fin de la board
            //Ne se déplace qu'en diagonale
            for(int i = 0; i < 4; i++){
                checkedAdress = startAdress;
                lastChecked = startAdress;
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
                    else if(checkedAdress == endAdress) return true;
                    else if(m_piece[checkedAdress]!=0) break;
                    lastChecked = checkedAdress;
                }while(true);
            }
            return false;

        //Rook
        case 5:
            //Diagonale haute gauche
            for(int i = 0; i < 4; i++){
                lastChecked = startAdress;
                checkedAdress = startAdress;
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
                    else if(checkedAdress == endAdress) return true;
                    else if(m_piece[checkedAdress]!=0) break;

                    lastChecked = checkedAdress;
                }while(true);
            }
            return false;

        //Queen
        case 9:
            for(int i = 0; i < 8; i++){
                checkedAdress = startAdress;
                lastChecked = startAdress;
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
                    else if(checkedAdress == endAdress) return true;
                    else if(m_piece[checkedAdress]!=0)break;

                    lastChecked = checkedAdress;
                }while(true);
            }
            return false;

        //Roi
        case 100:
            //Toutes directions, mais d'un seul pas ou castle
            //Castling
            if((startAdress == 4 || startAdress == 60) && abs(endAdress - startAdress) == 2 && m_piece[startAdress] == 100) {
                w_queenside = m_castlingRights >= 0x0100 && !inCheck(1)
                              && isPseudoLegal(0, 3, 1) && m_piece[0] == 5
                              && !underAttack(2, 2) && !underAttack(3, 2)
                              && startAdress == 4 && endAdress == 2;

                w_kingside = m_castlingRights >= 0x1000 && !inCheck(1)
                             && isPseudoLegal(7, 5, 1) && m_piece[7] == 5
                             && !underAttack(6, 2) && !underAttack(5, 2)
                             && startAdress == 4 && endAdress == 6;

                b_queenside = m_castlingRights >= 0x0001 && !inCheck(2)
                              && isPseudoLegal(56, 59, 2) && m_piece[56] == 5
                              && !underAttack(59, 1) && !underAttack(58, 1)
                              && startAdress == 60 && endAdress == 58;

                b_kingside = m_castlingRights >= 0x0010 && !inCheck(2)
                             && isPseudoLegal(63, 61, 2) && m_piece[63] == 5
                             && !underAttack(61, 1) && !underAttack(62, 1)
                             && startAdress == 60 && endAdress == 62;

                if(w_queenside || w_kingside || b_queenside || b_kingside) return true;
            }

            if(abs(endAdress - startAdress) != 1
                && abs(endAdress - startAdress) != 9
                && abs(endAdress - startAdress) != 8
                && abs(endAdress - startAdress) != 7
                || abs(endAdress%8 - startAdress%8) > 1) return false;


            break;
            //La case est vide ou invalide
        default:
            return false;

    }
    return true;
}

bool BoardRepresentation::underAttack(int square, int attackingSide){
    for(int i = 0; i < 64; i++){
        if(m_color[i] != attackingSide) break;
        else if(i != square && isPseudoLegal(i, square, attackingSide)) return true;
    }
    return false;
}



bool BoardRepresentation::makeMove(int startAdress, int endAdress, int special) {

    //Permet de checker les changements de m_enPassant
    int enPassantStart = m_enPassant;

    //Checks if the move is pseudoLegal to start, if not we don't bother
    if(!isPseudoLegal(startAdress, endAdress, m_sideToMove)) return false;

    //Checks if we want to take enPassant
    if(endAdress == m_enPassant && m_piece[startAdress] == 1){
        if(m_sideToMove == 1){
            m_piece[endAdress - 8] = 0; m_color[endAdress - 8] = 0;
        }
        else{
            m_piece[endAdress + 8] = 0; m_color[endAdress + 8] = 0;
        }
    }

    //Castling
    if((startAdress == 4 || startAdress == 60) && abs(endAdress - startAdress) == 2 && m_piece[startAdress] == 100){
        switch(endAdress){
            case 62:
                m_piece[63] = 0; m_color[63] = 0;
                m_piece[61] = 5; m_color[61] = 2;
                m_castlingRights -= 0x0011;
                break;

            case 58:
                m_piece[56] = 0; m_color[56] = 0;
                m_piece[59] = 5; m_color[59] = 2;
                m_castlingRights -= 0x0011;
                break;

            case 6:
                m_piece[7] = 0; m_color[7] = 0;
                m_piece[5] = 5; m_color[5] = 1;
                m_castlingRights -= 0x1100;
                break;

            case 2:
                m_piece[0] = 0; m_color[0] = 0;
                m_piece[3] = 5; m_color[3] = 1;
                m_castlingRights -= 0x1100;
                break;

            default : break;
        }
    }

    //Regle des 50 coups (qui apparemment sont en fait 100 coups wtf)
    if(m_piece[endAdress] == 0 || m_piece[startAdress] != 1) m_fifty++;
    else m_fifty = 0;

    bool isPromotable = m_piece[startAdress] == 1;

    m_piece[endAdress] = m_piece[startAdress];
    m_color[endAdress] = m_color[startAdress];
    m_piece[startAdress] = 0;
    m_color[startAdress] = 0;

    if(isPromotable && ((endAdress <= 63 && endAdress >= 56) || (endAdress <= 7 && endAdress >= 0))){
        switch(special){
            case 3:
                m_piece[endAdress] = 3;
                break;

            case 4:
                m_piece[endAdress] = 4;
                break;

            case 5:
                m_piece[endAdress] = 5;
                break;

            default:
                m_piece[endAdress] = 9;
                break;
        }
    }

    int currentSide = m_sideToMove;
    m_sideToMove = m_sideToMove == 1 ? 2: 1;

    //Si m_enPassant == enPAssantStart, on reset
    m_enPassant = m_enPassant == enPassantStart ? -1 : m_enPassant;

    m_positionHistory.push(getFEN());

    if(inCheck(currentSide)){
        takeback();
        return false;
    }

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

    std::vector<int> coup;

    for(int caseDepart = 0; caseDepart < 64; caseDepart++){
        for(int caseArrivee = 0; caseArrivee < 64; caseArrivee++){
            if(makeMove(caseDepart, caseArrivee, -1)){
                coup.clear();
                coup.push_back(caseDepart);
                coup.push_back(caseArrivee);
                coup.push_back(-1);

                takeback();
                m_moves.push_back(coup);

                if(m_piece[caseDepart] == 1 && (caseArrivee <= 63 && caseArrivee >= 56) || (caseArrivee <= 7 && caseArrivee >= 0)){
                    for(int prom : m_possibleProm){
                        coup.pop_back();
                        coup.push_back(prom);
                        m_moves.push_back(coup);
                    }
                }

            }
        }
    }
}

void BoardRepresentation::takeback() {
    if(m_positionHistory.size() > 1) m_positionHistory.pop();
    setFEN(m_positionHistory.top());
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

int BoardRepresentation::play(int engineSide, int cvSide) {

    if(m_sideToMove == 1) m_coups++;

    std::string move;
    std::string start;
    std::string end;
    int promotion = -1;

    moveGenerator();

    if(checkmated()) return m_sideToMove;

    if(engineSide == m_sideToMove){
        auto* engine = new Engine(3, m_sideToMove, this);
        std::vector<int> moves = engine->minimax();
        //std::vector<int> move = engine->valueBasedMove();
        start = adressToMoveParser(moves.at(0));
        end = adressToMoveParser(moves.at(1));
        promotion = moves.at(2);
    }

    else if(cvSide == m_sideToMove){
        auto* engine = new Engine(7, m_sideToMove, this);
        std::vector<int> moves = engine->minimax();
        //std::vector<int> move = engine->valueBasedMove();
        start = adressToMoveParser(moves.at(0));
        end = adressToMoveParser(moves.at(1));
        promotion = moves.at(2);
    }

    else{
        std::cout << "Enter the move you want to play : (forme <depart><arrivee><promotion>) " << std::endl;
        std::cin >> move;
        if(move.size() > 4) start = move.substr(0, 2), end = move.substr(2, 2), promotion = (int) move.substr(4, 1)[0] - (int)'0';
        else start = move.substr(0, 2), end = move.substr(2, 2);
    }

    if(!makeMove(writtenMoveParser(start), writtenMoveParser(end), promotion)){
        std::cout << "This move isn't legal ! Please enter a legal move" << std::endl << std::endl;
        play(engineSide, cvSide);
    }

    else{
        showCurrentPosition();
        play(engineSide, cvSide);
    }
    return -1;
}

int BoardRepresentation::checkmated() {
    if(m_moves.empty() && inCheck(m_sideToMove)) return 1;
    else if(m_moves.empty() || m_fifty >= 100) return -1;
    return 0;
}

std::vector<std::vector<int>> BoardRepresentation::getMoves() {
    return m_moves;
}

float BoardRepresentation::evalutation() {
    float eval = 0;
    for(int i = 0; i < 64; i++){
        if(m_color[i] == 1){
            switch(m_piece[i]){
                case 0:
                    break;

                case 1:
                    eval += 100;
                    eval += m_pieceSquare[0][i];
                    break;

                case 3:
                    eval += m_pieceSquare[1][i];
                case 4:
                    eval += 300;
                    eval += m_pieceSquare[2][i];
                    break;

                case 5:
                    eval += 500;
                    eval += m_pieceSquare[3][i];
                    break;

                case 9:
                    eval += 900;
                    eval += m_pieceSquare[4][i];
                    break;

                case 100:
                    eval += 9999;
                    eval += m_pieceSquare[5][i];
                    break;

                default:
                    break;
            }
        }

        else{
            switch(m_piece[i]){
                case 0:
                    break;

                case 1:
                    eval -= 100;
                    eval -= m_pieceSquare[0][i];
                    break;

                case 3:
                    eval -= m_pieceSquare[1][i];
                case 4:
                    eval -= 300;
                    eval -= m_pieceSquare[2][i];
                    break;

                case 5:
                    eval -= 500;
                    eval -= m_pieceSquare[3][i];
                    break;

                case 9:
                    eval -= 900;
                    eval -= m_pieceSquare[4][i];
                    break;

                case 100:
                    eval -= 9999;
                    eval -= m_pieceSquare[5][i];
                    break;

                default:
                    break;
            }
        }
    }
    //Checking is considered a bonus to avoid passive games
    if(inCheck(1)) eval -= 50;
    else if (inCheck(2)) eval += 50;
    return eval/10;
}
