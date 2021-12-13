#include "Piece.hpp"

#include <string>

#include <ostream>

Piece::Piece(PieceColor color, PieceType type)
{
    color_ = color;
    type_ = type;
}

// renamen naar toString ofzo
char Piece::text() const {
    char text;
    switch(this->type_) {
        case PieceType::Bishop :;
            text = 'B';
            break;
        case PieceType::King :
            text = 'K';
            break;
        case PieceType::Knight :
            text = 'N';
            break;
        case PieceType::Queen :
            text = 'Q';
            break;
        case PieceType::Pawn :
            text = 'P';
            break;
        case PieceType::Rook :
            text = 'R';
            break;
        default:
            text = 0;
            break;
    }

    if (this->color_ == PieceColor::Black) {
        text = std::tolower(text);
    }

    return text;
}


// nog veranderen naar switch case "is minden verbose"
Piece::Optional Piece::fromSymbol(char symbol) {
    if (symbol == 'P') {
        return Piece(PieceColor::White, PieceType::Pawn);
    }
    if (symbol == 'R') {
        return Piece(PieceColor::White, PieceType::Rook);
    }
    if (symbol == 'B') {
        return Piece(PieceColor::White, PieceType::Bishop);
    }
    if (symbol == 'N') {
        return Piece(PieceColor::White, PieceType::Knight);
    }
    if (symbol == 'K') {
        return Piece(PieceColor::White, PieceType::King);
    }
    if (symbol == 'Q') {
        return Piece(PieceColor::White, PieceType::Queen);
    }
    if (symbol == 'p') {
        return Piece(PieceColor::Black, PieceType::Pawn);

    }
    if (symbol == 'r') {
        return Piece(PieceColor::Black, PieceType::Rook);

    }
    if (symbol == 'b') {
        return Piece(PieceColor::Black, PieceType::Bishop);

    }
    if (symbol == 'n') {
        return Piece(PieceColor::Black, PieceType::Knight);

    }
    if (symbol == 'k') {
        return Piece(PieceColor::Black, PieceType::King);

    }
    if (symbol == 'q') {
        return Piece(PieceColor::Black, PieceType::Queen);
    }

    return std::nullopt;
}

PieceColor Piece::color() const {
    return color_;
}

PieceType Piece::type() const {
    return type_;
}

bool operator==(const Piece& lhs, const Piece& rhs) {
    if ((lhs.color() == rhs.color()) && (lhs.type() == rhs.type())) {
        return true;
    }
    else {
        return false;
    }
}

std::ostream& operator<<(std::ostream& os, const Piece& piece) {
    if (piece.color() == PieceColor::White) {
        if (piece.type() == PieceType::Pawn) {
            return os<<"P";
        }
        if (piece.type() == PieceType::Rook) {
            return os<<"R";
        }
        if (piece.type() == PieceType::Bishop) {
            return os<<"B";
        }
        if (piece.type() == PieceType::Knight) {
            return os<<"N";
        }
        if (piece.type() == PieceType::King) {
            return os<<"K";
        }
        if (piece.type() == PieceType::Queen) {
            return os<<"Q";
        }
    }

    else {
        if (piece.type() == PieceType::Pawn) {
            return os<<"p";
        }
        if (piece.type() == PieceType::Rook) {
            return os<<"r";
        }
        if (piece.type() == PieceType::Bishop) {
            return os<<"b";
        }
        if (piece.type() == PieceType::Knight) {
            return os<<"n";
        }
        if (piece.type() == PieceType::King) {
            return os<<"k";
        }
        if (piece.type() == PieceType::Queen) {
            return os<<"q";
        }
    }
    return os;
}

PieceColor operator!(PieceColor color) {
    if (color == PieceColor::Black) {
        return PieceColor::White;
    }
    else {
        return PieceColor::Black;
    }
}