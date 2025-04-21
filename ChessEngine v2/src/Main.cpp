#include "headers/Perft.h"
#include "headers/AttacksGen.h"
#include "headers/UCI-Protocol.h"
#include "headers/Move.h"
#include "headers/MoveGenerator.h"
#include "headers/Search.h"
#include "headers/HelperFunctions.h"
#include "headers/ZoobristKeys.h"
#include <chrono>
#include "headers/Evaluation.h"

int main() {
    Board board;
    TranspositionTable tt(256);
    initLeapersAttacks();
    initSliderAttacks(bishop);
    initSliderAttacks(rook);
    initRandomKeys();
    initEvaluationMasks();

    uciLoop(board, tt);

    /*board.parseFEN("rnbqkbnr/pp1ppppp/8/2p5/4P3/2N5/PPPP1PPP/R1BQKBNR b KQkq - 0 1");
    board.printBoard();*/

    /*board.parseFEN(startPosition);
    board.printBoard();
    while (true) {
        std::string moveString = " ";

        std::cin >> moveString;

        int move = parseMove(moveString, board);

        if (move && makeMove(move, allMoves, board))
            board.printBoard();
        else
            std::cout << "illegal move, try again!:\n";
    }*/
}