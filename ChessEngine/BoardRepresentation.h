//
// Created by bitterboyy on 12/11/20.
//

#ifndef BAUB_CHESS_BOARDREPRESENTATION_H
#define BAUB_CHESS_BOARDREPRESENTATION_H

#include <stack>
#include <string>
#include <iostream>
#include <random>
#include <utility>
#include <iterator>

class Engine;

class PieceList{
private:
    std::vector<int> m_adresses;

public:
    PieceList(){
        m_adresses = {-1};
    }

    explicit PieceList(std::vector<int> adresses){
        m_adresses = std::move(adresses);
    }

    std::vector<int> getAdresses(){
        return m_adresses;
    }

    void addAdress(int adress){
        m_adresses.push_back(adress);
    }

    bool exists(int adress){
        for(auto adressCheckes : m_adresses){
            if(adressCheckes == adress) return true;
        }
        return false;
    }

    void removeAdress(int adress){
        for(int i = 0; i < m_adresses.size(); i++){
            if(m_adresses.at(i) == adress) m_adresses.erase(m_adresses.begin() + i);
        }
    }
};

class Move{
private:
    int m_start;
    int m_end;
    int m_special = -1;

public:
    Move(int start, int end, int special = -1) : m_start(start), m_end(end), m_special(special){};

    void setSpe(int spe){m_special = spe;};

    int start(){return m_start;};
    int end(){return m_end;};
    int special(){return m_special;};
};

class BoardRepresentation {
protected:
    unsigned long long m_zobrist_hash[781];
    unsigned long long m_positionHash;

    PieceList m_blackPieces;
    PieceList m_whitePieces;


    /*Variable qui garde en mémoire le side to move */
    int m_sideToMove = 1;

    /*On créé deux boards différentes, qui en combinaison nous donne par exemple le Rook noir */
    int m_color[64]; /* Board contenant les couleurs des pièces */
    int m_piece[64]; /* Board contenant le type de pièce */

    /* Pour la board représentation, on utilise une mailbox
     * une mailbox est un array ou chaque case représente une adresse
     * (exemple : 0 correspond à a1, si on représente en LEFR).
     * Particulièrement utile pour vérifier si une case est valide,
     * car on propose deux rangs en haut et en bas avec les valeurs -1,
     * qui sont des cases invalides. Si une pièce bouge et se retrouverait sur une de ces cases, on peut rejeter le move */

    /* Représentation LEFR :
     * a1 b1 c1 d1 e1 f1 g1 h1
     * a2 b2 c2 d2 e2 f2 g2 h2
     * a3 b3 c3 d3 e3 f3 g3 h3
     * a4 b4 c4 d4 e4 f4 g4 h4
     * a5 b5 c5 d5 e5 f5 g5 h5
     * a6 b6 c6 d6 e6 f6 g6 h6
     * a7 b7 c7 d7 e7 f7 g7 h7
     * a8 b8 c8 d8 e8 f8 g8 h8
     *
     * En gros on retourne la board*/

    /*  ROSE DES VENTS
     *
     *  VERS LA FILE 8
     *
     *  9      8      7
     *
     * -1      .      1     Vers le rang H
     *
     * -7     -8     -9
     */

    //Garde un historique de positions sous forme de notations FERN
    //Permet de faciliter les takebacks, notamment
    std::stack<std::string> m_positionHistory;

    // Listes de coups pseudos légaux pour la position actuelle
    // Les coups sont stockés sous forme d'une liste (caseDepart, caseArrivee, valeur)
    std::vector<Move> m_moves;
    std::vector<Move> m_attackingMoves;
    std::vector<Move> m_quietMoves;

    /*
     * 1  2  3  4
     * K  Q  k  q
     */
    int m_castlingRights = 0x1111;

    Move w_kingSideR = Move(0, 3);
    Move w_queenSideR = Move(7, 5);
    Move b_kingSideR = Move(63, 61);
    Move b_queenSideR = Move(56, 59);

    //Indique l'adresse d'une case ou un pion peut prendre en passant
    int m_enPassant = -1;

    //Garde en mémoire la règle des 15 coups
    int m_fifty = 0;

