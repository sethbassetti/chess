#include <bitset>
#include "evaluation.h"

//c//onst vector<int> material_scores = {0, 100, 320, 325, 500, 975, 10000, -100, -320, -325, -500, -975, -10000};

/*
int Evaluate(Board board, int color){
    int score = 0;
    // +1 for white, -l for black
    int color_flag = (color - (color ^ 1)) * -1;

    for (unsigned int i = 0; i < board.pieces.size(); i++)
    {
        bitset<64> bitboard(position.pieces[i]);
        score += bitboard.count() * material_scores[i];
    }

    return score * color_flag;
}
*/