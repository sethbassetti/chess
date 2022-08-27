#include<iostream>
#include <stdlib.h>
#include "game.h"
#include "move_calc.h"
#include "utils.h"
using namespace std;





int main(){
    /*
    Game game;
    game.Start();
    
    printf("This is running\n");
    */

    //MoveCalc calc = MoveCalc();
    Board game_board = Board("r3kb1r/pppp1pPp/8/3Pp3/8/3B1N2/PPP2PPP/R3K2R w KQkq e6 0 1");
    game_board.Display();
    
    game_board.perft_driver();

    //U64 occupancy = 0ULL;

}