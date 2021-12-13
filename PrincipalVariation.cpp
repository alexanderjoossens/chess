#include "PrincipalVariation.hpp"

#include <ostream>


bool PrincipalVariation::isMate() const {
    return isMate_;
}

void PrincipalVariation::setMate(bool value) {
    this->isMate_=value;
}

int PrincipalVariation::score() const {
    return score_;
}

std::size_t PrincipalVariation::length() const {
    return pv.size();
}

PrincipalVariation::MoveIter PrincipalVariation::begin() const {
    return pv.begin();
}

PrincipalVariation::MoveIter PrincipalVariation::end() const {
    return pv.end();
}

std::ostream& operator<<(std::ostream& os, const PrincipalVariation& pv) {
    (void)pv;
    return os;
}
