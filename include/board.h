#include <vector>
#include <iostream>
#include "utils.h"
#include "position.h"

#pragma once







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
    
    // piece bitboards
    U64 pieces[12];

    // Occupancy bitboards (white, black, both)
    U64 occupancies[3];

    // Board state variables
    Position position;      // Holds the position of all pieces on the board
    int turn_to_move;       // Holds the color of whose turn it is
    int enpassant;          // Holds the square that a piece can make an en passant move to, if 0 then no en passant
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


    // Functions that calculate attack tables for the pieces
    U64 CalcPawnAttacks(int side, int square);

    void CalcPawnPushes();

    // Initializes the board and its pieces
    void InitializeBoard();


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
