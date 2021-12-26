

function initGame() {
    var board = document.getElementById("board");
    for (var rank = 0; rank < 8; rank++){
        for (var file = 0; file < 8; file++){
            var square_index = rank * 8 + file;
            var square = document.createElement("div");
            if ((rank+file) % 2 == 0) {
                square.className = "black";
            } else {
                square.className = "white"
            }
            board.appendChild(square);
            console.log("Hello");
        }
    }
}