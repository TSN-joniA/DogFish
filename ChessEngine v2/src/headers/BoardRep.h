#pragma once
#include <iostream>
#include <vector>
#include "HelperVariables.h"

extern uint64_t repetitionTable[150];
extern int repetitionIndex;

class Board {
public:
	// define bitboards
	uint64_t bitboards[12];

	// define occupancy bitboards
	uint64_t ocupancies[3];

	// color to move
	int colorToMove = 0;

	// enpassant square
	int enpassant = noSQ;

	// castling rights
	int castle = 0;

	// "almost" unique hash key
	uint64_t hashKey = 0;

	Board() {
		memset(bitboards, 0, sizeof(bitboards));
		memset(ocupancies, 0, sizeof(ocupancies));
		repetitionIndex++;
		repetitionTable[repetitionIndex] = hashKey;
	}

public:
	void printBoard();
	void parseFEN(const char* fen);
};

// preserve board state
#define copyBoard(board)																			  \
    uint64_t bitboardsCopy[12], occupanciesCopy[3], hashKeyCopy;									  \
    int colorToMoveCopy, enPassantCopy, castleCopy;													  \
    memcpy(bitboardsCopy, board.bitboards, 96);														  \
    memcpy(occupanciesCopy, board.ocupancies, 24);													  \
    colorToMoveCopy = board.colorToMove, enPassantCopy = board.enpassant, castleCopy = board.castle;  \
	hashKeyCopy = board.hashKey;																	  \

// restore board state
#define restoreBoard(board)                                                                           \
    memcpy(board.bitboards, bitboardsCopy, 96);                                                       \
    memcpy(board.ocupancies, occupanciesCopy, 24);                                                    \
    board.colorToMove = colorToMoveCopy, board.enpassant = enPassantCopy, board.castle = castleCopy;  \
	board.hashKey = hashKeyCopy;																	  