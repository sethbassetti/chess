#include <cstdint>
#include <iostream>
#include <stdio.h>
#include <bitset>
#include <bits/stdc++.h>
#include <strings.h>
#include <string.h>
#include <assert.h>  
#include <ctime>
#include <vector>
#include <algorithm>

#include "utils.h"
#include "board.h"
#include "move_calc.h"


using namespace std;


// Constructor for board, initializes board position and game state (en passant, castling, etc...)
Board::Board(){

    // Initialize the starting position on the board via the piece bitboards
    /* white pieces */
    pieces[P]   = 0xFF00LLU;    // white pawns
    pieces[N]   = 0x42LLU;      // white knights
    pieces[B]   = 0x24LLU;      // white bishops
    pieces[R]   = 0x81LLU;      // white rooks
    pieces[Q]   = 0x8LLU;       // white queen
    pieces[K]   = 0x10LLU;      // white king

    /* black pieces */
    pieces[p]   = 0xFF000000000000LLU;      // black pawns
    pieces[n]   = 0x4200000000000000LLU;    // black knights
    pieces[b]   = 0x2400000000000000LLU;    // black bishops
    pieces[r]   = 0x8100000000000000LLU;    // black rooks
    pieces[q]   = 0x800000000000000LLU;     // black queen
    pieces[k]   = 0x1000000000000000LLU;    // black king

    /* Define starting occupancies for both sides */
    occupancies[white] = 0xFFFFULL;
    occupancies[black] = 0xFFFF000000000000LLU;
    occupancies[both] = occupancies[white] | occupancies[black];

    // Sets white as the first turn
    turn_to_move = white;

    // Set en passant square to 0 (indicates no en passant available)
    enpassant = no_sq;

    // Sets castling rights such that all castling is available at the start (no pieces have moved)
    castling_rights = wk | wq | bk | bq;
}

/* Given some fen string, this function will initialize a board based on that string, set the board position and set
en passant/castling rights */
Board::Board(string fen_string){

    // Initialize the piece and occupancy bitboards as empty as empty
    for (int i = 0; i < 12; i++)    pieces[i] = 0ULL;
    for (int i = 0; i< 3; i++)      occupancies[i] = 0ULL;

    // Initializes the castling rights as 0
    castling_rights = 0;

    // Initialize no en passant square
    enpassant = no_sq;

    // A string to find which piece in the FEN string corresponds to which bitboard
    string piece_tokens = "PNBRQKpnbrqk";

    // Because FEN strings start on the 8th rank and first file and move top left to bottom right
    int rank = 7;
    int file = 0;

    /* Iterate over the FEN string assigning all the pieces to the board */
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

            // Iterate over the piece tokens string 
            for (unsigned int i = 0; i < piece_tokens.length(); i++)
            {   
                // If the token in the FEN string matches the piece token, assign a bit to the appropriate piece bitboard
                if (token == piece_tokens[i]) set_bit(pieces[i], square_index);
            }

            // Increment the file variable by 1
            file += 1;
        }
    }

    // Creates a stringstream that splits the FEN string up by spaces
    stringstream fen_split(fen_string);

    // The field will contain a current field from the FEN String
    string field;

    fen_split >> field; // Pushes the piece positions into field
    fen_split >> field; // Pushes the active color into field

    // Sets the active color based on the FEN string
    turn_to_move = (field == "w") ? white : black;

    fen_split >> field; // Pushes the castling rights into field

    // Iterates through each token in the castling rights
    for (char token : field)
    {
        // Depending on which token exists, add to the castling rights variable
        switch(token){
            case 'K':
                castling_rights |= wk;
                break;
            case 'Q':
                castling_rights |= wq;
                break;
            case 'k':
                castling_rights |= bk;
                break;
            case 'q':
                castling_rights |= bq;
                break;
            }
    }

    fen_split >> field; // Pushes the en passant info into the field

    // Iterates through every square
    for (int i = 0; i < 64; i++){

        // If the en passant field variable is the value of some square on the board, set the en passant square to that value
        if (square_index[i] == field) enpassant = i;
    }

    // initialize the occupancy bitboards
    occupancies[white] = pieces[P] | pieces[N] | pieces[B] | pieces[R] | pieces[Q] | pieces[K];
    occupancies[black] = pieces[p] | pieces[n] | pieces[b] | pieces[r] | pieces[q] | pieces[k];
    occupancies[both] = occupancies[white] | occupancies[black];
}

