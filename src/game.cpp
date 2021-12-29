#include <vector>
#include <string.h>
#include <iostream>
#include <algorithm>

#include "game.h"
#include "board.h"
#include "evaluation.h"

using namespace std;

const vector<string> square_index = {"a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
                                     "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
                                     "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
                                     "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
                                     "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
                                     "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
                                     "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
                                     "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8"};

/* Starts the game by initializing the board and calling the play function */
void Game::Start()
{
    Board game_board = Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    player_color = GetPlayerColor();
    Play();
}

/* This handles the main game loop, reading user input and making AI moves */
void Game::Play(){
    // If the player chooses black, have the AI make the first move, then go into the game loop
    if(player_color == black){
        MakeAIMove();
    }

    // Prints out the board
    game_board.Display();
    while (true)
    {
        MakePlayerMove();
        UpdateGame();
        MakeAIMove();
        UpdateGame();
        cout << Evaluate(game_board.GetPosition(), game_board.GetCurrentPlayer()) << endl;
    }
}


/* Prompts the user to select their starting color */
int Game::GetPlayerColor(){

    string color_input;

    // Prompts the user and retrieves their input from stdin
    cout << "Enter which color you want to play as e.g. (w or b): ";
    getline(cin, color_input);

    // Only breaks out of the while loop once the user has entered a valid input
    while(color_input != "w" && color_input != "b"){
        cout << "Invalid input. Please enter either 'b' or 'w': ";
        getline(cin, color_input);
    }

    // Returns the correct color
    if(color_input == "w"){
        return white;
    }
    else{
        return black;
    }
}


void Game::MakePlayerMove(){
    string player_input;
    cout << "Please enter a move in the form of origindestination e.g. 'a2a3': ";

    getline(cin, player_input);
    while (!IsValidInput(player_input)){
        cout << "Invalid input. Try again or type 'q' to quit: ";
        getline(cin, player_input);

        // If the player types q then print goodbye and exit program
        if(player_input == "q"){
            printf("Goodbye\n");
            exit(1);
        }
    }

    vector<int> move_vector = ParsePlayerInput(player_input);
    if(game_board.MakeMove(move_vector[0], move_vector[1])){
        return;
    }else{
        MakePlayerMove();
    }
}

void Game::MakeAIMove(){

    Move best_move = NegaMaxRoot(4);
    game_board.MakeMove(best_move);
}

int Game::IsValidInput(string input){
    if(input.size() != 4){
        return 0;
    }

    string start = string() + input[0] + input[1];
    string end = string() + input[2] + input[3];
    auto start_it = find(square_index.begin(), square_index.end(), start);
    auto end_it = find(square_index.begin(), square_index.end(), end);
    if (start_it == square_index.end() || end_it == square_index.end())
    {
        return 0;
    }
    else
    {
        return 1;
    }
    return 0;
}

vector<int> Game::ParsePlayerInput(string input){
    vector<int> move_vector;

    string start = string() + input[0] + input[1];
    string end = string() + input[2] + input[3];
    auto start_it = find(square_index.begin(), square_index.end(), start);
    auto end_it = find(square_index.begin(), square_index.end(), end);

    move_vector.push_back(start_it - square_index.begin());
    move_vector.push_back(end_it - square_index.begin());

    return move_vector;
}

void Game::UpdateGame(){
    game_board.Display();
    vector<Move> move_list = game_board.GenerateMoveList();
    int prev_color = game_board.GetCurrentPlayer() ^ 1;
    string winning_player = (prev_color == white) ? "White" : "Black";
    if (!move_list.size())
    {
        cout << "Game Over!" << endl;
        cout << winning_player << " won!" << endl;
        exit(0);
    }
}

Move Game::NegaMaxRoot(int depth){
    vector<Move> moves = game_board.GenerateMoveList();
    Move best_move;
    int score;
    int best_score = -1000000;
    for (Move move : moves)
    {
        // White turn
        game_board.MakeMove(move);
        // Black turn
        score = -NegaMax(depth - 1);
        game_board.UnMakeMove(move);
        if(score > best_score){
            best_score = score;
            best_move = move;
        }
    }
    return best_move;
}

int Game::NegaMax(int depth){
    int score;
    int max = -1000000;

    if (depth == 0)
    {
        // If position is better for black, returns negative, 
        return Evaluate(game_board.GetPosition(), game_board.GetCurrentPlayer());
    }

    
    for(Move move : game_board.GenerateMoveList()){
        game_board.MakeMove(move);
        score = -NegaMax(depth - 1);
        game_board.UnMakeMove(move);
        if(score > max){
            max = score;
        }
    }

    return max;
}