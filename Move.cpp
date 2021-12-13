#include "Move.hpp"

#include <ostream>
#include <optional>
#include<string>

Move::Move(const Square& from, const Square& to,
           const std::optional<PieceType>& promotion): from_(from),to_(to)
{
    //from en to staan hierboven al achter die dubbele punt dus moet nietmeer hier
//    from_ = from;
//    to_ = to;
    promotion_ = promotion;
}

//////////////////////  M ////////////

std::optional<PieceType> fromSymbol(char symbol) {
    switch(symbol) {
        case 'p':
            return PieceType::Pawn;
        case 'n':
            return PieceType::Knight;
        case 'b':
            return PieceType::Bishop;
        case 'r':
            return PieceType::Rook;
        case 'q':
            return PieceType::Queen;
        case 'k':
            return PieceType::King;
        default:
            return std::nullopt;
    }

}
Move::Optional Move::fromUci(const std::string& uci) { //aanpassen
    if (uci.length() ==4 || uci.length()==5)
    {
        std::optional<Square> from_opt = Square::fromName(uci.substr(0, 2));
        std::optional<Square> to_opt = Square::fromName(uci.substr(2, 2));
        if (!(from_opt.has_value() && to_opt.has_value())) {
            return std::nullopt;
        }
        Square from = from_opt.value();
        Square to = to_opt.value();
        std::optional<PieceType> type = std::nullopt;
        if (uci.length() == 5) {
            type = fromSymbol(uci[4]);
            if (!type.has_value()) {
                return std::nullopt;
            }
        }
        return Move(from, to, type);
    }
    else
    {
        return std::nullopt;
    }

    //////////////
//
//Move::Optional Move::fromUci(const std::string& uci) {
//    char from_col = uci.at(0);
//    char from_row = uci.at(1);
//    char to_col = uci.at(2);
//    char to_row = uci.at(3);
//    //char promotion = uci.at(4);
//
//    int from_col_int = 0;
//    int to_col_int = 0;
//
//    if (from_col == 'a') {
//        from_col_int = 0;
//    }
//    if (from_col == 'b') {
//        from_col_int = 1;
//    }
//    if (from_col == 'c') {
//        from_col_int = 2;
//    }
//    if (from_col == 'd') {
//        from_col_int = 3;
//    }
//    if (from_col == 'e') {
//        from_col_int = 4;
//    }
//    if (from_col == 'f') {
//        from_col_int = 5;
//    }
//    if (from_col == 'g') {
//        from_col_int = 6;
//    }
//    if (from_col == 'h') {
//        from_col_int = 7;
//    }
//
//
//    if (to_col == 'a') {
//        to_col_int = 0;
//    }
//    if (to_col == 'b') {
//        to_col_int = 1;
//    }
//    if (to_col == 'c') {
//        to_col_int = 2;
//    }
//    if (to_col == 'd') {
//        to_col_int = 3;
//    }
//    if (to_col == 'e') {
//        to_col_int = 4;
//    }
//    if (to_col == 'f') {
//        to_col_int = 5;
//    }
//    if (to_col == 'g') {
//        to_col_int = 6;
//    }
//    if (to_col == 'h') {
//        to_col_int = 7;
//    }
//
//    Index from_index = from_col_int + 8*((int)from_row-1-48);
//    Index to_index = to_col_int+ 8*((int)to_row-1-48);
//
//    Square from_Square = Square(from_index);
//    Square to_Square = Square(to_index);
//
//    Move move = Move(from_Square, to_Square);
//
//    return move;
}

Square Move::from() const {
    return from_;
}

Square Move::to() const {
    return to_;
}

std::optional<PieceType> Move::promotion() const {
    return promotion_;
}

std::ostream& operator<<(std::ostream& os, const Move& move) {
    Square from_move = move.from();
    Square to_move = move.to();
    Square::Coordinate from_file = from_move.file();
    Square::Coordinate from_row = from_move.rank();
    Square::Coordinate to_file = to_move.file();
    Square::Coordinate to_row = to_move.rank();
    int number=97;
    // 0 a, 1 b, ...
    // from_charvalue is de e in e1b2
    char from_charvalue = number+(from_file);
    char from_charrow = '1'+from_row;
    // 0 a, 1 b, ...
    // from_charvalue is de b in e1b2
    char to_charvalue = number+(to_file);
    char to_charrow ='1'+to_row;

    if (move.promotion()!=std::nullopt)
    {
        char promotion=0;
        if (move.promotion() == PieceType::Knight)
        {
            promotion = 'n';
        }
        if (move.promotion() == PieceType::Queen)
        {
            promotion = 'q';
        }
        if (move.promotion() == PieceType::Rook)
        {
            promotion = 'r';
        }
        if (move.promotion() == PieceType::Bishop)
        {
            promotion = 'b';
        }
        os<<std::string(1,from_charvalue)+from_charrow+to_charvalue+to_charrow+promotion;
    }
    else
    {
        os<<std::string(1,from_charvalue)+from_charrow+to_charvalue+to_charrow;
    }
    return os;
}

int value(PieceType type) {
    switch (type) {
        case PieceType::Queen :
            return 6;
        case PieceType::Rook :
            return 5;
        case PieceType::Bishop :
            return 4;
        case PieceType::Knight :
            return 3;
        case PieceType::Pawn :
            return 2;
        case PieceType::King :
            return 1;
        default:
            return 0;
    }
}

bool operator<(const Move& lhs, const Move& rhs) {
    if (lhs.from().index() < rhs.from().index()) {
        return true;
    }
    if (lhs.from().index() == rhs.from().index()) {
        if (lhs.to().index() < rhs.to().index()) {
            return true;
        }
        if (lhs.to().index() == rhs.to().index()) {
            if (lhs.promotion().has_value() && rhs.promotion().has_value()) {
                if (value(lhs.promotion().value()) < value(rhs.promotion().value())) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool operator==(const Move& lhs, const Move& rhs) {
//    if ((lhs.promotion()!=std::nullopt && rhs.promotion()!=std::nullopt) || (rhs.promotion()!=std::nullopt && lhs.promotion()==std::nullopt)) {
//        return false;
//    }
    return lhs.from()==rhs.from() && lhs.to()==rhs.to() && lhs.promotion() == rhs.promotion();
}

