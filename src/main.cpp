#include<iostream>
#include "game.h"
using namespace std;


int main(){
    /*
    Game game;
    game.Start();
    
    printf("This is running\n");
    */
   Board game_board = Board("rnbqkbnr/pKpppppp/8/8/3K2b1/8/PPPPPPPP/RNBQKBNR b KQq e5 0 1");
   game_board.Test();
}