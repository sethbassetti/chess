#include <cstdint>
#include <iostream>
#include <stdio.h>
#include <bitset>
#include <sstream>
#include <string>
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


// Define the most valuable victim, least valuable attacker table
int Board::mvv_lva[12][12] = 
{
    {105, 205, 305, 405, 505, 605,  105, 205, 305, 405, 505, 605},
    {104, 204, 304, 404, 504, 604,  104, 204, 304, 404, 504, 604},
    {103, 203, 303, 403, 503, 603,  103, 203, 303, 403, 503, 603},
    {102, 202, 302, 402, 502, 602,  102, 202, 302, 402, 502, 602},
    {101, 201, 301, 401, 501, 601,  101, 201, 301, 401, 501, 601},
    {100, 200, 300, 400, 500, 600,  100, 200, 300, 400, 500, 600},

    {105, 205, 305, 405, 505, 605,  105, 205, 305, 405, 505, 605},
    {104, 204, 304, 404, 504, 604,  104, 204, 304, 404, 504, 604},
    {103, 203, 303, 403, 503, 603,  103, 203, 303, 403, 503, 603},
    {102, 202, 302, 402, 502, 602,  102, 202, 302, 402, 502, 602},
    {101, 201, 301, 401, 501, 601,  101, 201, 301, 401, 501, 601},
    {100, 200, 300, 400, 500, 600,  100, 200, 300, 400, 500, 600}
};

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

/* Initializes a board with an FEN String by calling the SetFEN function */
Board::Board(string fen_string)
{
    // Uses the fen string to init all board state variables
    SetFEN(fen_string);
}


