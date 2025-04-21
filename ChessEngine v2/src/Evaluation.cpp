#include "headers/HelperFunctions.h"
#include "headers/HelperVariables.h"
#include "headers/Evaluation.h"
#include "headers/AttacksGen.h"

// file masks [square]
uint64_t fileMasks[64];

// rank masks [square]
uint64_t rankMasks[64];

// isolated pawn masks [square]
uint64_t isolatedMasks[64];

// white passed pawn masks [square]
uint64_t whitePassedMasks[64];

// black passed pawn masks [square]
uint64_t blackPassedMasks[64];

uint64_t setFileRankMask(int fileNumber, int rankNumber)
{
    // file or rank mask
    uint64_t mask = 0ULL;

    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;

            if (fileNumber != -1) {
                // on file match
                if (file == fileNumber)
                    // set bit on mask
                    mask |= setBit(mask, square);
            }

            else if (rankNumber != -1) {
                // on rank match
                if (rank == rankNumber)
                    // set bit on mask
                    mask |= setBit(mask, square);
            }
        }
    }

    return mask;
}

void initEvaluationMasks()
{
    /******** Init file masks ********/

    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;

            // init file mask for a current square
            fileMasks[square] |= setFileRankMask(file, -1);
        }
    }

    /******** Init rank masks ********/

    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;

            // init rank mask for a current square
            rankMasks[square] |= setFileRankMask(-1, rank);
        }
    }

    /******** Init isolated masks ********/

    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;

            // init isolated pawns masks for a current square
            isolatedMasks[square] |= setFileRankMask(file - 1, -1);
            isolatedMasks[square] |= setFileRankMask(file + 1, -1);
        }
    }

    /******** White passed masks ********/

    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;

            // init white passed pawns mask for a current square
            whitePassedMasks[square] |= setFileRankMask(file - 1, -1);
            whitePassedMasks[square] |= setFileRankMask(file, -1);
            whitePassedMasks[square] |= setFileRankMask(file + 1, -1);

            // loop over redudant ranks
            for (int i = 0; i < (8 - rank); i++)
                // reset redudant bits 
                whitePassedMasks[square] &= ~rankMasks[(7 - i) * 8 + file];
        }
    }

    /******** Black passed masks ********/

    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;

            // init black passed pawns mask for a current square
            blackPassedMasks[square] |= setFileRankMask(file - 1, -1);
            blackPassedMasks[square] |= setFileRankMask(file, -1);
            blackPassedMasks[square] |= setFileRankMask(file + 1, -1);

            // loop over redudant ranks
            for (int i = 0; i < rank + 1; i++)
                // reset redudant bits 
                blackPassedMasks[square] &= ~rankMasks[i * 8 + file];
        }
    }
}

int getGamePhaseScore(const Board& board) {
    /*
        The game phase score of the game is derived from the pieces
        (not counting pawns and kings) that are still on the board.
        The full material starting position game phase score is:

        4 * knight material score in the opening +
        4 * bishop material score in the opening +
        4 * rook material score in the opening +
        2 * queen material score in the opening
    */

    // white & black game phase scores
    int whitePieceScores = 0, blackPieceScores = 0;

    // loop over white pieces
    for (int piece = N; piece <= Q; piece++)
        whitePieceScores += countBits(board.bitboards[piece]) * materialScore[opening][piece];

    // loop over black pieces
    for (int piece = n; piece <= q; piece++)
        blackPieceScores += countBits(board.bitboards[piece]) * -materialScore[opening][piece];

    return whitePieceScores + blackPieceScores;
}

