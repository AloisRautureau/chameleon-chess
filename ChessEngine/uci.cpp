//
// Created by bitterboyy on 3/13/21.
//

#include "uci.h"

namespace Chameleon{
    namespace UCI{
        //Helper functions
        std::vector<std::string> split(const std::string& s, char delimiter) {
            std::vector<std::string> splitted;
            std::string word;
            for(char c : s){
                if(c == delimiter && !word.empty()){
                    splitted.push_back(word);
                    word.clear();
                }
                else word.push_back(c);
            }
            if(!word.empty()){
                splitted.push_back(word);
            }

            return splitted;
        }

        std::string move_to_str(movebits move) {
            //Get the move info
            int from = position::fromSq(move);
            int to = position::toSq(move);
            char flag = position::getFlag(move);

            std::string mvStr;
            mvStr.push_back(file(from) + 'a');
            mvStr.push_back(rank(from) + '1');
            mvStr.push_back(file(to) + 'a');
            mvStr.push_back(rank(to) + '1');

            //If the flag is a promotion flag, we need to add which piece type the pawn is promoting to
            if(flag & 0b1000){
                switch(flag & 0b0011){
                    case 0:
                        mvStr.push_back('n');
                        break;
                    case 1:
                        mvStr.push_back('b');
                        break;
                    case 2:
                        mvStr.push_back('r');
                        break;
                    case 3:
                    default:
                        mvStr.push_back('q');
                        break;
                }
            }

            return mvStr;
        }

        movebits str_to_move(position &pos, std::string move) {
            const std::map<std::string, int> square_to_index{
                    {"a1", 0x00}, {"b1", 0x01}, {"c1", 0x02}, {"d1", 0x03}, {"e1", 0x04}, {"f1", 0x05}, {"g1", 0x06}, {"h1", 0x07},
                    {"a2", 0x10}, {"b2", 0x11}, {"c2", 0x12}, {"d2", 0x13}, {"e2", 0x14}, {"f2", 0x15}, {"g2", 0x16}, {"h2", 0x17},
                    {"a3", 0x20}, {"b3", 0x21}, {"c3", 0x22}, {"d3", 0x23}, {"e3", 0x24}, {"f3", 0x25}, {"g3", 0x26}, {"h3", 0x27},
                    {"a4", 0x30}, {"b4", 0x31}, {"c4", 0x32}, {"d4", 0x33}, {"e4", 0x34}, {"f4", 0x35}, {"g4", 0x36}, {"h4", 0x37},
                    {"a5", 0x40}, {"b5", 0x41}, {"c5", 0x42}, {"d5", 0x43}, {"e5", 0x44}, {"f5", 0x45}, {"g5", 0x46}, {"h5", 0x47},
                    {"a6", 0x50}, {"b6", 0x51}, {"c6", 0x52}, {"d6", 0x53}, {"e6", 0x54}, {"f6", 0x55}, {"g6", 0x56}, {"h6", 0x57},
                    {"a7", 0x60}, {"b7", 0x61}, {"c7", 0x62}, {"d7", 0x63}, {"e7", 0x64}, {"f7", 0x65}, {"g7", 0x66}, {"h7", 0x67},
                    {"a8", 0x70}, {"b8", 0x71}, {"c8", 0x72}, {"d8", 0x73}, {"e8", 0x74}, {"f8", 0x75}, {"g8", 0x76}, {"h8", 0x77},
            };

            const std::map<char, int> promStr{
                    {'n', KNIGHT},
                    {'b', BISHOP},
                    {'r', ROOK},
                    {'q', QUEEN},
            };

            std::string fromStr;
            fromStr.push_back(move[0]);
            fromStr.push_back(move[1]);
            std::string toStr;
            toStr.push_back(move[2]);
            toStr.push_back(move[3]);

            int promoteTarget = QUEEN; //We assume the promote target is a queen if we don't have further info
            if(move.size() == 5){
                promoteTarget = promStr.at(move[4]);
            }

            //We use a precomputed map of string -> int to get the square index
            int from = square_to_index.at(fromStr);
            int to = square_to_index.at(toStr);
            char flag = QUIET;
            //We then need to set the move flag...
            if(pos.m_pieces[to] != EMPTY){
                flag += CAP;
                if(pos.m_pieces[from] == PAWN && pos.m_ep == to) flag += DPAWNPUSH; //Adding this flag to CAP makes EPCAP
            }
            else{
                if(abs(from - to) == 0x20 && pos.m_pieces[from] == PAWN){ //Double pawn push
                    flag = DPAWNPUSH;
                }
                if((from == 0x74 && to == 0x76) || (from == 0x04 && to == 0x06)){
                    flag = KCASTLE;
                }
                if((from == 0x74 && to == 0x72) || (from == 0x04 && to == 0x02)){
                    flag = QCASTLE;
                }
            }

            //Add promotion to all this
            if(rank(from) == (pos.m_side ? 1 : 6) && pos.m_pieces[from] == PAWN){ //That's a promotion
                switch(promoteTarget){
                    case KNIGHT:
                        flag += NPROM;
                        break;
                    case BISHOP:
                        flag += BPROM;
                        break;
                    case ROOK:
                        flag += RPROM;
                        break;
                    default:
                        flag += QPROM;
                }
            }

            //Then return the encoded move and voilà !
            return position::encodeMove(from, to, flag);
        }

