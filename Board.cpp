#include "Board.hpp"

#include <ostream>
#include <cassert>
#include <cmath>

// this library for debug prints
#include <iostream>

/// a board is represented as an array with length 64 integers
/// example: [R N B Q K B N R P P P P P P P              ...           p p p p p p p p r n b q k b n r]
Board::Board()
{
    for (int i = 0; i< 64; i++) {
        board[i]=0;
    }
    // give all fields an initial value
    enpassant_index = 64;
    rights = CastlingRights::All;
    turn_ = PieceColor::White;
}

void Board::setPiece(const Square& square, const Piece::Optional& piece) {
    if (piece.has_value()) {
        board [square.index()]=piece.value().text();
    }
    else
    { board [square.index()]=0;
    }
}

Piece::Optional Board::piece(const Square& square) const {
    return Piece::fromSymbol(board[square.index()]);
}

void Board::setTurn(PieceColor turn) {
    turn_ = turn;
}

PieceColor Board::turn() const {
    return turn_;
}

void Board::setCastlingRights(CastlingRights cr) {
    this -> rights = cr;
}

CastlingRights Board::castlingRights() const {
    return rights;
}

void Board::setEnPassantSquare(const Square::Optional& square) {
    if (square.has_value()) {
        this -> enpassant_index = square.value().index();
    }
    else {
        // set to impossible value (not on the board)
        this -> enpassant_index = 100;
    }
}

Square::Optional Board::enPassantSquare() const {
    return Square::fromIndex(enpassant_index);
}

void Board::makeMove(const Move& move) {

    // initializing
    Square fromSquare = move.from();
    Square toSquare = move.to();
    int from_index = move.from().index();
    int to_index = move.to().index();
    Piece::Optional pieceOnFrom = this->piece(fromSquare);

    // Castling rights
    if (piece(fromSquare)->type() == PieceType::King) {
        if (turn() == PieceColor::Black) {
            rights &= ~CastlingRights::Black;
        } else {
            rights &= ~CastlingRights::White;
        }
    }
    if (piece(fromSquare)->type() == PieceType::Rook) {
        if (from_index == 63 && turn_ == PieceColor::Black) {
            rights &= ~CastlingRights::BlackKingside;
        }
        if (from_index == 56 && turn_ == PieceColor::Black) {
            rights &= ~CastlingRights::BlackQueenside;
        }
        if (from_index == 7 && turn_ == PieceColor::White) {
            rights &= ~CastlingRights::WhiteKingside;
        }
        if (from_index == 0 && turn_ == PieceColor::White) {
            rights &= ~CastlingRights::WhiteQueenside;
        }
    }
    std::optional<Piece> attackedPiece = Piece::fromSymbol(this->board[move.to().index()]);
    if (attackedPiece.has_value()) {
///        search: this -> boardValue = this ->boardValue+valueUpdate*weights(attackedPiece.value().type());
        if (attackedPiece.value().type() == PieceType::Rook) {
            if (to_index == 63 && turn_ == PieceColor::White) {
                rights &= ~CastlingRights::BlackKingside;
            }
            if (to_index == 56 && turn_ == PieceColor::White) {
                rights &= ~CastlingRights::BlackQueenside;
            }
            if (to_index == 7 && turn_ == PieceColor::Black) {
                rights &= ~CastlingRights::WhiteKingside;
            }
            if (to_index == 0 && turn_ == PieceColor::Black) {
                rights &= ~CastlingRights::WhiteQueenside;
            }
        }
/// search    if (attackedPiece.value().type() == PieceType::King) {
//            this.kingAttacked=true;
//        }
    }

    // if there is an enpasant square, and the move goes there (en passant captures a pawn),
    // then delete that pawn fromSquare the board and set the enpassant index toSquare 100
    if (enpassant_index != 100) {
        if (piece(fromSquare)->type() == PieceType::Pawn && piece(fromSquare)->color() == PieceColor::White) {
            if (to_index == enpassant_index) {
                this->setPiece(Square::fromIndex(enpassant_index - 8).value(), std::nullopt);
            }
        }
        if (piece(fromSquare)->type() == PieceType::Pawn && piece(fromSquare)->color() == PieceColor::Black) {
            if (to_index == enpassant_index) {
                this->setPiece(Square::fromIndex(enpassant_index + 8).value(), std::nullopt);
            }
        }
        enpassant_index = 100;
    }

    // if pawn on begin row goes 2 forward -> update enpassant_index
    if(piece(toSquare)->color() == PieceColor::White) {
        if (fromSquare.rank() == 1 && piece(fromSquare)->type() == PieceType::Pawn && toSquare.rank() == 3) {
            enpassant_index = toSquare.index() - 8;
        }
    }
    if (piece(toSquare)->color() == PieceColor::Black) {
        if (fromSquare.rank() == 6 && piece(fromSquare)->type() == PieceType::Pawn && toSquare.rank() == 4) {
            enpassant_index = toSquare.index() + 8;
        }
    }

/// updating fromSquare an toSquare square on board ///

    // if promotion: update fromSquare and toSquare square on board (toSquare square = promoted pieceOnFrom)
    if (move.promotion().has_value()) {
        Piece new_piece = Piece(turn_, move.promotion().value());
        this->board[move.from().index()] = 0;
        this->board[move.to().index()] = new_piece.text();
    }

    // if no promotion
    else {

        // if no castling: update fromSquare and toSquare square on board
        if (!(piece(fromSquare)->type() == PieceType::King && abs(int(fromSquare.index() - toSquare.index())) > 1))  {
            this->board[fromSquare.index()] = 0;
            this->board[toSquare.index()] = pieceOnFrom->text();
        }

        // if castling: update fromSquare and toSquare square on board
        else {
            if (piece(fromSquare)->type() == PieceType::King && abs(int(fromSquare.index() - toSquare.index())) > 1) {
                bool leftside = ((int)move.from().file() - (int)move.to().file()) > 0;
                // queenside castling = leftside (always)
                if (leftside) {
                    // first move king and rook and then empty the 2 previous squares
                    this->setPiece(Square::fromIndex((toSquare.index())).value(), piece(Square::fromIndex(fromSquare.index()).value()));
                    this->setPiece(Square::fromIndex((toSquare.index() + 1)).value(), piece(Square::fromIndex(toSquare.index() - 2).value()));
                    this->setPiece(Square::fromIndex(fromSquare.index()).value(), std::nullopt);
                    this->setPiece(Square::fromIndex((toSquare.index() - 2)).value(), std::nullopt);
                }
                // kingside castling
                else {
                    // first move king and rook and then empty the 2 previous squares
                    this->setPiece(Square::fromIndex(toSquare.index()).value(), piece(Square::fromIndex(fromSquare.index()).value()));
                    this->setPiece(Square::fromIndex(fromSquare.index() + 1).value(), piece(Square::fromIndex(toSquare.index() + 1).value()));
                    this->setPiece(Square::fromIndex(fromSquare.index()).value(), std::nullopt);
                    this->setPiece(Square::fromIndex((toSquare.index() + 1)).value(), std::nullopt);
                }
            }
        }
    }

    this->turn_ = !this->turn_;
}

