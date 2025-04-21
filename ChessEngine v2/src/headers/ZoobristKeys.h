#pragma once
#include "BoardRep.h"
#include <cstdint>

// random piece keys [piece][square]
extern uint64_t pieceKeys[12][64];

// random enpassant keys [square]
extern uint64_t enpassantKeys[64];

// random castling keys
extern uint64_t castleKeys[16];

// random side key
extern uint64_t sideKey;

// init random hash keys
void initRandomKeys();

// generate "almost" unique position ID aka hash key from scratch
uint64_t generateHashKey(Board& board);