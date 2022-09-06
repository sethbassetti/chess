#include <iostream>
#include <string>
#include <stdlib.h>
#include <bits/stdc++.h>


#include "move_calc.h"
#include "utils.h"
#include "board.h"

using namespace std;


// initialize the board object
Board board = Board();

/* Handles the position command from GUI for UCI protocol */
void parse_position(string input_line)
{

    // Initialize a stringstream to split line up by spaces
    stringstream ss(input_line);

    // init token to hold individual words
    string token;

    // read the position word
    ss >> token;


    // read the next argument (either startpos or fen)
    ss >> token;


    if (token == "startpos")
    {

        // sets the board position to the starting position and display it
        board.SetFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

        // read the next token to see if it contains moves
        ss >> token;
    }
    else
    {   
        // init a fen string
        string fen_string;

        // while we are not at end of token, keep reading parts of the fen string
        while (ss >> token)
        {   
            // if we get to moves section, then break this while loop
            if (token == "moves") break;

            // add that substring to the full fen string (with a space separator)
            fen_string += token + " ";
        }

        // set the fen string of the board and display it
        board.SetFEN(fen_string);
    }

    // if token is equal to moves
    if (token == "moves")
    {
        // keep reading moves
        while (ss >> token)
        {
            // make each move 
            board.MakeMove(token);
        }
    }
}

void parse_go(string input_line)
{
    // create a stringstream to split by spaces and a token to contain the tokens
    stringstream ss(input_line);
    string token;

    // Create a string 
    int depth;

    // read the go command
    ss >> token;

    // read the next argument of the command
    ss >> token;

    // handle if we are doing a perft test
    if (token == "perft")
    {   
        // read in the perft depth
        ss >> token;

        // call the perft driver function with the depth (converted to int)
        board.perft_driver(stoi(token));

        // break out of function early after perft test
        return;
    }

    // if we are given a depth argument
    else if (token == "depth")
    {
        // read in the actual depth
        ss >> token;

        // get the depth from the command
        depth = stoi(token);

    }
    else
    {
        // otherwise default to a depth of 6
        depth = 6;
    }

    // use negamax to calculate best move to a certain depth
    int move = board.GetBestMove(depth);

    // print out that move to standard output
    PrintMove(move);

}

/* Controls the main input/output loop for UCI protocol */
void uci_loop()
{
    // Variable to hold some input received from GUI
    string input_line;

    // Tell the GUI name and author of chess engine
    cout << "id name SpaghettiChess" << endl;
    cout << "id author Seth Bassetti" << endl;

    // Tell the GUI we are in UCI mode and ready to process commands
    cout << "uciok" << endl;

    // Main input/output loop
    while (true)
    {
        // Reads any input from the line
        std::getline(cin, input_line);

        // Read in if position command is given
        if (input_line.rfind("position", 0) == 0)
        {   
            // parse the position to set the board state of the board
            parse_position(input_line);
        }

        // if isready command is given, respond that the engine is ready (readyok)
        else if(input_line == "isready")
            cout << "readyok" << endl;

        // if go command is given
        else if(input_line.rfind("go", 0) == 0)
        {
            parse_go(input_line);
        }

        // if ucinewgame command is sent
        else if(input_line == "ucinewgame")
        {   
            // init the board with the default starting position
            parse_position("position startpos");
        }


        // if d command is sent, display the board
        else if(input_line == "d")
        {
            board.Display();
        }

        // if quit command is given, exit the while loop
        else if(input_line == "quit")
            break;

        // reset the input line every iteration of the for loop
        input_line = "";
    }


}

int main(){

    // init msg variable from gui to engine
    string msg;

    // get a line from user
    getline(cin, msg);

    // if that line == uci, then start the uci loop
    if (msg == "uci")
        uci_loop();


}

