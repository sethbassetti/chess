#include<iostream>
#include <stdlib.h>


#include "move_calc.h"
#include "utils.h"
#include "board.h"

using namespace std;

int main(){
    
    Board game_board = Board("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 1");

    game_board.Display();
    cout << game_board.Evaluate() << endl;
    //PrintMove(game_board.GetRandomMove());


}

