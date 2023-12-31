#pragma once
#include "gobang.h"
#include "grid.hpp"

enum class ForbiddenChessChainType {
    None, // 未形成禁手局势
    AliveThree, // 活三：连三或跳三
    AliveFour, // 活四：紧密相连的同色四子
    OneSideBlockingFour, // 冲四：除活四外可以形成五子的情况
    LongContiguous, // 长连禁手
};

class ForbiddenMoveJudger {
private:
    ChessboardGrid & grid;
public:
    ForbiddenMoveJudger(ChessboardGrid & grid): grid(grid) {}
    bool isForbiddenMove(ChessPiece, int, int);
    ForbiddenChessChainType isAnyForbiddenChessChain(SingleChessChainStatus &);
    ForbiddenChessChainType AliveThreeJudger();
    ForbiddenChessChainType AliveFourJudger();
    ForbiddenChessChainType OneSideBlockingFourJudger();
    ForbiddenChessChainType LongContiguousJudger();
    constexpr static ForbiddenChessChainType (ForbiddenMoveJudger::*const ForbiddenChessChainJudger[])() = {
        nullptr,
        &ForbiddenMoveJudger::AliveThreeJudger,
        &ForbiddenMoveJudger::AliveFourJudger,
        &ForbiddenMoveJudger::OneSideBlockingFourJudger,
        &ForbiddenMoveJudger::LongContiguousJudger
    };
};