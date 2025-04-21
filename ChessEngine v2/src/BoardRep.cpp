#include "headers/BoardRep.h"
#include "headers/HelperFunctions.h"
#include "headers/ZoobristKeys.h"

uint64_t repetitionTable[150];
int repetitionIndex;

void Board::printBoard() {
    for (int rank = 0; rank < 8; rank++) {
        std::cout << " " << 8 - rank << "   ";
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            int piece = -1;

            // loop over all piece bitboards
            for (int bb_piece = P; bb_piece <= k; bb_piece++)
            {
                if (getBit(bitboards[bb_piece], square))
                    piece = bb_piece;
            }

            if (piece == -1)
                std::cout << ".";
            else
                std::cout << asciiPieces[piece];
            std::cout << " ";
        }
        std::cout << '\n';
    }
    std::cout << "\n     a b c d e f g h\n\n";
    std::cout << "Color to move : " << ((colorToMove) ? "black" : "white") << '\n';
    std::cout << "Is En Passant : " << ((enpassant != noSQ) ? squareToCoordinates[enpassant] : "no") << '\n';
    std::cout << "Castling Right: " << ((castle & wk) ? "K " : "- ") << ((castle & wq) ? "Q " : "- ") << ((castle & bk) ? "k " : "- ") << ((castle & bq) ? "q" : "-") << "\n";
    std::cout << "Zobrist Key   : " << hashKey << "\n\n";
}

void Board::parseFEN(const char* fen) {
    // reset board position and state variables
    memset(bitboards, 0ULL, sizeof(bitboards));
    memset(ocupancies, 0ULL, sizeof(ocupancies));
    colorToMove = 0;
    enpassant = noSQ;
    castle = 0;

    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;

            // match ascii pieces within FEN string
            if ((*fen >= 'a' && *fen <= 'z') || (*fen >= 'A' && *fen <= 'Z')) {
                int piece = convertToCharPiece(*fen);

                // set piece on corresponding bitboard
                setBit(bitboards[piece], square);

                fen++;
            }
            // match empty square numbers within FEN string
            if (*fen >= '0' && *fen <= '9') {
                // init offset (convert char 0 to int 0)
                int offset = *fen - '0';

                // define piece variable
                int piece = -1;

                // loop over all piece bitboards
                for (int bb_piece = P; bb_piece <= k; bb_piece++) {
                    // if there is a piece on current square
                    if (getBit(bitboards[bb_piece], square))
                        piece = bb_piece; // get piece code
                }

                // on empty current square
                if (piece == -1)
                    file--; // decrement file

                file += offset; // adjust file counter

                fen++;
            }

            // match rank separator
            if (*fen == '/')
                fen++; // increment pointer to FEN string
        }
    }

    // got to parsing side to move (increment pointer to FEN string)
    fen++;

    // parse side to move
    (*fen == 'w') ? (colorToMove = white) : (colorToMove = black);

    // go to parsing castling rights
    fen += 2;

    // parse castling rights
    while (*fen != ' ') {
        switch (*fen) {
        case 'K': castle |= wk; break;
        case 'Q': castle |= wq; break;
        case 'k': castle |= bk; break;
        case 'q': castle |= bq; break;
        case '-': break;
        }

        // increment pointer to FEN string
        fen++;
    }

    // got to parsing enpassant square (increment pointer to FEN string)
    fen++;

    // parse enpassant square
    if (*fen != '-') {
        // parse enpassant file & rank
        int file = fen[0] - 'a';
        int rank = 8 - (fen[1] - '0');

        // init enpassant square
        enpassant = rank * 8 + file;
    }
    else
        enpassant = noSQ;

    // loop over white pieces bitboards
    for (int piece = P; piece <= K; piece++)
        // populate white occupancy bitboard
        ocupancies[white] |= bitboards[piece];

    // loop over black pieces bitboards
    for (int piece = p; piece <= k; piece++)
        // populate white occupancy bitboard
        ocupancies[black] |= bitboards[piece];

    // init all occupancies
    ocupancies[both] |= ocupancies[white];
    ocupancies[both] |= ocupancies[black];

    hashKey = generateHashKey(*this);
}