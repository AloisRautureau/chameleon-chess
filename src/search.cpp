//
// Created by bitterboyy on 4/19/21.
//

#include <iostream>
#include "search.h"

using namespace Chameleon;
using namespace std::chrono;

namespace Search {
    movebyte search_root(position &pos, int maxdepth, int movetime, int timeleft, int increment, bool infinite){
        movebyte best{0};
        int score = -99999;
        int best_score = -99999;

        int alpha = -99999;
        int beta = 99999;
        int aspiration_window = 50;

        //We first need to generate every move for the root position, then check every single one of them to find our best move
        movestack stack;
        pos.gen(stack);

        //Re search counts (we widen the window more and more on each re search)
        int bad_alpha{0};
        int bad_beta{0};

        //Get going with time constraints
        if(!movetime && !infinite) movetime = timeboy(timeleft, increment);
        else if (infinite) movetime = 99999999;
        auto start_time = high_resolution_clock::now();
        auto current_time = high_resolution_clock::now();
        bool depth_complete{false};


        //The actual search
        int depth = 1;
        for(depth; depth <= maxdepth; depth++) {
            current_time = high_resolution_clock::now();
            if(duration_cast<milliseconds>(current_time - start_time).count() > movetime) break;
            for(int i = 0; i < stack.size; i++) {
                //Check if we've got time left before checking every move
                current_time = high_resolution_clock::now();
                if(duration_cast<milliseconds>(current_time - start_time).count() > movetime) break;

                pos.make(stack.moves[i]);
                score = -search_node(pos, depth - 1, -beta, -alpha);
                if (score <= alpha) { //Cases where we evaluate outside of the window
                    if(!bad_alpha){
                        alpha -= 100;
                        bad_alpha++;
                    } else if(bad_alpha == 1) {
                        alpha -= 200;
                        bad_alpha++;
                    } else {
                        alpha = -99999;
                        bad_alpha++;
                    }
                    score = -search_node(pos, depth - 1, -beta, -alpha);
                } else if(score >= beta) {
                    if(!bad_beta) {
                        beta += 100;
                        bad_beta++;
                    } else if(bad_beta == 1) {
                        beta += 200;
                        bad_beta++;
                    } else {
                        beta = 99999;
                        bad_beta++;
                    }
                    score = -search_node(pos, depth - 1, -beta, -alpha);
                }
                pos.unmake();

                if (score > best_score) {
                    best_score = score;
                    best = stack.moves[i];
                }
            }
            //Send some info
            std::cout << "info "
            << "depth " << depth
            << " score cp " << best_score
            << std::endl;
            //After searching every move we adjust the window
            alpha = best_score + aspiration_window;
            beta = best_score - aspiration_window;
        }

        //Send some info back to the user
        std::cout << "bestmove " << moveToString(best) << std::endl;

        //We searched everything, now we can just return the best move found
        return best;
    }

    int search_node(position &pos, int depth, int alpha, int beta) {
        //When reaching the max depth, return an evaluation
        if(depth == 0) return quiescence(pos, alpha, beta);

        movestack stack;
        pos.gen(stack);

        //If the movestack is empty, we're in a checkmate or stalemate
        if(!stack.size){
            if(pos.m_checked) return -MATE_SCORE;
            else return 0;
        }

        int score;
        for(int i = 0; i < stack.size; i++) {
            pos.make(stack.moves[i]);
            score = -search_node(pos, depth - 1, -beta, -alpha);
            pos.unmake();

            if(score >= beta) return score;
            if(score > alpha) alpha = score;
        }
        return alpha;
    }

    int quiescence(position &pos, int alpha, int beta) {
        int stand_pat = Eval::eval(pos); //Return a first evaluation
        if(stand_pat >= beta) return beta;
        if(stand_pat < alpha - 900) return alpha; //Delta pruning
        if(stand_pat > alpha) alpha = stand_pat;

        movestack stack;
        pos.gen(stack, true); //Generate all noisy moves

        for(int i = 0; i < stack.size; i++) {
            //TODO : check SEE for every move, if the move has SEE < 0, do not consider it at all
            pos.make(stack.moves[i]);
            stand_pat = -quiescence(pos, -beta, -alpha);
            pos.unmake();

            if(stand_pat >= beta) return beta;
            if(stand_pat > alpha) alpha = stand_pat;
        }
        return alpha;
    }

    int timeboy(int timeleft, int increment) {
        //As a starting point, we'll allow 20% of remaining time to search, plus any increment
        int time = 0;
        time += (int)(timeleft/50) + increment;
        return time;
    }
}