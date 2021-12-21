#include <vector>
#include <stdio.h>
#include <iostream>
#include <bitset>
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
/* Serializes the bitboard by finding all indices where the bit is set to 1 */
vector<int> SerializeBitboard(U64 bb){

    vector<int> bit_indexes;
    /* Does a bitscan to the first 1, adds it to the bit_indexes list, and resets it until bb is 0ULL */
    while (bb)
    {
        int index = BitScan(bb);
        bit_indexes.push_back(index);
        // Resets LSB to 0
        bb &= (bb - 1);
    }
    return bit_indexes;
}

/* Prints the board out in an 8x8 matrix format */
void PrintBoard(U64 bitboard){
   

    // Iterates through each rank of the board
    for(int rank=7; rank >= 0; rank--){

        printf("\n");

        // Iterates through each column of the board
        for(int file=0; file < 8; file++){

            // Convert file and rank into square index (0-63)
            int square = rank * 8 + file;
            

            // Print rank labels (1-8)
            if(!file){
                printf("  %d  " , rank+1);
            }
            
            // Prints the value of the bit at the given position
            printf(" %d ", (get_bit(bitboard, square) ? 1 : 0));
        }
    }

    // Print board file labels (A-H)
    printf("\n\n      A  B  C  D  E  F  G  H\n\n");

    // Prints out the binary representation of the board
    cout << "Binary: " << bitset<64>(bitboard) << endl;

    // Prints out hexadecimal representation of the board
    printf("Hexadecimal: 0x%lxULL\n", bitboard);
    
}