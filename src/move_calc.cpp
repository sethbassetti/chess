#include "utils.h"
#include "move_calc.h"



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