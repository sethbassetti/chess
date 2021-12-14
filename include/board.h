#pragma once

// Defines a type that can only hold a 64 bit integer
typedef uint64_t U64;

// Contains information that represents a move
struct Move{
    U64 start;
    U64 end;
    int move_type;
};

// Enumerates everything on the board so it can be described with numbers or letters
enum enumSquare{
    a1, b1, c1, d1, e1, f1, g1, h1,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a8, b8, c8, d8, e8, f8, g8, h8,
};

// Enumerates directions 
enum enumDirections
{
    Nort,
    NoEa,
    East,
    NoWe,
    SoEa,
    Sout,
    SoWe,
    West,
};

class Board
{

public:
    // Constructor function for board. Takes care of setting everything up.
    Board();

    // Helper function that prints a bitboard in a pretty format
    void PrintBoard(U64 bitboard);
    void Test();

private:
    /* Combined pieces */
    U64 white_pieces;
    U64 black_pieces;
    U64 occupancy;

    /* white pieces */
    U64 white_pawns;
    U64 white_knights;
    U64 white_bishops;
    U64 white_rooks;
    U64 white_queens;
    U64 white_kings;

    /* black pieces */
    U64 black_pawns;
    U64 black_knights;
    U64 black_bishops;
    U64 black_rooks;
    U64 black_queens;
    U64 black_kings;

    // Stores precalculated attack tables for various pieces
    U64 pawn_attacks[2][64];    // Moves of pawns depend on their color so we need two sides.
    U64 king_attacks[64];
    U64 knight_attacks[64];
    U64 rook_attacks[64];
    U64 bishop_attacks[64];
    U64 queen_attacks[64];

    // Stores precalculated ray attack tables for each of the 8 directions
    U64 ray_attacks[64][8];

    // Functions that calculate attack tables for the pieces
    U64 CalcPawnAttacks(int side, int square);
    U64 CalcKingAttacks(int square);
    U64 CalcKnightAttacks(int square);
    U64 CalcRookAttacks(int square);
    U64 CalcBishopAttacks(int square);
    U64 CalcQueenAttacks(int square);

    // Initializes the board and its pieces
    void InitializeBoard();

    // Function that simply calls the different functions that initialize attack tables
    void InitializeAttackSets();

    // Initializes attack tables for different piece types. Slider(rook, queen, bishop), and leaper(king,
    // knight, and pawn)
    void InitLeaperAttacks();
    void InitSliderAttacks();

    // Initializes ray attacks for each square in every direction
    void InitRayAttacks();
    
    // Gets a specific ray attack on a square in a direction (does not penetrate other pieces)
    U64 GetDirRayAttacks(enumDirections direction, int square);

    // Helper functions that perform bitscans either forward or reverse
    int BitScan(U64 bitboard, bool reverse);
    int bitScanReverse(U64 bitboard);
};