void Board::pseudoLegalMoves(MoveVec& legalMoves) const {
    for (int index = 0; index < 64; index++) {
        Square currentSquare = Square::fromIndex(index).value();
        pseudoLegalMovesFrom(currentSquare, legalMoves);
    }
}

void Board::pseudoLegalMovesFrom(const Square& from,
                                 Board::MoveVec& legalMoves) const {

    if (board[from.index()]!=0) {
        Piece piece = Piece::fromSymbol(board[from.index()]).value();
        if (piece.color() == turn())
        {
//            switch (piece.type()) {
//
//                case PieceType::King:
//                    legalMovesKing(legalMoves, piece.color(), from);
//
//                case PieceType::Pawn:
//                    legalMovesPawn(legalMoves, piece.color(), from);
//
//                case PieceType::Knight:
//                    legalMovesKnight(legalMoves, piece.color(), from);
//
//                case PieceType::Bishop:
//                    legalMovesBischop(legalMoves, piece.color(), from);
//
//                case PieceType::Rook:
//                    legalMovesRook(legalMoves, piece.color(), from);
//
//                case PieceType::Queen:
//                    legalMovesQueen(legalMoves, piece.color(), from);
//                default:
//                    std::cout << "Piecetype not allowed";
//            }

            // TODO: switch case van maken?
            if (piece.type() == PieceType::King) {
                legalMovesKing(legalMoves, piece.color(), from);
            }
            if (piece.type() == PieceType::Pawn) {
                legalMovesPawn(legalMoves, piece.color(), from);
            }
            if (piece.type() == PieceType::Knight) {
                legalMovesKnight(legalMoves, piece.color(), from);
            }
            if (piece.type() == PieceType::Bishop) {
                legalMovesBischop(legalMoves, piece.color(), from);
            }
            if (piece.type() == PieceType::Rook) {
                legalMovesRook(legalMoves, piece.color(), from);
            }
            if (piece.type() == PieceType::Queen) {
                legalMovesQueen(legalMoves, piece.color(), from);
            }
        }
    }
}

bool Board::occupied(int index) const {
    if (Piece::fromSymbol(this->board[index]).has_value()) {
        return true;
    }
    else { return false; }
}

