#pragma once
#include "BoardRep.h"
#include "Move.h"
#include "Transposition.h"

bool isRepetition(Board& board);

int quiescence(int alpha, int beta, Board& board, TranspositionTable& tt);

int negaMax(int alpha, int beta, int depth, Board& board, TranspositionTable& tt);

// Search current position for the best move
void searchPosition(int depth, Board& board, TranspositionTable& tt);

void sortMoves(MoveList& moveList);