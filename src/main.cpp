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

    MoveCalc calc = MoveCalc();
    Board game_board = Board("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq –");
    game_board.Display();
    U64 occupancy = 0ULL;

    
    set_bit(occupancy, c5);
    set_bit(occupancy, d2);
    PrintBoard(occupancy);
    //calc.InitMagicNumbers();
    U64 attack = calc.GetRookAttacks(d5, occupancy);
    PrintBoard(attack);
    //PrintBoard(occupancy);
    //PrintBoard(attack);
    //game_board.Test();

    

    
    

}