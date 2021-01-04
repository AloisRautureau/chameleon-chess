//
// Created by bitterboyy on 1/3/21.
//

#ifndef BAUB_CHESS_UTILITY_H
#define BAUB_CHESS_UTILITY_H

#include <string>
#include <vector>

std::vector<std::string> split(std::string toSplit, char splitter){
    std::vector<std::string> substrings;
    std::string substring;
    for(int i = 0; i <= toSplit.length(); i++){
        if(toSplit[i] == splitter || i == toSplit.length()){
            substrings.push_back(substring);
            substring = "";
        }

        else{
            substring += toSplit[i];
        }
    }
    return substrings;
}

#endif //BAUB_CHESS_UTILITY_H
