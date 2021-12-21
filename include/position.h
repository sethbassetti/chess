#include <vector>
#include "utils.h"

#pragma once

using namespace std;
class Position
{

public:
    // Constructor for position class. Initializes piece positions
    Position();

    /* Combined pieces */
    U64 white_pieces;
    U64 black_pieces;
    U64 occupancy;
    U64 empty;

    /* white pieces */
    U64 white_pawns;
    U64 white_knights;
    U64 white_bishops;
    U64 white_rooks;
    U64 white_queens;
    U64 white_kings;

    /* black pieces */
    U64 black_pawns;
    U64 black_knights;
    U64 black_bishops;
    U64 black_rooks;
    U64 black_queens;
    U64 black_kings;

    vector<U64> pieces;
    vector<U64> colors;

    void ResetOccupancy();
};