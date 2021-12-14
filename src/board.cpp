#include <cstdint>
#include <iostream>
#include <stdio.h>
#include <bitset>
#include <strings.h>
#include <string.h>
#include <assert.h>  
#include "board.h"

using namespace std;

//Macro that returns the bit of the bitboard at the square
#define get_bit(bitboard, square)(bitboard & (1ULL << square))

// Macro that sets a bit to 1 at a particular square
#define set_bit(bitboard, square)(bitboard |= (1ULL << square))

// Macro that pops a bit to 0 at a particular square
#define pop_bit(bitboard, square)(bitboard &= ~(1ULL << square))

// Macro that defines whether direction is negative ray direction, bases it off enum number
#define is_negative(dir)(dir > 3)

const U64 a_file = 0x101010101010101ULL;        // All 1's in a file
const U64 not_a_file = 0xfefefefefefefefeULL;   // All 1's except a file
const U64 not_h_file = 0x7f7f7f7f7f7f7f7fULL;   // All 1's except h file
const U64 not_ab_file = 0xfcfcfcfcfcfcfcfcULL;  // All 1's except ab files
const U64 not_gh_file = 0x3f3f3f3f3f3f3f3fULL;  // All 1's except gh files

const int index64[64] = {
    0, 47,  1, 56, 48, 27,  2, 60,
   57, 49, 41, 37, 28, 16,  3, 61,
   54, 58, 35, 52, 50, 42, 21, 44,
   38, 32, 29, 23, 17, 11,  4, 62,
   46, 55, 26, 59, 40, 36, 15, 53,
   34, 51, 20, 43, 31, 22, 10, 45,
   25, 39, 14, 33, 19, 30,  9, 24,
   13, 18,  8, 12,  7,  6,  5, 63
};

// White is 0, black is 1
enum{white, black};

// Constructor for board, initializes piece sets and attack sets
Board::Board(){
    InitializeBoard();
    InitializeAttackSets();
}

/* Initializes the piece sets for the board, constructs bitboards for all pieces as well as unions
of pieces to form occupancy sets */
void Board::InitializeBoard(){

    /* White pieces, black pieces, and all pieces */
    white_pieces = 0xFFFFULL;
    black_pieces = 0xFFFF000000000000LLU;
    occupancy = white_pieces | black_pieces;

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
}

/* Initializes attack sets for leaper pieces and sliding pieces */
void Board::InitializeAttackSets(){
    InitLeaperAttacks();
    InitSliderAttacks();
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

    // Prints out hexadecimal representation of the board
    printf("Hexadecimal: 0x%lxULL\n", bitboard);
    
}

/* Test function for implementing and development */
void Board::Test(){
    U64 test_board = 0ULL;

    for (int rank = 0; rank < 8; rank++)
    {
        for (int file = 0; file < 8; file++)
        {
            int square = rank * 8 + file;
            if(rank == file && rank <7){
                set_bit(test_board, square);
            }
        }
    }
    //PrintBoard(ray_attacks[g3][NoEa]);
    //PrintBoard(a_file | (a_file << 1) | (a_file << 2));
    //U64 x = 2ULL;
    //PrintBoard(x << -1);
    PrintBoard(queen_attacks[a]);
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

    // Calculates all possible knight attacks, intersects with files to avoid wrapping
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
    /* Constructs leaper attack tables for every square, so iterates through rank and file */
    for (int rank = 0; rank < 8; rank++){
        for (int file = 0; file < 8; file++){

            // Gets square index
            int square = rank * 8 + file;

            // Pawn attacks are dependent on their color, all other attacks are color agnostic
            pawn_attacks[white][square] = CalcPawnAttacks(white, square);
            pawn_attacks[black][square] = CalcPawnAttacks(black, square);
            king_attacks[square] = CalcKingAttacks(square);
            knight_attacks[square] = CalcKnightAttacks(square);
        }
    }
}

/* Calculates sliding attack sets for rooks given square parameter */
U64 Board::CalcRookAttacks(int square){
    // Stores the attacks
    U64 attacks = 0ULL;

    // Stores the piece on the square
    U64 bitboard = 0ULL;

    // set piece on board
    set_bit(bitboard, square);

    // Creates a vector of the rays to construct the attacks from
    enumDirections dirs[4] = {Nort, East, West, Sout};

    // Iterates over every direction to construct attack table
    for (int i = 0; i < 4; i++){
        
        // Assigns the current direction to dir
        enumDirections dir = dirs[i];
        
        // Gets the appropriate ray attack for the direction and square
        U64 ray_attack = GetDirRayAttacks(dir, square);

        // Adds the ray attack to the overall attacks table with a union
        attacks |= ray_attack;
    }

    return attacks;
}

U64 Board::CalcBishopAttacks(int square){
    // Stores the attacks
    U64 attacks = 0ULL;

    // Stores the piece on the square
    U64 bitboard = 0ULL;

    // set piece on board
    set_bit(bitboard, square);

    // Creates a vector of the rays to construct the attacks from
    enumDirections dirs[4] = {NoWe, SoWe, SoEa, NoEa};

    // Iterates over every direction to construct attack table
    for (int i = 0; i < 4; i++){
        
        // Assigns the current direction to dir
        enumDirections dir = dirs[i];
        
        // Gets the appropriate ray attack for the direction and square
        U64 ray_attack = GetDirRayAttacks(dir, square);

        // Adds the ray attack to the overall attacks table with a union
        attacks |= ray_attack;
    }

    return attacks;
}

