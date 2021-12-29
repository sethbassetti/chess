#include <vector>
#include "utils.h"

#pragma once

using namespace std;
class Position
{

public:
    // Constructor for position class. Initializes piece positions
    Position();

    Position(std::string FEN);

    /* Combined pieces */
    U64 occupancy;
    U64 empty;

    vector<U64> pieces;
    vector<U64> colors;

    void Update();

    int GetPieceType(int index);


        // Prints out board in chess format based on position object
    void PrintBoard();

    private:

        void SetPieceOnBitboard(char piece, int square);
        void ParseFEN(std::string fen_string);
};
