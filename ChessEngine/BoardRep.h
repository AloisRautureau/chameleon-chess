//
// Created by bitterboyy on 1/9/21.
//

#ifndef BAUB_CHESS_BOARDREP_H
#define BAUB_CHESS_BOARDREP_H

#include "Constants.h"




class BoardRep {
protected:
    //Deux array pour décrire la position actuelle, l'un pour les couleurs des pièces, l'autre pour leur type
    int m_pieces[64] = {
            3, 1, 2, 4, 5, 2, 1, 3,
            0, 0, 0, 0, 0, 0, 0, 0,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            0, 0, 0, 0, 0, 0, 0, 0,
            3, 1, 2, 4, 5, 2, 1, 3
    };
    int m_color[64] = {
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1
    };

    //Side to move, pour savoir qui joue
    int m_sideToMove = WHITE;
    int m_oppositeSide = BLACK;

    //Droits de roquer (<blancKing><blancQueen><blackKing><blackQueen>
    int m_castling = 0x1111;

    //Donne le move auquel on est, ainsi que la règle des 50 coups
    int m_fiftyM = 0;
    int m_coup = 0;

    //Garde en mémoire une éventuelle cas ou en passant est possible
    int m_enPassant = -1;

    //Garde en mémoire les coups joués pour les takeback
    std::stack<Position> m_history;

    MoveStack m_moveStack;

public:
    //Check si un coup est pseudo légal (les règles de déplacement l'autorise, mais il peut mettre le roi en échec
    bool isUnderAttack(int square, int attackingSide) {
        //On prends les pièces du joueur adverse
        std::vector<int> attackingPieces;
        bool done;

        for (int pieceAdresse : m_pieces) {
            if (m_color[pieceAdresse] != attackingSide) continue;
            done = false;
            int piece = m_pieces[pieceAdresse];
            //La pièce appartient au joueur, on vérifie donc qu'elle peut atteindre la square d'arrivée
            switch (piece) {
                case PAWN:
                    if (attackingSide == WHITE) {
                        if ((square - pieceAdresse == 9 && pieceAdresse % 8 != 0)
                            || (square - pieceAdresse == 7 && pieceAdresse % 8 != 7))
                            attackingPieces.push_back(pieceAdresse);
                    } else {
                        if ((square - pieceAdresse == -9 && pieceAdresse % 8 != 7)
                            || (square - pieceAdresse == -7 && pieceAdresse % 8 != 0))
                            attackingPieces.push_back(pieceAdresse);
                    }
                    break;

                case KNIGHT:
                    if ((abs(square - pieceAdresse) == 10
                         || abs(square - pieceAdresse) == 17
                         || abs(square - pieceAdresse) == 6
                         || abs(square - pieceAdresse) == 15)
                        && mailbox[mailbox64[pieceAdresse] - (square - pieceAdresse)] != -1)
                        attackingPieces.push_back(pieceAdresse);
                    break;

                case BISHOP:
                    for (int i = 0; i < sizeof(directionsFou); i++) {
                        int coupActuel = pieceAdresse + directionsFou[i];
                        while (coupActuel != square && !done) {
                            if ((coupActuel > 63 || coupActuel < 0)
                                || mailbox[mailbox64[pieceAdresse] - coupActuel - pieceAdresse] == -1
                                || m_color[coupActuel] == attackingSide)
                                done = true;
                            else if (coupActuel == square) attackingPieces.push_back(pieceAdresse);
                            coupActuel += directionsFou[i];
                        }
                    }
                    break;

                case ROOK:
                    for (int i = 0; i < sizeof(directionsTour); i++) {
                        int coupActuel = pieceAdresse + directionsTour[i];
                        while (coupActuel != square && !done) {
                            if ((coupActuel > 63 || coupActuel < 0)
                                || mailbox[mailbox64[pieceAdresse] - coupActuel - pieceAdresse] == -1
                                || m_color[coupActuel] == attackingSide)
                                done = true;
                            else if (coupActuel == square) attackingPieces.push_back(pieceAdresse);
                            coupActuel += directionsTour[i];
                        }
                    }
                    break;

                case QUEEN:
                    for (int i = 0; i < sizeof(directionsDame); i++) {
                        int coupActuel = pieceAdresse + directionsDame[i];
                        while (coupActuel != square && !done) {
                            if ((coupActuel > 63 || coupActuel < 0)
                                || mailbox[mailbox64[pieceAdresse] - coupActuel - pieceAdresse] == -1
                                || m_color[coupActuel] == attackingSide)
                                done = true;
                            else if (coupActuel == square) attackingPieces.push_back(pieceAdresse);
                            coupActuel += directionsDame[i];
                        }
                    }
                    break;

                case KING:
                    if ((abs(square - pieceAdresse) == 7 ||
                         abs(square - pieceAdresse) == 9 ||
                         abs(square - pieceAdresse) == 8 ||
                         abs(square - pieceAdresse) == 1)
                        && mailbox[mailbox64[pieceAdresse] - (square - pieceAdresse)] != -1)
                        attackingPieces.push_back(pieceAdresse);

                default:
                    break;
            }
        }

        //La pièce n'appartient pas au joueur
        if (attackingPieces.empty()) { return false; }
        else return true;
    }