void Board::legalMovesPawn(Board::MoveVec& legalMoves, PieceColor color, const Square& fromSquare) const {

    int from_index = fromSquare.index();
    int direction = 0;
    int begin_row = 0;
    int row_to_promote = 0;

    if (color == PieceColor::White) {
        begin_row = 1;
        direction = 1;
        row_to_promote = 6;

    } else {
        begin_row = 6;
        direction = -1;
        row_to_promote = 1;
    }

    /// DEEL 1: pion staat op de voorlaatste rij ///
    if ((int) fromSquare.rank() == row_to_promote) {

        /// 1.1: op voorlaatste rij: 1 vakje naarvoor gaan en promoten
        if (!(occupied(from_index + 8 * direction))) {

            // add all possible promotions to legalMoves
            if (color == PieceColor::White) {
                legalMoves.push_back(
                        Move(fromSquare, Square::fromIndex(from_index + 8 * direction).value(), PieceType::Knight));
                legalMoves.push_back(
                        Move(fromSquare, Square::fromIndex(from_index + 8 * direction).value(), PieceType::Bishop));
                legalMoves.push_back(
                        Move(fromSquare, Square::fromIndex(from_index + 8 * direction).value(), PieceType::Rook));
                legalMoves.push_back(
                        Move(fromSquare, Square::fromIndex(from_index + 8 * direction).value(), PieceType::Queen));
            } else {
                legalMoves.push_back(
                        Move(fromSquare, Square::fromIndex(from_index + 8 * direction).value(), PieceType::Knight));
                legalMoves.push_back(
                        Move(fromSquare, Square::fromIndex(from_index + 8 * direction).value(), PieceType::Bishop));
                legalMoves.push_back(
                        Move(fromSquare, Square::fromIndex(from_index + 8 * direction).value(), PieceType::Rook));
                legalMoves.push_back(
                        Move(fromSquare, Square::fromIndex(from_index + 8 * direction).value(), PieceType::Queen));
            }
        }

        /// 1.2: voorlaatste rij: links of rechtsvoor capturen en promoten
        int index_left_front = 0;
        int index_right_front = 0;

        if (color == PieceColor::White) {
            index_left_front = from_index + 7;
            index_right_front = from_index + 9;

            // check if the square with this index is occupied
            if (occupied(index_left_front)) {

                // if the piece on fromSquare is the other color, you can take it: add to list

                if (Piece::fromSymbol(this->board[index_left_front]).value().color() != color) {
                    legalMoves.push_back(Move(fromSquare, Square::fromIndex(index_left_front).value(),
                                              PieceType::Knight));
                    legalMoves.push_back(Move(fromSquare, Square::fromIndex(index_left_front).value(),
                                              PieceType::Rook));
                    legalMoves.push_back(Move(fromSquare, Square::fromIndex(index_left_front).value(),
                                              PieceType::Bishop));
                    legalMoves.push_back(Move(fromSquare, Square::fromIndex(index_left_front).value(),
                                              PieceType::Queen));
                }
            }
            // check if the square with this index is occupied
            if (occupied(index_right_front)) {
                // if the piece on fromSquare is the other color, you can take it: add to list
                if (Piece::fromSymbol(this->board[index_right_front]).value().color() != color) {
                    legalMoves.push_back(Move(fromSquare, Square::fromIndex(index_right_front).value(),
                                              PieceType::Knight));
                    legalMoves.push_back(Move(fromSquare, Square::fromIndex(index_right_front).value(),
                                              PieceType::Rook));
                    legalMoves.push_back(Move(fromSquare, Square::fromIndex(index_right_front).value(),
                                              PieceType::Bishop));
                    legalMoves.push_back(Move(fromSquare, Square::fromIndex(index_right_front).value(),
                                              PieceType::Queen));
                }
            }
        } else {
            index_left_front = from_index - 9;
            index_right_front = from_index - 7;
            if (occupied(index_left_front)) {
                // if the piece on fromSquare is the other color, you can take it: add to list
                    if (Piece::fromSymbol(this->board[index_left_front]).value().color() != color) {
                    legalMoves.push_back(Move(fromSquare, Square::fromIndex(index_left_front).value(),
                                              PieceType::Knight));
                    legalMoves.push_back(Move(fromSquare, Square::fromIndex(index_left_front + direction).value(),
                                              PieceType::Rook));
                    legalMoves.push_back(Move(fromSquare, Square::fromIndex(index_left_front + direction).value(),
                                              PieceType::Bishop));
                    legalMoves.push_back(Move(fromSquare, Square::fromIndex(index_left_front + direction).value(),
                                              PieceType::Queen));
                }
            }
            // check if the square with this index is occupied
            if (occupied(index_right_front)) {
                // if the piece on fromSquare is the other color, you can take it: add to list
                if (Piece::fromSymbol(this->board[index_right_front]).value().color() != color) {
                    legalMoves.push_back(Move(fromSquare, Square::fromIndex(index_right_front).value(),
                                              PieceType::Knight));
                    legalMoves.push_back(Move(fromSquare, Square::fromIndex(index_right_front).value(),
                                              PieceType::Rook));
                    legalMoves.push_back(Move(fromSquare, Square::fromIndex(index_right_front).value(),
                                              PieceType::Bishop));
                    legalMoves.push_back(Move(fromSquare, Square::fromIndex(index_right_front).value(),
                                              PieceType::Queen));
                }
            }
        }
    }

    /// DEEL 2: NIET op de voorlaatste rij
    else {

        /// 2.1: Niet op voorlaatste rij: gwn 1 of 2 vakjes naarvoor gaan
        int next_index = from_index + direction * 8;

        // check if the square with this index is empty
        if (!(occupied(next_index))) {
            legalMoves.push_back(Move(fromSquare, Square::fromIndex(next_index).value(), std::nullopt));

            // check if pawn is on begin_row
            if ((int) fromSquare.rank() == begin_row) {
                int next_next_index = next_index + direction * 8;

                // check if the square with this index is empty
                if (!(occupied(next_next_index))) {
                    legalMoves.push_back(Move(fromSquare, Square::fromIndex(next_next_index).value(), std::nullopt));
                }
            }
        }

        /// 2.2: NIET op voorlaatste rij: capture links of rechtsvoor
        int index_left_front = 0;
        int index_right_front = 0;
        int index_left = 0;
        int index_right = 0;

        if (color == PieceColor::White) {
            index_left_front = from_index + 7;
            index_right_front = from_index + 9;
            index_left = from_index - 1;
            index_right = from_index + 1;

            // check if the square with this index is occupied
            if (occupied(index_left_front)) {

                // if the piece on fromSquare is the other color, you can take it: add to list
                if (Piece::fromSymbol(this->board[index_left_front]).value().color() != color) {
                    legalMoves.push_back(
                            Move(fromSquare, Square::fromIndex(index_left_front).value(), std::nullopt));
                }
            }
            // check if the square with this index is occupied
            if (occupied(index_right_front)) {
                // if the piece on fromSquare is the other color, you can take it: add to list
                if (Piece::fromSymbol(this->board[index_right_front]).value().color() != color) {
                    legalMoves.push_back(
                            Move(fromSquare, Square::fromIndex(index_right_front).value(), std::nullopt));
                }
            }

            // en passant links voor
            if (index_left_front == enpassant_index) {
                if (Piece::fromSymbol(this->board[index_left]).value().color() != color) {
                    legalMoves.push_back(
                            Move(fromSquare, Square::fromIndex(index_left_front).value(), std::nullopt));
                }
            }

            // en passant rechts voor
            if (index_right_front == enpassant_index) {
                if (Piece::fromSymbol(this->board[index_right]).value().color() != color) {
                    legalMoves.push_back(
                            Move(fromSquare, Square::fromIndex(index_right_front).value(), std::nullopt));
                }
            }


        } else {
            index_left_front = from_index - 7; // zoals zwart naar het bord kijkt! dus omgekeerd
            index_right_front = from_index - 9;
            index_left = from_index + 1;
            index_right = from_index - 1;

            if (occupied(index_left_front)) {
                // if the piece on fromSquare is the other color, you can take it: add to list
                if (Piece::fromSymbol(this->board[index_left_front]).value().color() != color) {
                    legalMoves.push_back(
                            Move(fromSquare, Square::fromIndex(index_left_front).value(), std::nullopt));
                }
            }
            // check if the square with this index is occupied
            if (occupied(index_right_front)) {
                // if the piece on fromSquare is the other color, you can take it: add to list
                if (Piece::fromSymbol(this->board[index_right_front]).value().color() != color) {
                    legalMoves.push_back(
                            Move(fromSquare, Square::fromIndex(index_right_front).value(), std::nullopt));
                }
            }

            // en passant links voor
            if (index_left_front == enpassant_index) {
                if (Piece::fromSymbol(this->board[index_left]).value().color() != color) {
                    legalMoves.push_back(
                            Move(fromSquare, Square::fromIndex(index_left_front).value(), std::nullopt));
                }
            }
            // en passant rechts voor
            if (index_right_front == enpassant_index) {
                if (Piece::fromSymbol(this->board[index_right]).value().color() != color) {
                    legalMoves.push_back(
                            Move(fromSquare, Square::fromIndex(index_right_front).value(), std::nullopt));
                }
            }
        }
    }
}

