//
// Created by bitterboyy on 4/19/21.
//

#include "eval.h"

using namespace Chameleon;

namespace Eval {
    int eval(position &pos){
        int material = materialEval(pos, false);

        return (pos.m_side ? -1 : 1) * material;
    }


    int materialEval(position &pos, bool endgame){
        /*
         * TODO : add the other considerations (bishop pair, etc, etc)
         */
        //Loops through the piece list of the position, and multiplies the size of each by the value of the piece
        int material_score{0};
        int pc_sq_score{0};
        int square = 0;
        for(int side = 0; side < 2; side++){
            for(int piece_type = 0; piece_type < 6; piece_type++){
                for(int i = 0; i < pos.m_plists[side][piece_type].size; i++){
                    square = (side ? 0x70 : 0) ^ pos.m_plists[side][piece_type].indexes[i];
                    pc_sq_score += (side ? -1 : 1) * (endgame ? pcSqTablesEnd[piece_type][square] : pcSqTablesMiddle[piece_type][square]);
                    material_score += (side ? -1 : 1) * pieceValue[piece_type];
                }
            }
        }

        return material_score + pc_sq_score;
    }
}
