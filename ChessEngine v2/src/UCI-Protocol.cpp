#include "headers/UCI-Protocol.h"
#include "headers/Move.h"
#include "headers/MoveGenerator.h"
#include "headers/Search.h"
#include <string>

int parseMove(std::string moveString, const Board& board) {
	MoveList moveList;
	generateMoves(moveList, board);

	int startSquare = (moveString[0] - 'a') + (8 - (moveString[1] - '0')) * 8; // parse start square
	int targetSquare = (moveString[2] - 'a') + (8 - (moveString[3] - '0')) * 8; // parse target square

	// loop over the moves within a move list
	for (int moveCount = 0; moveCount < moveList.count; moveCount++) {
		int move = moveList.moves[moveCount];

		// make sure source & target squares are available within the generated move
		if (startSquare == getMoveStart(move) && targetSquare == getMoveTarget(move)) {
			int promotedPiece = getMovePromoted(move);

			if (promotedPiece) {
				if ((promotedPiece == Q || promotedPiece == q) && moveString[4] == 'q')
					return move;
				else if ((promotedPiece == R || promotedPiece == r) && moveString[4] == 'r')
					return move;
				else if ((promotedPiece == B || promotedPiece == b) && moveString[4] == 'b')
					return move;
				else if ((promotedPiece == N || promotedPiece == n) && moveString[4] == 'n')
					return move;

				continue;
			}

			return move;
		}
	}
	return 0;
}

void parsePosition(const std::string& command, Board& board, TranspositionTable& tt) {
    tt.clear();

    // Copy of command to work on since std::string is immutable when passed as const&
    std::string cmd = command;

    // Trim the first 9 characters ("position ")
    cmd = cmd.substr(9);

    // Parse UCI "startpos" command
    if (cmd.substr(0, 8) == "startpos") {
        // Init chess board with start position
        board.parseFEN(startPosition);
        tt.clear();
    }
    // Parse UCI "fen" command
    else if (cmd.find("fen") != std::string::npos) {
        // Find the "fen" position
        size_t fenPos = cmd.find("fen");
        // Shift to the fen string
        std::string fenString = cmd.substr(fenPos + 4);

        // Init chess board with position from FEN string
        board.parseFEN(fenString.c_str());
    }
    // Default to start position if neither "startpos" nor "fen" is found
    else {
        board.parseFEN(startPosition);
    }

    // Parse moves after position
    size_t movesPos = cmd.find("moves");

    // If moves are available
    if (movesPos != std::string::npos) {
        // Extract the moves string (everything after "moves ")
        std::string movesString = cmd.substr(movesPos + 6);

        // While there are still moves in the string
        size_t pos = 0;
        while (pos != std::string::npos) {
            // Find the next space (move delimiter)
            size_t spacePos = movesString.find(' ', pos);

            // Extract a single move
            std::string move = movesString.substr(pos, spacePos - pos);

            // Parse the move
            int parsedMove = parseMove(move, board);

            // If no more moves, break
            if (parsedMove == 0) {
                break;
            }

            // Make the move on the chess board
            makeMove(parsedMove, allMoves, board);

            // Move to the next move
            pos = (spacePos == std::string::npos) ? std::string::npos : spacePos + 1;
        }
    }
    board.printBoard();
}

void parseGo(const std::string& command, Board& board, TranspositionTable &tt) {
    // Initialize depth to a default value
    int depth = -1;

    // Check if the command contains the "depth" keyword
    size_t depthPos = command.find("depth");

    // Handle fixed depth search
    if (depthPos != std::string::npos) {
        // Extract the depth argument (number after "depth")
        std::string depthString = command.substr(depthPos + 6);

        // Convert the depth string to an integer
        depth = std::stoi(depthString);
    }
    else
        // Default depth if no depth is provided
        depth = 7;

    // Search position with the given depth
    searchPosition(depth, board, tt);
}

void uciLoop(Board& board, TranspositionTable &tt) {
    // Print engine info at the start
    std::cout << "id name DogFish" << std::endl;
    std::cout << "id author Jonatan" << std::endl;
    std::cout << "uciok" << std::endl;

    std::string input;

    // Main loop
    while (true) {
        // Get user / GUI input
        std::getline(std::cin, input);

        // Skip empty input
        if (input.empty())
            continue;

        // Parse UCI "isready" command
        if (input.compare(0, 7, "isready") == 0) {
            std::cout << "readyok" << std::endl;
            continue;
        }
        // Parse UCI "position" command
        else if (input.compare(0, 8, "position") == 0)
            parsePosition(input, board, tt);
        // Parse UCI "ucinewgame" command
        else if (input.compare(0, 10, "ucinewgame") == 0)
            parsePosition("position startpos", board, tt);
        // Parse UCI "go" command
        else if (input.compare(0, 2, "go") == 0)
            parseGo(input, board, tt);
        // Parse UCI "quit" command
        else if (input.compare(0, 4, "quit") == 0)
            break;
        // Parse UCI "uci" command
        else if (input.compare(0, 3, "uci") == 0) {
            // Print engine info
            std::cout << "id name DogFish" << std::endl;
            std::cout << "id author Jonatan" << std::endl;
            std::cout << "uciok" << std::endl;
        }

        std::cout << "Usage: " << tt.getFilledPercentage() << "%\n";

        // Flush output to ensure it reaches the GUI
        std::cout.flush();
    }
}
