#pragma once
#include "BoardRep.h"

int evaluate(Board& board);

int materialScore(int color, const Board& board);

int pieceCountWithoutKingPawn(int color, const Board& board);

int forceKingToCorner(Board& board, int color);

int pastPawnsBonus(Board& board, int color);