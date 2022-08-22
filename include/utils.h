#include <cstdint>
#include <vector>
#include <iostream>

#pragma once

// Defines a type that can only hold a 64 bit integer
typedef uint64_t U64;


const U64 a_file = 0x101010101010101ULL;        // All 1's in a file
const U64 not_a_file = 0xfefefefefefefefeULL;   // All 1's except a file
const U64 not_h_file = 0x7f7f7f7f7f7f7f7fULL;   // All 1's except h file
const U64 not_ab_file = 0xfcfcfcfcfcfcfcfcULL;  // All 1's except ab files
const U64 not_gh_file = 0x3f3f3f3f3f3f3f3fULL;  // All 1's except gh files
const U64 first_last_ranks = 0xff000000000000ffULL; // 1's on the bottom and top ranks
const U64 wk_castle_occupancy = 0x60ULL; // Squares set in between white rook and king
const U64 wq_castle_occupancy = 0xeULL;  // Squares set in between white rook and king for queenside castle
const U64 bk_castle_occupancy = 0x6000000000000000ULL; // Squares set in between black rook and king/kingside castle
const U64 bq_castle_occupancy = 0xe00000000000000ULL; // Squares set in between black rook and king for queenside castle

const std::vector<std::string> square_index = {"a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
                                     "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
                                     "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
                                     "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
                                     "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
                                     "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
                                     "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
                                     "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8"};



// Contains information that represents a move
struct Move{
    int start;
    int end;
    int move_type;
    int capture;
    int promo_piece;
};

enum moveType
{
    quiet,
    double_pawn_push,
    capture,
    ep_capture,
    castle,
    promotion
};

// These are castle bits
enum
{
    wk = 1,
    wq = 2,
    bk = 4,
    bq = 8
};

// White is 0, black is 1
enum Color{white, black};

enum {rook, bishop};

// Enumerates all possible kinds of pieces
enum pieceType
{
    blank,
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

#define count_bits(bitboard) __builtin_popcountll(bitboard)

// Macro that defines whether direction is negative ray direction, bases it off enum number
#define is_negative(dir)(dir > 3)

// Helper functions that perform bitscans either forward or reverse
int BitScan(U64 bitboard, bool reverse=false);

// Returns a vector of every index in the bitboard that is set to 1
std::vector<int> SerializeBitboard(U64 Bitboard);

// Prints out the bitboard in a pretty format
void PrintBoard(U64 bitboard);

void PrintMove(struct Move move);

void MoveBit(U64 &bitboard, int start, int end);

int GetIndexFromSquare(char *square);

U64 GenerateMagicCandidate();