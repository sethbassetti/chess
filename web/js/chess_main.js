
var board = null;
var currentColor;
DEPTH = 4;
var game = new Chess();

var slider = document.getElementById("myRange");
var output = document.getElementById("depth-text");
output.innerHTML = slider.value; // Display the default slider value

// Update the current slider value (each time you drag the slider handle)
slider.oninput = function() {
  output.innerHTML = this.value;
}

const squareIndexes = ["a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
  "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
  "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
  "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
  "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
  "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
  "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
  "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8"];


/* Initializes the board with configuration. This is called when the webpage loads */
function initBoard() {

    // defines various options about the board, such as pieces are draggable and to call the onDrop function
    var config = {
        draggable: true,
        onDrop: onDrop
    }

  // Defines the board variable with the given config
  board = Chessboard('board', config);
}

/* (Re)Starts the game. Gets the starting color from the radio button, starts the board and determines it's orientation */
function startGame() {

    // Grabs the value of the current color radio button
    currentColor = $("input[name='colorSelector']:checked").val();

    // Sets the board to the start position instantly
    board.start('false');

    // Sets the orientation of the board to be whichever color is selected
    board.orientation(currentColor);
    
    // Resets the game variable
    game = new Chess();

    // If the starting color is black, then make a random white move (after a delay) to start the game
    if (currentColor == "black") makeBestMove();
}

/* Function that is called when a piece is dropped, checks if that move is a legal move and if so it makes that move */
function onDrop(source, target) {

  // make the move on the game and assign it to a variable
  var move =  game.move({
    from: source,
    to: target,
    promotion: 'q'
  })

  

  // If the move does not exist, return the piece back to its position
  if (move == null) return 'snapback';

  // Otherwise, make a random move
  else {

    // If the game is in checkmate after the player moves, they win
    if (game.in_checkmate()){
      alert("You win!");
    }

    // Otherwise make a move
    window.setTimeout(makeBestMove, 250);
  }

  // After half a second, check if the player is in checkmate, indicating they lose
  window.setTimeout(() => {
    if (game.in_checkmate()) alert("You Lose")
  }, 1000);

}


/* Makes a random move on the board */
function makeRandomMove(){

  // init all possible moves from this game state
  var possibleMoves = game.moves();

  // If there are no moves, then return 
  if (possibleMoves.length == 0) return;

  // get a random index for the moves
  var randomIdx = Math.floor(Math.random() * possibleMoves.length);

  // make the move
  game.move(possibleMoves[randomIdx]);

  // set the board position to be the FEN string of the game variable
  board.position(game.fen());

}

/* Makes the best move for the current side on the board */
function makeBestMove() {

  // Look at the depth slider to determine depth of c++ search
  let depth = document.getElementById("myRange").value;

  // call the C++ engine to get the best move given the current board state
  var move = Module.GetBestMove(game.fen(), depth);

  // Make the move on the board
  game.move(move, {sloppy: true})

  // set the board position correctly
  board.position(game.fen())
}