/* Given a square, checks if that square is attacked by a given side (white or black) */
bool Board::IsSquareAttacked(int square, int side)
{
    // attacked by white pawns
    if((side == white) && (move_calc.pawn_attacks[black][square] & pieces[P])) return true;

    // attacked by black pawns
    if ((side==black) && (move_calc.pawn_attacks[white][square] & pieces[p])) return true;

    // attacked by knights
    if (move_calc.knight_attacks[square] & ((side == white) ? pieces[N] : pieces[n])) return true;

    // attacked by bishops
    if (move_calc.GetBishopAttacks(square, occupancies[both]) & ((side==white) ? pieces[B] : pieces[b])) return true;
    
    // attacked by rooks
    if (move_calc.GetRookAttacks(square, occupancies[both]) & ((side==white) ? pieces[R] : pieces[r])) return true;

    // attacked by queens
    if (move_calc.GetQueenAttacks(square, occupancies[both]) & ((side==white) ? pieces[Q] : pieces[q])) return true;

    // attacked by king
    if (move_calc.king_attacks[square] & ((side == white) ? pieces[K] : pieces[k])) return true;

    // If not attacked by anything, return false
    return false;
}


/* Generates a list of pseudo-legal moves */
void Board::GenerateMoves(MoveList *move_list)
{
    // Init the source square and target of any move
    int source_square, target_square;

    // Initializes the enemy color
    int enemy = turn_to_move ^ 1;

    // If white, offset will be 0, (P + 0 = P), otherwise offset will be 6 (P + 6 = p) to denote white/black pieces
    int offset = turn_to_move * 6;

    // init a bitboard to hold current piece as well as all of it's attacks
    U64 bitboard, attacks;

    // Generates castling moves, pawn pushes, and pawn attacks in their own functions since they are tricky to generate
    GenerateCastleMoves(move_list);
    GenerateQuietPawnMoves(move_list);
    GeneratePawnAttacks(move_list);


    /**** Knight Moves ****/

    // Get the appropriately colored knight bitboard
    bitboard = pieces[N + offset];

    while(bitboard)
    {
        // get source square and attacks
        source_square = BitScan(bitboard);
        attacks = move_calc.knight_attacks[source_square] & ~occupancies[turn_to_move];

        // Iterate over attacks
        while(attacks)
        {   
            // get target square from bitboard
            target_square = BitScan(attacks);

            // Construct the move and add it to the list
            int move = encode_move(source_square, target_square, (N + offset), 0, (get_bit(occupancies[enemy], target_square) ? 1 : 0), 0, 0, 0);
            AddMove(move_list, move);
            
            // pop after move is generated
            pop_bit(attacks, target_square);
        }

        // pop bit after 
        pop_bit(bitboard, source_square);
    }

    /**** Rook Moves ****/
    bitboard = (turn_to_move == white) ? pieces[R] : pieces[r];

    while(bitboard)
    {
        // get source square and attacks
        source_square = BitScan(bitboard);
        attacks = move_calc.GetRookAttacks(source_square, occupancies[both]) & ~occupancies[turn_to_move];

        // Iterate over attacks
        while(attacks)
        {   
            // get target square from bitboard
            target_square = BitScan(attacks);

            // Construct the move and add it to the list
            int move = encode_move(source_square, target_square, (R + offset), 0, (get_bit(occupancies[enemy], target_square) ? 1 : 0), 0, 0, 0);
            AddMove(move_list, move);
            
            // pop after move is generated
            pop_bit(attacks, target_square);
        }

        // pop bit after 
        pop_bit(bitboard, source_square);
    }

    /**** Bishop Moves ****/
    bitboard = (turn_to_move == white) ? pieces[B] : pieces[b];

    while(bitboard)
    {
        // get source square and attacks
        source_square = BitScan(bitboard);
        attacks = move_calc.GetBishopAttacks(source_square, occupancies[both]) & ~occupancies[turn_to_move];

        // Iterate over attacks
        while(attacks)
        {   
            // get target square from bitboard
            target_square = BitScan(attacks);

            // Construct the move and add it to the list
            int move = encode_move(source_square, target_square, (B + offset), 0, (get_bit(occupancies[enemy], target_square) ? 1 : 0), 0, 0, 0);
            AddMove(move_list, move);
            
            // pop after move is generated
            pop_bit(attacks, target_square);
        }

        // pop bit after 
        pop_bit(bitboard, source_square);
    }

    /**** Queen Moves ****/
    bitboard = (turn_to_move == white) ? pieces[Q] : pieces[q];

    while(bitboard)
    {
        // get source square and attacks
        source_square = BitScan(bitboard);
        attacks = move_calc.GetQueenAttacks(source_square, occupancies[both]) & ~occupancies[turn_to_move];

        // Iterate over attacks
        while(attacks)
        {   
            // get target square from bitboard
            target_square = BitScan(attacks);

            // Construct the move and add it to the list
            int move = encode_move(source_square, target_square, (Q + offset), 0, (get_bit(occupancies[enemy], target_square) ? 1 : 0), 0, 0, 0);
            AddMove(move_list, move);
            
            // pop after move is generated
            pop_bit(attacks, target_square);
        }

        // pop bit after 
        pop_bit(bitboard, source_square);
    }

    /**** King Moves ****/
    bitboard = (turn_to_move == white) ? pieces[K] : pieces[k];

    while(bitboard)
    {
        // get source square and attacks
        source_square = BitScan(bitboard);
        attacks = move_calc.king_attacks[source_square] & ~occupancies[turn_to_move];

        // Iterate over attacks
        while(attacks)
        {   
            // get target square from bitboard
            target_square = BitScan(attacks);

            // Construct the move and add it to the list
            int move = encode_move(source_square, target_square, (K + offset), 0, (get_bit(occupancies[enemy], target_square) ? 1 : 0), 0, 0, 0);
            AddMove(move_list, move);
            
            // pop after move is generated
            pop_bit(attacks, target_square);
        }

        // pop bit after 
        pop_bit(bitboard, source_square);
    }
}