int evaluate(const Board& board) {
    // get game phase score
    int gamePhaseScore = getGamePhaseScore(board);

    // game phase (opening, middle game, endgame)
    int gamePhase = -1;

    // pick up game phase based on game phase score
    if (gamePhaseScore > openingPhaseScore) gamePhase = opening;
    else if (gamePhaseScore < endgamePhaseScore) gamePhase = endgame;
    else gamePhase = middlegame;

    // static evaluation score
    int score = 0;

    uint64_t bitboard;

    int piece, square;

    // penalties
    int doublePawns = 0;

    // loop over piece bitboards
    for (int bbPiece = P; bbPiece <= k; bbPiece++) {
        // init piece bitboard copy
        bitboard = board.bitboards[bbPiece];

        // loop over pieces within a bitboard
        while (bitboard) {
            // init piece
            piece = bbPiece;

            // init square
            square = getLSBIndex(bitboard);

            /*
                Now in order to calculate interpolated score
                for a given game phase we use this formula
                (same for material and positional scores):

                (
                  scoreOpening * gamePhaseScore +
                  scoreEndgame * (openingPhaseScore - gamePhaseScore)
                ) / opening_phase_score

                E.g. the score for pawn on d4 at phase say 5000 would be
                interpolated_score = (12 * 5000 + (-7) * (6192 - 5000)) / 6192 = 8,342377261
            */

            // interpolate scores in middle_game
            if (gamePhase == middlegame)
                score += (
                    materialScore[opening][piece] * gamePhaseScore +
                    materialScore[endgame][piece] * (openingPhaseScore - gamePhaseScore)
                    ) / openingPhaseScore;

            // score material weights with pure scores in opening or endgame
            else score += materialScore[gamePhase][piece];

            // score positional piece scores
            switch (piece) {
                // evaluate white pawns
            case P:
                // interpolate scores in middle_game
                if (gamePhase == middlegame)
                    score += (
                        positionalScore[opening][PAWN][square] * gamePhaseScore +
                        positionalScore[endgame][PAWN][square] * (openingPhaseScore - gamePhaseScore)
                        ) / openingPhaseScore;

                // score material weights with pure scores in opening or endgame
                else score += positionalScore[gamePhase][PAWN][square];

                // double pawn penalty
                doublePawns = countBits(board.bitboards[P] & fileMasks[square]);

                // on double pawns (tripple, etc)
                if (doublePawns > 1)
                    score += doublePawns * doublePawnPenalty;

                // on isolated pawn
                if ((board.bitboards[P] & isolatedMasks[square]) == 0)
                    score += isolatedPawnPenalty;

                // on passed pawn
                if ((whitePassedMasks[square] & board.bitboards[p]) == 0)
                    score += passedPawnBonus[getRank[square]];
                
                break;

                // evaluate white knights
            case N:
                // interpolate scores in middle_game
                if (gamePhase == middlegame)
                    score += (
                        positionalScore[opening][KNIGHT][square] * gamePhaseScore +
                        positionalScore[endgame][KNIGHT][square] * (openingPhaseScore - gamePhaseScore)
                        ) / openingPhaseScore;

                // score material weights with pure scores in opening or endgame
                else score += positionalScore[gamePhase][KNIGHT][square];

                break;

                // evaluate white bishops
            case B:
                // interpolate scores in middle_game
                if (gamePhase == middlegame)
                    score += (
                        positionalScore[opening][BISHOP][square] * gamePhaseScore +
                        positionalScore[endgame][BISHOP][square] * (openingPhaseScore - gamePhaseScore)
                        ) / openingPhaseScore;

                // score material weights with pure scores in opening or endgame
                else score += positionalScore[gamePhase][BISHOP][square];

                // mobility
                score += countBits(getBishopAttacks(square, board.ocupancies[both]));

                break;

                // evaluate white rooks
            case R:
                /// interpolate scores in middle_game
                if (gamePhase == middlegame)
                    score += (
                        positionalScore[opening][ROOK][square] * gamePhaseScore +
                        positionalScore[endgame][ROOK][square] * (openingPhaseScore - gamePhaseScore)
                        ) / openingPhaseScore;

                // score material weights with pure scores in opening or endgame
                else score += positionalScore[gamePhase][ROOK][square];

                // semi open file
                if ((board.bitboards[P] & fileMasks[square]) == 0)
                    score += semiOpenFileScore;

                // open file
                if (((board.bitboards[P] | board.bitboards[p]) & fileMasks[square]) == 0)
                    score += openFileScore;
                break;

                // evaluate white queens
            case Q:
                /// interpolate scores in middle_game
                if (gamePhase == middlegame)
                    score += (
                        positionalScore[opening][QUEEN][square] * gamePhaseScore +
                        positionalScore[endgame][QUEEN][square] * (openingPhaseScore - gamePhaseScore)
                        ) / openingPhaseScore;

                // score material weights with pure scores in opening or endgame
                else score += positionalScore[gamePhase][QUEEN][square];

                // mobility
                score += countBits(getQueenAttacks(square, board.ocupancies[both]));
                break;

                // evaluate white king
            case K:
                /// interpolate scores in middle_game
                if (gamePhase == middlegame)
                    score += (
                        positionalScore[opening][KING][square] * gamePhaseScore +
                        positionalScore[endgame][KING][square] * (openingPhaseScore - gamePhaseScore)
                        ) / openingPhaseScore;

                // score material weights with pure scores in opening or endgame
                else score += positionalScore[gamePhase][KING][square];

                // semi open file
                if ((board.bitboards[P] & fileMasks[square]) == 0)
                    score -= semiOpenFileScore;

                // open file
                if (((board.bitboards[P] | board.bitboards[p]) & fileMasks[square]) == 0)
                    score -= openFileScore;

                // king safety bonus
                score += countBits(kingAttacks[square] & board.ocupancies[white]) * kingShieldBonus;
                break;

                // evaluate black pawns
            case p:
                // interpolate scores in middle_game
                if (gamePhase == middlegame)
                    score -= (
                        positionalScore[opening][PAWN][mirror_score[square]] * gamePhaseScore +
                        positionalScore[endgame][PAWN][mirror_score[square]] * (openingPhaseScore - gamePhaseScore)
                        ) / openingPhaseScore;

                // score material weights with pure scores in opening or endgame
                else score -= positionalScore[gamePhase][PAWN][mirror_score[square]];

                // double pawn penalty
                doublePawns = countBits(board.bitboards[p] & fileMasks[square]);

                // on double pawns (tripple, etc)
                if (doublePawns > 1)
                    score -= doublePawns * doublePawnPenalty;

                // on isolated pawnd
                if ((board.bitboards[p] & isolatedMasks[square]) == 0)
                    score -= isolatedPawnPenalty;

                // on passed pawn
                if ((blackPassedMasks[square] & board.bitboards[P]) == 0)
                    score -= passedPawnBonus[getRank[mirror_score[square]]];
                break;

                // evaluate black knights
            case n:
                // interpolate scores in middle_game
                if (gamePhase == middlegame)
                    score -= (
                        positionalScore[opening][KNIGHT][mirror_score[square]] * gamePhaseScore +
                        positionalScore[endgame][KNIGHT][mirror_score[square]] * (openingPhaseScore - gamePhaseScore)
                        ) / openingPhaseScore;

                // score material weights with pure scores in opening or endgame
                else score -= positionalScore[gamePhase][KNIGHT][mirror_score[square]];

                break;

                // evaluate black bishops
            case b:
                // interpolate scores in middle_game
                if (gamePhase == middlegame)
                    score -= (
                        positionalScore[opening][BISHOP][mirror_score[square]] * gamePhaseScore +
                        positionalScore[endgame][BISHOP][mirror_score[square]] * (openingPhaseScore - gamePhaseScore)
                        ) / openingPhaseScore;

                // score material weights with pure scores in opening or endgame
                else score -= positionalScore[gamePhase][BISHOP][mirror_score[square]];

                // mobility
                score -= countBits(getBishopAttacks(square, board.ocupancies[both]));
                break;

                // evaluate black rooks
            case r:
                // interpolate scores in middle_game
                if (gamePhase == middlegame)
                    score -= (
                        positionalScore[opening][ROOK][mirror_score[square]] * gamePhaseScore +
                        positionalScore[endgame][ROOK][mirror_score[square]] * (openingPhaseScore - gamePhaseScore)
                        ) / openingPhaseScore;

                // score material weights with pure scores in opening or endgame
                else score -= positionalScore[gamePhase][ROOK][mirror_score[square]];

                // semi open file
                if ((board.bitboards[p] & fileMasks[square]) == 0)
                    score -= semiOpenFileScore;

                // open file
                if (((board.bitboards[P] | board.bitboards[p]) & fileMasks[square]) == 0)
                    score -= openFileScore;
                break;

                // evaluate black queens
            case q:
                // interpolate scores in middle_game
                if (gamePhase == middlegame)
                    score -= (
                        positionalScore[opening][QUEEN][mirror_score[square]] * gamePhaseScore +
                        positionalScore[endgame][QUEEN][mirror_score[square]] * (openingPhaseScore - gamePhaseScore)
                        ) / openingPhaseScore;

                // score material weights with pure scores in opening or endgame
                else score -= positionalScore[gamePhase][QUEEN][mirror_score[square]];

                // mobility
                score -= countBits(getQueenAttacks(square, board.ocupancies[both]));
                break;

                // evaluate black king
            case k:
                // interpolate scores in middle_game
                if (gamePhase == middlegame)
                    score -= (
                        positionalScore[opening][KING][mirror_score[square]] * gamePhaseScore +
                        positionalScore[endgame][KING][mirror_score[square]] * (openingPhaseScore - gamePhaseScore)
                        ) / openingPhaseScore;

                // score material weights with pure scores in opening or endgame
                else score -= positionalScore[gamePhase][KING][mirror_score[square]];

                // semi open file
                if ((board.bitboards[p] & fileMasks[square]) == 0)
                    score += semiOpenFileScore;

                // open file
                if (((board.bitboards[P] | board.bitboards[p]) & fileMasks[square]) == 0)
                    score += openFileScore;

                // king safety bonus
                score -= countBits(kingAttacks[square] & board.ocupancies[black]) * kingShieldBonus;
                break;
            }

            // pop ls1b
            popBit(bitboard, square);
        }
    }

    // return final evaluation based on side
    return (board.colorToMove == white) ? score : -score;
}