// TODO change function
void Board::legalMovesRook(Board::MoveVec& moves, PieceColor color, const Square& fromSquare) const {

    int orientations[2] = {1, 8};
    int directions[2] = {-1, 1};

    // go over
    for (int i = 0; i<4; i++) {
        int next_index = fromSquare.index();
        int orientation = orientations[i/2];
        int direction = directions[i%2];
        bool pathFree = true;

        // go over whole row/col until it is blocked
        while (pathFree) {

            // next index is left/right/up/down
            next_index = next_index + orientation * direction;

            // check if square is on the same column/row and on the board
            if (((next_index % 8) == (int)fromSquare.file() || next_index / 8 == (int)fromSquare.rank()) && next_index < 64) {

                // check if there is a piece on the square
                if (occupied(next_index)) {
                    pathFree = false;

                    // if the piece on fromSquare is the other color, you can go there: add to list
                    if (Piece::fromSymbol(this->board[next_index]).value().color() != color) {
                        moves.push_back(Move(fromSquare, Square::fromIndex(next_index).value(), std::nullopt));
                    }
                } else {
                    // if there is no piece on the board: add to list
                    moves.push_back(Move(fromSquare, Square::fromIndex(next_index).value(), std::nullopt));
                }
            } else {
                pathFree = false;
            }
        }
    }
}

bool::Board::ifInBoundaryAddToLegalMoves(Board::MoveVec& legalMoves, const Square& currentSquare, PieceColor color, int index, bool boundary) const {
    if (boundary) {
        return false; }

    // check if the square with this index is occupied
    if (occupied(index)) {
        if (Piece::fromSymbol(board[index])->color() != color) {
            legalMoves.push_back(Move(currentSquare, Square::fromIndex(index).value()));
            return false;
        } else { return false; }
    } else {
        legalMoves.push_back(Move(currentSquare, Square::fromIndex(index).value(), std::nullopt));
        return true;
    }
}

void Board::legalMovesBischop(Board::MoveVec& legalMoves, PieceColor color, const Square& fromSquare) const {

//  right up
    int index = fromSquare.index() + 9;
    while (true) {
        if (ifInBoundaryAddToLegalMoves(legalMoves, fromSquare, color, index, (index % 8 == 0) || index > 63)) {
            index += 9;
            continue;
        } else { break; }
    }

// left up
    index = fromSquare.index() - 9;
    while (true) {
        if (ifInBoundaryAddToLegalMoves(legalMoves, fromSquare, color, index, (index % 8 == 7) || index < 0)) {
            index -= 9;
            continue;
        } else {break;}
    }

    // left next
    index = fromSquare.index() + 7;
    while (true) {
        if (ifInBoundaryAddToLegalMoves(legalMoves, fromSquare, color, index, (index % 8 == 7) || index > 63)) {
            index += 7;
            continue;
        } else { break; }
    }

    // right next
    index = fromSquare.index() - 7;
    while (true) {
        if (ifInBoundaryAddToLegalMoves(legalMoves, fromSquare, color, index, (index % 8 == 0) || index < 0)) {
            index -= 7;
            continue;
        } else { break; }
    }
}

void Board::legalMovesQueen(Board::MoveVec& legalMoves, PieceColor color, const Square& fromSquare) const {

    legalMovesBischop(legalMoves, color, fromSquare);
    legalMovesRook(legalMoves, color, fromSquare);
}

// TODO change function
bool Board::areSquaresAttackedByBishop(int attackingSquare, int increase, PieceColor color, int moduloResult, int upperBoundary, int token, PieceType type) const {
    bool boundaryBool = ((attackingSquare % 8 != moduloResult) && ((token) * attackingSquare <= upperBoundary));
    while (boundaryBool) {
        if (occupied(attackingSquare)) {
            if (Piece::fromSymbol(board[attackingSquare])->type() == type &&
                Piece::fromSymbol(board[attackingSquare])->color() != color) {
                return true;
            }
            else {
                break;
            }
        }
        attackingSquare= attackingSquare + increase;
        boundaryBool = ((attackingSquare % 8 != moduloResult) && ((token) * attackingSquare <= upperBoundary));
    }
    return false;
}

