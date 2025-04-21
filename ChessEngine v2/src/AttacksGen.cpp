#include "headers/AttacksGen.h"
#include "headers/HelperFunctions.h"
#include "headers/MagicNumbers.h"

uint64_t pawnAttacks[2][64];
uint64_t knightAttacks[64];
uint64_t kingAttacks[64];
uint64_t bishopMask[64];
uint64_t rookMask[64]; 
uint64_t bishopAttacks[64][512];
uint64_t rookAttacks[64][4096];

uint64_t maskPawnAttacks(int color, int square) {
    uint64_t bitboard = 0ULL;
    uint64_t attacks = 0ULL;
    setBit(bitboard, square);

    if (!color) {
        if ((bitboard >> 7) & NOT_A_FILE) attacks |= bitboard >> 7;
        if ((bitboard >> 9) & NOT_H_FILE) attacks |= bitboard >> 9;
    }
    else {
        if ((bitboard << 7) & NOT_H_FILE) attacks |= bitboard << 7;
        if ((bitboard << 9) & NOT_A_FILE) attacks |= bitboard << 9;
    }

    return attacks;
}

uint64_t maskKnightAttacks(int square) {
    uint64_t bitboard = 0ULL;
    uint64_t attacks = 0ULL;
    setBit(bitboard, square);

    if (bitboard >> 17 & NOT_H_FILE) attacks |= bitboard >> 17;
    if (bitboard >> 15 & NOT_A_FILE) attacks |= bitboard >> 15;
    if (bitboard >> 10 & NOT_HG_FILE) attacks |= bitboard >> 10;
    if (bitboard >> 6 & NOT_AB_FILE) attacks |= bitboard >> 6;

    if (bitboard << 17 & NOT_A_FILE) attacks |= bitboard << 17;
    if (bitboard << 15 & NOT_H_FILE) attacks |= bitboard << 15;
    if (bitboard << 10 & NOT_AB_FILE) attacks |= bitboard << 10;
    if (bitboard << 6 & NOT_HG_FILE) attacks |= bitboard << 6;

    return attacks;
}

uint64_t maskKingAttacks(int square) {
    uint64_t bitboard = 0ULL;
    uint64_t attacks = 0ULL;
    setBit(bitboard, square);

    if (bitboard >> 8) attacks |= (bitboard >> 8);
    if ((bitboard >> 9) & NOT_H_FILE) attacks |= (bitboard >> 9);
    if ((bitboard >> 7) & NOT_A_FILE) attacks |= (bitboard >> 7);
    if ((bitboard >> 1) & NOT_H_FILE) attacks |= (bitboard >> 1);
    if (bitboard << 8) attacks |= (bitboard << 8);
    if ((bitboard << 9) & NOT_A_FILE) attacks |= (bitboard << 9);
    if ((bitboard << 7) & NOT_H_FILE) attacks |= (bitboard << 7);
    if ((bitboard << 1) & NOT_A_FILE) attacks |= (bitboard << 1);

    return attacks;
}

uint64_t maskBishopAttacks(int square) {
    uint64_t attacks = 0ULL;

    // init ranks & files
    int r, f;

    // init target ranks & files
    int tr = square / 8;
    int tf = square % 8;

    // mask relevant bishop occupancy bits
    for (r = tr + 1, f = tf + 1; r < 7 && f < 7; r++, f++) attacks |= 1ULL << (r * 8 + f);
    for (r = tr - 1, f = tf + 1; r > 0 && f < 7; r--, f++) attacks |= 1ULL << (r * 8 + f);
    for (r = tr - 1, f = tf - 1; r > 0 && f > 0; r--, f--) attacks |= 1ULL << (r * 8 + f);
    for (r = tr + 1, f = tf - 1; r < 7 && f > 0; r++, f--) attacks |= 1ULL << (r * 8 + f);

    return attacks;
}

uint64_t maskRookAttacks(int square) {
    uint64_t attacks = 0ULL;

    // init ranks & files
    int r, f;

    // init target ranks & files
    int tr = square / 8;
    int tf = square % 8;

    // mask relevant rook occupancy bits
    for (r = tr + 1; r < 7; r++) attacks |= 1ULL << (r * 8 + tf);
    for (f = tf + 1; f < 7; f++) attacks |= 1ULL << (tr * 8 + f);
    for (r = tr - 1; r > 0; r--) attacks |= 1ULL << (r * 8 + tf);
    for (f = tf - 1; f > 0; f--) attacks |= 1ULL << (tr * 8 + f);

    return attacks;
}

// generate bishop attacks on the fly
uint64_t bishopAttacksOnTheFly(int square, uint64_t block) {
    uint64_t attacks = 0ULL;

    // init ranks & files
    int r, f;

    // init target ranks & files
    int tr = square / 8;
    int tf = square % 8;

    for (r = tr + 1, f = tf + 1; r < 8 && f < 8; r++, f++) {
        attacks |= 1ULL << (r * 8 + f);
        if (1ULL << (r * 8 + f) & block) break;
    }
    for (r = tr - 1, f = tf + 1; r >= 0 && f < 8; r--, f++) {
        attacks |= 1ULL << (r * 8 + f);
        if (1ULL << (r * 8 + f) & block) break;
    }
    for (r = tr - 1, f = tf - 1; r >= 0 && f >= 0; r--, f--) {
        attacks |= 1ULL << (r * 8 + f);
        if (1ULL << (r * 8 + f) & block) break;
    }
    for (r = tr + 1, f = tf - 1; r < 8 && f >= 0; r++, f--) {
        attacks |= 1ULL << (r * 8 + f);
        if (1ULL << (r * 8 + f) & block) break;
    }

    return attacks;
}

