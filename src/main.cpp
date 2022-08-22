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
    Board game_board = Board("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQ-q e3 0 1");
    game_board.Display();
    //U64 occupancy = 0ULL;

}