//
// Created by bitterboyy on 3/13/21.
//

#include "search.h"

namespace Chameleon{
    namespace Search{
        //Keep track of the start time of the search
        auto start = std::chrono::high_resolution_clock::now();

        movebits bestMove(position &position, int maxdepth, int maxTime, const std::vector<movebits>& moveList, long long maxNodes, bool infinite){
            //Variables keeping track of the best move found and its score
            //the way it works is that a move currently tested goes into iterationBest if it's score is the best of the current iteration
            //That way, if we do not finish up a search because of time constraints, we don't end up with what could be a really bad move
            movebits bestMove;
            movebits iterationBest;
            movebits currentMove;
            int currentScore;
            int iterationScore{-99999};
            int bestScore;

            //That part is used to implement the aspiration window
            //The idea is that after a first search, we consider that the results of the next one won't be too far off
            //So we can change the window for cutoffs, making more of them happen!
            int alpha = -99999;
            int beta = 99999;
            int aspirationWindow = 25;

            //Keep track of time spent on the search
            int timeSpent;
            std::chrono::duration<int> durationObject{};

            //The nodes part
            unsigned long long nodesSearched;

            //Storing generated moves
            movebits mvStack[256];
            int mvStackIndx{0};
            if(moveList.empty()){ //If no move list was given, just generate all moves
                position.gen(mvStack, mvStackIndx);
            } else { //Otherwise, store them
                for(auto move : moveList){
                    mvStack[mvStackIndx++] = move;
                }
            }

            //If no max depth was given, we assume that the search is meant to not care about depth and set it to be infinite
            if(!maxdepth || infinite) maxdepth = INT32_MAX;
            //Same with nodes
            if(!maxNodes || infinite) maxNodes = 0xFFFFFFFFFFFFFFFF;

            //In the case infinite wasn't set and we have time constraints, use manageTime to allocate search time
            if(!maxTime && !infinite) maxTime = manageTime(position);

            //Keep track of the start time of the search
            start = std::chrono::high_resolution_clock::now();
            //We can start the actual search now, going through each move in the stack
            //then comparing them with the bestMove that was found unti we hit a stop condition
            //We will do that while incrementing the depth to go to (iterative deepening)
            for(int depth = 1; depth < maxdepth; depth++){
                for(int i = 0; i < mvStackIndx; i++){
                    currentMove = mvStack[i];
                    position.make(currentMove);
                    maxNodes--;
                    currentScore = -searchNode(position, -beta, -alpha, depth - 1, maxNodes, maxTime);

                    //The bad side of aspiration windows: to make sure we don't miss stuff, if the score is too far
                    //we need to recalculate, because our window isn't good
                    if(currentScore <= alpha || currentScore >= beta){
                        //Reset alpha and beta
                        alpha = -99999;
                        beta = 99999;
                        aspirationWindow *= 10;
                        currentScore = -searchNode(position, -beta, -alpha, depth - 1, maxNodes, maxTime);
                    }
                    position.takeback();

                    //If the best score is less than what we got, we got ourselvs a new best move!
                    if(currentScore > iterationScore){
                        iterationScore = currentScore;
                        iterationBest = currentMove;
                    }
                    nodesSearched = (0xFFFFFFFFFFFFFFFF-maxNodes);
                    timeSpent = (int)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
                    if(maxTime && !infinite && timeSpent >= maxTime) break;
                    if(maxNodes && !infinite && nodesSearched >= maxNodes) break;
                }
                //Modify alpha and beta
                alpha = iterationScore + aspirationWindow;
                beta = iterationScore - aspirationWindow;

                nodesSearched = (0xFFFFFFFFFFFFFFFF-maxNodes);
                timeSpent = (int)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
                std::cout << "info depth " << depth << " score cp " << iterationScore << " nodes " << nodesSearched << " nps " << abs((int)(nodesSearched/(timeSpent*0.001)+1)) << std::endl;
                if(maxTime && !infinite && timeSpent >= maxTime) break;
                if(maxNodes && !infinite && nodesSearched >= maxNodes) break;

                //If we get there, the search has reached an end, so we can keep the move it produced
                bestMove = iterationBest;
                bestScore = iterationScore;
            }

            return bestMove;
        }

        int searchNode(position &position, int alpha, int beta, int depthLeft, long long &maxNodes, int maxTime) {
            maxNodes--;
            //We just hit a stop condition
            if(!depthLeft || !maxNodes){
                //Call quiescence to reduce horizon effect
                return quiescence(position, alpha, beta, maxTime);
            }

            //We're out of time
            if((int)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() >= maxTime){
                return 0;
            }

            //Generate and store moves
            movebits mvStack[256];
            int mvStackIndx{0};
            position.gen(mvStack, mvStackIndx);

            int copyBeta = beta;
            int score;
            for(int i = 0; i < mvStackIndx; i++){
                position.make(mvStack[i]);
                score = -searchNode(position, -copyBeta, -alpha, depthLeft - 1, maxNodes, maxTime);

                //That's part of negascout implementation, it adds up to alpha-beta by using a window
                //Basically, it increases the chances of a cutoff, but we need to re search if a score is out of the window
                if(score > alpha && score < beta && i){
                    score = -searchNode(position, -beta, -alpha, depthLeft - 1, maxNodes, maxTime);
                }
                position.takeback();

                //Then it's just normal alpha beta stuff
                if(alpha < score){
                    alpha = score;
                }
                if(alpha >= beta){
                    return alpha;
                }

                beta = alpha + 1; //We change the window for negascout
            }
            return alpha;
        }

        int quiescence(position &position, int alpha, int beta, int maxTime) {
            int stand_pat = Evaluation::eval(position);
            if(stand_pat >= beta){
                return stand_pat;
            }
            if(stand_pat < alpha - 900){ //Delta pruning
                return alpha;
            }
            if(alpha < stand_pat){
                alpha = stand_pat;
            }

            //We're out of time
            if((int)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() >= maxTime){
                return 0;
            }

            int score;

            //Generate and store noisy moves only as those are the ones interesting to us
            movebits mvStack[256];
            int mvStackIndx{0};
            position.genNoisy(mvStack, mvStackIndx);

            for(int i = 0; i < mvStackIndx; i++){
                position.make(mvStack[i]);
                score = -quiescence(position, -beta, -alpha, maxTime);
                position.takeback();

                //Then it's just normal alpha beta stuff
                if(alpha < score){
                    alpha = score;
                }
                if(score >= beta){
                    return beta;
                }
            }
            return alpha;
        }

        int manageTime(position &pos) {
            int timeLeft = (pos.m_side ? pos.m_btime : pos.m_wtime);
            int increment = (pos.m_side ? pos.m_binc : pos.m_winc);
            //Case where we don't consider time
            if((pos.m_side ? pos.m_btime : pos.m_wtime) == -1) return 0;

            //Normally, we allocate 2.5% of time left to the search + half the increment
            int moveTime = timeLeft/50 + increment/2;
            //In case the increment puts us above the time left, don't consider it
            if(moveTime > timeLeft){
                moveTime -= increment/2;
            }
            //In the rare cases where we get a time of 0, return 10 ms to at least get a move
            if(moveTime <= 0){
                return 10;
            }
            return moveTime;
        }

    }
}