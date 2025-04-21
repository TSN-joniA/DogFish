#include "headers/Perft.h"
#include "headers/MoveGenerator.h"

long long nodes = 0;

void perftDriver(int depth, Board& board) {
    if (depth == 0) {
        nodes++;
        return;
    }

    MoveList moveList;
    generateMoves(moveList, board);

    for (int moveCount = 0; moveCount < moveList.count; moveCount++) {
        copyBoard(board);

        if (!makeMove(moveList.moves[moveCount], allMoves, board))
            continue;

        perftDriver(depth - 1, board);

        restoreBoard(board);
    }
    return;
}