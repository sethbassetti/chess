
#pragma once


/* This class is in charge of anything relating to move calculation (not generation). It initializes
all precalculated attack tables and has functionality for on the fly move calculation. This class
should be used by the board class when performing move generation given the board state */
class MoveCalc
{
public:
    // Class constructor
    MoveCalc();

    // Given a square and an occupancy bitboard, retrieves the slider attack map for that piece
    U64 GetBishopAttacks(int square, U64 occupancy);
    U64 GetRookAttacks(int square, U64 occupancy);
        
    // Initializes a list of magic numbers
    void InitMagicNumbers(int bishop);



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

    /* Magic numbers for rooks and bishops, these numbers are used to obtain an index for pre-calculated
    sliding piece attacks */
    const U64 rook_magic_numbers[64] = {
         0x2080002010804002ULL,
        0x30c0011000402001ULL,
        0x4700084010200100ULL,
        0x500050020100008ULL,
        0x2200042010080200ULL,
        0x2500010008420400ULL,
        0x880008002000100ULL,
        0x2080002100004080ULL,
        0x4b20800040082092ULL,
        0x400020005000ULL,
        0x6000808020001000ULL,
        0x801000821061000ULL,
        0x1404800800820400ULL,
        0x4800200040080ULL,
        0x800100800200ULL,
        0x2041000040810022ULL,
        0xe00808000400028ULL,
        0x1020040820021ULL,
        0x110041002000ULL,
        0x808008001000ULL,
        0x4010808004000800ULL,
        0x2000808002000400ULL,
        0x30940041101208ULL,
        0x4028c20000804124ULL,
        0x180024140002000ULL,
        0x4004200440005000ULL,
        0x808200100104500ULL,
        0x4085100100230108ULL,
        0x480200d200082004ULL,
        0x4010040400200ULL,
        0x198100c00280702ULL,
        0x300006200040081ULL,
        0x408804004800030ULL,
        0x201001400040ULL,
        0xc40802002801000ULL,
        0xa10010008080080ULL,
        0x8004004040020ULL,
        0x421000249000400ULL,
        0x120021044000188ULL,
        0x410420000a5ULL,
        0x40400020808000ULL,
        0x440002010004040ULL,
        0x441001a804012000ULL,
        0x20100008008080ULL,
        0x4008040080800ULL,
        0x1000204010008ULL,
        0x21041040038ULL,
        0x8044020001ULL,
        0x216422a00810200ULL,
        0x40802000400080ULL,
        0x1082000104100ULL,
        0x2002040081200ULL,
        0x1001204080100ULL,
        0x22002850046200ULL,
        0x2803080102100400ULL,
        0x9644030200ULL,
        0x2040420011002882ULL,
        0x22100104a0082ULL,
        0x7501000840200011ULL,
        0x32201000090105ULL,
        0x801004800100205ULL,
        0x2a45000802040081ULL,
        0x168b0240881014ULL,
        0x4002104008042ULL,
    };

    const U64 bishop_magic_numbers[64] = {
            0x1140011802009025ULL,
            0x3148080800604800ULL,
            0x48020062040021ULL,
            0x88084100008000ULL,
            0x1104002080013ULL,
            0x131844022027408aULL,
            0x1004210110116020ULL,
            0x21082802080408ULL,
            0x4912021010a08ULL,
            0x4040108211500ULL,
            0x1100104c10822804ULL,
            0xe00080851009040ULL,
            0x200111040202420ULL,
            0x8008260201080ULL,
            0x2022010c30040500ULL,
            0x8e812c01141013ULL,
            0x408102002048800ULL,
            0x20901001410901ULL,
            0x150000204224102ULL,
            0x29200e411220002ULL,
            0x22000404a20000ULL,
            0x230022008201c0ULL,
            0x400108021000ULL,
            0x482060020842404ULL,
            0x7e10000a204840ULL,
            0x5a23000022ca804ULL,
            0x1408010042020200ULL,
            0x21a4010208200880ULL,
            0x81040012002101ULL,
            0x4109060019089001ULL,
            0x84809104060806ULL,
            0x490206808813ULL,
            0x2084100600280ULL,
            0x2240120008802d1ULL,
            0x40280400280020ULL,
            0x40400180211ULL,
            0x200404040440100ULL,
            0x2020008022010400ULL,
            0x8408408811100ULL,
            0x144410a008400ULL,
            0x4084024250984080ULL,
            0x208e208152004ULL,
            0x286010048008100ULL,
            0x1100024010400201ULL,
            0x48200e0c000082ULL,
            0x6844011801098600ULL,
            0x2108200880200ULL,
            0x2814210401040031ULL,
            0x6480848080200ULL,
            0x203100490420a450ULL,
            0x4021110c01042100ULL,
            0x80120108482810ULL,
            0x88020420821480ULL,
            0x200102001410000ULL,
            0x2040050802005200ULL,
            0x828842089600a0ULL,
            0x300610118202220ULL,
            0x100200524c042000ULL,
            0x2426000842009003ULL,
            0x8903161420a800ULL,
            0x2003102004050402ULL,
            0x800843180a01ULL,
            0x4020084308080110ULL,
            0x1840040400404100ULL
    };

    // Contains possible rook and bishop attacks (excluding blockers) for all squares
    U64 rook_masks[64];
    U64 bishop_masks[64];

    // Contains pre-initialized tables for all possible rook/bishop attacks on given squares
    U64 rook_attacks    [64][4096];
    U64 bishop_attacks  [64][512];

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

    // Constructs a potential set of blockers for a given index and attack mask
    U64 SetOccupancy(int index, int bits_in_mask, U64 attack_mask);

    // Pre-calculates rook and bishop attack tables
    void InitSliderMoves(int bishop);



    U64 FindMagicNumber(int square, int relevant_bits, int bishop);
};
