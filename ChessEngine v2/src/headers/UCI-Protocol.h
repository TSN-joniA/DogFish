#pragma once
#include "BoardRep.h"
#include "Transposition.h"

// parse user/GUI move string input (e.g. "f7f8q")
int parseMove(std::string moveString, const Board& board);

// parse UCI "position" command
void parsePosition(const std::string& command, Board& board, TranspositionTable& tt);

// parse UCI "go" command
void parseGo(const std::string& command, Board& board, TranspositionTable& tt);

// main UCI loop 
void uciLoop(Board& board, TranspositionTable& tt);