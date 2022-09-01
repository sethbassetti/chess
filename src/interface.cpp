

#include <iostream>
#include <vector>
#include <iostream>

#include <emscripten/bind.h>
#include "board.h"

using namespace emscripten;
using namespace std;

/* Calls the board to get the best move for the given player */
string GetBestMove(string fen, int depth)
{
    // Initializes the board with the given fen
    Board board = Board(fen);

    // Searches for the best move (to a depth of the given depth)
    int move = board.GetBestMove(depth);

    // Returns the string interpretation of that move
    return PrintMove(move);
}


EMSCRIPTEN_BINDINGS(my_module){
    emscripten::function("GetBestMove", &GetBestMove);
}
