#include "headers/OpeningTable.h"
#include "headers/BoardRep.h"
#include "headers/ZoobristKeys.h"
#include "headers/Move.h"

void OpeningTable::addEntry(uint64_t zobristKey, int move, int weight) {
    openingBook[zobristKey].push_back({ move, weight });
}

bool OpeningTable::getMove(uint64_t zobristKey, int& selectedMove) {
    auto it = openingBook.find(zobristKey);

    if (it != openingBook.end() && !it->second.empty()) {
        const std::vector<OpeningMove>& moves = it->second;
        selectedMove = chooseMove(moves);
        return true;
    }

    return false;
}

int OpeningTable::chooseMove(const std::vector<OpeningMove>& moves) {
    int totalWeight = 0;
    for (const auto& move : moves) {
        totalWeight += move.weight;
    }

    int randValue = rand() % totalWeight;
    int cumulativeWeight = 0;

    for (const auto& move : moves) {
        cumulativeWeight += move.weight;
        if (randValue < cumulativeWeight) {
            return move.move;
        }
    }

    return moves.front().move;  // Default fallback
}

void initOpeningTable(OpeningTable& openingTable) {
    openingTable.addEntry(7986424829486667136ULL, encodeMove(e2, e4, P, 0, 0, 1, 0, 0), 10);
    openingTable.addEntry(7986424829486667136ULL, encodeMove(d2, d4, P, 0, 0, 1, 0, 0), 9);
    openingTable.addEntry(7986424829486667136ULL, encodeMove(c2, c4, P, 0, 0, 1, 0, 0), 8);

    // sicilian
    openingTable.addEntry(16365716498592173011ULL, encodeMove(c7, c5, p, 0, 0, 1, 0, 0), 9);
    openingTable.addEntry(3031954926653116807ULL, encodeMove(b1, c3, N, 0, 0, 0, 0, 0), 10);
    openingTable.addEntry(3031954926653116807ULL, encodeMove(g1, f3, N, 0, 0, 0, 0, 0), 10);
    openingTable.addEntry(15817592729820126741ULL, encodeMove(b8, c5, n, 0, 0, 0, 0, 0), 8);
}