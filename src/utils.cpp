#include <vector>
#include <stdio.h>
#include <iostream>
#include <bitset>
#include <string.h>
#include "utils.h"

using namespace std;


/* Helper function used to find either the least significant bit (rightmost) or the most significant bit
(leftmost) of a bitboard. bitscan reverse is used to find the MSB */
int BitScan(U64 bitboard){

    // return the bitscan to get least significant bit using a built in function
    return __builtin_ffsll(bitboard) - 1;
}



/* Prints out the move in UCI format as  bestmove source - target - promoted piece */
string PrintMove(int move){

    // extract the start, end, and if applicable, promotion piece of the move
    int source = get_move_source(move);
    int target = get_move_target(move);
    int promoted = get_move_promoted(move);

    // construct the move string as start-end-promoted, or (e7e8q)/ (b1b7)
    string move_str = square_index[source] + square_index[target] + ((promoted) ? promoted_pieces[promoted] : "");
    cout <<  "bestmove " << move_str << endl;
    return move_str;
}
