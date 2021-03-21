//
// Created by bitterboyy on 3/13/21.
//

#include "search.h"

namespace Chameleon{
    namespace Search{
        unsigned long long nodesSearched{0};
        unsigned long long nodesOnDepth{0};
        unsigned long long nodesOnMove{0};

        //Reduction for null move pruning
        int reduction = 2;


        movebits bestMove(position &position, int maxdepth, int maxTime, const std::vector<movebits>& moveList, bool infinite){
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
            int alpha = -999999;
            int beta = 999999;
            int aspirationWindow = 50;

            //Keep track of time spent on the search
            int timeSpent;

            //Storing generated moves
            movestack stack;
            if(moveList.empty()){ //If no move list was given, just generate all moves
                position.gen(stack);
            } else { //Otherwise, store them
                for(auto move : moveList){
                    position.addToStack(stack, move);
                }
            }
            //If we only have one move, no need to actually search anything
            if(stack.size == 1) return stack.moves[0];
            position::sortStack(stack);

            //If no max depth was given, we assume that the search is meant to not care about depth and set it to be infinite
            if(!maxdepth || infinite) maxdepth = INT32_MAX;
            //Same with nodes

            //In the case infinite wasn't set and we have time constraints, use manageTime to allocate search time
            if(!maxTime && !infinite) maxTime = manageTime(position);

            //Keep track of the start time of the search
            auto start = std::chrono::high_resolution_clock::now();
            //We can start the actual search now, going through each move in the stack
            //then comparing them with the bestMove that was found unti we hit a stop condition
            //We will do that while incrementing the depth to go to (iterative deepening)
            for(int depth = 1; depth < maxdepth; depth++){
                iterationScore = -99999;
                for(int i = 0; i < stack.size; i++){
                    nodesOnMove = 0;
                    currentMove = stack.moves[i];
                    position.make(currentMove);
                    currentScore = -searchNode(position, -beta, -alpha, depth - 1, false);
                    //The bad side of aspiration windows: to make sure we don't miss stuff, if the score is too far
                    //we need to recalculate, because our window isn't good
                    if(currentScore <= alpha || currentScore >= beta){
                        //Reset alpha and beta
                        alpha -= aspirationWindow;
                        beta += aspirationWindow;
                        currentScore = -searchNode(position, -beta, -alpha, depth - 1, false);
                    }
                    position.takeback();

                    //If the best score is less than what we got, we got ourselves a new best move!
                    if(currentScore > iterationScore){
                        iterationScore = currentScore;
                        iterationBest = currentMove;
                    }
                    timeSpent = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
                    //Print out some info about the search for debugging purposes
                    std::cout
                            << "info currmove " << display::displayMove(currentMove)
                            << " currmovenumber " << i
                            << " score cp " << currentScore
                            << " nodes " << nodesOnMove
                            << std::endl;
                    nodesOnDepth += nodesOnMove;
                    if(maxTime && !infinite && timeSpent >= maxTime) return bestMove;
                }
                //Modify alpha and beta
                alpha = iterationScore - aspirationWindow;
                beta = iterationScore + aspirationWindow;

                timeSpent = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
                std::cout
                << "info depth " << depth
                << " score cp " << iterationScore
                << " time " << timeSpent
                << " nodes " << nodesOnDepth
                << " nps " << nodesOnDepth/(timeSpent*0.0001)
                << std::endl;
                nodesSearched += nodesOnDepth;
                nodesOnDepth = 0;

                if(maxTime && !infinite && timeSpent >= maxTime) return bestMove;
                //If we get there, the search has reached an end, so we can keep the move it produced
                bestMove = iterationBest;
                bestScore = iterationScore;
            }

            return bestMove;
        }