    //Check si le roi est en échec
    bool inCheck(int side) {
        int oppositeSide = side == WHITE ? BLACK : WHITE;
        for (int i = 0; i < 64; i++) {
            if (m_pieces[i] == KING && m_color[i] == side) {
                bool check = isUnderAttack(i, oppositeSide);
                return check;
            }
        }
        return true;
    }

    /*
     * GENERATEURS
     */

    //Generateur de coups general
    void generatePseudoLegal() {
        m_moveStack.reset();
        int done;

        //On check pour chaque pièce du côté attaquant, quelles cases elle peut atteindre
        //Ca génère les coups de base des pièces
        for (int adresse = 0; adresse < 64; adresse++) {
            if (m_color[adresse] != m_sideToMove) continue;
            done = false;
            int piece = m_pieces[adresse];
            //La pièce appartient au joueur, on vérifie donc qu'elle peut atteindre la square d'arrivée
            switch (piece) {
                case PAWN:
                    if (m_sideToMove == WHITE) {
                        if (m_color[adresse + 9] == BLACK && adresse % 8 != 0) {
                            Move move(adresse, adresse + 9, 0);
                            m_moveStack.storeMove(move, calcMoveScore(adresse, adresse + 9));
                        }
                        if (m_color[adresse + 7] == BLACK && adresse % 8 != 7) {
                            Move move(adresse, adresse + 7, 0);
                            m_moveStack.storeMove(move, calcMoveScore(adresse, adresse + 7));
                        }
                        if (m_pieces[adresse + 8] == EMPTY) {
                            Move move(adresse, adresse + 8, 0);
                            m_moveStack.storeMove(move, calcMoveScore(adresse, adresse + 8));
                            if (m_pieces[adresse + 16] == EMPTY) {
                                move = Move(adresse, adresse + 16, -4);
                                m_moveStack.storeMove(move, calcMoveScore(adresse, adresse + 16));
                            }
                        }
                        if (m_enPassant != -1) {
                            if (m_enPassant - adresse == 7 && adresse % 8 != 0) {
                                m_moveStack.storeMove(Move(adresse, m_enPassant, -2),
                                                          calcMoveScore(adresse, m_enPassant));
                            }
                            if (m_enPassant - adresse == 9 && adresse % 8 != 7) {
                                m_moveStack.storeMove(Move(adresse, m_enPassant, -2),
                                                          calcMoveScore(adresse, m_enPassant));
                            }
                        }

                    } else {
                        if (m_color[adresse - 9] == WHITE && adresse % 8 != 7) {
                            Move move(adresse, adresse - 9, 0);
                            m_moveStack.storeMove(move, calcMoveScore(adresse, adresse - 9));
                        }
                        if (m_color[adresse - 7] == WHITE && adresse % 8 != 0) {
                            Move move(adresse, adresse - 7, 0);
                            m_moveStack.storeMove(move, calcMoveScore(adresse, adresse - 7));
                        }
                        if (m_pieces[adresse - 8] == EMPTY) {
                            Move move(adresse, adresse - 8, 0);
                            m_moveStack.storeMove(move, calcMoveScore(adresse, adresse - 8));
                            if (m_pieces[adresse - 16] == EMPTY) {
                                move = Move(adresse, adresse - 16, -4);
                                m_moveStack.storeMove(move, calcMoveScore(adresse, adresse - 16));
                            }
                        }
                        if (m_enPassant != -1) {
                            if (m_enPassant - adresse == -7 && adresse % 8 != 7) {
                                m_moveStack.storeMove(Move(adresse, m_enPassant, -2),
                                                          calcMoveScore(adresse, m_enPassant));
                            }
                            if (m_enPassant - adresse == -9 && adresse % 8 != 0) {
                                m_moveStack.storeMove(Move(adresse, m_enPassant, -2),
                                                          calcMoveScore(adresse, m_enPassant));
                            }
                        }
                    }
                    break;

                case KNIGHT:
                    for (int i = 0; i < sizeof(pieceMovements[0]); i++) {
                        if (m_color[adresse + pieceMovements[0][i]] != m_sideToMove
                            && adresse + pieceMovements[0][i] >= 0 && adresse + pieceMovements[0][i] <= 63
                            && mailbox[mailbox64[adresse] + pieceMailboxMovements[0][i]] != -1) {
                            m_moveStack.storeMove(Move(adresse, adresse + pieceMovements[0][i], -1),
                                                      calcMoveScore(adresse, adresse + pieceMovements[0][i]));
                        }
                    }
                    break;

                case BISHOP:
                    for (int movement : pieceMovements[1]) {
                        int coupActuel = adresse + movement;
                        while (!done) {
                            if ((coupActuel > 63 || coupActuel < 0)
                                || mailbox[mailbox64[adresse] - coupActuel - adresse] == -1
                                || m_color[coupActuel] == m_sideToMove)
                                done = true;
                            else m_moveStack.storeMove(Move(adresse, coupActuel, -1),
                                                      calcMoveScore(adresse, coupActuel));
                            coupActuel += movement;
                        }
                    }
                    break;

                case ROOK:
                    for (int movement : pieceMovements[2]) {
                        int coupActuel = adresse + movement;
                        while (!done) {
                            if ((coupActuel > 63 || coupActuel < 0)
                                || mailbox[mailbox64[adresse] - coupActuel - adresse] == -1
                                || m_color[coupActuel] == m_sideToMove)
                                done = true;
                            else m_moveStack.storeMove(Move(adresse, coupActuel, -1),
                                                      calcMoveScore(adresse, coupActuel));
                            coupActuel += movement;
                        }
                    }
                    break;

                case QUEEN:
                    for (int movement : pieceMovements[3]) {
                        int coupActuel = adresse + movement;
                        while (!done) {
                            if ((coupActuel > 63 || coupActuel < 0)
                                || mailbox[mailbox64[adresse] - coupActuel - adresse] == -1
                                || m_color[coupActuel] == m_sideToMove)
                                done = true;
                            else m_moveStack.storeMove(Move(adresse, coupActuel, -1),
                                                      calcMoveScore(adresse, coupActuel));
                            coupActuel += movement;
                        }
                    }
                    break;

                case KING:
                    for (int movement : pieceMovements[3]) {
                        int coupActuel = adresse + movement;
                        if (!((coupActuel > 63 || coupActuel < 0)
                              || mailbox[mailbox64[adresse] - coupActuel - adresse] == -1
                              || m_color[coupActuel] == m_sideToMove))
                                m_moveStack.storeMove(Move(adresse, coupActuel, -1),
                                                      calcMoveScore(adresse, coupActuel));
                    }
                    break;

                default:
                    break;
            }
        }

        //On ajoute à ça les moves de castle
        int castle = m_castling;

        while (castle > 0) {
            if (castle >= 0x1000 && !inCheck(m_sideToMove) && !isUnderAttack(5, m_oppositeSide) &&
                !isUnderAttack(6, m_oppositeSide)
                && m_pieces[5] == EMPTY && m_pieces[6] == EMPTY) {
                if (m_sideToMove == WHITE) {
                    m_moveStack.storeMove(Move(4, 6, -3), 500);
                }
                castle -= 0x1000;
            } else if (castle >= 0x0100 && !inCheck(m_sideToMove) && !isUnderAttack(3, m_oppositeSide) &&
                       !isUnderAttack(2, m_oppositeSide)
                       && m_pieces[3] == EMPTY && m_pieces[2] == EMPTY && m_pieces[1] == EMPTY) {
                if (m_sideToMove == WHITE) {
                    m_moveStack.storeMove(Move(4, 2, -3), 500);
                }
                castle -= 0x0100;
            } else if (castle >= 0x0010 && !inCheck(m_sideToMove) && !isUnderAttack(61, m_oppositeSide) &&
                       !isUnderAttack(62, m_oppositeSide)
                       && m_pieces[61] == EMPTY && m_pieces[62] == EMPTY) {
                if (m_sideToMove == BLACK) {
                    m_moveStack.storeMove(Move(60, 62, -3), 500);
                }
                castle -= 0x0010;
            } else if (castle >= 0x0010 && !inCheck(m_sideToMove) && !isUnderAttack(59, m_oppositeSide) &&
                       !isUnderAttack(58, m_oppositeSide)
                       && m_pieces[59] == EMPTY && m_pieces[58] == EMPTY && m_pieces[57] == EMPTY) {
                if (m_sideToMove == BLACK) {
                    m_moveStack.storeMove(Move(60, 58, -3), 500);
                }
                castle -= 0x0001;
            } else {
                castle = 0;
            }
        }
    }

