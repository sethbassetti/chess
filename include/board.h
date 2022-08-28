#include <vector>
#include <iostream>
#include "utils.h"
#include "move_calc.h"

#pragma once


class Board
{

public:
    
    // Constructor function for board. Takes care of setting everything up.
    Board();

    // Constructor function for board that builds a board based on an FEN string
    Board(std::string fen_string);

    // Generates all possible moves and chooses a random one
    int GetRandomMove();

    // Chooses a move via the start and end positions and calls the make move function
    int MakeMove(int move, int move_flag);

    // Generates all possible moves based on the board state and adds them to the move list
    void GenerateMoves(MoveList* move_list);
    
    /* Driver around the perft function for move generation */
    void perft_driver(int depth);

    /* Displays the board in a human-readable format with ASCII pieces */
    void Display();

    // Evaluates the current state of the board and returns a number indicating which side has an advantage
    int Evaluate();

    
    

private:
    // Helper macro to copy the board state for copy/make approach
    #define copy_board()                                                                    \
        U64 pieces_copy[12], occupancies_copy[3];                                           \
        int turn_copy, enpassant_copy, castle_copy;                                         \
        memcpy(pieces_copy, pieces, sizeof(pieces));                                        \
        memcpy(occupancies_copy, occupancies, sizeof(occupancies));                         \
        turn_copy=turn_to_move, enpassant_copy=enpassant, castle_copy=castling_rights;  

    // Helper macro to restore the board state for copy/make approach
    #define take_back()                                                                     \
        memcpy(pieces, pieces_copy, sizeof(pieces));                                        \
        memcpy(occupancies, occupancies_copy, sizeof(occupancies));                         \
        turn_to_move=turn_copy, enpassant=enpassant_copy, castling_rights=castle_copy;  

    // piece bitboards
    U64 pieces[12];

    // Occupancy bitboards (white, black, both)
    U64 occupancies[3];

    // Board state variables
    int turn_to_move;       // Holds the color of whose turn it is
    int enpassant;          // Holds the square that a piece can make an en passant move to, if 0 then no en passant

    // This stores castling rights
    int castling_rights;

    // This board stores the castling rights for any potential moves. If a piece moves to or from a square
    // that isn't 15 (indicating full castling rights), they lose some castling right. For example if the rook at a1 moves,
    // the castling rights are &'ed with 13, meaning that white queenside castle is no longer available.
    const int board_castling_rights[64] = {
    13, 15, 15, 15,  12, 15, 15, 14,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
     7, 15, 15, 15,  3, 15, 15, 11 
};

    // Calculates and stores all of the pre-initialized attacks
    MoveCalc move_calc;

    // Used to generate moves for pawns, and king castling. Adds them to the move list pointer
    void GenerateQuietPawnMoves(MoveList* move_list);
    void GenerateCastleMoves(MoveList* move_list);
    void GeneratePawnAttacks(MoveList* move_list);

    // Function that adds a move to a move list struct and updates how many moves exist within it 
    void AddMove(MoveList *move_list, int move);

     

    // Returns true if the given square is being attacked by the given color side
    bool IsSquareAttacked(int square, int color);

    // Helper function, inner loop of perft driver that recursively generates moves to a certain depth
    int perft(int depth);



    

    /***
     * Tables used for positional piece evaluation 
     ***/
    // pawn positional score
    const int pawn_scores[64] = 
    {

        0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0, -10, -10,   0,   0,   0,
        0,   0,   0,   5,   5,   0,   0,   0,
        5,   5,  10,  20,  20,   5,   5,   5,
        10,  10,  10,  20,  20,  10,  10,  10,
        20,  20,  20,  30,  30,  30,  20,  20,
        30,  30,  30,  40,  40,  30,  30,  30,
        90,  90,  90,  90,  90,  90,  90,  90
    };

    // knight positional score
    const int knight_scores[64] = 
    {
        -5, -10,   0,   0,   0,   0, -10,  -5,
        -5,   0,   0,   0,   0,   0,   0,  -5,
        -5,   5,  20,  10,  10,  20,   5,  -5,
        -5,  10,  20,  30,  30,  20,  10,  -5,
        -5,  10,  20,  30,  30,  20,  10,  -5,
        -5,   5,  20,  20,  20,  20,   5,  -5,
        -5,   0,   0,  10,  10,   0,   0,  -5,
        -5,   0,   0,   0,   0,   0,   0,  -5
    };

    // bishop positional score
    const int bishop_scores[64] = 
    {
        0,   0, -10,   0,   0, -10,   0,   0,
        0,  30,   0,   0,   0,   0,  30,   0,
        0,  10,   0,   0,   0,   0,  10,   0,
        0,   0,  10,  20,  20,  10,   0,   0,
        0,   0,  10,  20,  20,  10,   0,   0,
        0,   0,   0,  10,  10,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0
    };

    // rook positional score
    const int rook_scores[64] =
    {
        0,   0,   0,  20,  20,   0,   0,   0,
        0,   0,  10,  20,  20,  10,   0,   0,
        0,   0,  10,  20,  20,  10,   0,   0,
        0,   0,  10,  20,  20,  10,   0,   0,
        0,   0,  10,  20,  20,  10,   0,   0,
        0,   0,  10,  20,  20,  10,   0,   0,
        50,  50,  50,  50,  50,  50,  50,  50,
        50,  50,  50,  50,  50,  50,  50,  50
    };

    // king positional score
    const int king_scores[64] = 
    {
        0,   0,   5,   0, -15,   0,  10,   0,
        0,   5,   5,  -5,  -5,   0,   5,   0,
        0,   0,   5,  10,  10,   5,   0,   0,
        0,   5,  10,  20,  20,  10,   5,   0,
        0,   5,  10,  20,  20,  10,   5,   0,
        0,   5,   5,  10,  10,   5,   5,   0,
        0,   0,   5,   5,   5,   5,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0
    };

    // mirror positional score tables for opposite side
    const int mirror_scores[128] =
    {
        a8, b8, c8, d8, e8, f8, g8, h8,
        a7, b7, c7, d7, e7, f7, g7, h7,
        a6, b6, c6, d6, e6, f6, g6, h6,
        a5, b5, c5, d5, e5, f5, g5, h5,
        a4, b4, c4, d4, e4, f4, g4, h4,
        a3, b3, c3, d3, e3, f3, g3, h3,
        a2, b2, c2, d2, e2, f2, g2, h2,
        a1, b1, c1, d1, e1, f1, g1, h1,   
    };

};
