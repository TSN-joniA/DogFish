#pragma once
#include <iostream>

class RandNumGen {
public:
	unsigned int getRandU32Num();
	uint64_t getRandU64Num();
	unsigned int randomState = 1804289383;
};