#include "headers/RandomNumberGen.h"

//generate 32 bit pseudo random number
unsigned int RandNumGen::getRandU32Num() {
    unsigned int number = randomState;

    // XOR shift algorithm
    number ^= number << 13;
    number ^= number >> 17;
    number ^= number << 5;

    // update random number state
    randomState = number;

    return number;
}

//generate 32 bit pseudo random number
uint64_t RandNumGen::getRandU64Num() {
    // define 4 random numbers
    uint64_t n1, n2, n3, n4;
    n1 = (uint64_t)(getRandU32Num()) & 0xFFFF;
    n2 = (uint64_t)(getRandU32Num()) & 0xFFFF;
    n3 = (uint64_t)(getRandU32Num()) & 0xFFFF;
    n4 = (uint64_t)(getRandU32Num()) & 0xFFFF;

    return n1 | (n2 << 16) | (n3 << 32) | (n4 << 48);
}