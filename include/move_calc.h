
#pragma once


/* This class is in charge of anything relating to move calculation (not generation). It initializes
all precalculated attack tables and has functionality for on the fly move calculation. This class
should be used by the board class when performing move generation given the board state */
class MoveCalc
{
public:

    // Precalculated pawn attack tables [side][square]
    U64 pawn_attacks[2][64];


    /* Below functions used for magic bitboard sliding piece move calculation */

    // Given a square, returns an attack map of where the piece could attack (not including edge of board)
    U64 MaskBishopAttacks(int square);
    U64 MaskRookAttacks(int square);

    /* Given a square and a bitboard of blockers returns an attack map of where the piece could attack, blocked by
    blocker pieces*/
    U64 BishopAttacksOnTheFly(int square, U64 block);
    U64 RookAttacksOnTheFly(int square, U64 block);

    // 
    U64 SetOccupancy(int index, int bits_in_mask, U64 attack_map);

private:

    /* These are the maximum number of attack bits (edge exclusive) that we need to consider for a bishop
    or rook placed on each square */
    const int bishop_relevant_bits[64] = {
        6, 5, 5, 5, 5, 5, 5, 6, 
        5, 5, 5, 5, 5, 5, 5, 5, 
        5, 5, 7, 7, 7, 7, 5, 5, 
        5, 5, 7, 9, 9, 7, 5, 5, 
        5, 5, 7, 9, 9, 7, 5, 5, 
        5, 5, 7, 7, 7, 7, 5, 5, 
        5, 5, 5, 5, 5, 5, 5, 5, 
        6, 5, 5, 5, 5, 5, 5, 6
    };
    const int rook_relevant_bits[64] = {
        12, 11, 11, 11, 11, 11, 11, 12, 
        11, 10, 10, 10, 10, 10, 10, 11, 
        11, 10, 10, 10, 10, 10, 10, 11, 
        11, 10, 10, 10, 10, 10, 10, 11, 
        11, 10, 10, 10, 10, 10, 10, 11, 
        11, 10, 10, 10, 10, 10, 10, 11, 
        11, 10, 10, 10, 10, 10, 10, 11, 
        12, 11, 11, 11, 11, 11, 11, 12
    };
};
