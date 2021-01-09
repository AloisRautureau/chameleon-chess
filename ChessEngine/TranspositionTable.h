//
// Created by bitterboyy on 1/3/21.
//

#ifndef BAUB_CHESS_TRANSPOSITIONTABLE_H
#define BAUB_CHESS_TRANSPOSITIONTABLE_H

#include <string>
#include <vector>



class TranspositionTable{
private:
    const static int m_size = 100000;
    unsigned long long m_hashtable[m_size];
    int m_scoreTables[m_size];
    int m_bestMoveStart[m_size];
    int m_bestMoveEnd[m_size];
    int m_bestMoveSpe[m_size];
    int m_depthTable[m_size];
    int m_flagTable[m_size];
    int m_ageTable[m_size];

public:
    //Returns an index based on the given hash
    static int hashfunction(unsigned long long hash){
        return (int)(hash%m_size);
    }

    void storePosition(unsigned long long hash, int depth, int score, int bestMoveS, int bestMoveE, int bestMoveSp, int flag){
        int index = hashfunction(hash);

        //Cas où l'index ne contient pas déjà un hash
        if(!m_hashtable[index] || m_depthTable[index] < depth || m_ageTable[index] > 20){
            m_hashtable[index] = hash;
            m_depthTable[index] = depth;
            m_scoreTables[index] = score;
            m_bestMoveStart[index] = bestMoveS;
            m_bestMoveEnd[index] = bestMoveE;
            m_bestMoveSpe[index] = bestMoveSp;
            m_flagTable[index] = flag;
            m_ageTable[index] = 0;
        }
    }

    //Called every move in BoardRepresentation::play() to make the nodes age
    void ageLikeFineWine(){
        for(int i = 0; i < m_size; i++){
            if(m_hashtable[i]) m_ageTable[i]++;
        }
    }

    //Return the index of the given psoition, or -1 if it isn't known
    bool hashCheck(unsigned long long hash){
        if(m_hashtable[hashfunction(hash)] == hash) return true;
        return false;
    }

    //Getters
    int getScore(int index){
        return m_scoreTables[index];
    }

    std::vector<int> getMove(int index){
        std::vector<int> move = {m_bestMoveStart[index], m_bestMoveEnd[index], m_bestMoveSpe[index]};
        return move;
    }


};

#endif //BAUB_CHESS_TRANSPOSITIONTABLE_H
