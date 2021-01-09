//
// Created by bitterboyy on 12/29/20.
//
#include "Engine.h"
#include <random>

int Engine::quiescence(int alpha, int beta){
    int evalActuelle = m_board->evalutation();
    if(evalActuelle >= beta) return beta;
    if(alpha < evalActuelle) alpha = evalActuelle;

    //On choppe les captures
    m_board->generateCaptures();
    std::vector<Move> captures = m_board->getAttackingMoves();

    PieceList whitePieces = m_board->getWhitePieceList();
    PieceList blackPieces = m_board->getBlackPieceList();

    //We check every capture
    if(!captures.empty()){
        for(Move capture : captures){
            m_board->makeMove(capture);
            int eval = -quiescence(-beta, -alpha);
            m_board->takeback(whitePieces, blackPieces);

            int delta = 1000;

            if(eval >= beta) return beta;
            if(eval < alpha - delta) return alpha;
            if(eval > alpha) alpha = eval;
        }
    }
    else return evalActuelle;

    return alpha;
};

int Engine::alphaBeta(int alpha, int beta, int depth){
    if(depth == 0){
        return quiescence(alpha, beta);
    }

    auto hash = m_board->getHash();
    auto tableIndex = TranspositionTable::hashfunction(hash);

    //Genere les coups
    m_board->moveGenerator();
    std::vector<Move> moves = m_board->getMoves();
    PieceList whitePieces = m_board->getWhitePieceList();
    PieceList blackPieces = m_board->getBlackPieceList();


    if(m_board->checkmated() == 1){
        return m_board->getSide() == 1 ? -10000 : 10000;
    }

    if(moves.empty()) return 0;


    //For each move, we call alphaBeta recursively and invert alpha and beta
    for(Move move : moves){
        m_board->makeMove(move);
        int eval = 0;


        eval = -alphaBeta(-beta, -alpha, depth - 1);

        m_board->takeback(whitePieces, blackPieces);

        if(eval >= beta) {
            return beta;
        }
        if(eval > alpha) {
            alpha = eval;
        }

    }

    return alpha;
}

Move Engine::search() {

    //Stocke les valeurs de chaque coup root+1
    Move bestMove = Move(0, 0, -1);
    int bestValue = -99999;
    Move worstMove = Move(0, 0, -1);
    int worstValue = 99999;

    //On génère les coups root+1
    m_board->moveGenerator();
    std::vector<Move> moves = m_board->getMoves();
    PieceList whitePieces = m_board->getWhitePieceList();
    PieceList blackPieces = m_board->getBlackPieceList();

    std::cout << "Testing moves 0/" << moves.size() << std::endl;
    int i = 0;

    for(Move move : moves){
        i++;
        std::cout << "Testing moves " << i << "/" << moves.size() << std::endl;


        //std::cout << "Testing " << BoardRepresentation::adressToMoveParser(move.start()) << " -> " << BoardRepresentation::adressToMoveParser(move.end()) << std::endl;
        //Pour chaque coup, on appelle alphabeta pour trouver la valeur de ce coup
        m_board->makeMove(move);
        int score = alphaBeta(-99999, 99999, m_depth);
        m_board->takeback(whitePieces, blackPieces);
        //std::cout << score << std::endl;
        if(score > bestValue){
            bestValue = score;
            bestMove = move;
        }
        
        if(score < worstValue){
            worstValue = score;
            worstMove = move;
        }
    }

    std::cout << std::endl << "Best move : " << BoardRepresentation::adressToMoveParser(bestMove.start()) << " -> " << BoardRepresentation::adressToMoveParser(bestMove.end()) << " " << bestValue <<std::endl;
    std::cout << "Worst move : " << BoardRepresentation::adressToMoveParser(worstMove.start()) << " -> " << BoardRepresentation::adressToMoveParser(worstMove.end()) << " " << worstValue << std::endl << std::endl;
    return m_board->getSide() == 1 ? bestMove : worstMove;
}