// TODO change function
bool Board::areSquaresAttacked(unsigned from_index, unsigned to_index, PieceColor color) const {

    // Knight attack
    int links1vooruit2opzij = 6;
    int links2vooruit1opzij = 15;
    int rechts2vooruit1opzij = 17;
    int rechts1vooruit2opzij=10;
    int rowdirection=1;
    if (color==PieceColor::Black) {
        links1vooruit2opzij=-10;
        links2vooruit1opzij=-17;
        rechts1vooruit2opzij=-6;
        rechts2vooruit1opzij=-15;
        rowdirection=-1;
    }
    int attackingSquare = 0;
    for (unsigned i=from_index; i<=to_index; i++) {
        attackingSquare = i + links1vooruit2opzij;
        if (Piece::fromSymbol(board[attackingSquare])->type() == PieceType::Knight && Piece::fromSymbol(board[attackingSquare])->color() != color) {
            return true;
        }
        attackingSquare = i + links2vooruit1opzij;
        if (Piece::fromSymbol(board[attackingSquare])->type() == PieceType::Knight && Piece::fromSymbol(board[attackingSquare])->color() != color) {
            return true;
        }
        attackingSquare = i + rechts1vooruit2opzij;
        if (Piece::fromSymbol(board[attackingSquare])->type() == PieceType::Knight && Piece::fromSymbol(board[attackingSquare])->color() != color) {
            return true;
        }
        attackingSquare = i + rechts2vooruit1opzij;
        if (Piece::fromSymbol(board[attackingSquare])->type() == PieceType::Knight && Piece::fromSymbol(board[attackingSquare])->color() != color) {
            return true;
        }

        // Pawn attack
        attackingSquare= i + rowdirection * 7;
        if (Piece::fromSymbol(board[attackingSquare])->type() == PieceType::Pawn && Piece::fromSymbol(board[attackingSquare])->color() != color) {
            return true;
        }
        attackingSquare= i + rowdirection * 9;
        if (Piece::fromSymbol(board[attackingSquare])->type() == PieceType::Pawn && Piece::fromSymbol(board[attackingSquare])->color() != color) {
            return true;
        }

        // Bishop attack
        attackingSquare = i + 9;
        //if (squaresAttackedByBishop(attackingSquare,9,color,((attackingSquare%8 != 0) && attackingSquare<64), PieceType::Bishop))
        if (areSquaresAttackedByBishop(attackingSquare, 9, color, 0, 63, 1, PieceType::Bishop)) {
            return true;
        }
        attackingSquare = i - 9;
        //if (squaresAttackedByBishop(attackingSquare,-9,color,((attackingSquare%8 != 7) && attackingSquare>=0), PieceType::Bishop))
        if (areSquaresAttackedByBishop(attackingSquare, -9, color, 7, 0, -1, PieceType::Bishop)) {
                    return true;
            }
        attackingSquare = i + 7;
        //if (squaresAttackedByBishop(attackingSquare,7,color,((attackingSquare%8 != 7) && attackingSquare<64), PieceType::Bishop))
        if (areSquaresAttackedByBishop(attackingSquare, 7, color, 7, 63, 1, PieceType::Bishop)) {
                    return true;
            }
        attackingSquare = i - 7;
        //if (squaresAttackedByBishop(attackingSquare,-7,color,((attackingSquare%8 != 0) && attackingSquare>=0), PieceType::Bishop))
        if (areSquaresAttackedByBishop(attackingSquare, -7, color, 0, 0, -1, PieceType::Bishop)) {
                    return true;
            }

        // Queen attack
        attackingSquare = i + 9;
        //if (squaresAttackedByBishop(attackingSquare,9,color,((attackingSquare%8 != 0) && attackingSquare<64), PieceType::Bishop))
        if (areSquaresAttackedByBishop(attackingSquare, 9, color, 0, 63, 1, PieceType::Bishop)) {
            return true;
        }
        attackingSquare = i - 9;
        //if (squaresAttackedByBishop(attackingSquare,-9,color,((attackingSquare%8 != 7) && attackingSquare>=0), PieceType::Bishop))
        if (areSquaresAttackedByBishop(attackingSquare, -9, color, 7, 0, -1, PieceType::Bishop)) {
            return true;
        }
        attackingSquare = i + 7;
        //if (squaresAttackedByBishop(attackingSquare,7,color,((attackingSquare%8 != 7) && attackingSquare<64), PieceType::Bishop))
        if (areSquaresAttackedByBishop(attackingSquare, 7, color, 7, 63, 1, PieceType::Bishop)) {
            return true;
        }
        attackingSquare = i - 7;
        //if (squaresAttackedByBishop(attackingSquare,-7,color,((attackingSquare%8 != 0) && attackingSquare>=0), PieceType::Bishop))
        if (areSquaresAttackedByBishop(attackingSquare, -7, color, 0, 0, -1, PieceType::Bishop)) {
            return true;
        }

        attackingSquare= i + 8;
        //if (squaresAttackedByBishop(attackingSquare,8,color,(attackingSquare < 64), PieceType::Queen))
        if (areSquaresAttackedByBishop(attackingSquare, 8, color, 20, 63, 1, PieceType::Queen)) {
            return true;
        }
        attackingSquare= i - 8;
        //if (squaresAttackedByBishop(attackingSquare,-8,color,(attackingSquare >= 0), PieceType::Queen))
        if (areSquaresAttackedByBishop(attackingSquare, -8, color, 20, 0, -1, PieceType::Queen)) {
            return true;
        }
        attackingSquare= i + 1;
        //if (squaresAttackedByBishop(attackingSquare,1,color,(attackingSquare%8 != 0), PieceType::Queen))
        if (areSquaresAttackedByBishop(attackingSquare, 1, color, 0, 63, 1, PieceType::Queen)) {
            return true;
        }
        attackingSquare= i - 1;
        //if (squaresAttackedByBishop(attackingSquare,-1,color,(attackingSquare%8 != 7), PieceType::Queen))
        if (areSquaresAttackedByBishop(attackingSquare, -1, color, 7, 0, -1, PieceType::Queen)) {
            return true;
        }

        // King attack
        attackingSquare = i + rowdirection * 7;
        if (Piece::fromSymbol(board[attackingSquare])->type() == PieceType::King && Piece::fromSymbol(board[attackingSquare])->color() != color) {
            return true;
        }
        attackingSquare = i + rowdirection * 8;
        if (Piece::fromSymbol(board[attackingSquare])->type() == PieceType::King && Piece::fromSymbol(board[attackingSquare])->color() != color) {
            return true;
        }
        attackingSquare = i + rowdirection * 9;
        if (Piece::fromSymbol(board[attackingSquare])->type() == PieceType::King && Piece::fromSymbol(board[attackingSquare])->color() != color) {
            return true;
        }

        // Rook attack
        attackingSquare= i + 8;
        //if (squaresAttackedByBishop(attackingSquare,8,color,(attackingSquare < 64), PieceType::Queen))
        if (areSquaresAttackedByBishop(attackingSquare, 8, color, 20, 63, 1, PieceType::Rook)) {
            return true;
        }
        attackingSquare= i - 8;
        //if (squaresAttackedByBishop(attackingSquare,-8,color,(attackingSquare >= 0), PieceType::Queen))
        if (areSquaresAttackedByBishop(attackingSquare, -8, color, 20, 0, -1, PieceType::Rook)) {
            return true;
        }
        attackingSquare= i + 1;
        //if (squaresAttackedByBishop(attackingSquare,1,color,(attackingSquare%8 != 0), PieceType::Queen))
        if (areSquaresAttackedByBishop(attackingSquare, 1, color, 0, 63, 1, PieceType::Rook)) {
            return true;
        }
        attackingSquare= i - 1;
        //if (squaresAttackedByBishop(attackingSquare,-1,color,(attackingSquare%8 != 7), PieceType::Queen))
        if (areSquaresAttackedByBishop(attackingSquare, -1, color, 7, 0, -1, PieceType::Rook)) {
            return true;
        }
    }
    return false;
}