/* Given some fen string, this function will initialize a board based on that string, set the board position and set
en passant/castling rights */
void Board::SetFEN(string fen_string){

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


/* Makes a move given a string (e5e6) and returns whether the move is valid or invalid */
bool Board::MakeMove(string move)
{
    // init values for the source square, target square, and promotion (if applicable)
    int source = 0;
    int target = 0;
    int promotion = 0;

    // iterate through every square
    for (int i = 0; i < 64; i++)
    {
        // if the square string notation is equal to first two characters of move, set source to that square
        if (square_index[i] == move.substr(0, 2)) source = i;

        // if square string notation == 3rd and 4th characters of move, set target square
        if (square_index[i] == move.substr(2, 2)) target = i;
    }

    // if move has 5 characters (indicating promotion piece)
    if (move.length() == 5){

        // iterate through all pieces
        for (int piece = P; piece <= k; piece++)
        {
            // if the promoted piece == the character in the move, set the promotion piece
            if (promoted_pieces[piece] == move.substr(4, 1)) promotion = piece;
        }
    }
    // otherwise set promotion to 0
    else promotion = 0;

    // init a list of moves
    MoveList move_list;


    // populate the move list
    GenerateMoves(&move_list);

    // iterate through the moves
    for (int count = 0; count < move_list.count; count++)
    {
        // retrieve the move
        int move = move_list.moves[count];
        
        // if the move matches the given source, target, and promotion, then make the move
        if (get_move_source(move) == source && get_move_target(move) == target && get_move_promoted(move) == promotion)
        {
            // return the value of make move (1 for legal, 0 for non-legal)
            return MakeMove(move, all_moves);
        }

    }

    // if none of the moves match, then return false
    return false;
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

    // declare a move variable to hold moves
    int move;


    
    

    /*** Quiet Pawn Moves ***/
    

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

    /*** Castle Moves ***/
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
            if (!get_bit(occupancies[both], c1) && !get_bit(occupancies[both], d1) && !get_bit(occupancies[both], b1))
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
            if (!get_bit(occupancies[both], c8) && !get_bit(occupancies[both], d8) && !get_bit(occupancies[both], b8))
            {
                // Make sure king does not move through an attacked square (and is not in check already)
                if (!IsSquareAttacked(d8, white) && !(IsSquareAttacked(e8, white))){
                    move = encode_move(e8, c8, k, 0, 0, 0, 0, 1);
                    AddMove(move_list, move);
                }
            }
        }
    }

    /*** Pawn attacks ***/
    // Define which source pawns we are going to use, depending on side
    bitboard = pieces[P + offset];

    // Iterate over the source squares
    while(bitboard)
    {
        // retrieve the source square
        source_square = BitScan(bitboard);

        // Retrieve the attacks from the current turn's side that can hit an enemy
        // Adds the en passant square to the move to indicate that it can move (if enpassant is available)
        attacks = move_calc.pawn_attacks[turn_to_move][source_square] & occupancies[enemy];

        


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

        // generate en passant captures
        if (enpassant != no_sq)
        {   
            // generate the square that the pawn can attack
            U64 enpassant_attacks = move_calc.pawn_attacks[turn_to_move][source_square] & (1ULL << enpassant);


            // if there is an en passant attack square
            if (enpassant_attacks)
            {   
                // get the target square
                target_square = BitScan(enpassant_attacks);

                // encode the move and add it to the list
                move = encode_move(source_square, target_square, (P + offset), 0, 0, 0, 1, 0);
                AddMove(move_list, move);
            }
        }

        pop_bit(bitboard, source_square);

    }

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
            move = encode_move(source_square, target_square, (N + offset), 0, (get_bit(occupancies[enemy], target_square) ? 1 : 0), 0, 0, 0);
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
            move = encode_move(source_square, target_square, (B + offset), 0, (get_bit(occupancies[enemy], target_square) ? 1 : 0), 0, 0, 0);
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
            move = encode_move(source_square, target_square, (R + offset), 0, (get_bit(occupancies[enemy], target_square) ? 1 : 0), 0, 0, 0);
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
            move = encode_move(source_square, target_square, (Q + offset), 0, (get_bit(occupancies[enemy], target_square) ? 1 : 0), 0, 0, 0);
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
            move = encode_move(source_square, target_square, (K + offset), 0, (get_bit(occupancies[enemy], target_square) ? 1 : 0), 0, 0, 0);
            AddMove(move_list, move);
            
            // pop after move is generated
            pop_bit(attacks, target_square);
        }

        // pop bit after 
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

        // preserve board state in case we have to take it back
        copy_board();

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
                    break;

                case (c1):
                    // Move A rook
                    pop_bit(pieces[R], a1);
                    set_bit(pieces[R], d1);   
                    break;

                case (g8):
                    // Move H rook
                    pop_bit(pieces[r], h8);
                    set_bit(pieces[r], f8);
                    break;
                
                case (c8):
                    // Move A Rook
                    pop_bit(pieces[r], a8);
                    set_bit(pieces[r], d8);
                    break;
            }
        }

        // update castling rights if either a rook or the king moves or a rook is captured
        castling_rights &= board_castling_rights[source_square];
        castling_rights &= board_castling_rights[target_square];

        // Re-initialize the occupancies bitboards after a move has been made
        occupancies[white] = pieces[P] | pieces[N] | pieces[R] | pieces[B] | pieces[Q] | pieces[K];
        occupancies[black] = pieces[p] | pieces[n] | pieces[r] | pieces[b] | pieces[q] | pieces[k];
        occupancies[both] = occupancies[white] | occupancies[black];

        // Toggle the current side
        turn_to_move ^= 1;

        // If the king of the last color is under attack, this is an illegal move
        if (IsSquareAttacked((turn_to_move == white) ? BitScan(pieces[k]) : BitScan(pieces[K]), turn_to_move))
        {
            // Take it back and return 0 for illegal move
            take_back();
            return 0;
        }
        else
            // return 1 for legal move
            return 1;

    }

    // capture moves
    else
    {
        // If it is a capture move, make it
        if (get_move_capture(move))
            return MakeMove(move, all_moves);

        // otherwise return 0
        else
            return 0;
    }
}

