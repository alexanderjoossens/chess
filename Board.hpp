#ifndef CHESS_ENGINE_BOARD_HPP
#define CHESS_ENGINE_BOARD_HPP

#include "Piece.hpp"
#include "Square.hpp"
#include "Move.hpp"
#include "CastlingRights.hpp"

#include <optional>
#include <iosfwd>
#include <vector>

class Board {
public:

    using Optional = std::optional<Board>;
    using MoveVec = std::vector<Move>;

    Board();

    void setPiece(const Square& square, const Piece::Optional& piece);
    Piece::Optional piece(const Square& square) const;
    void setTurn(PieceColor turn);
    PieceColor turn() const;
    void setCastlingRights(CastlingRights cr);
    CastlingRights castlingRights() const;
    void setEnPassantSquare(const Square::Optional& square);
    Square::Optional enPassantSquare() const;

    void makeMove(const Move& move);

    void pseudoLegalMoves(MoveVec& legalMoves) const;
    void pseudoLegalMovesFrom(const Square& from, MoveVec& moves) const;

    int getWhiteScore() const;

    int getBlackScore() const;

    bool squareCheck(int index, PieceColor color) const;

private:

    char board[64];
    PieceColor turn_;
    CastlingRights rights;
    int enpassant_index;

    //bool bezet(int index) const;

    //void legalMovesKing(MoveVec &moves, PieceType player);

    bool ifInBoundaryAddToLegalMoves(Board::MoveVec& legalMoves, const Square& currentSquare, PieceColor color, int index, bool boundary) const;

    void legalMovesRook(Board::MoveVec& legalMoves, PieceColor color, const Square &fromSquare) const;

    void legalMovesPawn(Board::MoveVec& legalMoves, PieceColor color, const Square &fromSquare) const;

    void legalMovesBischop(Board::MoveVec& legalMoves, PieceColor color, const Square &fromSquare) const;

    void legalMovesKing(Board::MoveVec& legalMoves, PieceColor color, const Square &fromSquare) const;

    void legalMovesKnight(Board::MoveVec& legalMoves, PieceColor color, const Square &fromSquare) const;

    void legalMovesQueen(Board::MoveVec& legalMoves, PieceColor color, const Square &fromSquare) const;

    bool occupied(int index) const;

    bool areSquaresAttacked(unsigned int from_index, unsigned int to_index, PieceColor color) const;

    bool
    areSquaresAttackedByBishop(int attackingSquare, int increase, PieceColor color, int moduloResult,
                               int upperBoundary,
                               int token, PieceType type) const;

    bool noPiecesInBetween(unsigned int from_index, unsigned int to_index) const;

};

std::ostream& operator<<(std::ostream& os, const Board& board);

#endif