        std::string sq_to_str(int index){
            if(index == inv) return "-";
            return std::string{char('a' + file(index)), char('1' + rank(index))};
        }

        void uciListen(position &pos) {
            std::string input;
            std::getline(std::cin, input);
            std::vector<std::string> args = split(input, ' ');
            std::string command = args[0];
            args.erase(args.begin());

            //Handle any command that was sent
            if(command == "uci") init(); //First command that should be sent
            if(command == "debug") 0; //Enables/Disables debug mode
            if(command == "isready") uciSend("readyok"); //Command is sent to check if the engine is idle
            if(command == "setoption") setoption(args); //Sets engine options, although it doesn't have any for now
            if(command == "ucinewgame") ucinewgame(args); //Clears anything related to the current game (ttable notably)
            if(command == "position") setpos(pos, args); //Sets the board position to a given fen string
            if(command == "go") go(pos, args);  //Fires up a search from the current position with some given options
            if(command == "stop") 0; //Stops any ongoing search
            if(command == "ponderhit") 0;
            if(command == "quit") exit(0);
            
            //Those are commands implemented for using Chameleon in a pure CLI environnement, although this is not recommended
            if(command == "show") showPosition(pos);
            if(command == "perft") Debug::perft(pos, std::stoi(args[0]));
            if(command == "divide") Debug::perftDivide(pos, std::stoi(args[0]));
            if(command == "showmoves") {
                movestack stack;
                pos.gen(stack);
                std::cout << stack.size << " moves : " << std::endl;
                for(int i = 0; i < stack.size; i++){
                    std::cout << move_to_str(stack.moves[i]) << std::endl;
                }
            }

            //Call the function as soon as the previous command has been handled
            uciListen(pos);
        }

        void uciSend(const std::string& output){
            std::cout << output << std::endl;
        }


        void init(){
            uciSend("id name Chameleon");
            uciSend("id author Alois R");
            //uciSend("option"); we would send this if Chameleon had any options
            uciSend("uciok");
        }

        void setoption(std::vector<std::string> args){
            //No need to set this for now since we've got no changeable options, but should be implemented later on
        }

        void ucinewgame(std::vector<std::string> args){
            //Not really useful as long as we don't have a transposition table, but will come in handy when I implement one
        }

        void setpos(position &pos, std::vector<std::string> args){
            std::string givenFEN;
            int argindx = 0;
            if(args[argindx++] == "startpos") givenFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
            else {
                givenFEN += args[0];
                for(argindx = 1; argindx < args.size() && args[argindx] != "moves"; argindx++){
                    givenFEN += " " + args[argindx];
                }
            }
            pos.setFEN(givenFEN);

            //Play the moves indicated if any
            for(int move = argindx+1; move < args.size(); move++){
                std::cout << str_to_move(pos, args[move]) << std::endl;
                pos.make(str_to_move(pos, args[move]));
            }
        }