/* Performance test driver, calls the recursive perft function to generate all moves to a given depth
and record the time taken to generate those moves */
void Board::perft_driver(int depth){
    
    // initialize a clock to keep track of the time it takes to search
    clock_t time_elapsed;

    // get the starting time
    time_elapsed = clock();

    // init the list of moves
    MoveList moves;

    // init num of nodes searched
    int nodes = 0;

    // init number of nodes searched for every move
    int move_nodes;

    // generate all moves for this board state
    GenerateMoves(&moves);

    // iterate over every move
    for (int i = 0; i < moves.count; i++){

        // grab the move from the MoveList object
        int move = moves.moves[i];

        // copy the board to restore move later
        copy_board();

        // make the move. If it is an illegal move, then continue to the next move
        if(!MakeMove(move, all_moves)){
            continue;


        }
            
        // recursively call the perft function for a depth of -1
        move_nodes = perft(depth-1);

        // print out the move and it's nodes for perft divide debugging purposes
        cout << square_index[get_move_source(move)] << square_index[get_move_target(move)] <<
            promoted_pieces[get_move_promoted(move)] << ": " << move_nodes << endl;

        // increment total nodes
        nodes += move_nodes;

        // restore the board state
        take_back();

    }

    // get the ending time
    time_elapsed = clock() - time_elapsed;
    float seconds = (float) time_elapsed / CLOCKS_PER_SEC;
    float seconds_per_mil_nodes = nodes / 1000000 / seconds;

    cout << "nodes searched: " << nodes  << endl;
    cout << "total time: " << seconds <<  " seconds" << endl;
    cout << seconds_per_mil_nodes << " million nodes per second" << endl;

}

// Generates a random legal move and returns it
int Board::GetRandomMove()
{
    // init move list
    MoveList move_list;

    // populate move list with moves
    GenerateMoves(&move_list);

    // Sets the random number generator to seed with the current system time
    srand(time(0));

    // get a random index from the moves generated
    int random_index = rand() % move_list.count;

    // return a random move
    return move_list.moves[random_index];
}

// Generates the best possible move searching through a certain depth
int Board::GetBestMove(int depth)
{

    // reset the num of nodes searched, ply, and best move variables
    nodes = 0;
    ply = 0;
    best_move = 0;

    // clear data structures for search
    memset(killer_moves, 0, sizeof(killer_moves));
    memset(pv_table, 0, sizeof(pv_table));
    memset(pv_length, 0, sizeof(pv_length));

    // run the negamax function to get an evaluation and set the best move variable
    int score = NegaMax(-50000, 50000, depth);

    // if the turn to move is black, negate the score 
    score = (turn_to_move == white) ? score : score * -1;

    // return the board evaluation score
    return score;
}

int Board::Quiescence(int alpha, int beta)
{   

    // increment nodes searched
    nodes++;

    // evaluate position
    int evaluation = Evaluate();

    // fail-hard beta cautoff
    if (evaluation >= beta)
        return beta;

    // found a better move
    if (evaluation > alpha)
    {
        alpha = evaluation;
    }


    // init move list
    MoveList move_list;

    // populate move list with moves
    GenerateMoves(&move_list);

    // sort the moves to search best moves first
    SortMoves(&move_list);

    // iterate over every move
    for (int count = 0; count < move_list.count; count++)
    {
        // copy board state
        copy_board();

        // update the ply
        ply++;

        // if this move is not valid
        if (!MakeMove(move_list.moves[count], only_captures))
        {
            // decrement ply
            ply--;

            // continue to next move
            continue;
        }
            

        // recursively get score from negamax function
        int score = -Quiescence(-beta, -alpha);

        // take the move back and decrement the ply
        take_back();
        ply--;

        // fail-hard beta cautoff
        if (score >= beta)
            return beta;

        // found a better move
        if (score > alpha)
        {
            alpha = score;
        }

    }

    // node fails low
    return alpha;

}

