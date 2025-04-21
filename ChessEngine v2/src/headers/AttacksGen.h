#pragma once
#include <iostream>
#include "HelperVariables.h"
#include "BoardRep.h"

// pawn attack table [color][square]
extern uint64_t pawnAttacks[2][64];

// knight attack table [square]
extern uint64_t knightAttacks[64];

// king attack table [square]
extern uint64_t kingAttacks[64];

// bishop attacks masks
extern uint64_t bishopMask[64];

// rook attacks masks
extern uint64_t rookMask[64];

// bishop attack table [square][occupancies]
extern uint64_t bishopAttacks[64][512];

// rook attack table [square][occupancies]
extern uint64_t rookAttacks[64][4096];

uint64_t maskPawnAttacks(int color, int square);
uint64_t maskKnightAttacks(int square);
uint64_t maskKingAttacks(int square);
uint64_t maskBishopAttacks(int square);
uint64_t maskRookAttacks(int square);

// generate bishop attacks on the fly
uint64_t bishopAttacksOnTheFly(int square, uint64_t block);

// generate rook attacks on the fly
uint64_t rookAttacksOnTheFly(int square, uint64_t block);

void initLeapersAttacks();
void initSliderAttacks(SlidingPieceType bishopFlag);

// get bishop attacks
uint64_t getBishopAttacks(int square, uint64_t occupancy);

// get rook attacks
uint64_t getRookAttacks(int square, uint64_t occupancy);

// get queen attacks
uint64_t getQueenAttacks(int square, uint64_t occupancy);

// is current given square attacked by the given color
bool isSquareAttacked(int square, int color, const Board& board);

// print attacked squares
void printAttackedSquares(Color color, const Board& board);