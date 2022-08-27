#include <iostream>
#include <vector>
#include "board.h"
/* Class that begins the game and handles taking input from player and making AI moves */
class Game{
    public:
        void Start();

    private:
        Board game_board;
        int player_color;

        void Play();

        
    
        int GetPlayerColor();

        //int IsValidInput(string input);
        //vector<int> ParsePlayerInput(string input);
        void MakePlayerMove();
        void MakeAIMove();

        void UpdateGame();

        //Move NegaMaxRoot(int depth);
        int NegaMax(int depth);
};
