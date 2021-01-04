//
// Created by bitterboyy on 12/29/20.
//
#include "Engine.h"
#include <random>

Engine::Engine(int depth, int side, BoardRepresentation* board) {
    m_board = board;
    m_depth = depth;
    m_side = side;
}

//The idea for minimax algorithm is that :
/*
 * For each possible move, the algorithm must check each possible move, until desired depth is reached.
 * Then, it must throw the result (board.evaluation()) back at the main function in order to add/decrease the strenght of that move
 * All of that, while not modifying the stat of the board in the end
*/

//The idea here is to follow a line up until the desired depth has been reached, then throw back the value of the last node
float Engine::followLine(BoardRepresentation board, int depth) {
    //If we're at the desired depth, we return the evaluation
    if(depth <= 0){
        float eval = board.evalutation();
        for(int i = 0; i < m_depth; i++){
            board.takeback();
        }
        return eval;
    }

    if(board.checkmated() == 1){
        float eval = board.getSide() == 1 ? -9999 : 9999;
        for(int i = depth; depth < m_depth; depth++){
            board.takeback();
        }
        return eval;
    }

    if(board.checkmated() == -1){
        for(depth; depth < m_depth; depth++){
            board.takeback();
        }
        return 0;
    }

    //Otherwise, we get alll of the possible moves, then for each move we call that function
    board.moveGenerator();
    std::vector<std::vector<int>> moves = board.getMoves();

    float value;
    for(auto move : moves){
        //Make the move
        if(board.makeMove(move.at(0), move.at(1), move.at(2))) {
            board.showCurrentPosition();

            //Continue following the line after this move
            depth = depth - 1;
            value = followLine(board, depth);
        }
    }

    return value;
}

std::vector<int> Engine::minimax() {
    //Get all the moves from the current position
    std::vector<std::vector<int>> moves = m_board -> getMoves();

    //This vector will hold the final values of each move
    std::vector<float> values;

    //For each move, we follow the line of the move, and we put the walue of this move in the values vector
    for(auto & move : moves){
        std::cout << "##### TESTING MOVE " << m_board->adressToMoveParser(move.at(0)) << " -> " << m_board->adressToMoveParser(move.at(1)) << " #####" << std::endl;
        m_board->makeMove(move.at(0), move.at(1), move.at(2));
        values.push_back(followLine(*m_board, m_depth-1));
        std::cin.get();
        m_board->takeback();
    }

    //m_board->moveGenerator();

    //On cherche la meilleure et la pire valeur
    float max = -2000;
    std::vector<int> bestMove = moves.at(0);
    float min = 2000;
    std::vector<int> worstMove = moves.at(0);
    for(int i = 0; i < values.size(); i++){

        std::cout << "Valeur de " << m_board->adressToMoveParser(moves.at(i).at(0)) << " -> " << m_board->adressToMoveParser(moves.at(i).at(1)) << " : " << values.at(i) << std::endl;

        if(values.at(i) < min){
            min = values.at(i);
            worstMove = moves.at(i);
        }
        else if(values.at(i) > max){
            max = values.at(i);
            bestMove = moves.at(i);
        }
    }


    if(m_side == 1) return bestMove;
    else return worstMove;
}