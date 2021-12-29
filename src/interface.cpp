#include <iostream>
#include <vector>
#include <emscripten/bind.h>
#include "board.h"

using namespace emscripten;

int IsValidMove(std::string position, int color, int start, int end)
{
    Board board;
    vector<Move> moves = board.GenerateMoveList();
    for(Move move : moves){
        if(move.start == start && move.end == end){
            return true;
        }
    }
    return false;
}


EMSCRIPTEN_BINDINGS(my_module){
    emscripten::function("IsValidMove", &IsValidMove);
}