// The negamax (modified minimax) algorithm to search for a move with alpha beta pruning
int Board::NegaMax(int alpha, int beta, int depth)
{

    // init PV length
    pv_length[ply] = ply;


    // if at the base depth (base case)
    if (depth == 0)
        // return quiescence search
        return Quiescence(alpha, beta);
    

    // increment num. of nodes searched
    nodes++;

    // determine if the king is in check or note
    bool in_check = IsSquareAttacked((turn_to_move == white) ? BitScan(pieces[K]) : BitScan(pieces[k]), turn_to_move ^ 1);

    // increase search depth if the king has been exposed into a check
    if (in_check) depth++;

    // count number of legal moves
    int legal_moves = 0;

    // init move list
    MoveList move_list;

    // populate move list with moves
    GenerateMoves(&move_list);

    // sort the moves to search in descending order
    SortMoves(&move_list);

    // iterate over every move
    for (int count = 0; count < move_list.count; count++)
    {
        // copy board state
        copy_board();

        // increment ply, meaning we are making a move
        ply++;

        // if this move is not valid
        if (!MakeMove(move_list.moves[count], all_moves))
        {

            // decrement ply
            ply--;

            // continue to next move
            continue;

        }
            

        // increment number of legal moves
        legal_moves++;
        
        // recursively get score from negamax function
        int score = -NegaMax(-beta, -alpha, depth - 1);

        // restore board state
        take_back();

        // decrement ply after taking move back
        ply--;

        // fail-hard beta cautoff
        if (score >= beta)
        {
            // on quiet moves
            if (!get_move_capture(move_list.moves[count]))
            {
                // store killer moves
                killer_moves[1][ply] = killer_moves[0][ply];
                killer_moves[0][ply] = move_list.moves[count];

            }
            
            
            // return beta value
            return beta;
        }

        // we have found a better move than previous best move
        if (score > alpha)
        {
            // update the alpha value
            alpha = score; 

            // write PV move
            pv_table[ply][ply] = move_list.moves[count];

            // loop over next ply
            for (int next_ply = ply + 1; next_ply < pv_length[ply + 1]; next_ply++)
                // copy move from deeper ply into current ply's line
                pv_table[ply][next_ply] = pv_table[ply + 1][next_ply];
            
            // adjust PV length
            pv_length[ply] = pv_length[ply + 1];
        }


       
    }

    // no legal moves to make in this position
    if (legal_moves == 0)   
    {
        // king is in check
        if (in_check)
            // return mating score ( + ply is so that it finds sooner checkmates)
            return -49000 + ply;
        else
            // return stalemate score
            return 0;
    } 
    return alpha;

}


/* Recursive perft function to traverse the tree of moves to a given depth */
int Board::perft(int depth){

    // If the depth is 0, we reached a leaf node so return 1
    if (depth == 0) 
        return 1;

    // otherwise search through all of the nodes in this tree
    else
    {
        // init the list of moves
        MoveList moves;

        // init num of nodes searched
        int nodes = 0;

        // generate all moves for this board state
        GenerateMoves(&moves);

        // iterate over every move
        for (int i = 0; i < moves.count; i++){
            

            // grab the move from the MoveList object
            int move = moves.moves[i];

            // copy the board to restore move later
            copy_board();

            // make the move. If it is an illegal move, then continue
            if(!MakeMove(move, all_moves))
                continue;

            // recursively call the perft function for a depth of -1
            nodes += perft(depth-1);

            // restore the board state
            take_back();
        }

        return nodes;

    }

    
    
}

/* Evaluates the board state using a number of factors, but mainly material advantage */
int Board::Evaluate()
{
    // Contains relative scores for each piece
    const int material_scores[12] = {100, 300, 350, 500, 1000, 10000, -100, -300, -350, -500, -1000, -10000};

    // init the evaluation score for the board
    int score = 0;

    // init bitboard variable to store piece and square variable to store squares
    U64 bitboard;
    int square;

    // Iterates over every piece
    for(int piece=P; piece <= k; piece++)
    {   
        // retrieve the piece bitboard
        bitboard = pieces[piece];
        // iterate over every bit
        while (bitboard)
        {
            // scans for the LS1B
            square = BitScan(bitboard);
            
            // adds the material score to overall score
            score += material_scores[piece];

            // score positional piece scores
            switch(piece)
            {
                // evaluate white pieces
                case P: score += pawn_scores[square]; break;
                case N: score += knight_scores[square]; break;
                case B: score += bishop_scores[square]; break;
                case R: score += rook_scores[square]; break;
                case K: score += king_scores[square]; break;

                // evaluate black pieces
                case p: score -= pawn_scores[mirror_scores[square]]; break;
                case n: score -= knight_scores[mirror_scores[square]]; break;
                case b: score -= bishop_scores[mirror_scores[square]]; break;
                case r: score -= rook_scores[mirror_scores[square]]; break;
                case k: score -= king_scores[mirror_scores[square]]; break;
            }
            // pops bit from the bitboard
            pop_bit(bitboard, square);
        }
    }


    // If white, return the score, otherwise return -score so that it is always relatively positive to current side's move
    return (turn_to_move == white) ? score : -score;
}

