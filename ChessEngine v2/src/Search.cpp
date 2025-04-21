#include "headers/Search.h"
#include "headers/Move.h"
#include "headers/MoveGenerator.h"
#include "headers/Evaluation.h"
#include "headers/AttacksGen.h"
#include "headers/HelperFunctions.h"
#include <iostream>

// half move counter
int ply;

// best move
int bestMove;

// position repetition detection
bool isRepetition(Board& board) {
    for (int i = 0; i < repetitionIndex; i++) {
        if (repetitionTable[i] == board.hashKey)
            return true;  
    }
    return false;
}

// quiescence search
int quiescence(int alpha, int beta, Board& board, TranspositionTable& tt) {
    uint64_t zobristKey = board.hashKey;

    // Probe the transposition table
    int ttScore;
    if (tt.probe(zobristKey, 0, alpha, beta, ttScore)) {
        return ttScore;
    }

    int evaluation = evaluate(board);

    // fail-hard beta cutoff
    if (evaluation >= beta) {
        tt.store(zobristKey, 0, beta, BETA, 0);
        // node (move) fails high
        return beta;
    }

    // found a better move
    if (evaluation > alpha) {
        // PV node (move)
        alpha = evaluation;
    }

    MoveList moveList;
    generateMoves(moveList, board);
    sortMoves(moveList);

    // loop over moves within a movelist
    for (int count = 0; count < moveList.count; count++) {
        copyBoard(board);
        ply++;
        repetitionIndex++;
        repetitionTable[repetitionIndex] = board.hashKey;

        // make sure to make only legal moves
        if (!makeMove(moveList.moves[count], captureMoves, board)) {
            ply--;
            repetitionIndex--;
            continue;
        }

        // score current move
        int score = -quiescence(-beta, -alpha, board, tt);
        ply--;
        repetitionIndex--;
        restoreBoard(board);

        // fail-hard beta cutoff
        if (score >= beta) {
            tt.store(zobristKey, 0, beta, BETA, 0);
            // node (move) fails high
            return beta;
        }

        // found a better move
        if (score > alpha) {
            // PV node (move)
            alpha = score;
        }
    }

    // node (move) fails low
    tt.store(zobristKey, 0, alpha, EXACT, 0);
    return alpha;
}

// negamax alpha beta search
int negaMax(int alpha, int beta, int depth, Board& board, TranspositionTable& tt) {
    uint64_t zobristKey = board.hashKey;

    // Probe the transposition table
    int ttScore;
    if (tt.probe(zobristKey, depth, alpha, beta, ttScore))
        return ttScore;

    if (ply && isRepetition(board))
        return 0;

    if (depth == 0)
        return quiescence(alpha, beta, board, tt);

    int bestSoFar = 0;

    // old value of alpha
    int oldAlpha = alpha;

    bool isCheck = isSquareAttacked(getLSBIndex(board.bitboards[board.colorToMove == white ? K : k]), board.colorToMove ^ 1, board);
    int legalMoveCounter = 0;

    MoveList moveList;
    generateMoves(moveList, board);
    sortMoves(moveList);

    // loop over moves within a movelist
    for (int count = 0; count < moveList.count; count++) {
        copyBoard(board);

        ply++;
        repetitionIndex++;
        repetitionTable[repetitionIndex] = board.hashKey;

        // make sure to make only legal moves
        if (!makeMove(moveList.moves[count], allMoves, board)) {
            ply--;
            repetitionIndex--;
            continue;
        }
        legalMoveCounter++;
        int score = -negaMax(-beta, -alpha, depth - 1, board, tt);

        ply--;
        repetitionIndex--;

        restoreBoard(board);

        // fail-hard beta cutoff
        if (score >= beta) {
            tt.store(zobristKey, depth, beta, BETA, moveList.moves[count]);
            // node (move) fails high
            return beta;
        }

        // found a better move
        if (score > alpha) {
            // PV node (move)
            alpha = score;

            // if root move
            if (ply == 0)
                bestSoFar = moveList.moves[count];
        }
    }

    if (legalMoveCounter == 0) {
        if (isCheck)
            return -49000 + ply;
        else
            return 0;
    }

    // found better move
    if (oldAlpha != alpha)
        bestMove = bestSoFar;

    // Store the result in the transposition table
    EntryType type = (alpha > oldAlpha) ? EXACT : ALPHA;  // If improved alpha, it's an EXACT entry
    tt.store(zobristKey, depth, alpha, type, bestSoFar);

    // node (move) fails low
    return alpha;
}

void searchPosition(int depth, Board& board, TranspositionTable& tt) {
    int score = negaMax(-50000, 50000, depth, board, tt);
	std::cout << "bestmove ";
	printMove(bestMove);
    std::cout << "score: " << score << '\n';
}

void sortMoves(MoveList& moveList) {
    MoveList sortedMoveList;
    for (int i = 0; i < moveList.count; i++) {
        if (getMoveCapture(moveList.moves[i]))
            addMove(sortedMoveList, moveList.moves[i]);
    }
    for (int i = 0; i < moveList.count; i++) {
        if (!getMoveCapture(moveList.moves[i]))
            addMove(sortedMoveList, moveList.moves[i]);
    }
    moveList = sortedMoveList;
}