    //Pareil, mais ne génère que les captures pour optimiser la recherche de quiétude
    void generateCaptures() {
        m_moveStack.reset();
        int done;

        //On check pour chaque pièce du côté attaquant, quelles cases elle peut atteindre
        //Ca génère les coups de base des pièces
        for (int adresse = 0; adresse < 64; adresse++) {
            if (m_color[adresse] != m_sideToMove) continue;
            done = false;
            int piece = m_pieces[adresse];
            //La pièce appartient au joueur, on vérifie donc qu'elle peut atteindre la square d'arrivée
            switch (piece) {
                case PAWN:
                    if (m_sideToMove == WHITE) {
                        if (m_color[adresse + 9] == BLACK && adresse % 8 != 0) {
                            Move move(adresse, adresse + 9, 0);
                            m_moveStack.storeMove(move, calcMoveScore(adresse, adresse + 9));
                        }
                        if (m_color[adresse + 7] == BLACK && adresse % 8 != 7) {
                            Move move(adresse, adresse + 7, 0);
                            m_moveStack.storeMove(move, calcMoveScore(adresse, adresse + 7));
                        }
                        if (m_enPassant != -1) {
                            if (m_enPassant - adresse == 7 && adresse % 8 != 0) {
                                m_moveStack.storeMove(Move(adresse, m_enPassant, -2),
                                                      calcMoveScore(adresse, m_enPassant));
                            }
                            if (m_enPassant - adresse == 9 && adresse % 8 != 7) {
                                m_moveStack.storeMove(Move(adresse, m_enPassant, -2),
                                                      calcMoveScore(adresse, m_enPassant));
                            }
                        }

                    } else {
                        if (m_color[adresse - 9] == WHITE && adresse % 8 != 7) {
                            Move move(adresse, adresse - 9, 0);
                            m_moveStack.storeMove(move, calcMoveScore(adresse, adresse - 9));
                        }
                        if (m_color[adresse - 7] == WHITE && adresse % 8 != 0) {
                            Move move(adresse, adresse - 7, 0);
                            m_moveStack.storeMove(move, calcMoveScore(adresse, adresse - 7));
                        }
                        if (m_enPassant != -1) {
                            if (m_enPassant - adresse == -7 && adresse % 8 != 7) {
                                m_moveStack.storeMove(Move(adresse, m_enPassant, -2),
                                                          calcMoveScore(adresse, m_enPassant));
                            }
                            if (m_enPassant - adresse == -9 && adresse % 8 != 0) {
                                m_moveStack.storeMove(Move(adresse, m_enPassant, -2),
                                                      calcMoveScore(adresse, m_enPassant));
                            }
                        }
                    }
                    break;

                case KNIGHT:
                    for (int i = 0; i < sizeof(pieceMovements[0]); i++) {
                        if (m_color[adresse + pieceMovements[0][i]] != m_sideToMove
                            && adresse + pieceMovements[0][i] >= 0 && adresse + pieceMovements[0][i] <= 63
                            && mailbox[mailbox64[adresse] + pieceMailboxMovements[0][i]] != -1 &&
                            m_pieces[adresse + pieceMovements[0][i]] != EMPTY) {
                                m_moveStack.storeMove(Move(adresse, adresse + pieceMovements[0][i], -1),
                                                      calcMoveScore(adresse, adresse + pieceMovements[0][i]));
                        }
                    }
                    break;

                case BISHOP:
                    for (int movement : pieceMovements[1]) {
                        int coupActuel = adresse + movement;
                        while (!done) {
                            if ((coupActuel > 63 || coupActuel < 0)
                                || mailbox[mailbox64[adresse] - coupActuel - adresse] == -1
                                || m_color[coupActuel] == m_sideToMove)
                                done = true;
                            else m_moveStack.storeMove(Move(adresse, coupActuel, -1),
                                                      calcMoveScore(adresse, coupActuel));
                            coupActuel += movement;
                        }
                    }
                    break;

                case ROOK:
                    for (int movement : pieceMovements[2]) {
                        int coupActuel = adresse + movement;
                        while (!done) {
                            if ((coupActuel > 63 || coupActuel < 0)
                                || mailbox[mailbox64[adresse] - coupActuel - adresse] == -1
                                || m_color[coupActuel] == m_sideToMove)
                                done = true;
                            else m_moveStack.storeMove(Move(adresse, coupActuel, -1),
                                                      calcMoveScore(adresse, coupActuel));
                            coupActuel += movement;
                        }
                    }
                    break;

                case QUEEN:
                    for (int movement : pieceMovements[3]) {
                        int coupActuel = adresse + movement;
                        while (!done) {
                            if ((coupActuel > 63 || coupActuel < 0)
                                || mailbox[mailbox64[adresse] - coupActuel - adresse] == -1
                                || m_color[coupActuel] == m_sideToMove)
                                done = true;
                            else m_moveStack.storeMove(Move(adresse, coupActuel, -1),
                                                      calcMoveScore(adresse, coupActuel));
                            coupActuel += movement;
                        }
                    }
                    break;

                case KING:
                    for (int movement : pieceMovements[3]) {
                        int coupActuel = adresse + movement;
                        if (!((coupActuel > 63 || coupActuel < 0)
                              || mailbox[mailbox64[adresse] - coupActuel - adresse] == -1
                              || m_color[coupActuel] == m_sideToMove) && m_pieces[coupActuel] != EMPTY)
                                m_moveStack.storeMove(Move(adresse, coupActuel, -1),
                                                      calcMoveScore(adresse, coupActuel));
                    }
                    break;

                default:
                    break;
            }
        }
    }