    int m_coups = 0;

    int m_nodeCount = 0;

    //On inclut pas le 9 car un pion sera autopromote en reine automatiquement si rien d'autre n'est précisé, pas la peine de générer des doublons
    int m_possibleProm[4] = {3, 4, 5};

    int m_pieceSquare[7][64] = {
            //PAWNS
            { 100,  100,  100,  100,  100,  100,  100,  100,
              50, 50, 50, 50, 50, 50, 50, 50,
              10, 10, 20, 30, 30, 20, 10, 10,
              5,  5, 10, 25, 25, 10,  5,  5,
              0,  0,  0, 20, 20,  0,  0,  0,
              5, -5,-10,  0,  0,-10, -5,  5,
              5, 10, 10,-20,-20, 10, 10,  5,
              0,  0,  0,  0,  0,  0,  0,  0},

              //KNIGHT
            {-50,-40,-30,-30,-30,-30,-40,-50,
             -40,-20,  0,  0,  0,  0,-20,-40,
             -30,  0, 10, 15, 15, 10,  0,-30,
             -30,  5, 15, 20, 20, 15,  5,-30,
             -30,  0, 15, 20, 20, 15,  0,-30,
             -30,  5, 10, 15, 15, 10,  5,-30,
             -40,-20,  0,  5,  5,  0,-20,-40,
             -50,-40,-30,-30,-30,-30,-40,-50,},

             //BISHOPS
            {-20,-10,-10,-10,-10,-10,-10,-20,
             -10,  0,  0,  0,  0,  0,  0,-10,
             -10,  0,  5, 10, 10,  5,  0,-10,
             -10,  5,  5, 10, 10,  5,  5,-10,
             -10,  0, 10, 10, 10, 10,  0,-10,
             -10, 10, 10, 10, 10, 10, 10,-10,
             -10,  5,  0,  0,  0,  0,  5,-10,
             -20,-10,-10,-10,-10,-10,-10,-20,},

             //ROOK
            {  0,  0,  0,  0,  0,  0,  0,  0,
               5, 10, 10, 10, 10, 10, 10,  5,
               -5,  0,  0,  0,  0,  0,  0, -5,
               -5,  0,  0,  0,  0,  0,  0, -5,
               -5,  0,  0,  0,  0,  0,  0, -5,
               -5,  0,  0,  0,  0,  0,  0, -5,
               -5,  0,  0,  0,  0,  0,  0, -5,
               0,  0,  0,  5,  5,  0,  0,  0},

            //QUEEEEEEN
            {-20,-10,-10, -5, -5,-10,-10,-20,
             -10,  0,  0,  0,  0,  0,  0,-10,
             -10,  0,  5,  5,  5,  5,  0,-10,
             -5,  0,  5,  5,  5,  5,  0, -5,
             0,  0,  5,  5,  5,  5,  0, -5,
             -10,  5,  5,  5,  5,  5,  0,-10,
             -10,  0,  5,  0,  0,  0,  0,-10,
             -20,-10,-10, -5, -5,-10,-10,-20},

             //KING MID-GAME
            {-30,-40,-40,-50,-50,-40,-40,-30,
             -30,-40,-40,-50,-50,-40,-40,-30,
             -30,-40,-40,-50,-50,-40,-40,-30,
             -30,-40,-40,-50,-50,-40,-40,-30,
             -20,-30,-30,-40,-40,-30,-30,-20,
             -10,-20,-20,-20,-20,-20,-20,-10,
             20, 20,  0,  0,  0,  0, 20, 20,
             20, 30, 10,  0,  0, 10, 30, 20},

             //KING ENDGAME
            {-50,-40,-30,-20,-20,-30,-40,-50,
             -30,-20,-10,  0,  0,-10,-20,-30,
             -30,-10, 20, 30, 30, 20,-10,-30,
             -30,-10, 30, 40, 40, 30,-10,-30,
             -30,-10, 30, 40, 40, 30,-10,-30,
             -30,-10, 20, 30, 30, 20,-10,-30,
             -30,-30,  0,  0,  0,  0,-30,-30,
             -50,-30,-30,-30,-30,-30,-30,-50}

    };

public:
    //CONSTRUCTEURS
    /* Initialise une board sur une position de base
     * et met en place les hash
     */
    BoardRepresentation(){
        int baseColor[64] = {
                1, 1, 1, 1, 1, 1, 1, 1,
                1, 1, 1, 1, 1, 1, 1, 1,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0,
                2, 2, 2, 2, 2, 2, 2, 2,
                2, 2, 2, 2, 2, 2, 2, 2
        };

        int basePiece[64] = {
                5, 3, 4, 9, 100, 4, 3, 5,
                1, 1, 1, 1, 1, 1, 1, 1,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0,
                1, 1, 1, 1, 1, 1, 1, 1,
                5, 3, 4, 9, 100, 4, 3, 5
        };

        for(int i = 0; i < 64; i++){
            m_color[i] = baseColor[i];
            m_piece[i] = basePiece[i];
        }

        m_blackPieces = PieceList({63, 62, 61, 60, 59, 58, 57, 56,
                                            55, 54, 53, 52, 51, 50, 49, 48});

        m_whitePieces = PieceList({0, 1, 2, 3, 4, 5, 6, 7,
                                   8, 9, 10, 11, 12, 13, 14, 15});

        std::mt19937_64 mersenne_twister;
        mersenne_twister.seed(random());

        for(unsigned long long & i : m_zobrist_hash){
            i = mersenne_twister();
        }


        m_positionHistory.push(getFEN());
    };

