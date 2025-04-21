#pragma once
#include "HelperVariables.h"
#include "BoardRep.h"

/*
             binary move rep                            hex dec constants
    0000 0000 0000 0000 0011 1111   start square        0x3f
    0000 0000 0000 1111 1100 0000   target square       0xfc0
    0000 0000 1111 0000 0000 0000   piece               0xf000
    0000 1111 0000 0000 0000 0000   promoted piece      0xf0000
    0001 0000 0000 0000 0000 0000   capture flag        0x100000
    0010 0000 0000 0000 0000 0000   double push flag    0x200000
    0100 0000 0000 0000 0000 0000   enpassant flag      0x400000
    1000 0000 0000 0000 0000 0000   castling flag       0x800000
*/
#define encodeMove(startSquare, targetSquare, piece, promotedPiece, capture, doublePush, enPassant, castling) \
(startSquare) | (targetSquare << 6) | (piece << 12) | (promotedPiece << 16) | (capture << 20) | (doublePush << 21) | (enPassant << 22) | (castling << 23) \

#define getMoveStart(move) (move & 0x3f)
#define getMoveTarget(move) ((move & 0xfc0) >> 6)
#define getMovePiece(move) ((move & 0xf000) >> 12)
#define getMovePromoted(move) ((move & 0xf0000) >> 16)
#define getMoveCapture(move) (move & 0x100000)
#define getMoveDouble(move) (move & 0x200000)
#define getMoveEnPassant(move) (move & 0x400000)
#define getMoveCastling(move) (move & 0x800000)

struct MoveList {
    int moves[256];
    int count = 0;
};

void removeMove(MoveList& moveList, int removeIndex);
void addMove(MoveList& moveList, int move);
void printMove(int move);
void printMoveList(MoveList& moveList);
bool makeMove(int move, MoveType moveFlag, Board& board);