var board;
var currentColor;

const squareIndexes = ["a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
  "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
  "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
  "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
  "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
  "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
  "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
  "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8"];

/* Initializes the board with configuration */
function initBoard() {
    var config = {
        draggable: true,
        onDrop: onDrop
      }
  board = Chessboard('board', config);
}

/* Starts the game. Gets the starting color from the radio button, starts the board and determines it's orientation */
function startGame() {
    currentColor = $("input[name='colorSelector']:checked").val();
    board.start('false');
    board.orientation(currentColor);
}

/* Function that is called when a piece is dropped, checks if that move is a legal move and if so it makes that move */
function onDrop(source, target, piece, newPos, oldPos, orientation) {

  // Retrieves the integer square for the source and target
  var source_index = squareIndexes.indexOf(source);
  var target_index = squareIndexes.indexOf(target);

  var valid_move = Module.MakeMove(source_index, target_index);

  // Checks the board to see if the move is legal, if not it snaps the move back
  if (!valid_move)
  {
    return 'snapback'
  }

    
  else
  {
    var move = Module.MakeRandomMove();
    board.move(move);
  }
  
}


$(document).ready(function () {
    $("#setStartBtn").click(function () {
        startGame();
    });
});

