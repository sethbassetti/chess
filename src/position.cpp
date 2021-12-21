#include "position.h"

Position::Position(){
    /* White pieces, black pieces, and all pieces */
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


    U64 null = 0ULL;
    pieces = {null, white_pawns, white_rooks, white_knights, white_bishops, white_queens, white_kings,
              black_pawns, black_rooks, black_knights, black_bishops, black_queens, black_kings};
    colors = {white_pieces, black_pieces};
    ResetOccupancy();
}

void Position::ResetOccupancy(){
    occupancy = colors[white] | colors[black];
    empty = ~occupancy;
}
