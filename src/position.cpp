#include <string.h>
#include <iostream>
#include "position.h"

using namespace std;

Position::Position(){
    /* White pieces, black pieces, and all pieces */
    U64 white_pieces = 0xFFFFULL;
    U64 black_pieces = 0xFFFF000000000000LLU;
    

    /* white pieces */
    U64 white_pawns    = 0xFF00LLU;
    U64 white_knights  = 0x42LLU;
    U64 white_bishops  = 0x24LLU;
    U64 white_rooks    = 0x81LLU;
    U64 white_queens   = 0x8LLU;
    U64 white_kings    = 0x10LLU;

    /* black pieces */
    U64 black_pawns    = 0xFF000000000000LLU;
    U64 black_knights  = 0x4200000000000000LLU;
    U64 black_bishops  = 0x2400000000000000LLU;
    U64 black_rooks    = 0x8100000000000000LLU;
    U64 black_queens   = 0x800000000000000LLU;
    U64 black_kings    = 0x1000000000000000LLU;



    U64 null = 0ULL;
    pieces = {null, white_pawns, white_rooks, white_knights, white_bishops, white_queens, white_kings,
              black_pawns, black_rooks, black_knights, black_bishops, black_queens, black_kings};
    colors = {white_pieces, black_pieces};
    en_passant = false;
    Update();
}

void Position::Update(){
    colors[white] = pieces[1] | pieces[2] | pieces[3] | pieces[4] | pieces[5] | pieces[6];
    colors[black] = pieces[7] | pieces[8] | pieces[9] | pieces[10] | pieces[11] | pieces[12];
    occupancy = colors[white] | colors[black];
    empty = ~occupancy;
}

int Position::GetPieceType(int index){

    for (int i = 0; i < 13; i++){
        if(get_bit(pieces[i], index)){
            return i;
        }
    }
    return 0;
}

void Position::PrintBoard(){
    string ascii[13] = {".", "\u2659", "\u2656", "\u2658", "\u2657", "\u2655", "\u2654", "\u265F", "\u265C",
                      "\u265E", "\u265D", "\u265B", "\u265A"};
    for (int rank = 7; rank >= 0; rank--)
    {
        cout << rank + 1 << "  ";
        for (int file = 0; file < 8; file++){
            int square = rank * 8 + file;
            int piece_type = GetPieceType(square);

            cout << ascii[piece_type] << "  ";
        }
        printf("\n");
    }
    // Print board file labels (A-H)
    printf("   A  B  C  D  E  F  G  H\n\n");
}
