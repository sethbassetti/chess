#include <string.h>

#include "utils.h"
#include "move_calc.h"


/* Constructor for MoveCalculator class. Initializes all pre-calculated attack tables */
MoveCalc::MoveCalc()
{   
    // Initialize the slider moves for bishops and rooks
    InitSliderMoves(bishop);
    InitSliderMoves(rook);
    
    // Initializes leaper move attack tables for kings and knights
    InitLeaperMoves();
}

/* Given a color and a square on the board, returns a bitboard representing where a pawn on that square
could attack*/
U64 MoveCalc::CalcPawnAttacks(int square, int side){

    // Stores the attacks
    U64 attacks = 0ULL;

    // Stores the piece on the square
    U64 bitboard = 0ULL;

    // set piece on board
    set_bit(bitboard, square);

    // white pawns
    if(!side)
    {
        // Only place an attack square up and to the left if not on outer "a" file
        if(bitboard & not_a_file){
            attacks |= (bitboard << 7);
        }

        // Only place an attack square up and to the right if not on outer "h" file
        if(bitboard & not_h_file){
            attacks |= (bitboard << 9);
        }
    }

    // Black pawns
    else
    {
        if(bitboard & not_a_file){
            attacks |= (bitboard >> 9);
        }
        if(bitboard & not_h_file){
            attacks |= (bitboard >> 7);
        }
    }

    return attacks;
}

/* Returns a bitboard of locations a king could attack if it were on a given square */
U64 MoveCalc::CalcKingAttacks(int square){
    // Stores the attacks
    U64 attacks = 0ULL;

    // Stores the piece on the square
    U64 bitboard = 0ULL;

    // set piece on board
    set_bit(bitboard, square);

    // If king not on outer a file, puts attacking square one to the left
    attacks |= (bitboard >> 1) & not_h_file;
    // If king not on outer h file, puts attacking square one to the right
    attacks |= (bitboard << 1) & not_a_file;
    

    /* At this point there is 1, 2, or 3 attacks in a horizontal line (depending on outer files)
    so shift it up one and down one to obtain all directions for king */
    bitboard |= attacks;
    // Shift up one
    attacks |= (bitboard << 8);

    // Shift down one
    attacks |= (bitboard >> 8);

    return attacks;
}

U64 MoveCalc::CalcKnightAttacks(int square)
{
    // Stores the attacks
    U64 attacks = 0ULL;

    // Stores the piece on the square
    U64 bitboard = 0ULL;

    // set piece on board
    set_bit(bitboard, square);

    // Calculates all possible knight attacks, intersects with files to avoid wrapping
    attacks |= (bitboard << 17) & not_a_file;   // NNE attacks
    attacks |= (bitboard << 10) & not_ab_file;  // NEE attacks
    attacks |= (bitboard >> 6) & not_ab_file;   // SEE attacks
    attacks |= (bitboard >> 15) & not_a_file;   // SSE attacks
    attacks |= (bitboard >> 17) & not_h_file;   // SSW attacks
    attacks |= (bitboard >> 10) & not_gh_file;  // SWW attacks
    attacks |= (bitboard << 6) & not_gh_file;   // NWW attacks
    attacks |= (bitboard << 15) & not_h_file;   // NNW attacks

    return attacks;
}

/* Mask bishop attacks for magic bitboard */
U64 MoveCalc::MaskBishopAttacks(int square)
{
    // result attacks bitboard
    U64 attacks = 0ULL;

    // init rank and file
    int r, f;

    // init target rank & files
    int tgt_r = square / 8;
    int tgt_f = square % 8;

    // mask relevant bishop occupancy bits
    for (r = tgt_r - 1, f = tgt_f + 1; r > 0 && f < 7; r--, f++) attacks |= 1ULL << (r * 8 + f);    // SE Ray
    for (r = tgt_r - 1, f = tgt_f - 1; r > 0 && f > 0; r--, f--) attacks |= 1ULL << (r * 8 + f);    // SW Ray
    for (r = tgt_r + 1, f = tgt_f - 1; r < 7 && f > 0; r++, f--) attacks |= 1ULL << (r * 8 + f);    // NW Ray
    for (r = tgt_r + 1, f = tgt_f + 1; r < 7 && f < 7; r++, f++) attacks |= 1ULL << (r * 8 + f);    // NE Ray

    // return attack map
    return attacks;
}