// generate rook attacks on the fly
uint64_t rookAttacksOnTheFly(int square, uint64_t block) {
    uint64_t attacks = 0ULL;

    // init ranks & files
    int r, f;

    // init target ranks & files
    int tr = square / 8;
    int tf = square % 8;

    for (r = tr + 1; r < 8; r++) {
        attacks |= 1ULL << (r * 8 + tf);
        if (1ULL << (r * 8 + tf) & block) break;
    }
    for (f = tf + 1; f < 8; f++) {
        attacks |= 1ULL << (tr * 8 + f);
        if (1ULL << (tr * 8 + f) & block) break;
    }
    for (r = tr - 1; r >= 0; r--) {
        attacks |= 1ULL << (r * 8 + tf);
        if (1ULL << (r * 8 + tf) & block) break;
    }
    for (f = tf - 1; f >= 0; f--) {
        attacks |= 1ULL << (tr * 8 + f);
        if (1ULL << (tr * 8 + f) & block) break;
    }

    return attacks;
}

void initLeapersAttacks() {
    for (int square = 0; square < 64; square++) {
        pawnAttacks[white][square] = maskPawnAttacks(white, square);
        pawnAttacks[black][square] = maskPawnAttacks(black, square);

        knightAttacks[square] = maskKnightAttacks(square);

        kingAttacks[square] = maskKingAttacks(square);
    }
}

// init slider pieces attack tables
void initSliderAttacks(SlidingPieceType bishopFlag) {
    for (int square = 0; square < 64; square++) {
        // init bishop and rook mask
        bishopMask[square] = maskBishopAttacks(square);
        rookMask[square] = maskRookAttacks(square);

        // init current mask
        uint64_t attackMask = (bishopFlag == bishop) ? bishopMask[square] : rookMask[square];

        // init relevant occupancy bit count
        int relevantBitsCount = countBits(attackMask);

        // init occupancy indecies
        int occupancyIndecies = 1 << relevantBitsCount;

        // loop over occupancy indecies
        for (int i = 0; i < occupancyIndecies; i++) {
            if (bishopFlag == bishop) {
                uint64_t occupancy = setOccupancy(i, relevantBitsCount, attackMask);

                // init magic index
                int magicIndex = (occupancy * bishopMagicNumbers[square]) >> (64 - bishopRelevantBits[square]);

                // init bishop attacks
                bishopAttacks[square][magicIndex] = bishopAttacksOnTheFly(square, occupancy);
            }
            else {
                uint64_t occupancy = setOccupancy(i, relevantBitsCount, attackMask);

                // init magic index
                int magicIndex = (occupancy * rookMagicNumbers[square]) >> (64 - rookRelevantBits[square]);

                // init bishop attacks
                rookAttacks[square][magicIndex] = rookAttacksOnTheFly(square, occupancy);
            }
        }
    }
}

// get bishop attacks
uint64_t getBishopAttacks(int square, uint64_t occupancy) {
    occupancy &= bishopMask[square];
    occupancy *= bishopMagicNumbers[square];
    occupancy >>= 64 - bishopRelevantBits[square];

    return bishopAttacks[square][occupancy];
}

// get rook attacks
uint64_t getRookAttacks(int square, uint64_t occupancy) {
    occupancy &= rookMask[square];
    occupancy *= rookMagicNumbers[square];
    occupancy >>= 64 - rookRelevantBits[square];

    return rookAttacks[square][occupancy];
}

// get queen attacks
uint64_t getQueenAttacks(int square, uint64_t occupancy) {

    uint64_t queenAttacks = 0ULL;
    uint64_t bishopOccupancies = occupancy;
    uint64_t rookOccupancies = occupancy;

    bishopOccupancies &= bishopMask[square];
    bishopOccupancies *= bishopMagicNumbers[square];
    bishopOccupancies >>= 64 - bishopRelevantBits[square];

    queenAttacks = bishopAttacks[square][bishopOccupancies];

    rookOccupancies &= rookMask[square];
    rookOccupancies *= rookMagicNumbers[square];
    rookOccupancies >>= 64 - rookRelevantBits[square];

    queenAttacks |= rookAttacks[square][rookOccupancies];

    return queenAttacks;
}

// is current given square attacked by the given color
bool isSquareAttacked(int square, int color, const Board& board) {

    if ((color == white) && (pawnAttacks[black][square] & board.bitboards[P])) return true; // attacked by white pawns

    if ((color == black) && (pawnAttacks[white][square] & board.bitboards[p])) return true; // attacked by black pawns

    if (knightAttacks[square] & ((color == white) ? board.bitboards[N] : board.bitboards[n])) return true; // attacked by knights

    if (kingAttacks[square] & ((color == white) ? board.bitboards[K] : board.bitboards[k])) return true; // attacked by kings

    if (getBishopAttacks(square, board.ocupancies[both]) & ((color == white) ? board.bitboards[B] : board.bitboards[b])) return 1; // attacked by bishops

    if (getRookAttacks(square, board.ocupancies[both]) & ((color == white) ? board.bitboards[R] : board.bitboards[r])) return 1; // attacked by rooks

    if (getQueenAttacks(square, board.ocupancies[both]) & ((color == white) ? board.bitboards[Q] : board.bitboards[q])) return 1; // attacked by bishops

    return false;
}

// print attacked squares
void printAttackedSquares(Color color, const Board& board) {
    std::cout << "\n\n";

    for (int rank = 0; rank < 8; rank++) {
        std::cout << " " << (8 - rank) << "   ";
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;

            // check whether current square is attacked or not
            std::cout << (isSquareAttacked(square, color, board) ? 1 : 0) << ' ';
        }
        std::cout << '\n';
    }
    std::cout << "\n     a b c d e f g h\n";
}