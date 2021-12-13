//#ifndef CHESS_ENGINE_PRINCIPALVARIATION_HPP
//#define CHESS_ENGINE_PRINCIPALVARIATION_HPP
//
//#include "Move.hpp"
//#include "Piece.hpp"
//
//#include <iosfwd>
//#include <cstddef>

//class PrincipalVariation {
//public:
//
//    using MoveIter = Move*;
//
//    bool isMate() const;
//    int score() const;
//
//    std::size_t length() const;
//    MoveIter begin() const;
//    MoveIter end() const;
//};
//
//std::ostream& operator<<(std::ostream& os, const PrincipalVariation& pv);
//
//#endif


#ifndef CHESS_ENGINE_PRINCIPALVARIATION_HPP
#define CHESS_ENGINE_PRINCIPALVARIATION_HPP

#include "Move.hpp"
#include "Piece.hpp"

#include <iosfwd>
#include <cstddef>
#include <vector>

class PrincipalVariation {
public:

    using MoveIter = std::vector<Move>::const_iterator;

    bool isMate() const;
    int score() const;

    std::size_t length() const;
    MoveIter begin() const;
    MoveIter end() const;
    void setMate(const bool value);
    std::vector<Move> pv;

private:
    bool isMate_ = false;
    int score_ = 0;
};

std::ostream& operator<<(std::ostream& os, const PrincipalVariation& pv);

#endif