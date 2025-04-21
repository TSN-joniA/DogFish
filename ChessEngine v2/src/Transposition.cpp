#include "headers/Transposition.h"

TranspositionTable::TranspositionTable(size_t sizeMB) {
	size = (sizeMB * 1024 * 1024) / sizeof(TranspositionTableEntry);
	table.resize(size);
}

void TranspositionTable::store(uint64_t zobristKey, int depth, int score, EntryType type, int bestMove) {
    size_t index = zobristKey % size;

    // If we're storing a new entry or replacing an older entry with lower depth
    if (table[index].zobristKey == 0)
        filledEntries++;  // Increment filled entries if it's a new entry

    // Replace the entry if the new depth is greater or equal (deeper search is more valuable)
    if (table[index].depth <= depth)
        table[index] = { zobristKey, score, depth, type, bestMove };
}

bool TranspositionTable::probe(uint64_t zobristKey, int depth, int alpha, int beta, int& score) {
    size_t index = zobristKey % size;
    const auto& entry = table[index];
    if (entry.zobristKey == zobristKey) {
        // Check depth to ensure the stored evaluation is valid for the current search
        if (entry.depth >= depth) {
            if (entry.type == EXACT) {
                score = entry.score;
                return true;
            }
            else if (entry.type == ALPHA && entry.score <= alpha) {
                score = alpha;
                return true;
            }
            else if (entry.type == BETA && entry.score >= beta) {
                score = beta;
                return true;
            }
        }
    }
    return false;
}

double TranspositionTable::getFilledPercentage() const {
    return (static_cast<double>(filledEntries) / size) * 100.0;
}

void TranspositionTable::clear() {
    table.clear();
    table.resize(size);
    filledEntries = 0;
}
