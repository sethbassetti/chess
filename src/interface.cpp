

#include <iostream>
#include <vector>
#include <iostream>
/*
#include <emscripten/bind.h>
#include "board.h"

using namespace emscripten;

// global board variable to keep track of game state
Board board = Board();



int MakeMove(int start, int end)
{
    // init a move list
    MoveList move_list;

    // init a move variable to store each move
    int move;

    // populate the move list with moves
    board.GenerateMoves(&move_list);

    // iterate through each move
    for(int i = 0; i < move_list.count; i++)
    {   
        PrintMove(move);
        // retrieve the current move
        move = move_list.moves[i];

        // if there is a move whose start and end match the queried start and end, then it is a valid move and make that move
        if((get_move_source(move) == start) && (get_move_target(move) == end))
            return board.MakeMove(move, all_moves);
    }

    return 0;
}

std::string MakeRandomMove()
{
    int move = board.GetRandomMove();

    while (!board.MakeMove(move, all_moves))
        move = board.GetRandomMove();


    return PrintMove(move);

}



EMSCRIPTEN_BINDINGS(my_module){
    emscripten::function("MakeMove", &MakeMove);
    emscripten::function("MakeRandomMove", &MakeRandomMove);
}
*/