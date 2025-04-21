#include "headers/HelperFunctions.h"

int getLSBIndex(uint64_t bitboard) {
    if (bitboard == 0)
        return -1; // Return -1 if there is no set bit

    int index = 0;
    while ((bitboard & 1) == 0) {
        bitboard >>= 1;
        index++;
    }
    return index;
}

int countBits(uint64_t bitboard) {
    int count = 0;

    // consecutively reset least significant 1st bit
    while (bitboard) {
        count++;
        bitboard &= bitboard - 1;
    }

    return count;
}

uint64_t setOccupancy(int index, int bitsInMask, uint64_t attackMask) {
    // occupancy map
    uint64_t occupancy = 0ULL;

    // loop over the range of bits within attack mask
    for (int count = 0; count < bitsInMask; count++) {
        // get LSB index of attack mask
        int square = getLSBIndex(attackMask);

        //pop LSB in attack mask
        popBit(attackMask, square);

        //make sure occupancy is on board
        if (index & 1 << count)
            // populate occupancy map
            occupancy |= (1ULL << square);
    }
    return occupancy;
}

void printBitboard(uint64_t bitboard) {
    for (int rank = 0; rank < 8; rank++) {
        std::cout << " " << 8 - rank << "   ";
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            std::cout << ((bitboard >> square) & 1) << " ";
        }
        std::cout << '\n';
    }
    std::cout << "\n     a b c d e f g h\n\n";
    std::cout << " Value: " << bitboard << "\n\n";
}