#include <cstdint>
#pragma once

// Defines a type that can only hold a 64 bit integer
typedef uint64_t U64;

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

std::vector<int> GetSetBits(U64 Bitboard);