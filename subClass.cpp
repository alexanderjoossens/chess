//
// Created by alexander on 11.12.21.
//

#include "subClass.hpp"
#include <iostream>
using namespace std;



std::string subClass::name() const
{
    return "AJSchaakEngine";
}

std::string subClass::version() const
{
    return "23";

}
std::string subClass::author() const
{
    return "AJ";

}
void subClass::newGame() { //Optional
    //log_ << "new game...";
}

// TODO change function
int calculateScore(const Board& board, PieceColor maximizing_color) {
    //cout << "calculateScore!";
    // check if in check, or checkmate or stalemate
    if (maximizing_color == PieceColor::White) {
        // TODO typecasten naar int?
        return board.getWhiteScore()-board.getBlackScore();
    }
    if (maximizing_color==PieceColor::Black) {
        return board.getBlackScore()-board.getWhiteScore();
    }
    else {
        return 0;
    }
}

bool kingIsGone(const Move& move, const Board& board) {
    Square to = move.to();
    if (board.piece(to)->type()==PieceType::King) {
        return true;
    }
    return false;
}

bool otherKingIsCheckMate(const Move& move, const Board& board) {
    Square to = move.to();
    if (board.piece(to)->type()==PieceType::King) {
        return true;
    }
    return false;
}

bool gameOver(const Board& board) {
//    cout << "checking if game over...\n";
    int nbKingsOnBoard = 0;
    for (int i=0;i<64;i++) {
        if (board.piece(Square::fromIndex(i).value())->type() == PieceType::King) {
            nbKingsOnBoard += 1;
        }
    }
//    cout << "nb kings = ";
//    cout << nbKingsOnBoard;
//    cout << "\n";
    if (nbKingsOnBoard != 2) {
//        cout << "\nnot 2 kings on board, gameOver!\n";
        return true;
    }
    return false;
    // TODO: stalemate
}

