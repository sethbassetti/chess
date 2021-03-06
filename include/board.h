#include <vector>
#include <iostream>
#include "utils.h"
#include "position.h"

#pragma once




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

struct gameState{
    Position position;
    int castling_rights;
    int en_passant_square;
    U64 rook_attacks[64];
    U64 bishop_attacks[64];
    U64 queen_attacks[64];
};

class Board
{

public:
    // Constructor function for board. Takes care of setting everything up.
    Board();

    Board(std::string fen_string);

    void Test();

    // Chooses a move via the start and end positions and calls the make move function
    int MakeMove(int start, int end);
    void MakeMove(Move move);
    void UnMakeMove(Move move);

    
    void Display();

    int perft(int depth);

    std::vector<Move> GenerateMoveList();

    int GetCurrentPlayer();
    Position GetPosition();

    int IsValidMove(int start, int end);

    std::string GetBoardFEN();

private:
    // Board state variables
    Position position;      // Holds the position of all pieces on the board
    int turn_to_move;       // Holds the color of whose turn it is
    int en_passant_square;  // Holds the square that a piece can make an en passant move to, if 0 then no en passant
    bool test_flag;

    // This stores castling rights
    int castling_rights;

    // Stores history of game states so that moves can be unmade efficiently
    vector<gameState> game_state_hist;

    // Stores precalculated attack tables for various pieces
    U64 pawn_attacks[2][64];    // Moves of pawns depend on their color so we need two sides.
    U64 king_attacks[64];
    U64 knight_attacks[64];
    U64 rook_attacks[64];
    U64 bishop_attacks[64];
    U64 queen_attacks[64];

    // Stores where pawns can be pushed to (not diagonal attacks)
    U64 single_pawn_pushes[2];
    U64 double_pawn_pushes[2];

    // Stores precalculated ray attack tables for each of the 8 directions
    U64 ray_attacks[64][8];

    // Functions that calculate attack tables for the pieces
    U64 CalcPawnAttacks(int side, int square);
    U64 CalcKingAttacks(int square);
    U64 CalcKnightAttacks(int square);
    U64 CalcRookAttacks(int square);
    U64 CalcBishopAttacks(int square);
    U64 CalcQueenAttacks(int square);

    void CalcPawnPushes();

    // Initializes the board and its pieces
    void InitializeBoard();



    // Initializes attack tables for different piece types. Slider(rook, queen, bishop), and leaper(king,
    // knight, and pawn)
    void InitLeaperAttacks();
    void InitSliderAttacks();

    // Initializes ray attacks for each square in every direction
    void InitRayAttacks();
    
    // Gets a specific ray attack on a square in a direction (does not penetrate other pieces)
    U64 GetDirRayAttacks(enumDirections direction, int square);



    int GetPieceType(int index);

    void GeneratePawnMoves(std::vector<Move>  &move_list);
    void GenerateKnightMoves(std::vector<Move> &move_list);
    void GenerateKingMoves(std::vector<Move> &move_list);
    void GenerateSliderMoves(std::vector<Move> &move_list);

    void FillMoveList(std::vector<int> piece_indices, U64 attack_map[64], std::vector<Move> &move_list);




    std::vector<Move> ParseLegalMoves(std::vector<Move> move_list);

    bool KingInCheck(int color);
    bool IsSquareAttacked(int square, int color);



    void perft_test(int depth);

    void ToggleMove();

    void ParseFENColorCastling(std::string fen_string);
};