/* Generates all pawn pushes (single and double) moves, including promotions */
void Board::GenerateQuietPawnMoves(MoveList *move_list)
{

    // Initialize a variable to hold the source square and target square
    int source_square, target_square;
    
    // Initialize a move variable to contain the moves
    int move;

    // This will hold the pawn pieces
    U64 bitboard;

    // If turn to move is white, generate white pawn piece moves
    if (turn_to_move == white)
    {
        bitboard = pieces[P];

        // single push targets will be the white pawn shifted up 8 anded with negation of occupancies (empty squares)
        U64 white_pawn_single_targets = bitboard << 8 & ~occupancies[both];

        // Loop over target squares for white pawns
        while (white_pawn_single_targets)
        {
            // init target square
            target_square = BitScan(white_pawn_single_targets);

            // source square will be rank below target
            source_square = target_square - 8;
            
            // If target square is in the back row
            if ((1ULL << target_square) & first_last_ranks)
            {

                // Add all the possible promotions
                move = encode_move(source_square, target_square, P, N, 0, 0, 0, 0);
                AddMove(move_list, move);
                move = encode_move(source_square, target_square, P, R, 0, 0, 0, 0);
                AddMove(move_list, move);
                move = encode_move(source_square, target_square, P, B, 0, 0, 0, 0);
                AddMove(move_list, move);
                move = encode_move(source_square, target_square, P, Q, 0, 0, 0, 0);
                AddMove(move_list, move);
            }

            // Otherwise print it out with no promotions
            else 
            {
                move = encode_move(source_square, target_square, P, 0, 0, 0, 0, 0);
                AddMove(move_list, move);
            }

            // pop the bit from the target squares
            pop_bit(white_pawn_single_targets, target_square);
        }

        // Double push targets are pawns shifted up 16 that end up on rank 4
        U64 white_pawn_double_targets = (bitboard << 16) & rank4 & ~(occupancies[both] |(occupancies[both] << 8));

        // Loop over targets
        while(white_pawn_double_targets)
        {
            // Get target square
            target_square = BitScan(white_pawn_double_targets);

            // get source square
            source_square = target_square - 16;
            move = encode_move(source_square, target_square, P, 0, 0, 1, 0, 0);
            AddMove(move_list, move);

            pop_bit(white_pawn_double_targets, target_square);
        }
    }
    // Otherwise generate black pieces moves
    else
    {
        bitboard = pieces[p];

        // single push targets will be the white pawn shifted up 8 anded with negation of occupancies (empty squares)
        U64 black_pawn_single_targets = bitboard >> 8 & ~occupancies[both];

        // Loop over target squares for white pawns
        while (black_pawn_single_targets)
        {
            // init target square
            target_square = BitScan(black_pawn_single_targets);

            // source square will be rank below target
            source_square = target_square + 8;
            
            // If target square is in the back row
            if ((1ULL << target_square) & first_last_ranks)
            {
                // Add all the possible promotions
                move = encode_move(source_square, target_square, p, n, 0, 0, 0, 0);
                AddMove(move_list, move);
                move = encode_move(source_square, target_square, p, r, 0, 0, 0, 0);
                AddMove(move_list, move);
                move = encode_move(source_square, target_square, p, b, 0, 0, 0, 0);
                AddMove(move_list, move);
                move = encode_move(source_square, target_square, p, q, 0, 0, 0, 0);
                AddMove(move_list, move);
            }

            // Otherwise print it out with no promotions
            else 
            {
                move = encode_move(source_square, target_square, p, 0, 0, 0, 0, 0);
                AddMove(move_list, move);
            }

            // pop the bit from the target squares
            pop_bit(black_pawn_single_targets, target_square);
        }

        // Double push targets are pawns shifted up 16 that end up on rank 4
        U64 black_pawn_double_targets = (bitboard >> 16) & rank5 & ~(occupancies[both] | (occupancies[both] >> 8)) ;

        // Loop over targets
        while(black_pawn_double_targets)
        {
            // Get target square
            target_square = BitScan(black_pawn_double_targets);

            // get source square
            source_square = target_square + 16;
            move = encode_move(source_square, target_square, p, 0, 0, 1, 0, 0);
            AddMove(move_list, move);

            pop_bit(black_pawn_double_targets, target_square);
        }
    }
}

