#include <string.h>

#include "utils.h"
#include "move_calc.h"


/* Constructor for MoveCalculator class. Initializes all pre-calculated attack tables */
MoveCalc::MoveCalc()
{
    InitSliderMoves();
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


U64 MoveCalc::FindMagicNumber(int square, int relevant_bits, int bishop)
{
    // init occupancies (every different combination of occupied pieces, up to 4096 for a rook, 512 for a bishop)
    U64 occupancies[4096];

    // init attack tables (all possible attacks given those occupancies)
    U64 attacks[4096];

    // init used attacks (attacks that have already been found)
    U64 used_attacks[4096];

    // init attack_mask for a current piece
    U64 attack_mask = bishop ? MaskBishopAttacks(square) : MaskRookAttacks(square);

    // occupancy indices (relevant bits tell us how many combinations there are, the max number this could be is 4096)
    int occupancy_indices = 1 << relevant_bits;

    // loop over occupancy indices
    for (int index = 0; index < occupancy_indices; index++)
    {
        // init occupancies
        occupancies[index] = SetOccupancy(index, relevant_bits, attack_mask);

        // init attacks (true attacks for a bishop/ rook given some occupancy index)
        attacks[index] = bishop ? BishopAttacksOnTheFly(square, occupancies[index]) : RookAttacksOnTheFly(square, occupancies[index]);

    }

    // test magic numbers loop
    for (int random_count=0; random_count < 100000000; random_count++)
    {
        // generate magic number candidate
        U64 magic_number_candidate = GenerateMagicCandidate();

        // skip inappropriate magic numbers
        if (count_bits((attack_mask * magic_number_candidate) & 0xFF00000000000000) < 6) continue;

        // init used attacks array
        memset(used_attacks, 0ULL, sizeof(used_attacks));

        // init index & fail flag
        int index, fail;

        // test magic numbers loop
        for (index=0, fail=0; !fail && index < occupancy_indices; index++)
        {
            // init magic index
            int magic_index = (int) ((occupancies[index] * magic_number_candidate) >> (64 - relevant_bits));

            // If magic index works, initialize used attacks
            if (used_attacks[magic_index] == 0ULL){
                used_attacks[magic_index] = attacks[index];
            }
            else if(used_attacks[magic_index] != attacks[index]){
                // magic index doesn't work
                fail=1;
            }
        }

        // If magic number works
        if (!fail){
            // return it
            return magic_number_candidate;
        }
    }
    printf("Magic number fails");
    return 0ULL;
}

void MoveCalc::InitMagicNumbers()
{
    for (int rank=0; rank < 8; rank++){

        for (int file=0; file<8; file++)
        {
            int square = rank * 8 + file;
            int relevant_bits = bishop_relevant_bits[square];
            U64 magic_number = FindMagicNumber(square, relevant_bits, bishop);
            printf("%10d, ",(int) magic_number);
        }
        printf("\n");
    }
}

void MoveCalc::InitSliderMasks()
{
    for (int square=0; square < 64; square++)
    {
        rook_attack_masks[square] = MaskRookAttacks(square);
        bishop_attack_masks[square] = MaskBishopAttacks(square);
    }
}
/* Iterates through every square and index to construct all possible slider moves */
void MoveCalc::InitSliderMoves()
{   

    // Populate rook and bishop attack mask tables
    InitSliderMasks();


    // Constructs attacks for each square
    for(int square=0; square < 64; square++){

        // Retrieve the appropriate sliding piece attack map

        U64 rook_attack_mask = rook_attack_masks[square];
        U64 bishop_attack_mask = bishop_attack_masks[square];

        // How many bits could represent blocking pieces (up to 12)
        int rook_occupancy_bits = rook_relevant_bits[square];
        int bishop_occupancy_bits = bishop_relevant_bits[square];

        // Populate the rook attack table for each possible combination of blocking pieces
        for(int index=0; index < 4096; index++)
        {
            // Construct a potential map of blockers
            U64 occupancy = SetOccupancy(index, rook_occupancy_bits, rook_attack_mask);

            // Generate the rook attacks
            U64 attack = RookAttacksOnTheFly(square, occupancy);

            // Store it in the attack table
            rook_attacks[square][index] = attack;
        }

        // Populate the bishop attack table for each possible combination of blocking pieces
        for(int index=0; index < 512; index++)
        {
            // Construct a potential map of blockers
            U64 occupancy = SetOccupancy(index, bishop_occupancy_bits, bishop_attack_mask);

            // Generate the bishop attacks
            U64 attack = BishopAttacksOnTheFly(square, occupancy);

            // Store it in the attack table
            bishop_attacks[square][index] = attack;
        }
    }
}