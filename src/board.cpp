#include <cstdint>
#include <iostream>
#include <stdio.h>
#include <bitset>
#include "board.h"
using namespace std;


//Macro that returns the bit of the bitboard at the square
#define get_bit(bitboard, square)(bitboard & (1ULL << square))

// Macro that sets a bit to 1 at a particular square
#define set_bit(bitboard, square)(bitboard |= (1ULL << square))

// Macro that pops a bit to 0 at a particular square
#define pop_bit(bitboard, square)(bitboard &= ~(1ULL << square))






const U64 not_a_file = 0xfefefefefefefefeULL;
const U64 not_h_file = 0x7f7f7f7f7f7f7f7fULL;

const U64 not_ab_file = 0xfcfcfcfcfcfcfcfcULL;
const U64 not_gh_file = 0x3f3f3f3f3f3f3f3fULL;

// White is 0, black is 1
enum{white, black};



Board::Board(){
    InitializeBoard();
}

void Board::InitializeBoard(){

    white_pieces = 0xFFFFULL;
    black_pieces = 0xFFFF000000000000LLU;

    /* white pieces */
    white_pawns    = 0xFF00LLU;
    white_knights  = 0x42LLU;
    white_bishops  = 0x24LLU;
    white_rooks    = 0x81LLU;
    white_queens   = 0x8LLU;
    white_kings    = 0x10LLU;

    /* black pieces */
    black_pawns    = 0xFF000000000000LLU;
    black_knights  = 0x4200000000000000LLU;
    black_bishops  = 0x2400000000000000LLU;
    black_rooks    = 0x8100000000000000LLU;
    black_queens   = 0x800000000000000LLU;
    black_kings    = 0x1000000000000000LLU;

    InitLeaperAttacks();
}

/* Prints the board out in an 8x8 matrix format */
void Board::PrintBoard(U64 bitboard){
   

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
    printf("Hexadecimal: %lx\n", bitboard);
    
}

void Board::Test(){
    U64 test_board = 0ULL;

    for (int rank = 0; rank < 8; rank++)
    {
        for (int file = 0; file < 8; file++)
        {
            int square = rank * 8 + file;
            if(file <6){
                set_bit(test_board, square);
            }
        }
    }
    PrintBoard(test_board);
}

/* Given a color and a square on the board, returns a bitboard representing where a pawn on that square
could attack*/
U64 Board::CalcPawnAttacks(int side, int square){

    // Stores the attacks
    U64 attacks = 0ULL;

    // Stores the piece on the square
    U64 bitboard = 0ULL;

    // set piece on board
    set_bit(bitboard, square);

    // white pawns
    if(!side)
    {
        // Only place an attack square up and to the left if not on outer "a" file
        if(bitboard & not_a_file){
            attacks |= (bitboard << 7);
        }

        // Only place an attack square up and to the right if not on outer "h" file
        if(bitboard & not_h_file){
            attacks |= (bitboard << 9);
        }
    }

    // Black pawns
    else
    {
        if(bitboard & not_a_file){
            attacks |= (bitboard >> 9);
        }
        if(bitboard & not_h_file){
            attacks |= (bitboard >> 7);
        }
    }

    return attacks;
}

/* Returns a bitboard of locations a king could attack if it were on a given square */
U64 Board::CalcKingAttacks(int square){
    // Stores the attacks
    U64 attacks = 0ULL;

    // Stores the piece on the square
    U64 bitboard = 0ULL;

    // set piece on board
    set_bit(bitboard, square);

    // If king not on outer a file, puts attacking square one to the left
    attacks |= (bitboard >> 1) & not_h_file;
    // If king not on outer h file, puts attacking square one to the right
    attacks |= (bitboard << 1) & not_a_file;
    

    /* At this point there is 1, 2, or 3 attacks in a horizontal line (depending on outer files)
    so shift it up one and down one to obtain all directions for king */
    bitboard |= attacks;
    // Shift up one
    attacks |= (bitboard << 8);

    // Shift down one
    attacks |= (bitboard >> 8);

    return attacks;
}

U64 Board::CalcKnightAttacks(int square){
    // Stores the attacks
    U64 attacks = 0ULL;

    // Stores the piece on the square
    U64 bitboard = 0ULL;

    // set piece on board
    set_bit(bitboard, square);

    attacks |= (bitboard << 17) & not_a_file;   // NNE attacks
    attacks |= (bitboard << 10) & not_ab_file;  // NEE attacks
    attacks |= (bitboard >> 6) & not_ab_file;   // SEE attacks
    attacks |= (bitboard >> 15) & not_a_file;   // SSE attacks
    attacks |= (bitboard >> 17) & not_h_file;   // SSW attacks
    attacks |= (bitboard >> 10) & not_gh_file;  // SWW attacks
    attacks |= (bitboard << 6) & not_gh_file;   // NWW attacks
    attacks |= (bitboard << 15) & not_h_file;   // NNW attacks

    return attacks;
}

/* Retrieves attack tables for leaper pieces (pawn, king, knight) */
void Board::InitLeaperAttacks(){
    for (int rank = 0; rank < 8; rank++){
        for (int file = 0; file < 8; file++){
            int square = rank * 8 + file;
            pawn_attacks[white][square] = CalcPawnAttacks(white, square);
            pawn_attacks[black][square] = CalcPawnAttacks(black, square);
            king_attacks[square] = CalcKingAttacks(square);
            king_attacks[square] = CalcKingAttacks(square);
            knight_attacks[square] = CalcKnightAttacks(square);
        }
    }
    PrintBoard(king_attacks[h1]);
}