void Board::GenerateCastleMoves(MoveList *move_list)
{

    // stores the move
    int move;

    // Generate white side castling moves
    if (turn_to_move == white)
    {
        // Make sure white can castle king side
        if (castling_rights & wk)
        {
            // Make sure there are no squares blocking the castle
            if (!get_bit(occupancies[both], f1) && !get_bit(occupancies[both], g1))
            {
                // Make sure king does not move through an attacked square (and is not in check already)
                if (!IsSquareAttacked(e1, black) && !(IsSquareAttacked(f1, black))){
                    move = encode_move(e1, g1, K, 0, 0, 0, 0, 1);
                    AddMove(move_list, move);
                }
            }
        }
        // Make sure white can castle queen side
        if(castling_rights & wq)
        {
             // Make sure there are no squares blocking the castle
            if (!get_bit(occupancies[both], c1) && !get_bit(occupancies[both], d1))
            {
                // Make sure king does not move through an attacked square (and is not in check already)
                if (!IsSquareAttacked(d1, black) && !(IsSquareAttacked(e1, black))){
                    move = encode_move(e1, c1, K, 0, 0, 0, 0, 1);
                    AddMove(move_list, move);
                }
            }
        }
    }

    // Generate black side castling moves
    if (turn_to_move == black)
    {
        // Make sure white can castle king side
        if (castling_rights & bk)
        {
            // Make sure there are no squares blocking the castle
            if (!get_bit(occupancies[both], f8) && !get_bit(occupancies[both], g8))
            {
                // Make sure king does not move through an attacked square (and is not in check already)
                if (!IsSquareAttacked(e8, white) && !(IsSquareAttacked(f8, white))){
                    move = encode_move(e8, g8, k, 0, 0, 0, 0, 1);
                    AddMove(move_list, move);
                }
            }
        }
        // Make sure white can castle queen side
        if(castling_rights & bq)
        {
             // Make sure there are no squares blocking the castle
            if (!get_bit(occupancies[both], c8) && !get_bit(occupancies[both], d8))
            {
                // Make sure king does not move through an attacked square (and is not in check already)
                if (!IsSquareAttacked(d8, white) && !(IsSquareAttacked(e8, white))){
                    move = encode_move(e8, c8, k, 0, 0, 0, 0, 1);
                    AddMove(move_list, move);
                }
            }
        }
    }

        

}

