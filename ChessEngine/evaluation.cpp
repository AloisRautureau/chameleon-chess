//
// Created by bitterboyy on 3/4/21.
//

#include "evaluation.h"

namespace Chameleon {
    namespace Evaluation {
        int eval(position &pos){
            int material{0};
            int placement{0};
            int total{0};
            int pawnsW = pos.m_plist[WHITE][PAWN].size();
            int pawnsB = pos.m_plist[BLACK][PAWN].size();
            int sq = inv;

            //We first evaluate the position based on a pure material perspective, also delaing with placement while we're at it
            for(int side = 0; side < 2; side++){
                for(int pieceType = PAWN; pieceType <= KING; pieceType++){
                    if(pieceType == KNIGHT){
                        material += (pos.m_plist[side][pieceType].size() * (m_pieceValue[pieceType] + knightPawnsValue[side ? pawnsB : pawnsW])) * (side ? -1 : 1);
                    }
                    else if(pieceType == ROOK){
                        material += (pos.m_plist[side][pieceType].size() * (m_pieceValue[pieceType] + rookPawnsValue[side ? pawnsB : pawnsW])) * (side ? -1 : 1);
                    }
                    else{
                        material += (pos.m_plist[side][pieceType].size() * m_pieceValue[pieceType]) * (side ? -1 : 1);
                    }

                    //For each adress, calculate the piece-square value
                    for(int i = 0; i < pos.m_plist[side][pieceType].size(); i++){
                        sq = pos.m_plist[side][pieceType].get(i);
                        if(side) sq ^= 0x70;
                        placement += pcsqTable[pieceType][sq] * (side ? -1 : 1);
                    }
                }
            }

            //Calculate bonuses
            if(pos.m_plist[WHITE][BISHOP].size() > 1) material += bishopPair;
            if(pos.m_plist[BLACK][BISHOP].size() > 1) material -= bishopPair;
            if(pos.m_plist[WHITE][KNIGHT].size() > 1) material += knightPair;
            if(pos.m_plist[BLACK][KNIGHT].size() > 1) material -= knightPair;
            if(pos.m_plist[WHITE][ROOK].size() > 1) material += rookPair;
            if(pos.m_plist[BLACK][ROOK].size() > 1) material -= rookPair;

            if(!pawnsW) material += noPawnPenalty;
            if(!pawnsB) material -= noPawnPenalty;

            total += (material + placement) * (pos.m_side ? -1 : 1);

            //If we're in a draw position, just cut the eval altogether and return 0;
            //if(drawDetection(pos, material)) return 0;
            return total;
        }

        bool drawDetection(position &pos, int materialScore){
            //If both sides have no pawns, one side needs a +4 pawns material advantage in order to not draw
            if(!pos.m_plist[WHITE][PAWN].size() && !pos.m_plist[BLACK][PAWN].size()) return (materialScore < 400 && materialScore > -400);
            return false;
        }
    }
}