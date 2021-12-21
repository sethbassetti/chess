#include <vector>
#include "utils.h"

using namespace std;

/* Helper function used to find either the least significant bit (rightmost) or the most significant bit
(leftmost) of a bitboard. bitscan reverse is used to find the MSB */
int BitScan(U64 bitboard, bool reverse=false){

    // If reverse is not true, do a forward bitscan with ffs() function. Subtract 1 to get 0 based index.
    if(!reverse){
        return __builtin_ffsll(bitboard) - 1;
    }
    // Uses builtin function that returns trailing zeros from most significant bit. Need to subtract
    // from 63 to get correct 0 based index.
    else{
        return 63 - __builtin_clzll(bitboard);
    }
}

vector<int> GetSetBits(U64 bb){

    vector<int> bit_indexes;
    while (bb)
    {
        int index = BitScan(bb);
        bit_indexes.push_back(index);
        bb &= (bb - 1);
    }
    return bit_indexes;
}