/* Generates all possible pawn capture moves */
void Board::GeneratePawnAttacks(MoveList* move_list)
{

    // init source and target squares as well as attack and source bitboards
    int source_square, target_square;
    U64 bitboard, attacks;

    // init move variable to store move
    int move;

    // define the color of the enemy pieces
    int enemy = turn_to_move ^ 1;

    // define an offset to determine the appropriately colored piece
    int offset = turn_to_move * 6;

    // Define which source pawns we are going to use, depending on side
    bitboard = (turn_to_move  == white) ? pieces[P] : pieces[p];

    // Iterate over the source squares
    while(bitboard)
    {
        // retrieve the source square
        source_square = BitScan(bitboard);

        // Retrieve the attacks from the current turn's side that can hit an enemy
        // Adds the en passant square to the move to indicate that it can move (if enpassant is available)
        attacks = move_calc.pawn_attacks[turn_to_move][source_square] & (occupancies[enemy] | (1ULL << enpassant));

        


        // Iterate over the attack target squares
        while(attacks)
        {
            // Retrieve the target square from the attacks
            target_square = BitScan(attacks);

            // If the target square is in the first or last ranks, this should be a promotion move
            if (1ULL << target_square & first_last_ranks)
            {   
                // Add all the possible promotions
                move = encode_move(source_square, target_square, (P+offset), (N+offset), 1, 0, 0, 0);
                AddMove(move_list, move);
                move = encode_move(source_square, target_square, (P+offset), (R+offset), 1, 0, 0, 0);
                AddMove(move_list, move);
                move = encode_move(source_square, target_square, (P+offset), (B+offset), 1, 0, 0, 0);
                AddMove(move_list, move);
                move = encode_move(source_square, target_square, (P+offset), (Q+offset), 1, 0, 0, 0);
                AddMove(move_list, move);
            }
            else
            {
                
                // Otherwise print out the capture
                move = encode_move(source_square, target_square, (P+offset), 0, 1, 0, (target_square == enpassant), 0);
                AddMove(move_list, move);
            }

           
            // Pop bit off attacks bitboard
            pop_bit(attacks, target_square);
        }
        pop_bit(bitboard, source_square);


    }
}

/* Adds a move to the given move list */
void Board::AddMove(MoveList *move_list, int move){

    // Add the given move
    move_list->moves[move_list->count] = move;

    // Increment the number of moves currently held
    move_list->count++;
    
}


int Board::MakeMove(int move, int move_flag)
{

    // Used for looking at the correct side
    int offset = (turn_to_move) * 6;
    int enemy_offset = (turn_to_move ^ 1) * 6;

    // quiet moves
    if (move_flag == all_moves)
    {
        // parse move
        int source_square = get_move_source(move);
        int target_square = get_move_target(move);
        int piece = get_move_piece(move);
        int promoted = get_move_promoted(move);
        int capture = get_move_capture(move);
        int double_push = get_move_double(move);
        int enpass = get_move_enpassant(move);
        int castling = get_move_castling(move);

        // move piece
        pop_bit(pieces[piece], source_square);
        set_bit(pieces[piece], target_square);

        // handle capture moves
        if (capture)
        {
            

            // pick up bitboard piece index ranges depending on side
            int start_piece = P + enemy_offset;
            int end_piece = K + enemy_offset;

            // loop over all of the enemy pieces to see which one is being captures
            for (int bb_piece = start_piece; bb_piece <= end_piece; bb_piece++)
            {
                // if there is a piece on the target square
                if (get_bit(pieces[bb_piece], target_square))
                {
                    // then remove it and stop looking for pieces   
                    pop_bit(pieces[bb_piece], target_square);
                    break;
                }
            }
        }

        // Handle promotions
        if (promoted)
        {   
            // remove the enemy pawn from the last rank
            pop_bit(pieces[P + offset], target_square);

            // initialize a new promoted piece on that square
            set_bit(pieces[promoted], target_square);
        }

        

        // Handle en passant moves
        if (enpass)
        {
            cout << square_index[target_square] << endl;
            // If white's turn, remove black pawn at the appropriate spot, otherwise remove white pawn
            (turn_to_move == white) ? (pop_bit(pieces[p], (target_square - 8))) : (pop_bit(pieces[P], (target_square + 8)));

        }

        // Always reset en passant square before checking for double pushes and after checking for en passant captures
        enpassant = no_sq;

        // Handle double pushes
        if (double_push)
        {   
            // If a pawn has a double move, then set the en passant square
            enpassant = (turn_to_move == white) ? (target_square - 8) : (target_square + 8);
        }

        // Handle castle moves
        if (castling)
        {
            switch(target_square)
            {
                case (g1):
                    // Move H rook
                    pop_bit(pieces[R], h1);
                    set_bit(pieces[R], f1);

                    // update castling rights so white can't castle
                    castling_rights &= ~(wk | wq);
                    break;

                case (c1):
                    // Move A rook
                    pop_bit(pieces[R], a1);
                    set_bit(pieces[R], d1);

                    // update castling rights so white can't castle
                    castling_rights &= ~(wk | wq);       
                    break;

                case (g8):
                    // Move H rook
                    pop_bit(pieces[r], h8);
                    set_bit(pieces[r], f8);

                    // update castling rights so black can't castle
                    castling_rights &= ~(bk | bq);
                    break;
                
                case (c8):
                    // Move A Rook
                    pop_bit(pieces[r], a8);
                    set_bit(pieces[r], d8);

                    // update castling rights so black can't castle
                    castling_rights &= ~(bk | bq);
                    break;
            }
        }

        // Re-initialize the occupancies bitboards after a move has been made
        occupancies[white] = pieces[P] | pieces[N] | pieces[R] | pieces[B] | pieces[Q] | pieces[K];
        occupancies[black] = pieces[p] | pieces[n] | pieces[r] | pieces[b] | pieces[q] | pieces[k];
        occupancies[both] = occupancies[white] | occupancies[black];

    }

    // capture moves
    else
    {
        // If it is a capture move, make it
        if (get_move_capture(move))
            MakeMove(move, all_moves);

        // otherwise return 0
        else
            return 0;
    }
}


