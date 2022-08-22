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
#include "position.h"


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
                        if (target == enpassant)
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
    enpassant = prev_state.en_passant_square;
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