/* Mask rook attacks for magic bitboard */
U64 MoveCalc::MaskRookAttacks(int square)
{
    // Result attacks bitboard
    U64 attacks = 0ULL;

    // init rank and file
    int r, f;

    // init target rank and file
    int tgt_r = square / 8;
    int tgt_f = square % 8;

    // mask relevant rook occupancy bits
    for (r=tgt_r + 1; r < 7; r++) attacks |= 1ULL << (r * 8 + tgt_f);   // S attacks
    for (r=tgt_r - 1; r > 0; r--) attacks |= 1ULL << (r * 8 + tgt_f);   // N attacks
    for (f=tgt_f - 1; f > 0; f--) attacks |= 1ULL << (tgt_r * 8 + f);   // W attacks
    for (f=tgt_f + 1; f < 7; f++) attacks |= 1ULL << (tgt_r * 8 + f);   // E attacks

    // return attack map
    return attacks;
}


/* Generate bishop attacks on the fly */
U64 MoveCalc::BishopAttacksOnTheFly(int square, U64 block)
{
    // result attacks bitboard
    U64 attacks = 0ULL;

    // init rank and file
    int r, f;

    // init target rank & files
    int tgt_r = square / 8;
    int tgt_f = square % 8;

    // generate potential bishop attacks

    // SE Ray
    for (r = tgt_r - 1, f = tgt_f + 1; r >= 0 && f <= 7; r--, f++)
    {
        attacks |= 1ULL << (r * 8 + f);
        if ((1ULL << (r * 8 + f))  & block) break;
    } 

    // SW Ray
    for (r = tgt_r - 1, f = tgt_f - 1; r >= 0 && f >= 0; r--, f--)
    {
        attacks |= 1ULL << (r * 8 + f);
        if ((1ULL << (r * 8 + f))  & block) break;
    }

    // NW Ray
    for (r = tgt_r + 1, f = tgt_f - 1; r <= 7 && f >= 0; r++, f--)
    {
        attacks |= 1ULL << (r * 8 + f);
        if ((1ULL << (r * 8 + f))  & block) break;
    }

    // NE Ray
    for (r = tgt_r + 1, f = tgt_f + 1; r <= 7 && f <= 7; r++, f++)
    {
        attacks |= 1ULL << (r * 8 + f);    // NE Ray
        if ((1ULL << (r * 8 + f))  & block) break;
    }

    // return attack map
    return attacks;
}

/* Generate rook attacks on the fly */
U64 MoveCalc::RookAttacksOnTheFly(int square, U64 block)
{
    // Result attacks bitboard
    U64 attacks = 0ULL;

    // init rank and file
    int r, f;

    // init target rank and file
    int tgt_r = square / 8;
    int tgt_f = square % 8;

    // mask relevant rook occupancy bits

    // N attacks
    for (r=tgt_r + 1; r <= 7; r++)
    {
        attacks |= 1ULL << (r * 8 + tgt_f);
        if((1ULL << (r * 8 + tgt_f)) & block) break;
    }

    // S attacks
    for (r=tgt_r - 1; r >= 0; r--)
    {
        attacks |= 1ULL << (r * 8 + tgt_f);
        if((1ULL << (r * 8 + tgt_f)) & block) break;
    }

    // W Attacks
    for (f=tgt_f - 1; f >= 0; f--)
    {
        attacks |= 1ULL << (tgt_r * 8 + f);
        if((1ULL << (tgt_r * 8 + f)) & block) break;
    }

    // E Attacks
    for (f=tgt_f + 1; f <= 7; f++)
    {
        attacks |= 1ULL << (tgt_r * 8 + f);   // S attacks
        if((1ULL << (tgt_r * 8 + f)) & block) break;
    }

    // return attack map
    return attacks;
}