bool Board::noPiecesInBetween(unsigned from_index, unsigned to_index) const {
    for (unsigned i=from_index+1;i<to_index;i++) {
        if (occupied(i)) {
            return false;
        }
    }
    return true;
}

void Board::legalMovesKing(Board::MoveVec& legalMoves, PieceColor color, const Square& fromSquare) const {
    int rank = fromSquare.rank();
    int file = fromSquare.file();
    // heb volgorde omgewisseld

    // all surrounding row-col combinations
    int rows[8] = {rank-1, rank-1, rank-1, rank, rank, rank+1, rank+1, rank+1};
    int cols[8] = {file -1, file , file+1, file-1, file +1, file -1, file, file +1};

    // go over all surrounding squares
    for (int i=0; i<8; i++) {

        // check if the squares are in the board
        if (rows[i] >= 0 && rows[i] < 8 && cols[i] >= 0 && cols[i] < 8) {

            // check if there is a piece on the square
            if (occupied(cols[i]+rows[i]*8)) {

                // if the piece is fromSquare the other color, you can go there: add to list
                if (Piece::fromSymbol(this->board[cols[i] + rows[i] * 8]).value().color() != color) {
                    legalMoves.push_back(Move(fromSquare, Square::fromCoordinates(cols[i], rows[i]).value(), std::nullopt));
                }
                // if there is no piece on the board: add to list
            } else {
                legalMoves.push_back(Move(fromSquare, Square::fromCoordinates(cols[i], rows[i]).value(), std::nullopt));
            }
        }
    }

    // Castling
    // TODO change function?
    CastlingRights kingSide = CastlingRights::None;
    CastlingRights queenSide = CastlingRights::None;
    if (color==PieceColor::Black) {
        kingSide = CastlingRights::BlackKingside;
        queenSide= CastlingRights::BlackQueenside;
    }
    else {
        kingSide = CastlingRights::WhiteKingside;
        queenSide = CastlingRights::WhiteQueenside;
    }
    if ((rights & kingSide) != CastlingRights::None) {
        // kingSide? of toch queenSide castling is available for black
        // check if there are pawns
        if (noPiecesInBetween(fromSquare.index(), fromSquare.index() + 3) && !areSquaresAttacked(fromSquare.index(), fromSquare.index() + 2, color)) {
            legalMoves.push_back(Move(fromSquare, Square::fromIndex(fromSquare.index() + 2).value()));
        }
    }
    if ((rights & queenSide) != CastlingRights::None) {
        //Queenside castling is available for black
        // check if there are pawns
        if (noPiecesInBetween(fromSquare.index() - 4, fromSquare.index()) && !areSquaresAttacked(fromSquare.index() - 2, fromSquare.index(), color)) {
            legalMoves.push_back(Move(fromSquare, Square::fromIndex(fromSquare.index() - 2).value()));
        }
    }
}

void Board::legalMovesKnight(Board::MoveVec& legalMoves, PieceColor color, const Square& fromSquare) const {

    int rank = fromSquare.rank();
    int file = fromSquare.file();
    // heb volgorde gewisseld
    // all L-shaped knight move squares
//    int rows[8] = {rank-2, rank-1, rank+1, rank+2, rank+2, rank+1, rank-1, rank-2};
//    int cols[8] = {file - 1, file - 2, file - 2, file - 1, file + 1, file + 2, file + 2, file + 1};
    int rows[8] = {rank-2, rank-2, rank-1, rank-1, rank+1, rank+1, rank+2, rank+2};
    int cols[8] = {file -1, file +1, file -2, file +2, file -2, file +2, file -1, file +1};

    // go over all possible L-shape squares
    for (int i=0; i<8; i++) {

        // check if the squares are in the board
        if (rows[i] >= 0 && rows[i] < 8 && cols[i] >= 0 && cols[i] < 8) {

            // check if there is a piece on the square
            if (occupied(cols[i] + rows[i] * 8)) {

                // if the piece is fromSquare the other color, you can go there: add to list
                if (Piece::fromSymbol(this->board[cols[i] + rows[i] * 8]).value().color() != color) {
                    legalMoves.push_back(Move(fromSquare, Square::fromCoordinates(cols[i], rows[i]).value(), std::nullopt));
                }
            } else {
                legalMoves.push_back(Move(fromSquare, Square::fromCoordinates(cols[i], rows[i]).value(), std::nullopt));
            }
        }
    }
}


