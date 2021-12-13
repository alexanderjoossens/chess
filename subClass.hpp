//
// Created by alexander on 11.12.21.
//

#ifndef CPLCHESS_SUBCLASS_HPP
#define CPLCHESS_SUBCLASS_HPP
#include "Engine.hpp"
typedef struct bnode {

    int eval;
    std::optional<Move> move;
    struct bnode *onder;
} MinMaxNode;

class subClass: public Engine {

    std::string name() const override;
    std::string version() const override;
    std::string author() const override;

    void newGame() override;
    PrincipalVariation pv(
            const Board& board,
            const TimeInfo::Optional& timeInfo = std::nullopt
    ) override;

private:

};




#endif //CPLCHESS_SUBCLASS_HPP