U64 MoveCalc::SetOccupancy(int index, int bits_in_mask, U64 attack_map)
{
    // occupancy map
    U64 occupancy_map = 0ULL;

    // loop over the range of bits within attack mask
    for (int count = 0; count < bits_in_mask; count++)
    {
        // Get LSB index of attack map
        int square = BitScan(attack_map);

        // Pop the least significant bit from attack map
        pop_bit(attack_map, square);

        // Make sure occupancy is on board
        if (index & (1 << count))
        {
            occupancy_map |= (1ULL << square);
        }
    }

    return occupancy_map;
}

/* Iterates through every square and index to construct all possible slider moves */
void MoveCalc::InitSliderMoves(int bishop)
{   
    // Constructs attacks for each square
    for(int square=0; square < 64; square++){

        // init mask tables
        rook_masks[square] = MaskRookAttacks(square);
        bishop_masks[square] = MaskBishopAttacks(square);

        // Retrieve the appropriate sliding piece attack map
        U64 attack_mask = bishop ? bishop_masks[square] : rook_masks[square];

        // Init relevant occupancy bit count and occupancy indices
        int relevant_bits = count_bits(attack_mask);
        int occupancy_indices = 1 << relevant_bits;

        // Populate the rook attack table for each possible combination of blocking pieces
        for(int index=0; index < occupancy_indices; index++)
        {
            // Construct a potential map of blockers
            U64 occupancy = SetOccupancy(index, relevant_bits, attack_mask);
            if(bishop){

                // Generate the magic index and use it to populate the bishop attack table
                int magic_index = (occupancy * bishop_magic_numbers[square]) >> (64 - bishop_relevant_bits[square]);
                bishop_attacks[square][magic_index] = BishopAttacksOnTheFly(square, occupancy);
            }
            else
            {   
                // Generate the magic index and use it to populate the rook attack table
                int magic_index = (occupancy * rook_magic_numbers[square]) >> (64 - rook_relevant_bits[square]);
                rook_attacks[square][magic_index] = RookAttacksOnTheFly(square, occupancy);
            }
        }
    }
}

void MoveCalc::InitLeaperMoves()
{
    // initialize attack tables for knights and kings for every square
    for (int square = 0; square < 64; square++)
    {
        king_attacks[square] = CalcKingAttacks(square);
        knight_attacks[square] = CalcKnightAttacks(square);

        // Iterate over both white and black colors
        for (int color: {white, black})
        {
            // Calculate pawn attacks for that square and color and store it in bitboards
            pawn_attacks[color][square] = CalcPawnAttacks(square, color);
        }
    }
}

/* Uses magic bitboard technique to get bishop attacks */
U64 MoveCalc::GetBishopAttacks(int square, U64 occupancy)
{ 
    // Generates relevant blockers
    occupancy &= bishop_masks[square];

    // Obtains the index
    occupancy *= bishop_magic_numbers[square];

    // Gets only relevant bits for the index
    occupancy >>= 64 - bishop_relevant_bits[square];

    return bishop_attacks[square][occupancy];

}

/* Uses magic bitboard technique to get rook attacks */
U64 MoveCalc::GetRookAttacks(int square, U64 occupancy)
{ 
    // Generates relevant blockers
    occupancy &= rook_masks[square];

    // Obtains the index
    occupancy *= rook_magic_numbers[square];

    // Gets only relevant bits for the index
    occupancy >>= 64 - rook_relevant_bits[square];

    return rook_attacks[square][occupancy];

}

/* Uses the union of rook and bishop attacks to construct a queen attack */
U64 MoveCalc::GetQueenAttacks(int square, U64 occupancy)
{
    return GetRookAttacks(square, occupancy) | GetBishopAttacks(square, occupancy);
}