std::ostream& operator<<(std::ostream& os, const Board& board) {
    int index = 56;
    while (index >= 0) {
        Piece::Optional piece = board.piece(Square::fromIndex(index).value());
        // TODO use occupied function here?
        if (piece.has_value()) {
            os<<piece->text();
        }
        else {
            os << ".";
        }
        if (index%8 == 7) {
            index-=16;
            os<<std::string("\n");
        }
        index++;
    }
    return os;
}

int Board::getWhiteScore() const {
    int score = 0;
    for (int i = 0; i<64; i++) {
        if (occupied(i)) {
            Piece::Optional piece = this->piece(Square::fromIndex(i).value());
            if (piece.value().color() == PieceColor::White) {

                // TODO: na if break; zetten zodat die niet al de rest voor niks controleert?
                if (piece->type() == PieceType::King) {
                    score += 900;
                }
                if (piece->type() == PieceType::Queen) {
                    score += 90;
                }
                if (piece->type() == PieceType::Rook) {
                    score += 50;
                }
                if (piece->type() == PieceType::Bishop) {
                    score += 30;
                }
                if (piece->type() == PieceType::Knight) {
                    score += 30;
                }
                if (piece->type() == PieceType::Pawn) {
                    score += 10;
                }
            }
        }
    }
    return score;
}

int Board::getBlackScore() const {
    int score = 0;
    for (int i = 0; i<64; i++) {
        if (occupied(i)) {
            Piece::Optional piece = this->piece(Square::fromIndex(i).value());
            if (piece.value().color() == PieceColor::Black) {

                // TODO: na if break; zetten zodat die niet al de rest voor niks controleert?
                if (piece->type() == PieceType::King) {
                    score += 900;
                }
                if (piece->type() == PieceType::Queen) {
                    score += 90;
                }
                if (piece->type() == PieceType::Rook) {
                    score += 50;
                }
                if (piece->type() == PieceType::Bishop) {
                    score += 30;
                }
                if (piece->type() == PieceType::Knight) {
                    score += 30;
                }
                if (piece->type() == PieceType::Pawn) {
                    score += 10;
                }
            }
        }
    }
    return score;
}

bool Board::squareCheck(int index, PieceColor color) const {
    //deze functie w nog niet gebruikt
    if (occupied(index)) {
        Piece::Optional piece = this->piece(Square::fromIndex(index).value());
        if (piece->color() == color) {
            return true;
        }
    }
    return false;
}