    /* Initialise une board utilisant une notation FERN */
    explicit BoardRepresentation(std::string fen){
        setFEN(std::move(fen));

        m_blackPieces = PieceList({63, 62, 61, 60, 59, 58, 57, 56,
                                   55, 54, 53, 52, 51, 50, 49, 48});

        m_whitePieces = PieceList({0, 1, 2, 3, 4, 5, 6, 7,
                                   8, 9, 10, 11, 12, 13, 14, 15});

        std::mt19937_64 mersenne_twister;
        mersenne_twister.seed(random());

        for(unsigned long long & i : m_zobrist_hash){
            i = mersenne_twister();
        }

        m_positionHistory.push(getFEN());
    };

    //Permet d'utiliser la notation FERN pour donner une position
    void setFEN(std::string fen);

    //Permet de récupérer la notation FEN de la position actuelle
    //Utile pour créer un historique de position
    std::string getFEN();

    //Genere le hash de la position actuelle
    unsigned long long getHash();

    //Vérifie si le côté donné est en echec ou non
    bool inCheck(int side);

    /* Check si un move donné est pseudo-légal
     * cad que le move obéit aux règles de mouvement de la pièce sur la case de départ
     * mais ne vérifie pas si le move laisse le roi en échec */
    bool isPseudoLegal(Move move, int side);

    /* Fait le move si celui-ci est pseudo-légal
     * Vérifie à la fin si le roi est laissé en echec, si c'est la cas on takeback et on renvoie false */
    bool makeMove(Move move);

    //Génère tous les moves possibles pour la position actuelle
    void moveGenerator();
    void generateCaptures();

    /* Permet de rétablir la position d'avant
     * Lancée automatiquement si la position actuelle est illégale */
    void takeback(PieceList whitePieces, PieceList blackPieces);

    //Comme dit le nom, ça montre la position dans la console
    void showCurrentPosition();


    //Change un move en adresse (e6 => 44)
    static int writtenMoveParser(std::string move);

    //Change une adresse en case (44 => e6)
    static std::string adressToMoveParser(int adress);

    int checkmated();

    //Plays one move before changing side
    int play(int engineSide);
    int playMove(int engineSide, Engine* engine, bool perft);

    //Renvoie les moves disponibles
    std::vector<Move> getMoves();
    std::vector<Move> getAttackingMoves(){return m_attackingMoves;}

    //Evalue la position
    int evalutation();

    int getSide() const {return m_sideToMove;}

    bool underAttack(int square, int attackingSide);

    PieceList getWhitePieceList(){ return m_whitePieces; }
    PieceList getBlackPieceList(){ return m_blackPieces; }
};


#endif //BAUB_CHESS_BOARDREPRESENTATION_H