void Board::perft_driver(){
    
    MoveList moves;

    GenerateMoves(&moves);

    for (int i = 0; i < moves.count; i++){
        int move = moves.moves[i];

        copy_board();
        getchar();
        Display();
        MakeMove(move, all_moves);
        getchar();
        Display();

        take_back();

    }

}
/*

void Board::GenerateKingMoves(vector<Move> &move_list){

    int offset = turn_to_move * 6;
    U64 king = position.pieces[white_king + offset];

    vector<int> king_indices = SerializeBitboard(king);
    FillMoveList(king_indices, king_attacks, move_list);

    
    // Generate castling moves for white
    if(!turn_to_move){
        // Kingside castling rights
        if(castling_rights & wk){
            if(!(wk_castle_occupancy & position.occupancy)){
                Move castle_move = {king_indices.at(0), g1, castle};
                move_list.push_back(castle_move);
            }
        }
        // Queenside castling rights
        if(castling_rights & wq){
            if(!(wq_castle_occupancy & position.occupancy)){
                Move castle_move = {king_indices.at(0), c1, castle};
                move_list.push_back(castle_move);
            }
        }
    }else{
        // Kingside castling rights
        if(castling_rights & bk){
            if(!(bk_castle_occupancy & position.occupancy)){
                Move castle_move = {king_indices.at(0), g8, castle};
                move_list.push_back(castle_move);
            }
        }
        // Queenside castling rights
        if(castling_rights & bq){
            if(!(bq_castle_occupancy & position.occupancy)){
                Move castle_move = {king_indices.at(0), c8, castle};
                move_list.push_back(castle_move);
            }
        }
    }
    
}
*/

