#include <cstdint>
#include <iostream>
#include <stdio.h>
#include <bitset>
#include <strings.h>
#include <string.h>
#include <assert.h>  
#include <vector>
#include "utils.h"
#include "board.h"
#include "position.h"


using namespace std;



// Constructor for board, initializes board position and preset attack tables
Board::Board(){
    
    // Constructs a position object that holds information about piece positions
    position = Position();

    // Initializes leaper attack tables, since they are independent of board position
    InitLeaperAttacks();
}




/* Test function for implementing and development */
void Board::Test(){
    U64 test_board = 0ULL;

    for (int rank = 0; rank < 8; rank++)
    {
        for (int file = 0; file < 8; file++)
        {
            int square = rank * 8 + file;
            if(rank == file && rank <7){
                set_bit(test_board, square);
            }
        }
    }


    vector<Move> moves = GenerateMoveList(black);
    printf("Num Moves: %ld\n", moves.size());
    for(Move move : moves){
        printf("%d\n", move.capture);
    }
    MakeMove(moves.at(17));
    Move move = {d2, d4, quiet, blank, white};
    MakeMove(move);
    InitializeAttackSets();
    Move capture_move = {c6, d4, capture, white_pawn, black};
    MakeMove(capture_move);
    InitializeAttackSets();
    PrintBoard(position.white_pawns);
}

