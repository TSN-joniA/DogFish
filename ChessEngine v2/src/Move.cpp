#include "headers/Move.h"
#include "headers/HelperFunctions.h"
#include "headers/AttacksGen.h"
#include "headers/ZoobristKeys.h"

// make move on chess board
bool makeMove(int move, MoveType moveFlag, Board& board) {
    // quiet moves 
    if (moveFlag == allMoves) {
        copyBoard(board);

        // parse move
        int startSquare = getMoveStart(move);
        int targetSquare = getMoveTarget(move);
        int piece = getMovePiece(move);
        int promotedPiece = getMovePromoted(move);
        int capture = getMoveCapture(move);
        int doublePush = getMoveDouble(move);
        int enPass = getMoveEnPassant(move);
        int castling = getMoveCastling(move);

        // move piece
        popBit(board.bitboards[piece], startSquare);
        setBit(board.bitboards[piece], targetSquare);

        // hash piece
        board.hashKey ^= pieceKeys[piece][startSquare]; // remove piece from source square in hash key
        board.hashKey ^= pieceKeys[piece][targetSquare]; // set piece to the target square in hash key

        // handling capture moves
        if (capture) {
            // pick up bitboard piece index ranges depending on color
            int startPiece = board.colorToMove == white ? p : P;
            int endPiece = board.colorToMove == white ? k : K;

            // loop over bitboards opposite to the current color to move
            for (int bbPiece = startPiece; bbPiece <= endPiece; bbPiece++) {
                if (getBit(board.bitboards[bbPiece], targetSquare)) {
                    popBit(board.bitboards[bbPiece], targetSquare);
                    // remove the piece from hash key
                    board.hashKey ^= pieceKeys[bbPiece][targetSquare];
                    break;
                }
            }
        }
        // handle pawn promotions
        if (promotedPiece) {
            popBit(board.bitboards[piece], targetSquare);
            setBit(board.bitboards[promotedPiece], targetSquare);
            board.hashKey ^= pieceKeys[piece][targetSquare];
            board.hashKey ^= pieceKeys[promotedPiece][targetSquare];
        }

        // handle enpassant captures
        if (enPass) {
            board.colorToMove == white ? popBit(board.bitboards[p], targetSquare + 8) 
                                       : popBit(board.bitboards[P], targetSquare - 8); // erase the pawn depending on color

            board.hashKey ^= board.colorToMove == white ? pieceKeys[p][targetSquare + 8] 
                                                        : pieceKeys[P][targetSquare - 8];
        }

        if (board.enpassant != noSQ) board.hashKey ^= enpassantKeys[board.enpassant];
        
        board.enpassant = noSQ;
        // handle double pawn push
        if (doublePush) {
            (board.colorToMove == white) ? board.enpassant = targetSquare + 8 : board.enpassant = targetSquare - 8;
            board.hashKey ^= enpassantKeys[(board.colorToMove == white) ? targetSquare + 8 : targetSquare - 8];
        } 

        // handling castling moves
        if (castling) {
            switch (targetSquare) {
            case g1: popBit(board.bitboards[R], h1); setBit(board.bitboards[R], f1);
                     board.hashKey ^= pieceKeys[R][h1]; board.hashKey ^= pieceKeys[R][f1]; 
                     break;
            case c1: popBit(board.bitboards[R], a1); setBit(board.bitboards[R], d1); 
                     board.hashKey ^= pieceKeys[R][a1]; board.hashKey ^= pieceKeys[R][d1]; 
                     break;
            case g8: popBit(board.bitboards[r], h8); setBit(board.bitboards[r], f8); 
                     board.hashKey ^= pieceKeys[r][h8]; board.hashKey ^= pieceKeys[r][f8]; 
                     break;
            case c8: popBit(board.bitboards[r], a8); setBit(board.bitboards[r], d8);
                     board.hashKey ^= pieceKeys[r][a8]; board.hashKey ^= pieceKeys[r][d8]; 
                     break;
            }
        }
        board.hashKey ^= castleKeys[board.castle];

        // update castling rights
        board.castle &= castlingRights[startSquare];
        board.castle &= castlingRights[targetSquare];

        board.hashKey ^= castleKeys[board.castle];

        // update occupancies
        memset(board.ocupancies, 0ULL, 24);
        for (int bbPiece = P; bbPiece <= K; bbPiece++)
            board.ocupancies[white] |= board.bitboards[bbPiece];
        for (int bbPiece = p; bbPiece <= k; bbPiece++)
            board.ocupancies[black] |= board.bitboards[bbPiece];
        board.ocupancies[both] |= board.ocupancies[white];
        board.ocupancies[both] |= board.ocupancies[black];

        board.colorToMove ^= 1; // change colorToMove to the opposite color

        board.hashKey ^= sideKey;

        // check if the king is not in check
        if (isSquareAttacked(board.colorToMove == white ? getLSBIndex(board.bitboards[k]) : getLSBIndex(board.bitboards[K]), board.colorToMove, board)) {
            restoreBoard(board);
            return false;
        }
        else {
            return true;
        }
            
    }
    else {
        // make sure the move is a capture
        if (getMoveCapture(move))
            makeMove(move, allMoves, board);
        else
            return false;
    }
    return false;
}

// UCI purpose
void printMove(int move) {
    printf("%s%s%c\n", squareToCoordinates[getMoveStart(move)],
        squareToCoordinates[getMoveTarget(move)],
        getMovePromoted(move) ? std::tolower(asciiPieces[getMovePromoted(move)]) : ' ');
}

void printMoveList(MoveList& moveList) {
    std::cout << "    move    piece   capture   double    enpass    castling\n\n";
    for (int moveCount = 0; moveCount < moveList.count; moveCount++) {
        int move = moveList.moves[moveCount];
        printf("    %s%s%c   %c       %d         %d         %d         %d\n", squareToCoordinates[getMoveStart(move)],
                                                                              squareToCoordinates[getMoveTarget(move)],
                                                                              getMovePromoted(move) ? std::tolower(asciiPieces[getMovePromoted(move)]) : ' ',
                                                                              asciiPieces[getMovePiece(move)],
                                                                              getMoveCapture(move) ? 1 : 0,
                                                                              getMoveDouble(move) ? 1 : 0,
                                                                              getMoveEnPassant(move) ? 1 : 0,
                                                                              getMoveCastling(move) ? 1 : 0);
    }
    std::cout << "\n    Total number of moves: " << moveList.count << '\n';
}

void addMove(MoveList& moveList, int move) {
    moveList.moves[moveList.count] = move;
    moveList.count++;
}
void removeMove(MoveList& moveList, int removeIndex) {
    for (int i = removeIndex; i < moveList.count; i++) {
        if (!(i == moveList.count - 1))
            moveList.moves[i] = moveList.moves[i + 1];
    }
    moveList.count--;
}