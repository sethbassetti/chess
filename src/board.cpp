#include <cstdint>
#include <iostream>
#include <stdio.h>
#include <bitset>
#include <strings.h>
#include <string.h>
#include <assert.h>  
#include <ctime>
#include <vector>
#include <algorithm>
#include "utils.h"
#include "board.h"
#include "position.h"


using namespace std;


// Constructor for board, initializes board position and preset attack tables
Board::Board(){
    // Constructs a position object that holds information about piece positions, sets up initial board position
    position = Position();

    // Sets white as the first turn
    turn_to_move = white;

    // Set en passant square to 0 (indicates no en passant available)
    en_passant_square = 0;

    // Sets castling rights such that all castling is available at the start (no pieces have moved)
    castling_rights = wk | wq | bk | bq;

    // Initializes leaper attack tables, since they are independent of board position
    InitLeaperAttacks();

    // Initializes precalculated ray attack lookup tables
    InitRayAttacks();

}

Board::Board(std::string fen_string){

    // Builds the position based on the fen_string
    position = Position(fen_string);

    ParseFENColorCastling(fen_string);

    // Initializes leaper attack tables, since they are independent of board position
    InitLeaperAttacks();

    // Initializes precalculated ray attack lookup tables
    InitRayAttacks();
}

void Board::ParseFENColorCastling(string fen_string){
    char *token;

    // Copies the fen_string variable into a char array to work with easier
    char char_fen[fen_string.size() + 1];
    fen_string.copy(char_fen, fen_string.size() + 1);
    char_fen[fen_string.size()] = '\0';

    // Gets the first segment of the FEN
    token = strtok(char_fen, " ");

    // Gets the second segment of the FEN
    token = strtok(NULL, " ");

    // Determines the current turn
    if(strcmp(token, "w") == 0){
        turn_to_move = white;
    }else{
        turn_to_move = black;
    }

    // Determines the castling rights
    token = strtok(NULL, " ");
    
    // Copies the token to a string for easier iteration
    string castling_right_string;
    castling_right_string.append(token);
    
    // Sets castling rights to 0 at first and then builds them up
    castling_rights = 0;
    

    // Iterates over each character in the string
    for (unsigned int i = 0; i < castling_right_string.size(); i++){
        
        // Otherwise, look at each character and assign appropriate castling rights
        switch(castling_right_string.at(i)){
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

    // Determine en passant square
    token = strtok(NULL, " ");
    
    // If character is a dash, no en passant is available
    if(token[0] == '-'){
        en_passant_square = 0;
    
    // Otherwise, set the en passant square
    }else{
        // Converting the token into a string for easy iteration
        string en_passant_square_string;
        en_passant_square_string.append(token);

        // Finds the index of the square in the square_index table
        auto it = std::find(square_index.begin(), square_index.end(), en_passant_square_string);
        int square_index_num = it - square_index.begin();

        // Sets the ne passant square to that number
        en_passant_square = square_index_num;
    }
}
/* Test function for implementing and development */
void Board::Test(){
    position.PrintBoard();
    printf("%d\n", turn_to_move);
    printf("%d\n", castling_rights);
    printf("%d\n", en_passant_square);
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
    U64 blocker = ray_attack & position.occupancy;

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



/* Retrieves attack tables for slider pieces (bishop, rook, queen) */
void Board::InitSliderAttacks(){

    

    // Constructs rook attack table for every square on board
    for (int rank = 0; rank < 8; rank++)
    {
        for (int file = 0; file < 8; file++){
            int square = rank * 8 + file;
            rook_attacks[square] = CalcRookAttacks(square);
            bishop_attacks[square] = CalcBishopAttacks(square);
            queen_attacks[square] = CalcQueenAttacks(square);
        }
    }
}

/* Given a color and reference to a move list, populates the move list
with all pawn moves of the given color */
void Board::GeneratePawnMoves(vector<Move> &moves){

    // Offset changes the pawn bitboards from black or white (since they are 6 spots apart in the array)
    int offset = turn_to_move * 6;

    // Ends up being -1 for black, 1 for white
    int color_flag = (turn_to_move - (turn_to_move ^ 1)) * -1;

    // Selects pawn 
    U64 pawns = position.pieces[white_pawn + offset];
    U64 enemy_pieces = position.colors[turn_to_move ^ 1];
    

    // Serialize the pawns into a vector of their integer indices
    vector<int> pawn_indices = SerializeBitboard(pawns);

    // Get pawn attacks
    for(int pawn : pawn_indices)
                {

                    // Iterates through each posible attack the pawn could make
                    vector<int> attack_indices = SerializeBitboard(pawn_attacks[turn_to_move][pawn]);

                    // Iterates through the attacks, adding moves to the move list
                    for (int target : attack_indices)
                    {
                        // If the bit is set on enemy pieces, this is a capture
                        if (get_bit(enemy_pieces, target))
                        {
                            // Retrieve which type of piece is being captured
                            int captured_piece = position.GetPieceType(target);
                            struct Move move = {pawn, target, capture, captured_piece};
                            if (get_bit(first_last_ranks, target))
                            {
                                move.promo_piece = white_knight;
                                struct Move secondPromo = move;
                                secondPromo.promo_piece = white_queen;
                                Move thirdPromo = secondPromo;
                                thirdPromo.promo_piece = white_rook;
                                Move fourthPromo = secondPromo;
                                fourthPromo.promo_piece = white_bishop;
                                moves.push_back(secondPromo);
                                moves.push_back(thirdPromo);
                                moves.push_back(fourthPromo);
                            }
                            moves.push_back(move);
                        }
                        if (target == en_passant_square)
                        {
                            int offset = (turn_to_move == white) ? -8 : 8;
                            int captured_piece = position.GetPieceType(target + offset);
                            struct Move move = {pawn, target, ep_capture, captured_piece};
                            moves.push_back(move);
                        }
                    }
                }

    // Calls CalcPawnPushes() to construct setwise pawn push tables
    CalcPawnPushes();

    // Gets single pawn pushes
    vector<int> single_pawn_push_indices = SerializeBitboard(single_pawn_pushes[turn_to_move]);
    
    // Iterates through every target within the pawn push table
    for(int target : single_pawn_push_indices){

        // Subtract(for white), or adds (for black) 8 to get the rank either directly above or directly
        // below the target. This is because for a single pawn push, the origin will always be the previous rank
        int start = target - 8 * color_flag;
        struct Move move = {start, target, quiet, blank};
        
        if(get_bit(first_last_ranks, target)){
                    move.promo_piece = white_knight;
                    struct Move secondPromo = move;
                    secondPromo.promo_piece = white_queen;
                    Move thirdPromo = secondPromo;
                    thirdPromo.promo_piece = white_rook;
                    Move fourthPromo = secondPromo;
                    fourthPromo.promo_piece = white_bishop;
                    moves.push_back(secondPromo);
                    moves.push_back(thirdPromo);
                    moves.push_back(fourthPromo);
        }
        moves.push_back(move);
    }
    // Gets double pawn push moves
    vector<int> double_pawn_push_indices = SerializeBitboard(double_pawn_pushes[turn_to_move]);
    for(int target : double_pawn_push_indices){
        // Either subtracts or adds 16 to get the origin square two ranks above or below the target
        // square
        int start = target - 16 * color_flag;
        struct Move move = {start, target, double_pawn_push, blank};
        moves.push_back(move);
    }

    
}

void Board::CalcPawnPushes(){

    U64 rank4 = 0x00000000FF000000ULL;
    U64 rank5 = 0x000000FF00000000ULL;
    U64 white_pawns = position.pieces[white_pawn];
    U64 black_pawns = position.pieces[black_pawn];

    single_pawn_pushes[white] = (white_pawns << 8) & position.empty;
    single_pawn_pushes[black] = (black_pawns >> 8) & position.empty;

    double_pawn_pushes[white] = (single_pawn_pushes[white] << 8) & position.empty & rank4;
    double_pawn_pushes[black] = (single_pawn_pushes[black] >> 8) & position.empty & rank5;

}


void Board::GenerateKnightMoves(vector<Move> &move_list){

    int offset = turn_to_move * 6;
    U64 knights = position.pieces[white_knight + offset];

    vector<int> knight_indices = SerializeBitboard(knights);
    FillMoveList(knight_indices, knight_attacks, move_list);
}

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

void Board::GenerateSliderMoves(vector<Move> &move_list){

    int offset = turn_to_move * 6;

    U64 queens = position.pieces[white_queen + offset];
    U64 rooks = position.pieces[white_rook + offset];
    U64 bishops = position.pieces[white_bishop + offset];

    vector<int> queen_index = SerializeBitboard(queens);
    vector<int> rook_index = SerializeBitboard(rooks);
    vector<int> bishop_index = SerializeBitboard(bishops);
    InitSliderAttacks();

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

/* Parses the move list and returns a new move list with only legal moves. I.e. moves that do not put the king
in check and valid castle moves (no intermediate squares under attack) */
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

/* This function takes a move object and moves the necessary pieces on the board. It looks at the move type,
such as castling, promotions, or en passants, and does the necessary board adjustments to accurately make that move.
At the end of the function, it resets the board's position object to reflect the changes */
void Board::MakeMove(Move move){

    // Before making the move, store the board's game state so that this move can be unmade
    gameState state = {position, castling_rights, en_passant_square};

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
        en_passant_square = (move.start + move.end) / 2;

    // If it is not a double pawn push, reset the en passant square
    }else{
        en_passant_square = 0;
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
    InitSliderAttacks();
    // Switches whose turn it is to play
    ToggleMove();
}

/* Attempts to make a move with a starting and ending square, returns 1 if successful, 0 otherwise */
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

/* Given a move object, reverse the operations performed by MakeMove(). This restores all aspects of the game state, such
as castling rights and piece positions */
void Board::UnMakeMove(Move move)
{

    gameState prev_state = game_state_hist.back();
    game_state_hist.pop_back();
    castling_rights = prev_state.castling_rights;
    memcpy(queen_attacks, prev_state.queen_attacks, sizeof(queen_attacks));
    memcpy(rook_attacks, prev_state.rook_attacks, sizeof(rook_attacks));
    memcpy(bishop_attacks, prev_state.bishop_attacks, sizeof(bishop_attacks));
    en_passant_square = prev_state.en_passant_square;
    position = prev_state.position;
    ToggleMove();
}

/* Returns true if the king of the input color is being attacked by any other enemy piece */
bool Board::KingInCheck(int color){
    
    // Offset determines what color of king to retrieve
    int offset = 6 * color;
    U64 king = position.pieces[white_king + offset];

    // Retrieves the index of the king's location
    int king_square = BitScan(king);


    // If the king square is being attacked, return true, since it is in check
    return IsSquareAttacked(king_square, color);
}

/* Returns true if a piece on a square of a given color could be attacked by an enemy of the opposite color */
bool Board::IsSquareAttacked(int square, int color){

    int enemy_offset = 6 * (color ^ 1);

    // Retrieves the bitboard of enemy pawns opposite the kings color
    U64 enemy_pawns = position.pieces[white_pawn + enemy_offset];
    U64 enemy_knights = position.pieces[white_knight + enemy_offset];
    U64 enemy_king = position.pieces[white_king + enemy_offset];
    U64 enemy_rooks = position.pieces[white_rook + enemy_offset];
    U64 enemy_bishops = position.pieces[white_bishop + enemy_offset];
    U64 enemy_queens = position.pieces[white_queen + enemy_offset];
    
    // Goes through every piece type, looking for a possible attack
    bool attacked_by_pawn = pawn_attacks[color][square] & enemy_pawns;
    bool attacked_by_knight = knight_attacks[square] & enemy_knights;
    bool attacked_by_king = king_attacks[square] & enemy_king;
    bool attacked_by_rook = rook_attacks[square] & enemy_rooks;
    bool attacked_by_bishop = bishop_attacks[square] & enemy_bishops;
    bool attacked_by_queen = queen_attacks[square] & enemy_queens;

    // If any of these are true, the square is under attack
    return attacked_by_pawn | attacked_by_knight | attacked_by_rook | attacked_by_queen | attacked_by_king | attacked_by_bishop;
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

void Board::perft_test(int depth){
    
    clock_t start;
    double duration;
    start = clock();
    
    
    int total_count = 0;
    vector<Move> moves = GenerateMoveList();
    for(Move move : moves){
        MakeMove(move);
        int count = perft(depth - 1);
        total_count += count;
        UnMakeMove(move);
        PrintMove(move);
        printf(": %d\n", count);
    }
    duration = (clock() - start) / (double)CLOCKS_PER_SEC;
    printf("Perft: %d nodes searched\n", total_count);
    printf("Took %f seconds\n", duration);
}

void Board::ToggleMove(){
    turn_to_move ^= 1;
}

void Board::Display(){
    position.PrintBoard();
}

int Board::GetCurrentPlayer(){
    return turn_to_move;
}

Position Board::GetPosition(){
    return position;
}

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
}

std::string Board::GetBoardFEN(){
    return position.GenerateFEN();
}