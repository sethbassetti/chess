#include <iostream>
#include <vector>
#include <iostream>
#include <emscripten/bind.h>
#include "board.h"

using namespace emscripten;
Board board;

int IsValidMove(int start, int end)
{
    vector<Move> moves = board.GenerateMoveList();
    for(Move move : moves){
        if(move.start == start && move.end == end){
            board.MakeMove(move);
            return true;
        }
    }
    return false;
}

std::string GetBoardPosition(){
    return board.GetBoardFEN();
}

void ResetBoard(){
    board = Board();
}

void MakeAIMove(){
    vector<Move> moves = board.GenerateMoveList();
    int move_choice = rand() % moves.size();
    board.MakeMove(moves[move_choice]);
}

EMSCRIPTEN_BINDINGS(my_module){
    emscripten::function("IsValidMove", &IsValidMove);
    emscripten::function("GetBoardPosition", &GetBoardPosition);
    emscripten::function("ResetBoard", &ResetBoard);
    emscripten::function("MakeAIMove", &MakeAIMove);
}
