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
   Board game_board = Board("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq –");
   game_board.Display();
   //game_board.Test();
    MoveCalc calc = MoveCalc();
    U64 magic = calc.FindMagicNumber(e4, 9, 1);
    PrintBoard(magic);
    calc.InitMagicNumbers();

    

    
    

}