        int searchNode(position &position, int alpha, int beta, int depthLeft, bool nullAllowed) {
            nodesOnMove++;
            //We just hit a stop condition
            if(depthLeft <= 0){
                //Call quiescence to reduce horizon effect
                return quiescence(position, alpha, beta, 3);
            }
            if(depthLeft == 1){ //We're on a frontier node and can use futility pruning
                if(Evaluation::eval(position) + Evaluation::m_pieceValue[BISHOP] < alpha){
                    return quiescence(position, alpha, beta, 4);
                }
            }
            if(depthLeft == 2){ //Extended futility pruning, same thing but using a bigger margin
                if(Evaluation::eval(position) + Evaluation::m_pieceValue[ROOK] < alpha){
                    return quiescence(position, alpha, beta, 5);
                }
            }
            if(depthLeft == 3){ //That's called razoring, we reduce the depth of the search if we're a queen away from alpha
                if(Evaluation::eval(position) + Evaluation::m_pieceValue[QUEEN] < alpha){
                    depthLeft--;
                }
            }

            //Generate and store moves
            movestack stack;
            position.gen(stack);
            //Here, we check if we're in a checkmate or stalemate position
            if(position.check && !stack.size) return -100000; //Checkmate
            else if(!stack.size) return 0; //Stalemate draw

            position::sortStack(stack);
            int score;
            int b = beta;

            for(int i = 0; i < stack.size; i++){
                position.make(stack.moves[i]);
                score = -searchNode(position, -b, -alpha, depthLeft - 1, true);
                //if the score falls outside the window, we research the move
                if(score > alpha && score < beta && i > 0){
                    score = -searchNode(position, -beta, -alpha, depthLeft-1, true);
                }
                position.takeback();

                if(score > alpha){
                    alpha = score;
                }
                if(alpha >= beta){
                    return alpha;
                }
                b = alpha + 1;
            }
            return alpha;
        }