int checkMate(const Board& board) {     // TODO: check if you can take attacking piece
    cout << "checkMate";

    bool isCheckMate;
    for (int i=0;i<64;i++) {
        isCheckMate = false;
        if (board.piece(Square::fromIndex(i).value())->type() == PieceType::King) {
            if (board.squareCheck(i, board.piece(Square::fromIndex(i).value())->color())) {
                isCheckMate = true;
                if (isCheckMate && ((i+1)%8!=0 && i<64)) {
                    if (board.squareCheck(i+1, board.piece(Square::fromIndex(i).value())->color())) {
                        isCheckMate=true;
                    }
                    else { isCheckMate = false; }
                }
                if (isCheckMate && ((i-1)%8!=7 && i>=0)) {
                    if (board.squareCheck(i+7, board.piece(Square::fromIndex(i).value())->color())) {
                        isCheckMate=true;
                    }
                    else { isCheckMate = false; }
                }
                if (isCheckMate && (i+7)%8!=7 && i<64) {
                    if (board.squareCheck(i+7, board.piece(Square::fromIndex(i).value())->color())) {
                        isCheckMate = true;
                    }
                    else { isCheckMate = false; }
                }
                if (isCheckMate && (i-7)%8!=0 && i>=0) {
                    if (board.squareCheck(i-7, board.piece(Square::fromIndex(i).value())->color())) {
                        isCheckMate = true;
                    }
                    else { isCheckMate = false; }
                }
                if (isCheckMate && i<64) {
                    if (board.squareCheck(i+8, board.piece(Square::fromIndex(i).value())->color())) {
                        isCheckMate = true;
                    }
                    else { isCheckMate = false; }
                }
                if (isCheckMate && i>=8) {
                    if (board.squareCheck(i-8, board.piece(Square::fromIndex(i).value())->color())) {
                        isCheckMate=true;
                    }
                    else {isCheckMate=false;}
                }
                if (isCheckMate && (i+9)%8!=0 && i<64) {
                    if (board.squareCheck(i+9, board.piece(Square::fromIndex(i).value())->color())) {
                        isCheckMate=true;
                    }
                    else { isCheckMate=false; }
                }
                if (isCheckMate && (i-9)%8!=7 && i>=0) {
                    if (board.squareCheck(i-9, board.piece(Square::fromIndex(i).value())->color())) {
                        isCheckMate=true;
                    }
                    else {isCheckMate=false;}
                }
            }
            if (isCheckMate) {
                if (board.piece(Square::fromIndex(i).value())->color()==PieceColor::Black) {
                    return 2;
                }
                if (board.piece(Square::fromIndex(i).value())->color()==PieceColor::White) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

MinMaxNode *minimax(const Board& board,
                    const TimeInfo::Optional& timeInfo, bool maximizing_player, PieceColor maximizing_color, std::vector<Move>& pvBestMoves, int depth, int alpha, int beta)
{
    MinMaxNode *p = nullptr;
    cout << "in minimax\n";

    int maxeval;
    int mineval;
    if (depth == 0 || gameOver(board))//onderaan depth // checkmate idk BASE CASE
    {
//        cout<< "depth = ";
//        cout << depth;
//        cout << "\n";
//        cout << gameOver(board);
//        cout << "\nand gameover = ";
//        cout << "\n";
        p = new MinMaxNode[sizeof(MinMaxNode)];
//        cout << "p =new MinMaxNode[sizeof(MinMaxNode)];\n";
        p->move=std::nullopt;
        p->eval= calculateScore(board, maximizing_color);
//        std::cout << "p->eval = ";
//        std::cout<<p->eval;
//        std::cout << "\n";
        p->next=(MinMaxNode *)NULL;
        return p;
    }
    MinMaxNode *c = nullptr;
    (void)timeInfo;

    std::vector<Move> moves; /////////////////////// Dit is wrs niet juist want geheugen wordt overschreven met recursive calls
//    cout << "std::vector<Move> moves = ;";
//    cout << "board.pseudoLegalMoves(moves);\n";
    board.pseudoLegalMoves(moves);


    if (maximizing_player)
    {
        p =new  MinMaxNode[sizeof(MinMaxNode)];

        maxeval = -std::numeric_limits<int>::max();

        for (int i=0;i<(int)moves.size();i++)
        {

            Board new_board = Board(board);
            new_board.makeMove(moves[i]);
            c = new MinMaxNode[sizeof(MinMaxNode)];

            c = minimax(new_board, std::nullopt, false, maximizing_color, pvBestMoves, depth - 1, alpha, beta);

            if (c->eval > maxeval)
            {
                maxeval = c->eval;
                p->eval = c->eval;
                p->move =moves[i];
                p->next = c;
            }
            if (c->eval > alpha)
            {
                alpha=c->eval;
            }
            if (beta <= alpha)
            {
                break;
            }
        }

        (void) pvBestMoves;
        return p;
    }
    else
    {
        p =new  MinMaxNode[sizeof(MinMaxNode)];
        mineval = std::numeric_limits<int>::max();
        for (int i=0;i<(int)moves.size();i++)
        {
            Board nieuwboard = Board(board);

            nieuwboard.makeMove(moves[i]);

            //c = new MinMaxNode[sizeof(MinMaxNode)];

            c = minimax(nieuwboard, std::nullopt, true, maximizing_color,pvBestMoves,depth-1,alpha,beta);

            if (c->eval < mineval)
            {
                mineval = c->eval;
                p->eval = c->eval;
                p->move=moves[i];
                p->next=c;
            }

            if (c->eval < beta)
            {
                beta=c->eval;
            }
            if (beta <= alpha)
            {
                break;
            }
        }
        (void) pvBestMoves;

        return p;
    }
}

void putMovesInPV(MinMaxNode *p, std::vector<Move>& pvBestMoves) {
    if (p->next != (MinMaxNode *)NULL) {
        pvBestMoves.push_back(p->move.value());
        putMovesInPV(p->next, pvBestMoves);
    }
}

PrincipalVariation subClass::pv( const Board& board, const TimeInfo::Optional& timeInfo) {
    // implement minmax
    cout << "in pv\n";
    PrincipalVariation current_pv = PrincipalVariation();
    MinMaxNode *node = nullptr;
    int alpha = -std::numeric_limits<int>::max();
    int beta = std::numeric_limits<int>::max();
    cout << "starting minimax...\n";
    node = minimax(board, timeInfo, true, board.turn(), current_pv.pv, 3, alpha, beta);
    cout << "made node out of minimax (contains eval, move, next)\n";
    cout << "putMoves(node, current_pv.pv)\n";
    putMovesInPV(node, current_pv.pv);
    if (abs(node->eval) == 900) {
        if (current_pv.length() == 0) {
            current_pv.setMate(true);
        }
    }
//    (void) board;
//    (void) timeInfo;
    cout << "return current_pv";
    return current_pv;
}