/*

void Board::GenerateSliderMoves(vector<Move> &move_list){

    int offset = turn_to_move * 6;

    U64 queens = position.pieces[white_queen + offset];
    U64 rooks = position.pieces[white_rook + offset];
    U64 bishops = position.pieces[white_bishop + offset];

    vector<int> queen_index = SerializeBitboard(queens);
    vector<int> rook_index = SerializeBitboard(rooks);
    vector<int> bishop_index = SerializeBitboard(bishops);

    FillMoveList(queen_index, queen_attacks, move_list);
    FillMoveList(rook_index, rook_attacks, move_list);
    FillMoveList(bishop_index, bishop_attacks,  move_list);
}

void Board::FillMoveList(vector<int> piece_list, U64 attack_map[64], vector<Move> &move_list){

    U64 color_pieces = position.colors[turn_to_move];
    for (int piece : piece_list)
    {
        vector<int> targets = SerializeBitboard(attack_map[piece] & ~color_pieces);
        for(int target : targets){
            int captured_piece = position.GetPieceType(target);
            Move move = {piece, target, quiet, captured_piece};
            if (captured_piece)
            {
                move.move_type = capture;
            }

            move_list.push_back(move);
        }
    }
}
vector<Move> Board::GenerateMoveList(){

    vector<U64> piece_bitboards;
    vector<Move> move_list;
    vector<Move> legal_moves;

    // Adds all pawn moves for the color to the move list
    GeneratePawnMoves(move_list);

    // Adds all knight moves for the color to the move list
    GenerateKnightMoves(move_list);

    // Adds all king moves for the color to the move list
    GenerateKingMoves(move_list);

    GenerateSliderMoves(move_list);

    legal_moves = ParseLegalMoves(move_list);

    return legal_moves;
}

 Parses the move list and returns a new move list with only legal moves. I.e. moves that do not put the king
in check and valid castle moves (no intermediate squares under attack) 

vector<Move> Board::ParseLegalMoves(vector<Move> move_list){

    vector<Move> legal_moves;

    // Iterate over the move list, only adding moves that are legal
    for (Move move : move_list)
    {

        // If the move is a castle move, check that none of the squares the king moves through are under attack
        if(move.move_type == castle){
            if(IsSquareAttacked(move.start, turn_to_move) || IsSquareAttacked(move.end, turn_to_move) || IsSquareAttacked((move.start + move.end)/2, turn_to_move)){
                continue;
            }else{
                legal_moves.push_back(move);
                continue;
            }
        }

        // Make the move and look if the king is in check. If not, add the move to the list, and unmake it
        MakeMove(move);

        // Since MakeMove() toggles the turn to move, toggle it again when calling the kingincheck() function
        if(!KingInCheck(turn_to_move ^ 1)){
            legal_moves.push_back(move);
        }
        UnMakeMove(move);
    }

    return legal_moves;
}

 This function takes a move object and moves the necessary pieces on the board. It looks at the move type,
such as castling, promotions, or en passants, and does the necessary board adjustments to accurately make that move.
At the end of the function, it resets the board's position object to reflect the changes 
void Board::MakeMove(Move move){

    // Before making the move, store the board's game state so that this move can be unmade
    gameState state = {position, castling_rights, enpassant};

    // Stores the slider attacks so InitSliderAttacks() does not have to be called on unmake move
    memcpy(state.queen_attacks, queen_attacks, sizeof(queen_attacks));
    memcpy(state.rook_attacks, rook_attacks, sizeof(rook_attacks));
    memcpy(state.bishop_attacks, bishop_attacks, sizeof(bishop_attacks));

    // Pushes the state onto the game history stack
    game_state_hist.push_back(state);

    // Gets the type of piece being moved to update it's bitboard
    int moving_piece_type = position.GetPieceType(move.start);


    // Moves the piece to its ending position
    MoveBit(position.pieces[moving_piece_type], move.start, move.end);
    //pop_bit(position.pieces[moving_piece_type], move.start);

    // If this is a capture, remove the captured piece from its bitboard
    if(move.move_type == capture){
        pop_bit(position.pieces[move.capture], move.end);

    // If this is an en passant capture, calculate where the pawn that needs to be captured is, then remove it
    }else if(move.move_type == ep_capture){
        
        // The offset looks at the piece behind the moving pawn to obtain the pawn to be captured
        int offset = (turn_to_move == white) ? -8 : 8;
        pop_bit(position.pieces[move.capture], (move.end + offset));
    }

    //If this is a promotion, remove the pawn from the last rank and replace it with the promotion choice
    if(move.promo_piece){
        pop_bit(position.pieces[moving_piece_type], move.end);
        set_bit(position.pieces[move.promo_piece + turn_to_move * 6], move.end);
    }

    // If this move is a double pawn push, set the en passant square
    if(move.move_type == double_pawn_push){
        enpassant = (move.start + move.end) / 2;

    // If it is not a double pawn push, reset the en passant square
    }else{
        enpassant = 0;
    }

    // If this move is a castle, determine what kind of castle, move the appropriate rook, and adjust castling rights
    if(move.move_type == castle){
        // Depending on how the king is castling, needs to move the correct rook
        switch(move.end){
            case g1:
                MoveBit(position.pieces[white_rook], h1, f1);
                break;
            case c1:
                MoveBit(position.pieces[white_rook], a1, d1);
                break;
            case c8:
                MoveBit(position.pieces[black_rook], a8, d8);
                break;
            case g8:
                MoveBit(position.pieces[black_rook], h8, f8);
                break;
        }
    }
    // Adjusts castling rights, if a king or rook moves
    switch(moving_piece_type){
        // If either of the kings move, take away castling rights for that color
        case white_king:
            castling_rights &= 12;
            break;
        case black_king:
            castling_rights &= 3;
            break;

        // If a rook moves, look at the starting square to determine which side (king/queen) loses castling rights
        case white_rook:
            if(move.start == a1){
                castling_rights &= 15 - wq;
            }
            else if(move.start == h1){
                castling_rights &= 15 - wk;
            }
            break;

        case black_rook:
            if(move.start == a8){
                castling_rights &= 15 - bq;
            }else if(move.start == h8){
                castling_rights &= 15 - bk;
            }
            break;
        }

    // Updates the position pieces
    position.Update();
    // Switches whose turn it is to play
    ToggleMove();
}

Attempts to make a move with a starting and ending square, returns 1 if successful, 0 otherwise 
int Board::MakeMove(int start, int end){
    vector<Move> moves = GenerateMoveList();
    for(Move move : moves){
        if(start == move.start && end == move.end){
            MakeMove(move);
            return 1;
        }
    }
    printf("Invalid Move!\n");
    return 0;
}


Returns true if the king of the input color is being attacked by any other enemy piece 
bool Board::KingInCheck(int color){
    
    // Offset determines what color of king to retrieve
    int offset = 6 * color;
    U64 king = position.pieces[white_king + offset];

    // Retrieves the index of the king's location
    int king_square = BitScan(king);


    // If the king square is being attacked, return true, since it is in check
    return IsSquareAttacked(king_square, color);
}


int Board::perft(int depth){
    int nodes = 0;

    if (depth == 0)
    {
        return 1;
    }

    vector<Move> moves = GenerateMoveList();
    for (Move move : moves)
    {
        MakeMove(move);
        
        nodes += perft(depth - 1);
        
        
        UnMakeMove(move);
    }

    return nodes;
}
*/

