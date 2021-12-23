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
    U64 occupancy;
    U64 empty;

    vector<U64> pieces;
    vector<U64> colors;

    bool en_passant;

    void ResetOccupancy();

    int GetPieceType(int index);

    

    // Prints out board in chess format based on position object
    void PrintBoard();
};

