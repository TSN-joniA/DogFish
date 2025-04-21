#include "headers/Evaluation.h"
#include "headers/HelperFunctions.h"

const int pawnValue = 200;
const int knightValue = 650;
const int bishopValue = 700;
const int rookValue = 1000;
const int queenValue = 1950;
const int kingValue = 10000;

const int pawnScore[64] = {
	90,  90,  90,  90,  90,  90,  90,  90,
	50,  50,  50,  50,  50,  50,  50,  50,
	10,  10,  20,  30,  30,  20,  10,  10,
	 5,   5,  10,  25,  25,  10,   5,   5,
	 0,   0,   0,  20,  20,   0,   0,   0,
	 5,  -5, -10,   0,   0, -10,  -5,   5,
	 5,  10,  10, -20, -20,  10,  10,   5,
	 0,   0,   0,   0,   0,   0,   0,   0
};

const int endgamePawnScore[64] = {
    90,  90,  90,  90,  90,  90,  90,  90,
    80,  80,  80,  80,  80,  80,  80,  80,
    50,  50,  50,  50,  50,  50,  50,  50,
    30,  30,  30,  30,  30,  30,  30,  30,
    20,  20,  20,  20,  20,  20,  20,  20,
    10,  10,  10,  10,  10,  10,  10,  10,
    10,  10,  10,  10,  10,  10,  10,  10,
     0,   0,   0,   0,   0,   0,   0,   0
};

// knight positional score
const int knightScore[64] = {
    -50, -40, -30, -30, -30, -30, -40, -50,
    -40, -20,   0,   0,   0,   0, -20, -40,
    -30,   0,  10,  15,  15,  10,   0, -30,
    -30,   5,  15,  20,  20,  15,   5, -30,
    -30,   0,  15,  20,  20,  15,   0, -30,
    -30,   5,  10,  15,  15,  10,   5, -30,
    -40, -20,   0,   5,   5,   0, -20, -40,
    -50, -40, -30, -30, -30, -30, -40, -50
};

// bishop positional score
const int bishopScore[64] = {
    -20, -10, -10, -10, -10, -10, -10, -20,
    -10,   0,   0,   0,   0,   0,   0, -10,
    -10,   0,   5,  10,  10,   5,   0, -10,
    -10,   5,   5,  10,  10,   5,   5, -10,
    -10,   0,  10,  10,  10,  10,   0, -10,
    -10,  10,  10,  10,  10,  10,  10, -10,
    -10,   5,   0,   0,   0,   0,   5, -10,
    -20, -10, -10, -10, -10, -10, -10, -20,
};

// rook positional score
const int rookScore[64] = {
    50,  50,  50,  50,  50,  50,  50,  50,
    50,  50,  50,  50,  50,  50,  50,  50,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,   0,  20,  20,   5,   0,   0
};

// openeing- and middlegame king positional score
const int openMiddleGameKingScore[64] = {
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -20, -30, -30, -40, -40, -30, -30, -20,
    -10, -20, -20, -20, -20, -20, -20, -10,
     20,  20,   0,   0,   0,   0,  20,  20,
     20,  30,  10,   0,   0,  10,  30,  20
};

// endgame king positional score
const int endGameKingScore[64] = {
     0,   0,   0,   0,   0,   0,   0,   0,
     0,   5,   5,   5,   5,   5,   5,   0,
     0,   5,   5,  10,  10,   5,   5,   0,
     0,   5,  10,  20,  20,  10,   5,   0,
     0,   5,  10,  20,  20,  10,   5,   0,
     0,   5,   5,  10,  10,   5,   5,   0,
     0,   5,   5,   5,   5,   5,   5,   0,
     0,   0,   0,   0,   0,   0,   0,   0
};

const int opponentEndGameKingScores[64] = {
     25,   25,   25,   25,   25,   25,   25,   25,
     25,   -5,   -5,   -5,   -5,   -5,   -5,   25,
     25,   -5,  -10,  -10,  -10,  -10,   -5,   25,
     25,   -5,  -10,  -20,  -20,  -10,   -5,   25,
     25,   -5,  -10,  -20,  -20,  -10,   -5,   25,
     25,   -5,  -10,  -10,  -10,  -10,   -5,   25,
     25,   -5,   -5,   -5,   -5,   -5,   -5,   25,
     25,   25,   25,   25,   25,   25,   25,   25
};

const int mirrorScore[64] = {
    a1, b1, c1, d1, e1, f1, g1, h1,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a8, b8, c8, d8, e8, f8, g8, h8
};