    /*
     * PLAY FUNCTIONS
     */

    //does a move, and checks at the end if the king is in check
    //If he is, the move isn't legal and we takeback the move
    bool makeMove(Move move) {
        //We store takeback information in case we need it
        m_history.push(Position(m_castling, m_fiftyM, m_pieces, m_color));

        //Castling move
        if (move.special == -3) {
            //On check la case de fin pour déterminer le type de castle
            switch (move.end) {
                //White queenside
                case 2:
                    m_pieces[3] = ROOK;
                    m_color[3] = WHITE;
                    m_pieces[0] = EMPTY;
                    m_color[0] = EMPTY;
                    m_castling -= 0x1100;
                    break;

                    //White kingside
                case 6:
                    m_pieces[5] = ROOK;
                    m_color[5] = WHITE;
                    m_pieces[7] = EMPTY;
                    m_color[7] = EMPTY;
                    m_castling -= 0x1100;
                    break;

                    //Black kingside
                case 62:
                    m_pieces[61] = ROOK;
                    m_color[61] = BLACK;
                    m_pieces[63] = EMPTY;
                    m_color[63] = EMPTY;
                    m_castling -= 0x0011;
                    break;

                    //BLack queenside
                case 58:
                    m_pieces[59] = ROOK;
                    m_color[59] = BLACK;
                    m_pieces[56] = EMPTY;
                    m_color[56] = EMPTY;
                    m_castling -= 0x0011;
                    break;

                default:
                    break;
            }
        }

        //We update the info for enPassant, castling rights and increment/reset fifty move rule
        if (m_pieces[move.start] == KING) {
            m_castling -= m_sideToMove == WHITE ? 0x1100 : 0x0011;
        } else if (m_pieces[move.start] == ROOK) {
            switch (move.start) {
                case 0:
                    m_castling -= 0x0100;
                    break;

                case 7:
                    m_castling -= 0x1000;
                    break;

                case 56:
                    m_castling -= 0x0001;
                    break;

                case 63:
                    m_castling -= 0x0010;
                    break;

                default:
                    break;
            }
        }

        if (move.special == -4) {
            m_enPassant = m_sideToMove == WHITE ? move.start + 8 : move.start - 8;
        } else {
            m_enPassant = -1;
        }

        if (m_pieces[move.end] == EMPTY && m_pieces[move.start] != PAWN) m_fiftyM++;
        else m_fiftyM = 0;

        m_coup++;


        //Moving the piece in itself
        bool capture = m_pieces[move.end] != EMPTY;

        int movingPiece = m_pieces[move.start];
        m_pieces[move.start] = EMPTY;
        m_color[move.start] = EMPTY;
        m_pieces[move.end] = movingPiece;
        m_color[move.end] = m_sideToMove;


        //Promote the pawn if it ends up on the last rank
        if (m_pieces[move.end] == PAWN && ((move.end <= 63 && move.end >= 56) xor (move.end <= 7 && move.end >= 0))) {
            m_pieces[move.end] = move.special;
        }


        //In the case of an en passant move, we delete the pawn behind ours
        if (move.special == ENPASSANT) {
            if (m_sideToMove == WHITE) {
                m_pieces[move.end - 8] = EMPTY;
                m_color[move.end - 8] = EMPTY;
            } else {
                m_pieces[move.end + 8] = EMPTY;
                m_color[move.end + 8] = EMPTY;
            }
        }

        //We change side
        int side = m_sideToMove;
        m_sideToMove = m_oppositeSide;
        m_oppositeSide = side;

        //Check if the king is attacked, and takeback if it's the case
        if (inCheck(m_oppositeSide)) {
            takeback();
            return false;
        }

        return true;
    }

