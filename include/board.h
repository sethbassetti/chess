#include <vector>
#include <iostream>
#include "utils.h"
#include "move_calc.h"

#pragma once






/*
struct gameState{
    int castling_rights;
    int en_passant_square;
    U64 rook_attacks[64];
    U64 bishop_attacks[64];
    U64 queen_attacks[64];
};*/

class Board
{

public:
    
    // Constructor function for board. Takes care of setting everything up.
    Board();

    // Constructor function for board that builds a board based on an FEN string
    Board(std::string fen_string);

    // Chooses a move via the start and end positions and calls the make move function
    int MakeMove(int move, int move_flag);
    
    /* Driver around the perft function for move generation */
    void perft_driver(int depth);

    /* Displays the board in a human-readable format with ASCII pieces */
    void Display();

    
    

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

    // Calculates and stores all of the pre-initialized attacks
    MoveCalc move_calc;

    // Used to generate moves for pawns, and king castling. Adds them to the move list pointer
    void GenerateQuietPawnMoves(MoveList* move_list);
    void GenerateCastleMoves(MoveList* move_list);
    void GeneratePawnAttacks(MoveList* move_list);

    // Function that adds a move to a move list struct and updates how many moves exist within it 
    void AddMove(MoveList *move_list, int move);

    // Generates all possible moves based on the board state and adds them to the move list
    void GenerateMoves(MoveList* move_list);

    // Returns true if the given square is being attacked by the given color side
    bool IsSquareAttacked(int square, int color);

    // Helper function, inner loop of perft driver that recursively generates moves to a certain depth
    int perft(int depth);

};
