//
// Created by bitterboyy on 1/14/21.
//

#ifndef BAUB_CHESS_ENGINE_H
#define BAUB_CHESS_ENGINE_H

#include <utility>

#include "Constants.h"
#include "BoardRep.h"

class Engine{
private:
    int m_depth;
    int m_nodes = 0;
    BoardRep *m_board;

public:
    explicit Engine(int depth, BoardRep *board) : m_depth(depth), m_board(board){};

    ~Engine(){delete m_board;}

    /*
     * TODO search should implement alpha beta, opening books, iterative deepening and transposition tables/PV nodes
     */


    //TODO Fonction qui lance la recherche de la node racine et renvoie le meilleur coup
    Move searchRoot(int sideToMax){
        int bestValue = -99999;
        int worstValue = 99999;

        m_board->generatePseudoLegal();
        std::vector<Move> moves = m_board->getMoveStack().getStack();
        Move worstMove = moves[0];
        Move bestMove = moves[0];

        for(Move move : moves){
            std::cout << move.start << " " << move.end << " : ";
            m_board->makeMove(move);
            int eval = searchNode(-99999, 99999, m_depth - 1);
            std::cout << eval << std::endl;
            m_board->takeback();

            if(eval > bestValue){
                bestMove = move;
                bestValue = eval;
            }

            if(eval < worstValue){
                worstMove = move;
                worstValue = eval;
            }
        }

        std::cout << "The engine found " << bestMove.start << " " << bestMove.end << " to be the best move" << std::endl;
        std::cout << "The engine found " << worstMove.start << " " << worstMove.end << " to be the worst move" << std::endl;
        std::cout << "Nodes searched : " << m_nodes << std::endl;

        if(sideToMax == WHITE) return bestMove;
        else return worstMove;
    }

    //TODO Ajouter la collection de la PV
    int searchNode(int alpha, int beta, int depth){

        //Arrivé à la profondeur souhaitée, on appelle la fonction quiescence pour éviter de se heurter à l'horizon d'évenements trop fort
        if(depth == 0){
            return quiescence(alpha, beta);
        }
        m_nodes++;

        //On généère les coups pseudo-légaux
        m_board->generatePseudoLegal();
        std::vector<Move> moves = m_board->getMoveStack().getStack();

        if(moves.empty()) return -10000;

        //Recurively call the search function for each possible move
        for(Move move : moves){
            //Si le move est légal..
            if(m_board->makeMove(move)){
                //On évalue la position, puis on annule le coup
                int eval = -searchNode(-beta, -alpha, depth-1);
                m_board->takeback();

                if(eval >= beta) return beta;
                if(eval > alpha) alpha = eval;
            }
        }
        return alpha;
    };

    //TODO fonction quiescence
    int quiescence(int alpha, int beta){
        int stand_pat = m_board->evaluation();
        if(stand_pat >= beta) return beta;
        if(alpha < stand_pat) alpha = stand_pat;

        //Generate all capture moves
        m_board->generateCaptures();
        std::vector<Move> captures = m_board->getMoveStack().getStack();

        if(captures.empty()) return stand_pat;

        for(Move capture : captures){
            if(m_board->makeMove(capture)){
                int eval = -quiescence(-beta, -alpha);
                m_board->takeback();

                if(eval >= beta) return beta;
                if(eval > alpha) alpha = eval;
            }
        }
        return alpha;
    }



};

#endif //BAUB_CHESS_ENGINE_H