/* Returns a numerical score that ranks the strength of the given move. Used for earlier beta-cutoffs */
int Board::ScoreMove(int move)
{

    // score a capture move
    if (get_move_capture(move))
    {   
        // init a target piece that is being captured
        int target_piece = P;

        // to loop over bitboards and find what piece is being captured
        int start_piece, end_piece;

        // If white, then loop through white pawns to white king, otherwise black pawns to black king
        start_piece = (turn_to_move == white) ? p : P;
        end_piece = (turn_to_move == white) ? k : K;

        int target_square = get_move_target(move);

        // loop through all the piece bitboard
        for (int piece = start_piece; piece <= end_piece; piece++)
        {   
            // if there is a piece on that square
            if (get_bit(pieces[piece], target_square))
            {
                // set our target piece to that piece and break
                target_piece = piece;
                break;
            }
        }
        // score move by MVV LVA lookup [source piece][target piece]
        return mvv_lva[get_move_piece(move)][target_piece];


    }

    // score quiet move
    else
    {
        // score 1st killer move
        if (killer_moves[0][ply] == move)
            return 9000;

        // score 2nd killer move
        else if (killer_moves[1][ply] == move)
            return 8000;
    }

    return 0;
}

/* Sorts the moves in descending moves so best move is searched first */
void Board::SortMoves(MoveList *move_list)
{
    // initialize all of the move scores
    int move_scores[move_list->count];

    // iterate over all of the moves
    for (int count = 0; count < move_list->count; count++)
        // populate the move_scores array with the scores of the moves
        move_scores[count] = ScoreMove(move_list->moves[count]);
    

    // keeps track of whether bubble sort performs a swap or not (if it doesn't then array is sorted)
    bool has_swapped = true;

    // iterate until has_swapped is not true
    while(has_swapped)
    {
        // start each iteration with no swaps
        has_swapped = false;

        // iterate through all of the moves (up to but not including the last move)
        for (int i = 0; i < (move_list->count - 1); i++)
        {
            // if the next move is better than the current move
            if (move_scores[i+1] > move_scores[i])
            {
                // keep track of temp variable to swap
                int temp_move = move_list->moves[i];

                // swap the moves
                move_list->moves[i] = move_list->moves[i+1];
                move_list->moves[i + 1] = temp_move;

                //swap the scores
                int temp_score = move_scores[i];
                move_scores[i] = move_scores[i + 1];
                move_scores[i + 1] = temp_score;

                // indicate that a swap has occurred
                has_swapped = true;
            }
        }
    }


}

/* Calls the board's constructor to reset it to the original start position */
void Board::Reset(){
    Board();
}

string Board::GenerateFEN(){

    // init fen string to build the str
    string fen_string;
    
    // init str to hold str repr of pieces
    string piece_str = "";

    // Iterate over every rank from top to bottom
    for (int rank = 7; rank >= 0; rank--){

        // init a counter to count unoccupied spots on the board within a rank
        int empty_counter = 0;

        // iterate over files from a-h
        for (int file = 0; file < 8; file++)
        {

            // get the square index
            int square = rank * 8 + file;

            // iterate over every piece type
            for (int piece = P; piece <= k; piece++)
            {

                // if the bit on this square is occupied, it is of type piece
                if (get_bit(pieces[piece], square)){
                    piece_str = piece_to_str[piece];
                }
            }


            // if a piece was found 
            if(piece_str != "") 
            {

                // if empty counter is greater than 0
                if(empty_counter)
                {

                    // push back the count of empty pieces to the fen string and reset the counter
                    fen_string.append(to_string(empty_counter));
                    empty_counter = 0;
                }

                // append the piece to the fen string and reset it to a blank
                fen_string.append(piece_str);
                piece_str = "";
            }
            
            // otherwise, if no piece was found, then increment the empty counter
            else
            {
                empty_counter++;
            }
        }

        // if empty counter has  a value at the end of the rank, append the value to the string
        if(empty_counter)
            fen_string.append(to_string(empty_counter));
        
        // if the rank isn't the last rank, then add the slash (rank divider)
        if(rank != 0)
            fen_string.append("/");
        

    }
    return fen_string;
} 

/* Displays the board in ASCII format */
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
