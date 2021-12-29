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

function onDrop(source, target, piece, newPos, oldPos, orientation) {
  var source_index = squareIndexes.indexOf(source);
  var target_index = squareIndexes.indexOf(target);

    
  if (!isLegalMove(source_index, target_index, oldPos)) {
    return 'snapback'
  }
  }


function isLegalMove(source, target, oldPosition) {
  var validMove = Module.IsValidMove("hello", 3, source, target);
  return validMove;
}

$(document).ready(function () {
    $("#setStartBtn").click(function () {
        startGame();
    });
});

