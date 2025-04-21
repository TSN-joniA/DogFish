#pragma once
#include <iostream>

#define setBit(bitboard, square) ((bitboard) |= (1ULL << (square)))
#define getBit(bitboard, square) ((bitboard) & (1ULL << (square)))
#define popBit(bitboard, square) ((bitboard) &= ~(1ULL << (square)))


int getLSBIndex(uint64_t bitboard);
int countBits(uint64_t bitboard);

uint64_t setOccupancy(int index, int bitsInMask, uint64_t attackMask);
void printBitboard(uint64_t bitboard);