    //Annule le dernier coup joué
    void takeback() {
        //La position à remettre en place
        Position positionToRestore = m_history.top();

        //On commence par réinitialiser les attributs de base
        m_castling = positionToRestore.castling;
        m_sideToMove = m_sideToMove == WHITE ? BLACK : WHITE;
        m_oppositeSide = m_sideToMove == WHITE ? BLACK : WHITE;
        m_fiftyM = positionToRestore.fiftyMoveRule;

        for(int i = 0; i < 64; i++){
            m_pieces[i] = positionToRestore.pieces[i];
            m_color[i] = positionToRestore.color[i];
        }

        m_coup--;

        //On retire la position de l'historique
        m_history.pop();
    }

    //Prints the current position on screen
    void showCurrentPosition() {
        for (int i = 0; i < 64; i++) {
            int toPrint = (7 - (int) (i / 8)) * 8 + i % 8;
            //Changment de ligne
            if (i % 8 == 0) std::cout << std::endl << 8 - (int) i / 8 << " | ";

            switch (m_pieces[toPrint]) {
                case PAWN:
                    std::cout << (m_color[toPrint] == WHITE ? "P" : "p") << " | ";
                    break;

                case KNIGHT:
                    std::cout << (m_color[toPrint] == WHITE ? "N" : "n") << " | ";
                    break;

                case BISHOP:
                    std::cout << (m_color[toPrint] == WHITE ? "B" : "b") << " | ";
                    break;

                case ROOK:
                    std::cout << (m_color[toPrint] == WHITE ? "R" : "r") << " | ";
                    break;

                case QUEEN:
                    std::cout << (m_color[toPrint] == WHITE ? "Q" : "q") << " | ";
                    break;

                case KING:
                    std::cout << (m_color[toPrint] == WHITE ? "K" : "k") << " | ";
                    break;

                case EMPTY:
                    std::cout << "  | ";
            }
        }

        std::cout << std::endl << "  | ";

        for (int i = 0; i < 8; i++) {
            std::cout << (char) ('A' + i) << " | ";
        }

        std::cout << std::endl << std::endl << (m_sideToMove == WHITE ? "White " : "Black ") << "to move!" << std::endl;
    }