//bool Board::isKingAttacked(unsigned index_king, PieceColor color) const {
//
//    // Knight attack
//    int links1vooruit2opzij = 6;
//    int links2vooruit1opzij = 15;
//    int rechts2vooruit1opzij = 17;
//    int rechts1vooruit2opzij=10;
//    int rowdirection=1;
//    if (color==PieceColor::Black) {
//        links1vooruit2opzij=-10;
//        links2vooruit1opzij=-17;
//        rechts1vooruit2opzij=-6;
//        rechts2vooruit1opzij=-15;
//        rowdirection=-1;
//    }
//    int attackingSquare = 0;
//    for (unsigned i=from_index; i<=to_index; i++) {
//        attackingSquare = i + links1vooruit2opzij;
//        if (Piece::fromSymbol(board[attackingSquare])->type() == PieceType::Knight && Piece::fromSymbol(board[attackingSquare])->color() != color) {
//            return true;
//        }
//        attackingSquare = i + links2vooruit1opzij;
//        if (Piece::fromSymbol(board[attackingSquare])->type() == PieceType::Knight && Piece::fromSymbol(board[attackingSquare])->color() != color) {
//            return true;
//        }
//        attackingSquare = i + rechts1vooruit2opzij;
//        if (Piece::fromSymbol(board[attackingSquare])->type() == PieceType::Knight && Piece::fromSymbol(board[attackingSquare])->color() != color) {
//            return true;
//        }
//        attackingSquare = i + rechts2vooruit1opzij;
//        if (Piece::fromSymbol(board[attackingSquare])->type() == PieceType::Knight && Piece::fromSymbol(board[attackingSquare])->color() != color) {
//            return true;
//        }
//
//        // Pawn attack
//        attackingSquare= i + rowdirection * 7;
//        if (Piece::fromSymbol(board[attackingSquare])->type() == PieceType::Pawn && Piece::fromSymbol(board[attackingSquare])->color() != color) {
//            return true;
//        }
//        attackingSquare= i + rowdirection * 9;
//        if (Piece::fromSymbol(board[attackingSquare])->type() == PieceType::Pawn && Piece::fromSymbol(board[attackingSquare])->color() != color) {
//            return true;
//        }
//
//        // Bishop attack
//        attackingSquare = i + 9;
//        //if (squaresAttackedByBishop(attackingSquare,9,color,((attackingSquare%8 != 0) && attackingSquare<64), PieceType::Bishop))
//        if (areSquaresAttackedByBishop(attackingSquare, 9, color, 0, 63, 1, PieceType::Bishop)) {
//            return true;
//        }
//        attackingSquare = i - 9;
//        //if (squaresAttackedByBishop(attackingSquare,-9,color,((attackingSquare%8 != 7) && attackingSquare>=0), PieceType::Bishop))
//        if (areSquaresAttackedByBishop(attackingSquare, -9, color, 7, 0, -1, PieceType::Bishop)) {
//            return true;
//        }
//        attackingSquare = i + 7;
//        //if (squaresAttackedByBishop(attackingSquare,7,color,((attackingSquare%8 != 7) && attackingSquare<64), PieceType::Bishop))
//        if (areSquaresAttackedByBishop(attackingSquare, 7, color, 7, 63, 1, PieceType::Bishop)) {
//            return true;
//        }
//        attackingSquare = i - 7;
//        //if (squaresAttackedByBishop(attackingSquare,-7,color,((attackingSquare%8 != 0) && attackingSquare>=0), PieceType::Bishop))
//        if (areSquaresAttackedByBishop(attackingSquare, -7, color, 0, 0, -1, PieceType::Bishop)) {
//            return true;
//        }
//
//        // Queen attack
//        attackingSquare = i + 9;
//        //if (squaresAttackedByBishop(attackingSquare,9,color,((attackingSquare%8 != 0) && attackingSquare<64), PieceType::Bishop))
//        if (areSquaresAttackedByBishop(attackingSquare, 9, color, 0, 63, 1, PieceType::Bishop)) {
//            return true;
//        }
//        attackingSquare = i - 9;
//        //if (squaresAttackedByBishop(attackingSquare,-9,color,((attackingSquare%8 != 7) && attackingSquare>=0), PieceType::Bishop))
//        if (areSquaresAttackedByBishop(attackingSquare, -9, color, 7, 0, -1, PieceType::Bishop)) {
//            return true;
//        }
//        attackingSquare = i + 7;
//        //if (squaresAttackedByBishop(attackingSquare,7,color,((attackingSquare%8 != 7) && attackingSquare<64), PieceType::Bishop))
//        if (areSquaresAttackedByBishop(attackingSquare, 7, color, 7, 63, 1, PieceType::Bishop)) {
//            return true;
//        }
//        attackingSquare = i - 7;
//        //if (squaresAttackedByBishop(attackingSquare,-7,color,((attackingSquare%8 != 0) && attackingSquare>=0), PieceType::Bishop))
//        if (areSquaresAttackedByBishop(attackingSquare, -7, color, 0, 0, -1, PieceType::Bishop)) {
//            return true;
//        }
//
//        attackingSquare= i + 8;
//        //if (squaresAttackedByBishop(attackingSquare,8,color,(attackingSquare < 64), PieceType::Queen))
//        if (areSquaresAttackedByBishop(attackingSquare, 8, color, 20, 63, 1, PieceType::Queen)) {
//            return true;
//        }
//        attackingSquare= i - 8;
//        //if (squaresAttackedByBishop(attackingSquare,-8,color,(attackingSquare >= 0), PieceType::Queen))
//        if (areSquaresAttackedByBishop(attackingSquare, -8, color, 20, 0, -1, PieceType::Queen)) {
//            return true;
//        }
//        attackingSquare= i + 1;
//        //if (squaresAttackedByBishop(attackingSquare,1,color,(attackingSquare%8 != 0), PieceType::Queen))
//        if (areSquaresAttackedByBishop(attackingSquare, 1, color, 0, 63, 1, PieceType::Queen)) {
//            return true;
//        }
//        attackingSquare= i - 1;
//        //if (squaresAttackedByBishop(attackingSquare,-1,color,(attackingSquare%8 != 7), PieceType::Queen))
//        if (areSquaresAttackedByBishop(attackingSquare, -1, color, 7, 0, -1, PieceType::Queen)) {
//            return true;
//        }
//
//        // King attack
//        attackingSquare = i + rowdirection * 7;
//        if (Piece::fromSymbol(board[attackingSquare])->type() == PieceType::King && Piece::fromSymbol(board[attackingSquare])->color() != color) {
//            return true;
//        }
//        attackingSquare = i + rowdirection * 8;
//        if (Piece::fromSymbol(board[attackingSquare])->type() == PieceType::King && Piece::fromSymbol(board[attackingSquare])->color() != color) {
//            return true;
//        }
//        attackingSquare = i + rowdirection * 9;
//        if (Piece::fromSymbol(board[attackingSquare])->type() == PieceType::King && Piece::fromSymbol(board[attackingSquare])->color() != color) {
//            return true;
//        }
//
//        // Rook attack
//        attackingSquare= i + 8;
//        //if (squaresAttackedByBishop(attackingSquare,8,color,(attackingSquare < 64), PieceType::Queen))
//        if (areSquaresAttackedByBishop(attackingSquare, 8, color, 20, 63, 1, PieceType::Rook)) {
//            return true;
//        }
//        attackingSquare= i - 8;
//        //if (squaresAttackedByBishop(attackingSquare,-8,color,(attackingSquare >= 0), PieceType::Queen))
//        if (areSquaresAttackedByBishop(attackingSquare, -8, color, 20, 0, -1, PieceType::Rook)) {
//            return true;
//        }
//        attackingSquare= i + 1;
//        //if (squaresAttackedByBishop(attackingSquare,1,color,(attackingSquare%8 != 0), PieceType::Queen))
//        if (areSquaresAttackedByBishop(attackingSquare, 1, color, 0, 63, 1, PieceType::Rook)) {
//            return true;
//        }
//        attackingSquare= i - 1;
//        //if (squaresAttackedByBishop(attackingSquare,-1,color,(attackingSquare%8 != 7), PieceType::Queen))
//        if (areSquaresAttackedByBishop(attackingSquare, -1, color, 7, 0, -1, PieceType::Rook)) {
//            return true;
//        }
//    }
//    return false;
//}