        bool go(position &pos, std::vector<std::string> args){
            std::cout << "Called go with args ";
            for(auto arg : args){
                std::cout << arg << " ";
            }
            std::cout << std::endl;
            std::string arg;
            int searchdepth{0};
            int searchnodes{0};
            int searchtime{0};
            int searchinf{0};
            std::vector<movebits> searchmovelist{};
            for(int i = 0; i < args.size(); i++){
                arg = args[i];
                if(arg == "searchmoves"){
                    std::string move = args[++i];
                    while(move != "ponder" && move != "wtime" && move != "btime" && move != "winc" && move != "binc" &&
                    move != "movestogo" && move != "depth" && move != "nodes" && move != "mate" && move != "movetime" &&
                    move != "infinite"){
                        searchmovelist.push_back(str_to_move(pos, move));
                        move = args[++i];
                    }
                }
                if(arg == "ponder"){
                    //AAAAAAH THE ENGINE CAN'T PONDER YET
                }
                //Setting time and increments
                if(arg == "wtime") pos.m_wtime = std::stoi(args[++i]);
                if(arg == "btime") pos.m_btime = std::stoi(args[++i]);
                if(arg == "winc") pos.m_winc = std::stoi(args[++i]);
                if(arg == "binc") pos.m_binc = std::stoi(args[++i]);

                if(arg == "movestogo"){
                    //Implement time control plz
                    i++;
                }

                if(arg == "depth") searchdepth = std::stoi(args[++i]);
                if(arg == "nodes") searchnodes = std::stoi(args[++i]);
                if(arg == "mate") i++; //Search for mate in x moves
                if(arg == "movetime") searchtime = std::stoi(args[++i]);
                if(arg == "infinite") searchinf = true;
            }

            //Once we have set every option, launch the search, which handles sending info about itself
            //We just need to return the final chosen move
            std::string bestMove = move_to_str(Search::bestMove(pos, searchdepth, searchtime, searchmovelist, searchinf));
            uciSend("bestmove " + bestMove);
            return true;
        }

        void showPosition(position &pos) {
            int square;
            std::cout << std::endl << (!pos.m_side ? "White " : "Black ") << "to move!" << std::endl << std::endl;
            std::cout << "+-----+-----+-----+-----+-----+-----+-----+-----+" << std::endl;
            for(int i = 0; i < 8; i++){
                for(int j = 0; j < 8; j++){
                    square = (7-i) * 0x10 + j;

                    switch(pos.m_pieces[square]){
                        case PAWN:
                            std::cout << (pos.m_color[square] == WHITE ? "|  P  " : "|  p  ");
                            break;
                        case KNIGHT:
                            std::cout << (pos.m_color[square] == WHITE ? "|  N  " : "|  n  ");
                            break;
                        case BISHOP:
                            std::cout << (pos.m_color[square] == WHITE ? "|  B  " : "|  b  ");
                            break;
                        case ROOK:
                            std::cout << (pos.m_color[square] == WHITE ? "|  R  " : "|  r  ");
                            break;
                        case QUEEN:
                            std::cout << (pos.m_color[square] == WHITE ? "|  Q  " : "|  q  ");
                            break;
                        case KING:
                            std::cout << (pos.m_color[square] == WHITE ? "|  K  " : "|  k  ");
                            break;
                        case EMPTY:
                            std::cout << "|     ";
                            break;
                    }


                    //End of rank, print the rank and return
                    if(file(square) == 7) std::cout << "|  " << rank(square)+1 << std::endl << "+-----+-----+-----+-----+-----+-----+-----+-----+" << std::endl;
                }
            }

            //We end up by printing the file names
            for(int i = 0; i < 8; i++){
                std::cout << "   " << char('A' + i) << "  ";
            }

            std::cout << std::endl << std::endl << "Move " << pos.m_ply << " (" << pos.m_halfclock << " moves since last capture or pawn move)" << std::endl;
            std::cout << "En passant square : " << sq_to_str(pos.m_ep) << std::endl;
            std::cout << "Castling rights : " << (pos.m_castling & WKCASTLE ? "K" : "")
                      << (pos.m_castling & WQCASTLE ? "Q" : "")
                      << (pos.m_castling & BKCASTLE ? "k" : "")
                      << (pos.m_castling & BQCASTLE ? "q" : "") << std::endl << std::endl;

            std::cout << "Position hash : " << pos.positionHash << std::endl << std::endl;
        }
    }
}