        int quiescence(position &position, int alpha, int beta, int maxDepth) {
            nodesOnMove++;
            int stand_pat = Evaluation::eval(position);
            if(maxDepth <= -5) return stand_pat;
            if(stand_pat >= beta){
                return stand_pat;
            }
            if(stand_pat < alpha - 900){ //Delta pruning
                return alpha;
            }
            if(alpha < stand_pat){
                alpha = stand_pat;
            }

            int score;

            //Generate and store noisy moves only as those are the ones interesting to us
            movestack stack;
            position.genNoisy(stack);

            //Here, we check if we're in a checkmate or stalemate position
            if(position.check && !stack.size) return -100000; //Checkmate
            if(!stack.size) return stand_pat; //The position is already quiet
            position::sortStack(stack);

            for(int i = 0; i < stack.size; i++){
                if(position::getFlag(stack.moves[i]) & CAP && Evaluation::see(position, stack.moves[i], position.m_side) < 0) {
                    continue; //No point making the move, it is bad
                }
                if(maxDepth <= 0 && !(position::getFlag(stack.moves[i]) & CAP)) continue; //We don't need to care about checks after a certain point
                position.make(stack.moves[i]);
                score = -quiescence(position, -beta, -alpha, maxDepth-1);
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

namespace Chameleon{
    namespace Evaluation{
        static int see(const position &pos, movebits move, bool side) {
            //We first need to find every potential attackers to the square, making sure they aren't pinned !
            pieceList wAttackers{{}};
            pieceList bAttackers{{}};
            int scores[32];
            int scoreIndex = 0;
            int from = position::fromSq(move);
            int to = position::toSq(move);
            int attackerDelta = 0;

            //Check for the bishop deltas
            for (auto delta : m_pieceDelta[BISHOP]) {
                if (!delta) break;
                //While we check for diagonnaly attacking pieces, might as well take care of pawns/kings
                if (pos.m_pieces[to + delta] == PAWN || pos.m_pieces[to + delta] == KING) {
                    if (to + delta == from) {
                        attackerDelta = delta;
                        continue;
                    }
                    if (pos.m_color[to + delta] == WHITE) wAttackers.add(to + delta);
                    else bAttackers.add(to + delta);
                    continue;
                }

                for (int currSquare = to + delta; !(currSquare & 0x88); currSquare += delta) {
                    if (pos.m_pieces[currSquare] == BISHOP || pos.m_pieces[currSquare] == QUEEN) {
                        if (currSquare == from) {
                            attackerDelta = delta;
                            break;
                        }
                        if (pos.m_color[currSquare] == WHITE) wAttackers.add(currSquare);
                        else bAttackers.add(currSquare);
                        break;
                    } else if (pos.m_pieces[currSquare] != EMPTY) break;
                }
            }

            //Same thing for rook
            for (auto delta : m_pieceDelta[ROOK]) {
                if (!delta) break;
                //Here we just need to check for king moves
                if (pos.m_pieces[to + delta] == KING) {
                    if (to + delta == from) {
                        attackerDelta = delta;
                        continue; //We don't add the original attacker to the plist
                    }
                    if (pos.m_color[to + delta] == WHITE) wAttackers.add(to + delta);
                    else bAttackers.add(to + delta);
                    continue;
                }

                for (int currSquare = to + delta; !(currSquare & 0x88); currSquare += delta) {
                    if (pos.m_pieces[currSquare] == ROOK || pos.m_pieces[currSquare] == QUEEN) {
                        if (currSquare == from) {
                            attackerDelta = delta;
                            break;
                        }
                        if (pos.m_color[currSquare] == WHITE) wAttackers.add(currSquare);
                        else bAttackers.add(currSquare);
                        break;
                    } else if (pos.m_pieces[currSquare] != EMPTY) break;
                }
            }


            //Finally, same for knights
            for (auto delta : m_pieceDelta[KNIGHT]) {
                if (!delta) break;
                if (pos.m_pieces[to + delta] == KNIGHT) {
                    if (to + delta == from) {
                        attackerDelta = delta;
                        break;
                    }
                    if (pos.m_color[to + delta] == WHITE) wAttackers.add(to + delta);
                    else bAttackers.add(to + delta);
                }
            }

            //Now we've got every direct attacker, we can simulate the initial capture
            int capturedValue = m_pieceValue[pos.m_pieces[to]];
            int attackerValue = m_pieceValue[pos.m_pieces[from]];
            scores[scoreIndex++] = capturedValue;
            side ^= 1;

            char attacker = pos.m_pieces[from];
            int attackerSquare = from;
            //We'll repeat this for every attacker
            do {
                //The attacker is now the captures piece
                capturedValue = attackerValue;

                //We now add hidden attackers after the first move, those can be found by following the attackerDelta from the from square
                if (pos.m_pieces[attacker] != KNIGHT && pos.m_pieces[attacker] != KING && scoreIndex < 32) {
                    for (int currSquare = from + attackerDelta; !(currSquare & 0x88); currSquare += attackerDelta) {
                        if (pos.m_pieces[currSquare] != EMPTY) {
                            //Check if the piece found can use the delta
                            int attack = pos.m_attackArray[to - currSquare + 128];
                            switch (pos.m_pieces[currSquare]) {
                                case BISHOP:
                                    if (attack == position::attKQBbP || attack == position::attKQBwP ||
                                        attack == position::attQB) {
                                        if (pos.m_color[currSquare] == WHITE) wAttackers.add(currSquare);
                                        else bAttackers.add(currSquare);
                                    }
                                    break;
                                case ROOK:
                                    if (attack == position::attKQR || attack == position::attQR) {
                                        if (pos.m_color[currSquare] == WHITE) wAttackers.add(currSquare);
                                        else bAttackers.add(currSquare);
                                    }
                                    break;
                                case QUEEN:
                                    if (attack != position::attNONE && attack != position::attN) {
                                        if (pos.m_color[currSquare] == WHITE) wAttackers.add(currSquare);
                                        else bAttackers.add(currSquare);
                                    }
                                    break;
                                default:
                                    break;
                            }
                            break;
                        }
                    }
                }
                if (bAttackers.size() < 1 || wAttackers.size() < 1) break;

                //Now, we find the least valuable attacking piece, and make it capture the square
                int size = side ? bAttackers.size() : wAttackers.size();
                for (int i = 0; i < size; i++) {
                    if (i == 0 ||
                        m_pieceValue[pos.m_pieces[side ? bAttackers.get(i) : wAttackers.get(i)]] < attackerValue) {
                        attackerSquare = side ? bAttackers.get(i) : wAttackers.get(i);
                        attacker = pos.m_pieces[attackerSquare];
                        attackerValue = m_pieceValue[attacker];
                    }
                }

                // We should remove the attacker since it won't be attacking anymore
                if (!side && wAttackers.size() > 0) wAttackers.remove(attackerSquare);
                else if(bAttackers.size() > 0) bAttackers.remove(attackerSquare);

                //And update the score
                scores[scoreIndex] = capturedValue - scores[scoreIndex - 1];
                scoreIndex++;
                side ^= 1;
            } while (bAttackers.size() > 0 && wAttackers.size() > 0);

            //Now evaluate the whole sequence
            while (scoreIndex > 1) {
                scoreIndex--;
                if (scores[scoreIndex - 1] > -scores[scoreIndex]) {
                    scores[scoreIndex - 1] = -scores[scoreIndex];
                }
            }

            return scores[0];
        }
    }
}
