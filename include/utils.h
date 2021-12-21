#include <cstdint>
#pragma once

// Defines a type that can only hold a 64 bit integer
typedef uint64_t U64;


const U64 a_file = 0x101010101010101ULL;        // All 1's in a file
const U64 not_a_file = 0xfefefefefefefefeULL;   // All 1's except a file
const U64 not_h_file = 0x7f7f7f7f7f7f7f7fULL;   // All 1's except h file
const U64 not_ab_file = 0xfcfcfcfcfcfcfcfcULL;  // All 1's except ab files
const U64 not_gh_file = 0x3f3f3f3f3f3f3f3fULL;  // All 1's except gh files

// White is 0, black is 1
enum Color{white, black};

// Enumerates all possible kinds of pieces
enum pieceType
{
    white_pawn,
    white_rook,
    white_knight,
    white_bishop,
    white_queen,
    white_king,
    black_pawn,
    black_rook,
    black_knight,
    black_bishop,
    black_queen,
    black_king
};

//Macro that returns the bit of the bitboard at the square
#define get_bit(bitboard, square)(bitboard & (1ULL << square))

// Macro that sets a bit to 1 at a particular square
#define set_bit(bitboard, square)(bitboard |= (1ULL << square))

// Macro that pops a bit to 0 at a particular square
#define pop_bit(bitboard, square)(bitboard &= ~(1ULL << square))

// Macro that defines whether direction is negative ray direction, bases it off enum number
#define is_negative(dir)(dir > 3)

// Helper functions that perform bitscans either forward or reverse
int BitScan(U64 bitboard, bool reverse);

// Returns a vector of every index in the bitboard that is set to 1
std::vector<int> SerializeBitboard(U64 Bitboard);

// Prints out the bitboard in a pretty format
void PrintBoard(U64 bitboard);