/*
void Board::ToggleMove(){
    turn_to_move ^= 1;
}

 Displays the board in ASCII format */
void Board::Display(){

    // ASCII characters for all of the board pieces
    string ascii[12] = {"♙", "♘", "♗", "♖", "♕", "♔", "♟", "♞", "♝", "♜", "♛", "♚"};

    // Iterate over every rank
    for (int rank = 7; rank >= 0; rank--)
    {
        // Print the rank and a space
        cout << rank + 1 << "  ";

        // Go through every file
        for (int file = 0; file < 8; file++){

            // Init the square
            int square = rank * 8 + file;

            // Initialize the piece as -1 (indicates a blank)
            int piece = -1;

            // Checks to see if there is a piece on this square
            for (int bb_piece=P; bb_piece <= k; bb_piece++)
            {
                // If one of the bitboards has a piece on that square, set the piece to the appropriate one
                if (get_bit(pieces[bb_piece], square)) piece = bb_piece;
            }

            // If the piece is -1 just print out a dot, otherwise print out the ASCII for the piece
            string piece_str = (piece == -1) ? "." : ascii[piece];
            cout << ascii[piece] << "  ";
        }
        
        // Print a new rank
        printf("\n");
    }
    // Print board file labels (A-H)
    printf("   A  B  C  D  E  F  G  H\n\n");

    // Prints the current player's turn
    printf("Side to move:\t%s\n", turn_to_move ? "black" : "white");

    // Retrieves the en passant square (if that is possible) and prints it out
    cout << "En passant:\t" << ((enpassant == no_sq) ? "No" : square_index[enpassant]) << endl;

    // Checks each castling right and prints out that side can castle if they can
    cout << "Castling:\t" <<
                                 ((castling_rights & wk) ? "wk | " : "") << 
                                 ((castling_rights & wq) ? "wq | " : "") <<
                                 ((castling_rights & bk) ? "bk | " : "") <<
                                 ((castling_rights & bq) ? "bq" : "") <<
                                 endl;


}

/*
int Board::IsValidMove(int start, int end)
{
    vector<Move> moves = GenerateMoveList();
    for(Move move : moves){
        if(move.start == start && move.end == end){
            MakeMove(move);
            return true;
        }
    }
    return false;
}*/