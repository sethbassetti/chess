
#pragma once


/* This class is in charge of anything relating to move calculation (not generation). It initializes
all precalculated attack tables and has functionality for on the fly move calculation. This class
should be used by the board class when performing move generation given the board state */
class MoveCalc
{
public:
    // Class constructor
    MoveCalc();

    // Precalculated pawn attack tables [side][square]
    U64 pawn_attacks[2][64];

    void InitMagicNumbers();

    U64 FindMagicNumber(int square, int relevant_bits, int bishop);

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
         276840450,    4202497,  270532864,  537919496,  268960256,  138544128,   33554688,      16512, 
        1074274450,  536891392,  536875008,  554045440,    8520704,     262272,    8389120, 1082196002, 
           4194344, 1082261537, 1090527232,  134221824,   67110912,   33555456, 1091572232,    8405284, 
        1073750016, 1073762304,    1066240,    2294024,     532484, 1077936640,    2623234,     262273, 
          75497520,   20971584,   41947136,  134742144,   67371040, 1224737792, 1140851080, 1107296421, 
         545292288,  268451904,   67182592,  134250624, 1074268160,   67174408, 1090781240, 1140981761, 
           8454656,    4194432,    1065216, 1074270720,   67633408, 1342464512,   34604032, 1141047808, 
         285223042,  273285250, 1075838993,     590085,    1049093,   33816705, 1082658836,   67141698,
    };

    const U64 bishop_magic_numbers[64] = {
         603988488,    6309888, 1644429345,      32768,   34078739,  539443338,  269574176,   34079752, 
         553716232,  136385792,  276965380, 1358991424, 1075848224, 1612714112,  805569792,   18092051, 
          33851392,   21039361,   69353730,  287440898,   77725696,    8520128,  134352896,  545530884, 
         169887808,   36481028, 1107427840,  136317056,  301998337,  419991553,   67504134,  109086739, 
           6292096,    8913617,    2621472,    1573393, 1078198528,  570491904,  142676224,  167805952, 
        1352155264,  135602180, 1207992576,  272630273,  201326722,   17401344,    8913408,   17039409, 
        1208484352,   69248080,   17047808,  138946576,  545395840,   21037056,   33575424,  144048288, 
         404759072, 1275338752, 1107333123,  337684480,   67437570, 1125648897,  134742288,    4210944,
    };

    // Contains possible rook and bishop attacks (excluding blockers) for all squares
    U64 rook_attack_masks[64];
    U64 bishop_attack_masks[64];

    // Contains pre-initialized tables for all possible rook/bishop attacks on given squares
    U64 rook_attacks[64][4096];
    U64 bishop_attacks[64][512];

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
    void InitSliderMoves();

    // Calculates sliding piece masks
    void InitSliderMasks();
};
