#include "headers/ZoobristKeys.h"
#include "headers/RandomNumberGen.h"
#include "headers/BoardRep.h"
#include "headers/HelperFunctions.h"
#include "headers/HelperVariables.h"

uint64_t pieceKeys[12][64];

uint64_t enpassantKeys[64];

uint64_t castleKeys[16];

uint64_t sideKey;

void initRandomKeys() {
    RandNumGen randNumGen;
    // update pseudo random number state (seed)
    randNumGen.randomState = 1804289383;

    for (int piece = P; piece <= k; piece++) {
        for (int square = 0; square < 64; square++)
            pieceKeys[piece][square] = randNumGen.getRandU64Num();
    }

    for (int square = 0; square < 64; square++)
        enpassantKeys[square] = randNumGen.getRandU64Num();

    // loop over castling keys
    for (int index = 0; index < 16; index++)
        castleKeys[index] = randNumGen.getRandU64Num();

    // init random side key
    sideKey = randNumGen.getRandU64Num();
}

uint64_t generateHashKey(Board& board) {
    // final hash key
    uint64_t finalKey = 0ULL;

    // temp piece bitboard copy
    uint64_t bitboard;

    // loop over piece bitboards
    for (int piece = P; piece <= k; piece++) {
        bitboard = board.bitboards[piece];

        // loop over the pieces within a bitboard
        while (bitboard) {
            int square = getLSBIndex(bitboard);

            // hash piece
            finalKey ^= pieceKeys[piece][square];

            // pop LS1B
            popBit(bitboard, square);
        }
    }

    // if enpassant square is on board
    if (board.enpassant != noSQ)
        // hash enpassant
        finalKey ^= enpassantKeys[board.enpassant];

    // hash castling rights
    finalKey ^= castleKeys[board.castle];

    // hash the colorToMove only if black is to move
    if (board.colorToMove == black) finalKey ^= sideKey;

    // return generated hash key
    return finalKey;
}