/* Unions the rook and bishop attacks to form queen attack table. NOTE: This function HAS to be called after
the rook and bishop attack tables are built or it will construct the empty set */
U64 Board::CalcQueenAttacks(int square){
    return rook_attacks[square] | bishop_attacks[square];
}
/* Returns the ray attack in a given direction (N, NW, S, SE, etc...) from the given square.
This function considers blocker pieces. Thus, the ray attacks only go as far as line of sight from the
square allows */
U64 Board::GetDirRayAttacks(enumDirections dir, int square){

    // Retrieves ray attacks from lookup table
    U64 ray_attack = ray_attacks[square][dir];

    // Calculates every piece that ray can hit with intersection of ray attack and occupancy    
    U64 blocker = ray_attack & occupancy;

    // If there is a blocker, make sure ray attack cannot go through that square
    if(blocker){
        // Gets the index of the first piece the ray attack would hit
        int index = BitScan(blocker, is_negative(dir));

        // Does an XOR to calculate the ray attack UP TO the first piece it hits and no further. 
        // This works by looking up what the ray would be at the first blocker and then simply XORing
        // to get the original ray minus the ray from the blocker square
        ray_attack ^= ray_attacks[index][dir];
    }

    return ray_attack;
}

/* Initialize Ray Attack table. Constructs north, south, east, and west ray attacks (occupancy agnostic)
for every square on the board */
void Board::InitRayAttacks(){
    // North ray is a line of ones extending northwards from (but not including) a1
    U64 north = 0x0101010101010100ULL;
  
    //  Left shifts 1 bit each time for every square, constructing the north ray attack table
    for (int sq = 0; sq < 64; sq++, north <<=1){
        ray_attacks[sq][Nort] = north;
    }

    // A line of ones extending southwards from (but not including) h8
    U64 south = 0x0080808080808080ULL;

    // Right shifts one bit each time for every square to construct the south attack table
    for (int sq = 63; sq >= 0; sq--, south >>= 1)
    {
        ray_attacks[sq][Sout] = south;
    }

    // A line of ones extending eastwards from, but not including, a1
    U64 east = 0xfeULL;

    // Iterates through rank and file respectively. For every rank, copies the east bitboard to new_east
    // and left shifts new_east for every file. To avoid file wrapping, new_east is intersected with the original
    // east bitboard so only ones on the original rank remain. Every rank, the original east bitboard is
    // shifted up a rank (leftshift of 8)
    for (int rank = 0; rank < 8; rank++, east <<= 8)
    {
        U64 new_east = east;
        for (int file = 0; file < 8; file += 1, new_east <<= 1)
        {
            ray_attacks[rank * 8 + file][East] = east & new_east;
        }
    }

    // A line of ones extending westwards from, but not including, h8
    U64 west = 0x7f00000000000000ULL;

    // Performs the same process as calculating the east ray attacks, but in reverse. Starts in top rank
    // and last file, and works its way downwards from h8 to a1.
    for (int rank = 7; rank >= 0; rank--, west >>= 8)
    {
        U64 new_west = west;
        for (int file = 7; file >= 0; file --, new_west >>= 1)
        {
            ray_attacks[rank * 8 + file][West] = west & new_west;
        }
    }

    // A line of ones extending northeastwards from a1
    U64 noea = 0x8040201008040200ULL;
    
    // Iterates through every rank and file. Starts with a mask in the a_file, that is unioned
    // with itself shifted one for every file. The negation of this is intersected with the original ray
    // attack to prevent wrapping around.
    for (int rank = 0; rank < 8; rank++){
        U64 wrap_mask = not_a_file;
        for (int file = 0; file < 8; file++, noea <<= 1, wrap_mask&= wrap_mask << 1)
        {
            ray_attacks[rank * 8 + file][NoEa] = noea & wrap_mask;
        }
    }

    U64 nowe = 0x102040810204000ULL;
    U64 rank_nowe = nowe;

    for (int rank = 0; rank < 8; rank++)
    {
        rank_nowe = nowe << (8 * rank);

        U64 wrap_mask = not_h_file;
        for (int file = 7; file >= 0; file--, rank_nowe >>= 1, wrap_mask &= wrap_mask >> 1)
        {
            int square = rank * 8 + file;
            //printf("%d\n", square);
            ray_attacks[square][NoWe] = rank_nowe & wrap_mask;
        }
    }

    U64 soea = 0x2040810204080ULL;
    U64 rank_soea;
    for (int rank = 7; rank >= 0; rank--)
    {
        U64 wrap_mask = not_a_file;
        rank_soea = soea >> ((7 - rank) * 8);
        for (int file = 0; file < 8; file++, rank_soea <<= 1, wrap_mask &= wrap_mask << 1)
        {
            int square = rank * 8 + file;
            ray_attacks[square][SoEa] = rank_soea & wrap_mask;
        }
    }

    U64 sowe = 0x40201008040201ULL;
    for (int rank = 7; rank >= 0; rank--){
        U64 wrap_mask = not_h_file;
        for (int file = 7; file >= 0; file--, sowe >>= 1, wrap_mask &= wrap_mask >> 1)
        {
            int square = rank*8 + file;
            ray_attacks[square][SoWe] = sowe & wrap_mask;
        }
    }
}

/* Retrieves attack tables for slider pieces (bishop, rook, queen) */
void Board::InitSliderAttacks(){

    // Initializes precalculated ray attack lookup tables
    InitRayAttacks();

    // Constructs rook attack table for every square on board
    for (int rank = 0; rank < 8; rank++)
    {
        for (int file = 0; file < 8; file++){
            int square = rank * 8 + file;
            rook_attacks[square] = CalcRookAttacks(square);
            bishop_attacks[square] = CalcBishopAttacks(square);
            queen_attacks[square] = CalcQueenAttacks(square);
        }
    }
}

/* Helper function used to find either the least significant bit (rightmost) or the most significant bit
(leftmost) of a bitboard. bitscan reverse is used to find the MSB */
int Board::BitScan(U64 bitboard, bool reverse=false){

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