/* Given a color and a square on the board, returns a bitboard representing where a pawn on that square
could attack*/
U64 Board::CalcPawnAttacks(int side, int square){

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
U64 Board::CalcKingAttacks(int square){
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

U64 Board::CalcKnightAttacks(int square){
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

/* Retrieves attack tables for leaper pieces (pawn, king, knight) */
void Board::InitLeaperAttacks(){
    /* Constructs leaper attack tables for every square, so iterates through rank and file */
    for (int rank = 0; rank < 8; rank++){
        for (int file = 0; file < 8; file++){

            // Gets square index
            int square = rank * 8 + file;

            // Pawn attacks are dependent on their color, all other attacks are color agnostic
            pawn_attacks[white][square] = CalcPawnAttacks(white, square);
            pawn_attacks[black][square] = CalcPawnAttacks(black, square);
            king_attacks[square] = CalcKingAttacks(square);
            knight_attacks[square] = CalcKnightAttacks(square);
        }
    }
}

/* Calculates sliding attack sets for rooks given square parameter */
U64 Board::CalcRookAttacks(int square){
    // Stores the attacks
    U64 attacks = 0ULL;

    // Stores the piece on the square
    U64 bitboard = 0ULL;

    // set piece on board
    set_bit(bitboard, square);

    // Creates a vector of the rays to construct the attacks from
    enumDirections dirs[4] = {Nort, East, West, Sout};

    // Iterates over every direction to construct attack table
    for (int i = 0; i < 4; i++){
        
        // Assigns the current direction to dir
        enumDirections dir = dirs[i];
        
        // Gets the appropriate ray attack for the direction and square
        U64 ray_attack = GetDirRayAttacks(dir, square);

        // Adds the ray attack to the overall attacks table with a union
        attacks |= ray_attack;
    }

    return attacks;
}

U64 Board::CalcBishopAttacks(int square){
    // Stores the attacks
    U64 attacks = 0ULL;

    // Stores the piece on the square
    U64 bitboard = 0ULL;

    // set piece on board
    set_bit(bitboard, square);

    // Creates a vector of the rays to construct the attacks from
    enumDirections dirs[4] = {NoWe, SoWe, SoEa, NoEa};

    // Iterates over every direction to construct attack table
    for (int i = 0; i < 4; i++){
        
        // Assigns the current direction to dir
        enumDirections dir = dirs[i];
        
        // Gets the appropriate ray attack for the direction and square
        U64 ray_attack = GetDirRayAttacks(dir, square);

        // Adds the ray attack to the overall attacks table with a union
        attacks |= ray_attack;
    }

    return attacks;
}

/* Unions the rook and bishop attacks to form queen attack table. NOTE: This function HAS to be called after
the rook and bishop attack tables are built or it will construct the empty set */
U64 Board::CalcQueenAttacks(int square){
    return rook_attacks[square] | bishop_attacks[square];
}
/* Returns the ray attack in a given direction (N, NW, S, SE, etc...) from the given square.
This function considers blocker pieces. Thus, the ray attacks only go as far as line of sight from the
square allows */
U64 Board::GetDirRayAttacks(enumDirections dir, int square){

    // Retrieves ray attacks from lookup table
    U64 ray_attack = ray_attacks[square][dir];

    // Calculates every piece that ray can hit with intersection of ray attack and occupancy    
    U64 blocker = ray_attack & occupancy;

    // If there is a blocker, make sure ray attack cannot go through that square
    if(blocker){
        // Gets the index of the first piece the ray attack would hit
        int index = BitScan(blocker, is_negative(dir));

        // Does an XOR to calculate the ray attack UP TO the first piece it hits and no further. 
        // This works by looking up what the ray would be at the first blocker and then simply XORing
        // to get the original ray minus the ray from the blocker square
        ray_attack ^= ray_attacks[index][dir];
    }

    return ray_attack;
}

/* Initialize Ray Attack table. Constructs north, south, east, and west ray attacks (occupancy agnostic)
for every square on the board */
void Board::InitRayAttacks(){
    // North ray is a line of ones extending northwards from (but not including) a1
    U64 north = 0x0101010101010100ULL;
  
    //  Left shifts 1 bit each time for every square, constructing the north ray attack table
    for (int sq = 0; sq < 64; sq++, north <<=1){
        ray_attacks[sq][Nort] = north;
    }

    // A line of ones extending southwards from (but not including) h8
    U64 south = 0x0080808080808080ULL;

    // Right shifts one bit each time for every square to construct the south attack table
    for (int sq = 63; sq >= 0; sq--, south >>= 1)
    {
        ray_attacks[sq][Sout] = south;
    }

    // A line of ones extending eastwards from, but not including, a1
    U64 east = 0xfeULL;

    // Iterates through rank and file respectively. For every rank, copies the east bitboard to new_east
    // and left shifts new_east for every file. To avoid file wrapping, new_east is intersected with the original
    // east bitboard so only ones on the original rank remain. Every rank, the original east bitboard is
    // shifted up a rank (leftshift of 8)
    for (int rank = 0; rank < 8; rank++, east <<= 8)
    {
        U64 new_east = east;
        for (int file = 0; file < 8; file += 1, new_east <<= 1)
        {
            ray_attacks[rank * 8 + file][East] = east & new_east;
        }
    }

    // A line of ones extending westwards from, but not including, h8
    U64 west = 0x7f00000000000000ULL;

    // Performs the same process as calculating the east ray attacks, but in reverse. Starts in top rank
    // and last file, and works its way downwards from h8 to a1.
    for (int rank = 7; rank >= 0; rank--, west >>= 8)
    {
        U64 new_west = west;
        for (int file = 7; file >= 0; file --, new_west >>= 1)
        {
            ray_attacks[rank * 8 + file][West] = west & new_west;
        }
    }

    // A line of ones extending northeastwards from a1
    U64 noea = 0x8040201008040200ULL;
    
    // Iterates through every rank and file. Starts with a mask in the a_file, that is unioned
    // with itself shifted one for every file. The negation of this is intersected with the original ray
    // attack to prevent wrapping around.
    for (int rank = 0; rank < 8; rank++){
        U64 wrap_mask = not_a_file;
        for (int file = 0; file < 8; file++, noea <<= 1, wrap_mask&= wrap_mask << 1)
        {
            ray_attacks[rank * 8 + file][NoEa] = noea & wrap_mask;
        }
    }

    U64 nowe = 0x102040810204000ULL;
    U64 rank_nowe = nowe;

    for (int rank = 0; rank < 8; rank++)
    {
        rank_nowe = nowe << (8 * rank);

        U64 wrap_mask = not_h_file;
        for (int file = 7; file >= 0; file--, rank_nowe >>= 1, wrap_mask &= wrap_mask >> 1)
        {
            int square = rank * 8 + file;
            //printf("%d\n", square);
            ray_attacks[square][NoWe] = rank_nowe & wrap_mask;
        }
    }

    U64 soea = 0x2040810204080ULL;
    U64 rank_soea;
    for (int rank = 7; rank >= 0; rank--)
    {
        U64 wrap_mask = not_a_file;
        rank_soea = soea >> ((7 - rank) * 8);
        for (int file = 0; file < 8; file++, rank_soea <<= 1, wrap_mask &= wrap_mask << 1)
        {
            int square = rank * 8 + file;
            ray_attacks[square][SoEa] = rank_soea & wrap_mask;
        }
    }

    U64 sowe = 0x40201008040201ULL;
    for (int rank = 7; rank >= 0; rank--){
        U64 wrap_mask = not_h_file;
        for (int file = 7; file >= 0; file--, sowe >>= 1, wrap_mask &= wrap_mask >> 1)
        {
            int square = rank*8 + file;
            ray_attacks[square][SoWe] = sowe & wrap_mask;
        }
    }
}

/* Retrieves attack tables for slider pieces (bishop, rook, queen) */
void Board::InitSliderAttacks(){

    // Initializes precalculated ray attack lookup tables
    InitRayAttacks();

    // Constructs rook attack table for every square on board
    for (int rank = 0; rank < 8; rank++)
    {
        for (int file = 0; file < 8; file++){
            int square = rank * 8 + file;
            rook_attacks[square] = CalcRookAttacks(square);
            bishop_attacks[square] = CalcBishopAttacks(square);
            queen_attacks[square] = CalcQueenAttacks(square);
            CalcPawnPushes();
        }
    }
}

/* Given a color and reference to a move list, populates the move list
with all pawn moves of the given color */
void Board::GeneratePawnMoves(int color, vector<Move> &moves){

    /* generic variables for bitboards that are set to either white or black respectively */
    U64 pawns;
    U64 enemy_pieces;
    int color_flag;

    // If white, pawns are white pawns, enemy is black pieces and color flag is 1 (for pushes later)
    if(!color){
        pawns = white_pawns;
        enemy_pieces =  black_pieces;
        color_flag = 1;
    }
    // Otherwise, reverse the above
    else
    {
        pawns = black_pawns;
        enemy_pieces = white_pieces;
        color_flag = -1;
    }
    
    // Serialize the pawns into a vector of their integer indices
    vector<int> pawn_indices = SerializeBitboard(pawns);

    // Get pawn attacks
    for(int pawn : pawn_indices){

        // Iterates through each posible attack the pawn could make 
        vector<int> attack_indices = SerializeBitboard(pawn_attacks[color][pawn]);

        // Iterates through the attacks, adding moves to the move list
        for(int target : attack_indices){
            // If the bit is set on enemy pieces, this is a capture
            if(get_bit(enemy_pieces, target)){
                // Retrieve which type of piece is being captured
                int captured_piece = GetPieceType(target);
                struct Move move = {pawn, target, capture, captured_piece, color};
                moves.push_back(move);
            }
        }
    }
        

    // Calls CalcPawnPushes() to construct setwise pawn push tables
    CalcPawnPushes();

    // Gets single pawn pushes
    vector<int> single_pawn_push_indices = SerializeBitboard(single_pawn_pushes[color]);
    
    // Iterates through every target within the pawn push table
    for(int target : single_pawn_push_indices){

        // Subtract(for white), or adds (for black) 8 to get the rank either directly above or directly
        // below the target. This is because for a single pawn push, the origin will always be the previous rank
        int start = target - 8 * color_flag;
        struct Move move = {start, target, quiet, blank, color};
        moves.push_back(move);
    }
    // Gets double pawn push moves
    vector<int> double_pawn_push_indices = SerializeBitboard(double_pawn_pushes[color]);
    for(int target : double_pawn_push_indices){
        // Either subtracts or adds 16 to get the origin square two ranks above or below the target
        // square
        int start = target - 16 * color_flag;
        struct Move move = {start, target, en_passant, blank, color};
        moves.push_back(move);
    }
}

void Board::CalcPawnPushes(){

    U64 rank4 = 0x00000000FF000000ULL;
    U64 rank5 = 0x000000FF00000000ULL;

    single_pawn_pushes[white] = (white_pawns << 8) & empty;
    single_pawn_pushes[black] = (black_pawns >> 8) & empty;

    double_pawn_pushes[white] = (single_pawn_pushes[white] << 8) & empty & rank4;
    double_pawn_pushes[black] = (single_pawn_pushes[black] >> 8) & empty & rank5;

}


void Board::GenerateKnightMoves(int color, vector<Move> &move_list){
    U64 knights = (!color) ? white_knights : black_knights;

    vector<int> knight_indices = SerializeBitboard(knights);
    FillMoveList(knight_indices, knight_attacks, color, move_list);
}

void Board::GenerateKingMoves(int color, vector<Move> &move_list){
    U64 king = (!color) ? white_kings : black_kings;

    vector<int> king_indices = SerializeBitboard(king);
    FillMoveList(king_indices, king_attacks, color, move_list);
}

void Board::GenerateSliderMoves(int color, vector<Move> &move_list){
    U64 queens = black_queens;
    U64 rooks = black_rooks;
    U64 bishops = black_bishops;

    if(!color){
        queens = white_queens;
        rooks = white_rooks;
        bishops = white_bishops;
    }

    vector<int> queen_index = SerializeBitboard(queens);
    vector<int> rook_index = SerializeBitboard(rooks);
    vector<int> bishop_index = SerializeBitboard(bishops);
    InitSliderAttacks();

    FillMoveList(queen_index, queen_attacks, color, move_list);
    FillMoveList(rook_index, rook_attacks, color, move_list);
    FillMoveList(bishop_index, bishop_attacks, color,  move_list);
}

void Board::FillMoveList(vector<int> piece_list, U64 attack_map[64], int color, vector<Move> &move_list){
    U64 color_pieces = black_pieces;
    if (!color)
    {
        color_pieces = white_pieces;
    }

    for(int piece : piece_list){
        vector<int> targets = SerializeBitboard(attack_map[piece] & ~color_pieces);
        for(int target : targets){
            Move move;
            move.start = piece;
            move.end = target;
            move.capture = GetPieceType(target);
            move.move_type = (move.capture) ? capture : quiet;
            move.color = color;
            move_list.push_back(move);
        }
    }
}
vector<Move> Board::GenerateMoveList(int color){

    vector<U64> piece_bitboards;
    vector<Move> move_list;

    // Adds all pawn moves for the color to the move list
    GeneratePawnMoves(color, move_list);

    // Adds all knight moves for the color to the move list
    GenerateKnightMoves(color, move_list);

    // Adds all king moves for the color to the move list
    GenerateKingMoves(color, move_list);

    GenerateSliderMoves(color, move_list);

    return move_list;
}

int Board::GetPieceType(int index){
    U64 null = 0ULL;
    U64 piece_boards[13] = {null, white_pawns, white_rooks, white_knights, white_bishops, white_queens, white_kings,
                            black_pawns, black_rooks, black_knights, black_bishops, black_queens, black_kings};
    for (int i = 0; i < 13; i++){
        if(get_bit(piece_boards[i], index)){
            return i;
        }
    }
    return 0;
}

void Board::MakeMove(Move move){
    
    U64 null = 0ULL;
    U64 piece_boards[13] = {null, white_pawns, white_rooks, white_knights, white_bishops, white_queens, white_kings,
                            black_pawns, black_rooks, black_knights, black_bishops, black_queens, black_kings};
    int moving_piece = GetPieceType(move.start);
    int captured_piece = GetPieceType(move.end);
    U64 moving_piece_board = piece_boards[moving_piece];
    U64 captured_piece_board = piece_boards[captured_piece];
    if (move.color == white)
    {
        pop_bit(white_pieces, move.start);
        pop_bit(black_pieces, move.end);
        pop_bit(captured_piece_board, move.end);
        set_bit(white_pieces, move.end);
        set_bit(moving_piece_board, move.end);
    }
    else
    {
        pop_bit(black_pieces, move.start);
        pop_bit(white_pieces, move.end);
        pop_bit(captured_piece_board, move.end);
        set_bit(black_pieces, move.end);
        set_bit(moving_piece_board, move.end);
    }
    piece_boards[1] = 0ULL;
}

    void Board::UnMakeMove(Move move)
    {


}