    //Checks if a side is in a checkmate/stalemate
    int checkmated(int side) {
        if (inCheck(side) && m_moveStack.empty()) return 1; //Checkmate
        else if (m_moveStack.empty()) return 0; //Stalemate
        else return -1;
    }

    /*
     * UTILITY
     */

    int calcMoveScore(int start, int end) {
        int score = 0;
        if (m_pieces[end] != EMPTY) {
            score = 10000 + m_pieces[end] * 10 - m_pieces[start] * 10;
        }
        return score;
    }

    MoveStack getMoveStack() { return m_moveStack; }

    int getSide() const { return m_sideToMove; }


    /*
     * EVALUATION
     */

    int evaluation() {
        //Deux array pour stocker les valeurs matérielles de chaque côté en séparant pions et pièces
        //Cette séparation servira à définir le moment ou on entre dans la fin de partie
        int material_pawns[2] = {0, 0};
        int material_pieces[2] = {0, 0};

        //Socre général dedeux cotés
        int scores[2] = {0, 0};

        //On itère les listes de pièces une première fois pour avoir les valeurs matérielles
        for (int i = 0; i < 64; i++) {
            switch (m_pieces[i]) {
                default:
                case EMPTY:
                    break;

                case PAWN:
                    if (m_color[i] == WHITE) {
                        material_pieces[WHITE] += pieceValue[PAWN];
                        scores[WHITE] += TBLPIONS[INVERT[i]];
                    } else {
                        material_pieces[BLACK] += pieceValue[PAWN];
                        scores[BLACK] += TBLPIONS[i];
                    }
                    break;

                case KNIGHT:
                    if (m_color[i] == WHITE) {
                        material_pieces[WHITE] += pieceValue[KNIGHT];
                        scores[WHITE] += TBLKNIGHT[INVERT[i]];
                    } else {
                        material_pieces[BLACK] += pieceValue[KNIGHT];
                        scores[BLACK] += TBLKNIGHT[i];
                    }
                    break;

                case BISHOP:
                    if (m_color[i] == WHITE) {
                        material_pieces[WHITE] += pieceValue[BISHOP];
                        scores[WHITE] += TBLBISHOP[INVERT[i]];
                    } else {
                        material_pieces[BLACK] += pieceValue[BISHOP];
                        scores[BLACK] += TBLBISHOP[i];
                    }
                    break;

                case ROOK:
                    if (m_color[i] == WHITE) {
                        material_pieces[WHITE] += pieceValue[ROOK];
                        scores[WHITE] += TBLROOK[INVERT[i]];
                    } else {
                        material_pieces[BLACK] += pieceValue[ROOK];
                        scores[BLACK] += TBLROOK[i];
                    }
                    break;

                case QUEEN:
                    if (m_color[i] == WHITE) {
                        material_pieces[WHITE] += pieceValue[QUEEN];
                        scores[WHITE] += TBLQUEEN[INVERT[i]];
                    } else {
                        material_pieces[BLACK] += pieceValue[QUEEN];
                        scores[BLACK] += TBLQUEEN[i];
                    }
                    break;

                case KING:
                    if (m_color[i] == WHITE) scores[WHITE] += pieceValue[KING];
                    else scores[BLACK] += pieceValue[KING];
                    break;
            }
        }
        scores[WHITE] += material_pieces[WHITE] + material_pawns[WHITE];
        scores[BLACK] += material_pieces[BLACK] + material_pawns[BLACK];

        //On réitère une dernière fois pour évaluer la position du roi
        for (int i = 0; i < 64; i++) {
            if (m_pieces[i] == KING) {
                if (m_color[i] == WHITE) {
                    scores[WHITE] += material_pieces[BLACK] <= 1350 ? TBLKING_END[INVERT[i]] : TBLKING[INVERT[i]];
                } else {
                    scores[BLACK] += material_pieces[WHITE] <= 1350 ? TBLKING_END[i] : TBLKING[i];
                }
            }
        }

        //Renvoit la différence de score, négative si les noirs ont l'avantage, positive si les blanc l'ont
        return scores[WHITE] - scores[BLACK];
    }

};
#endif //BAUB_CHESS_BOARDREP_H
