#include "headers/MoveGenerator.h"
#include <iostream>
#include "headers/BoardRep.h"
#include "headers/HelperFunctions.h"
#include "headers/AttacksGen.h"

void generateMoves(MoveList& moveList, const Board& board) {
    moveList.count = 0;

    // define start square & target square
    int startSquare, targetSquare;

    // define current piece bitboards copy & its attacks
    uint64_t bitboard, attacks;

    for (int piece = P; piece <= k; piece++) {
        bitboard = board.bitboards[piece]; // init piece bitboard copy

        // generate white pawn and white king castling moves
        if (board.colorToMove == white) {
            if (piece == P) {
                while (bitboard) {
                    startSquare = getLSBIndex(bitboard);
                    targetSquare = startSquare - 8;

                    // generate quiet pawn moves
                    if (!(targetSquare < a8) && !getBit(board.ocupancies[both], targetSquare)) {
                        if (startSquare >= a7 && startSquare <= h7) {
                            addMove(moveList, encodeMove(startSquare, targetSquare, piece, Q, 0, 0, 0, 0));
                            addMove(moveList, encodeMove(startSquare, targetSquare, piece, R, 0, 0, 0, 0));
                            addMove(moveList, encodeMove(startSquare, targetSquare, piece, B, 0, 0, 0, 0));
                            addMove(moveList, encodeMove(startSquare, targetSquare, piece, N, 0, 0, 0, 0));
                        }
                        else {
                            // one pawn pushes
                            addMove(moveList, encodeMove(startSquare, targetSquare, piece, 0, 0, 0, 0, 0));

                            // double pawn pushes
                            if (startSquare >= a2 && startSquare <= h2 && !getBit(board.ocupancies[both], targetSquare - 8))
                                addMove(moveList, encodeMove(startSquare, targetSquare - 8, piece, 0, 0, 1, 0, 0));
                        }
                    }

                    // init pawn attacks bitboard
                    attacks = pawnAttacks[board.colorToMove][startSquare] & board.ocupancies[black];

                    // generate capture pawn moves
                    while (attacks) {
                        targetSquare = getLSBIndex(attacks);

                        if (startSquare >= a7 && startSquare <= h7) {
                            addMove(moveList, encodeMove(startSquare, targetSquare, piece, Q, 1, 0, 0, 0));
                            addMove(moveList, encodeMove(startSquare, targetSquare, piece, R, 1, 0, 0, 0));
                            addMove(moveList, encodeMove(startSquare, targetSquare, piece, B, 1, 0, 0, 0));
                            addMove(moveList, encodeMove(startSquare, targetSquare, piece, N, 1, 0, 0, 0));
                        }
                        else
                            addMove(moveList, encodeMove(startSquare, targetSquare, piece, 0, 1, 0, 0, 0));

                        popBit(attacks, targetSquare);
                    }

                    // generate en passant moves
                    if (board.enpassant != noSQ) {
                        uint64_t enPassantAttacks = pawnAttacks[board.colorToMove][startSquare] & (1ULL << board.enpassant);

                        if (enPassantAttacks) {
                            // init en passant capture square
                            int targetEnPassant = getLSBIndex(enPassantAttacks);
                            addMove(moveList, encodeMove(startSquare, targetEnPassant, piece, 0, 1, 0, 1, 0));
                        }
                    }

                    popBit(bitboard, startSquare);
                }
            }

            // castling
            if (piece == K) {
                // kingside casteling is available
                if (board.castle & wk) {
                    // make sure square between king and king's rook are empty and make sure king and the f1 squares are not under attacks
                    // we check if the g1 square is under attack in the make move function (is king square under attacks)
                    if ((!getBit(board.ocupancies[both], f1) && !getBit(board.ocupancies[both], g1)) && (!isSquareAttacked(e1, black, board) && !isSquareAttacked(f1, black, board)))
                        addMove(moveList, encodeMove(e1, g1, piece, 0, 0, 0, 0, 1));
                }
                // queen side castling is available
                if (board.castle & wq) {
                    // make sure square between king and king's rook are empty and make sure king and the d1 squares are not under attacks
                    // we check if the g1 square is under attack in the make move function (is king square under attacks)
                    if ((!getBit(board.ocupancies[both], d1) && !getBit(board.ocupancies[both], c1) && !getBit(board.ocupancies[both], b1)) && (!isSquareAttacked(e1, black, board) && !isSquareAttacked(d1, black, board)))
                        addMove(moveList, encodeMove(e1, c1, piece, 0, 0, 0, 0, 1));
                }
            }
        }
        // generate black pawn and black king castling moves
        else {
            if (piece == p) {
                while (bitboard) {
                    startSquare = getLSBIndex(bitboard);
                    targetSquare = startSquare + 8;

                    // generate quiet pawn moves
                    if (!(targetSquare > h1) && !getBit(board.ocupancies[both], targetSquare)) {
                        if (startSquare >= a2 && startSquare <= h2) {
                            addMove(moveList, encodeMove(startSquare, targetSquare, piece, q, 0, 0, 0, 0));
                            addMove(moveList, encodeMove(startSquare, targetSquare, piece, r, 0, 0, 0, 0));
                            addMove(moveList, encodeMove(startSquare, targetSquare, piece, b, 0, 0, 0, 0));
                            addMove(moveList, encodeMove(startSquare, targetSquare, piece, n, 0, 0, 0, 0));
                        }
                        else {
                            // one pawn pushes
                            addMove(moveList, encodeMove(startSquare, targetSquare, piece, 0, 0, 0, 0, 0));
                            // double pawn pushes
                            if ((startSquare >= a7 && startSquare <= h7) && !getBit(board.ocupancies[both], targetSquare + 8))
                                addMove(moveList, encodeMove(startSquare, targetSquare + 8, piece, 0, 0, 1, 0, 0));
                        }
                    }

                    // init pawn attacks bitboard
                    attacks = pawnAttacks[board.colorToMove][startSquare] & board.ocupancies[white];

                    // generate capture pawn moves
                    while (attacks) {
                        targetSquare = getLSBIndex(attacks);

                        if (startSquare >= a2 && startSquare <= h2) {
                            addMove(moveList, encodeMove(startSquare, targetSquare, piece, q, 1, 0, 0, 0));
                            addMove(moveList, encodeMove(startSquare, targetSquare, piece, r, 1, 0, 0, 0));
                            addMove(moveList, encodeMove(startSquare, targetSquare, piece, b, 1, 0, 0, 0));
                            addMove(moveList, encodeMove(startSquare, targetSquare, piece, n, 1, 0, 0, 0));
                        }
                        else
                            addMove(moveList, encodeMove(startSquare, targetSquare, piece, 0, 1, 0, 0, 0));

                        popBit(attacks, targetSquare);
                    }

                    // generate en passant moves
                    if (board.enpassant != noSQ) {
                        uint64_t enPassantAttacks = pawnAttacks[board.colorToMove][startSquare] & (1ULL << board.enpassant);

                        if (enPassantAttacks) {
                            // init en passant capture square
                            int targetEnPassant = getLSBIndex(enPassantAttacks);
                            addMove(moveList, encodeMove(startSquare, targetEnPassant, piece, 0, 1, 0, 1, 0));
                        }
                    }

                    popBit(bitboard, startSquare);
                }
            }

            // castling
            if (piece == k) {
                // kingside casteling is available
                if (board.castle & bk) {
                    // make sure square between king and king's rook are empty and make sure king and the f8 squares are not under attacks
                    // we check if the g8 square is under attack in the make move function (is king square under attacks)
                    if ((!getBit(board.ocupancies[both], f8) && !getBit(board.ocupancies[both], g8)) && (!isSquareAttacked(e8, white, board) && !isSquareAttacked(f8, white, board)))
                        addMove(moveList, encodeMove(e8, g8, piece, 0, 0, 0, 0, 1));
                }
                // queen side castling is available
                if (board.castle & bq) {
                    // make sure square between king and king's rook are empty and make sure king and the d8 squares are not under attacks
                    // we check if the c8 square is under attack in the make move function (is king square under attacks)
                    if ((!getBit(board.ocupancies[both], d8) && !getBit(board.ocupancies[both], c8) && !getBit(board.ocupancies[both], b8)) && (!isSquareAttacked(e8, white, board) && !isSquareAttacked(d8, white, board)))
                        addMove(moveList, encodeMove(e8, c8, piece, 0, 0, 0, 0, 1));
                }
            }
        }

        // generate knight moves
        if (board.colorToMove == white ? piece == N : piece == n) {
            // loop over piece squares of piece bitboard copy
            while (bitboard) {
                startSquare = getLSBIndex(bitboard);
                attacks = knightAttacks[startSquare] & (board.colorToMove == white ? ~board.ocupancies[white] : ~board.ocupancies[black]);

                while (attacks) {
                    targetSquare = getLSBIndex(attacks);

                    // quiet move
                    if (!getBit((board.colorToMove == white ? board.ocupancies[black] : board.ocupancies[white]), targetSquare))
                        addMove(moveList, encodeMove(startSquare, targetSquare, piece, 0, 0, 0, 0, 0));
                    else // capture move
                        addMove(moveList, encodeMove(startSquare, targetSquare, piece, 0, 1, 0, 0, 0));

                    popBit(attacks, targetSquare);
                }

                popBit(bitboard, startSquare);
            }
        }

        // generate bishop moves
        if (board.colorToMove == white ? piece == B : piece == b) {
            // loop over piece squares of piece bitboard copy
            while (bitboard) {
                startSquare = getLSBIndex(bitboard);
                attacks = getBishopAttacks(startSquare, board.ocupancies[both]) & (board.colorToMove == white ? ~board.ocupancies[white] : ~board.ocupancies[black]);

                while (attacks) {
                    targetSquare = getLSBIndex(attacks);

                    // quiet move
                    if (!getBit((board.colorToMove == white ? board.ocupancies[black] : board.ocupancies[white]), targetSquare))
                        addMove(moveList, encodeMove(startSquare, targetSquare, piece, 0, 0, 0, 0, 0));
                    else // capture move
                        addMove(moveList, encodeMove(startSquare, targetSquare, piece, 0, 1, 0, 0, 0));

                    popBit(attacks, targetSquare);
                }

                popBit(bitboard, startSquare);
            }
        }

        // generate rook moves
        if (board.colorToMove == white ? piece == R : piece == r) {
            // loop over piece squares of piece bitboard copy
            while (bitboard) {
                startSquare = getLSBIndex(bitboard);
                attacks = getRookAttacks(startSquare, board.ocupancies[both]) & (board.colorToMove == white ? ~board.ocupancies[white] : ~board.ocupancies[black]);

                while (attacks) {
                    targetSquare = getLSBIndex(attacks);

                    // quiet move
                    if (!getBit((board.colorToMove == white ? board.ocupancies[black] : board.ocupancies[white]), targetSquare))
                        addMove(moveList, encodeMove(startSquare, targetSquare, piece, 0, 0, 0, 0, 0));
                    else // capture move
                        addMove(moveList, encodeMove(startSquare, targetSquare, piece, 0, 1, 0, 0, 0));

                    popBit(attacks, targetSquare);
                }

                popBit(bitboard, startSquare);
            }
        }

        // generate queen moves
        if (board.colorToMove == white ? piece == Q : piece == q) {
            // loop over piece squares of piece bitboard copy
            while (bitboard) {
                startSquare = getLSBIndex(bitboard);
                attacks = getQueenAttacks(startSquare, board.ocupancies[both]) & (board.colorToMove == white ? ~board.ocupancies[white] : ~board.ocupancies[black]);

                while (attacks) {
                    targetSquare = getLSBIndex(attacks);

                    // quiet move
                    if (!getBit((board.colorToMove == white ? board.ocupancies[black] : board.ocupancies[white]), targetSquare))
                        addMove(moveList, encodeMove(startSquare, targetSquare, piece, 0, 0, 0, 0, 0));
                    else // capture move
                        addMove(moveList, encodeMove(startSquare, targetSquare, piece, 0, 1, 0, 0, 0));

                    popBit(attacks, targetSquare);
                }

                popBit(bitboard, startSquare);
            }
        }

        // generate normal king moves
        if (board.colorToMove == white ? piece == K : piece == k) {
            // loop over piece squares of piece bitboard copy
            while (bitboard) {
                startSquare = getLSBIndex(bitboard);
                attacks = kingAttacks[startSquare] & (board.colorToMove == white ? ~board.ocupancies[white] : ~board.ocupancies[black]);

                while (attacks) {
                    targetSquare = getLSBIndex(attacks);

                    // quiet move
                    if (!getBit((board.colorToMove == white ? board.ocupancies[black] : board.ocupancies[white]), targetSquare))
                        addMove(moveList, encodeMove(startSquare, targetSquare, piece, 0, 0, 0, 0, 0));
                    else // capture move
                        addMove(moveList, encodeMove(startSquare, targetSquare, piece, 0, 1, 0, 0, 0));

                    popBit(attacks, targetSquare);
                }

                popBit(bitboard, startSquare);
            }
        }
    }
}