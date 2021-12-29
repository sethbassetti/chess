#include <string.h>
#include <iostream>
#include <cctype>
#include <algorithm>
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

    Update();
}

/* Initializes all piece and color bitboards to 0 and then passes them to parseFEN to populate them */
Position::Position(std::string fen_string){

    /* White pieces, black pieces, and all pieces */
    U64 white_pieces = 0ULL;
    U64 black_pieces = 0ULL;

    /* white pieces */
    U64 white_pawns    = 0ULL;
    U64 white_knights  = 0ULL;
    U64 white_bishops  = 0ULL;
    U64 white_rooks    = 0ULL;
    U64 white_queens   = 0ULL;
    U64 white_kings    = 0ULL;

    /* black pieces */
    U64 black_pawns    = 0ULL;
    U64 black_knights  = 0ULL;
    U64 black_bishops  = 0ULL;
    U64 black_rooks    = 0ULL;
    U64 black_queens   = 0ULL;
    U64 black_kings    = 0ULL;

    U64 null = 0ULL;

    pieces = {null, white_pawns, white_rooks, white_knights, white_bishops, white_queens, white_kings,
              black_pawns, black_rooks, black_knights, black_bishops, black_queens, black_kings};
    
    colors = {white_pieces, black_pieces};
    ParseFEN(fen_string);
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

void Position::ParseFEN(std::string fen_string){

    // Because FEN strings start on the 8th rank and first file and move top left to bottom right
    int rank = 7;
    int file = 0;
    /* This for loop will place the pieces on the boards */
    for (unsigned int i = 0; i < fen_string.length(); i++){
        // Retrieves the token in the fen_string
        char token = fen_string.at(i);
        int square_index = rank * 8 + file;

        // If it is a whitespace, break out of this for loop since we are done assigning pieces
        if(token == ' '){
            break;
        }

        // If it is a slash, move to the next rank and reset the file
        else if(token == '/'){
            rank--;
            file = 0;

        // If it is a digit, skip ahead that many spaces
        }else if(isdigit(token)){
            int int_token = token - '0';
            file += int_token;

        // Otherwise, it is a letter, so set the piece on the bitboard
        }else{
            SetPieceOnBitboard(token, square_index);
            file += 1;
        }
    }
}

/* Given a character representing the piece and a square, places the piece on that square */
void Position::SetPieceOnBitboard(char token, int square){
    // A vector of characters that holds the same indexes as the pieces vector in the position class
    vector<char> piece_chars = {'0', 'P', 'R', 'N', 'B', 'Q', 'K', 'p', 'r', 'n', 'b', 'q', 'k'};

    // Finds the index of the character in the character vector 
    auto it = std::find(piece_chars.begin(), piece_chars.end(), token);
    int piece_index = it - piece_chars.begin();

    // Uses the index to retrieve the piece bitboard and sets the bit corresponding to the square
    set_bit(pieces[piece_index], square);
}

std::string Position::GenerateFEN(){
    int rank = 7;
    int file = 0;
    std::string fen_string;

    // A vector of characters that holds the same indexes as the pieces vector in the position class
    vector<char> piece_chars = {'0', 'P', 'R', 'N', 'B', 'Q', 'K', 'p', 'r', 'n', 'b', 'q', 'k'};

    for (int rank = 7; rank >= 0; rank--){
        int empty_counter = 0;
        for (int file = 0; file < 8; file++)
        {
            int square_index = rank * 8 + file;
            int piece_type = GetPieceType(square_index);
            if(piece_type){
                if(empty_counter){
                    fen_string.append(to_string(empty_counter));
                    empty_counter = 0;
                }
                fen_string.push_back(piece_chars[piece_type]);
            }else{
                empty_counter++;
            }
        }
        if(empty_counter){
            fen_string.append(to_string(empty_counter));
        }
        if(rank != 0){
            fen_string.append("/");
        }
        
    }
    return fen_string;
}