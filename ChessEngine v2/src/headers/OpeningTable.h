#pragma once
#include <cstdint>
#include <unordered_map>

/**       Currently not fully implemented       **/

struct OpeningMove {
    int move;  // Encoded move
    int weight;  // Weight or popularity of the move
};

class OpeningTable {
public:
    // Add a new entry to the opening table
    void addEntry(uint64_t zobristKey, int move, int weight = 1);

    // Retrieve a move from the opening table if available
    bool getMove(uint64_t zobristKey, int& selectedMove);

private:
    std::unordered_map<uint64_t, std::vector<OpeningMove>> openingBook; // uint64_t is the zobrist key for the position

private: 
    // Choose a move randomly based on weights (more weight = higher chance)
    int chooseMove(const std::vector<OpeningMove>& moves);
};