int evaluate(Board& board) {
    bool isEndgameWhite = pieceCountWithoutKingPawn(white, board) <= 2;
    bool isEndgameBlack = pieceCountWithoutKingPawn(black, board) <= 2;

	int whiteEval = materialScore(white, board);
	int blackEval = materialScore(black, board);

    for (int piece = P; piece <= k; piece++) {
        uint64_t currentBitboardCopy = board.bitboards[piece];
        while (currentBitboardCopy) {
            int square = getLSBIndex(currentBitboardCopy);
            switch (piece) {
            case P: whiteEval += isEndgameWhite ? endgamePawnScore[square] // make sure its endgame
                                                : pawnScore[square];
                                                break;
            case N: whiteEval += knightScore[square]; break;
            case B: whiteEval += bishopScore[square]; break;
            case R: whiteEval += rookScore[square];   break;
            case K: whiteEval += isEndgameWhite ? endGameKingScore[square] // make sure its endgame
                                                : openMiddleGameKingScore[square];
                                                break;
            case p: blackEval += isEndgameBlack ? endgamePawnScore[mirrorScore[square]] // make sure its endgame
                                                : pawnScore[mirrorScore[square]];
                                                break;
            case n: blackEval += knightScore[mirrorScore[square]]; break;
            case b: blackEval += bishopScore[mirrorScore[square]]; break;
            case r: blackEval += rookScore[mirrorScore[square]];   break;
            case k: blackEval += isEndgameBlack ? endGameKingScore[mirrorScore[square]] // make sure its endgame
                                                : openMiddleGameKingScore[mirrorScore[square]];
                                                break;
            }
            popBit(currentBitboardCopy, square);
        }
    }

    whiteEval += pastPawnsBonus(board, white);
    blackEval += pastPawnsBonus(board, black);

    if (pieceCountWithoutKingPawn(board.colorToMove, board) <= 2) {
        whiteEval += forceKingToCorner(board, white);
        blackEval += forceKingToCorner(board, black);
    }

	int evaluation = whiteEval - blackEval;
	int perspective = (board.colorToMove == white) ? 1 : -1;

	return evaluation * perspective;
}

int pieceCountWithoutKingPawn(int color, const Board& board) {
    int count = 0;
    count += countBits(board.bitboards[(color == white) ? N : n]);
    count += countBits(board.bitboards[(color == white) ? B : b]);
    count += countBits(board.bitboards[(color == white) ? R : r]);
    count += countBits(board.bitboards[(color == white) ? Q : q]);
    return count;
}

int materialScore(int color, const Board& board) {
	int materialScore = 0;
	materialScore += countBits(board.bitboards[(color == white) ? P : p]) * pawnValue;
	materialScore += countBits(board.bitboards[(color == white) ? N : n]) * knightValue;
	materialScore += countBits(board.bitboards[(color == white) ? B : b]) * bishopValue;
	materialScore += countBits(board.bitboards[(color == white) ? R : r]) * rookValue;
	materialScore += countBits(board.bitboards[(color == white) ? Q : q]) * queenValue;
	materialScore += countBits(board.bitboards[(color == white) ? K : k]) * kingValue;
	return materialScore;
}

int forceKingToCorner(Board& board, int color) {
    int evaluation = opponentEndGameKingScores[getLSBIndex(board.bitboards[color == white ? k : K])];

    int oKingR = getLSBIndex(board.bitboards[k]) / 8;
    int oKingF = getLSBIndex(board.bitboards[k]) % 8;

    int fKingR = getLSBIndex(board.bitboards[K]) / 8;
    int fKingF = getLSBIndex(board.bitboards[K]) % 8;

    int dstBetKingR = std::abs(fKingR - oKingR);
    int dstBetKingF = std::abs(fKingF - oKingF);
    int dstBetKings = dstBetKingR + dstBetKingF;

    evaluation += 14 - dstBetKings;

    return evaluation * 5;
}

int pastPawnsBonus(Board& board, int color) {
    uint64_t pastPawnMask = 0ULL;
    int bonus = 0;
    int whiteRankBonuses[8] = { 70, 60, 50, 40, 30, 20, 10,  0 };
    int blackRankBonuses[8] = {  0, 10, 20, 30, 40, 50, 60, 70 };

    uint64_t aFile = NOT_A_FILE ^ ULLONG_MAX;

    if (color == white) {
        uint64_t currentBitboardCopy = board.bitboards[P];
        while (currentBitboardCopy) {
            int square = getLSBIndex(currentBitboardCopy);
            int rank = square / 8;
            int file = square % 8;

            uint64_t fileMask = aFile << file;
            uint64_t leftFileMask = aFile << std::max(0, file - 1);
            uint64_t rightFileMask = aFile << std::min(7, file + 1);

            uint64_t tripleFileMask = fileMask | leftFileMask | rightFileMask;

            uint64_t forwardMask = ULLONG_MAX >> (8 * (8 - rank));

            pastPawnMask = forwardMask & tripleFileMask;

            if (!(pastPawnMask & board.bitboards[p]))
                bonus += whiteRankBonuses[rank];

            popBit(currentBitboardCopy, square);
        }
    } 
    else {
        uint64_t currentBitboardCopy = board.bitboards[p];
        while (currentBitboardCopy) {
            int square = getLSBIndex(currentBitboardCopy);
            int rank = square / 8;
            int file = square % 8;

            uint64_t fileMask = aFile << file;
            uint64_t leftFileMask = aFile << std::max(0, file - 1);
            uint64_t rightFileMask = aFile << std::min(7, file + 1);

            uint64_t tripleFileMask = fileMask | leftFileMask | rightFileMask;

            uint64_t forwardMask = ULLONG_MAX << (8 * (rank + 1));

            pastPawnMask = forwardMask & tripleFileMask;

            if (!(pastPawnMask & board.bitboards[P]))
                bonus += blackRankBonuses[rank];

            popBit(currentBitboardCopy, square);
        }
    }
    return bonus;
}