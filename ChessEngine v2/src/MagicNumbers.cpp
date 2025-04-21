#include "headers/MagicNumbers.h"
#include "headers/HelperFunctions.h"
#include "headers/AttacksGen.h"

uint64_t findMagicNumber(int square, int relevantBits, SlidingPieceType bishopFlag) {
    uint64_t occupancies[4096];

    // init attack tables
    uint64_t attacks[4096];

    // init used attacks
    uint64_t usedAttacks[4096];

    // init attack mask for current piece
    uint64_t attackMask = (bishopFlag == bishop) ? maskBishopAttacks(square) : maskRookAttacks(square);

    // init occupancy indecies
    int occupancyIndecies = 1 << relevantBits;

    // loop over occupancy indecies
    for (int i = 0; i < occupancyIndecies; i++) {
        // init occupancies
        occupancies[i] = setOccupancy(i, relevantBits, attackMask);

        attacks[i] = (bishopFlag == bishop) ? bishopAttacksOnTheFly(square, occupancies[i]) : rookAttacksOnTheFly(square, occupancies[i]);
    }

    // test magic numbers loop
    for (int randomCount = 0; randomCount < 100000000; randomCount++) {
        uint64_t magicNumber = generateMagicNumber();

        // skip inappropriate magic numbers
        if (countBits((attackMask * magicNumber & 0xFF00000000000000)) < 6) continue;

        // init used attacks
        memset(usedAttacks, 0ULL, sizeof(usedAttacks));

        // init index & fail flag
        int index, fail;

        // test magic index loop
        for (index = 0, fail = 0; !fail && index < occupancyIndecies; index++) {
            // init magic index
            int magicIndex = (int)((occupancies[index] * magicNumber) >> (64 - relevantBits));

            // on empty index available
            if (usedAttacks[magicIndex] == 0ULL)
                usedAttacks[magicIndex] = attacks[index];
            else if (usedAttacks[magicIndex] != attacks[index])
                fail = 1;
        }
        if (!fail)
            return magicNumber;
    }
    return 0;
}