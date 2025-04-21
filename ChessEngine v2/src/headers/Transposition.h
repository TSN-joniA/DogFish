#pragma once
#include <cstdint>
#include <vector>

enum EntryType {
    EXACT,  // Exact evaluation
    ALPHA,  // Lower bound
    BETA    // Upper bound
};

struct TranspositionTableEntry {
    uint64_t zobristKey;  // Zobrist hash of the position
    int score;            // Evaluation score
    int depth;            // Depth of search
    EntryType type;       // Exact, Alpha, or Beta score
    int bestMove;         // Optional: Best move found at this position
};

class TranspositionTable {
public:
    std::vector<TranspositionTableEntry> table;
    size_t size;  // Size of the table
    size_t filledEntries = 0;  // Counter for filled entries

public:
    TranspositionTable(size_t sizeMB);

    void store(uint64_t zobristKey, int depth, int score, EntryType type, int bestMove);

    bool probe(uint64_t zobristKey, int depth, int alpha, int beta, int& score);

    double getFilledPercentage() const;
    void clear();
};
