#include <algorithm>
#include "forbiddenMove.h"

static bool AliveThreeJudger();
static bool AliveFourJudger();
static bool OneSideBlockingFourJudger();
static bool LongContiguousJudger();

static bool (*ForbiddenChessChainJudger[])() = {
    nullptr,
    AliveThreeJudger,
    AliveFourJudger,
    OneSideBlockingFourJudger,
    LongContiguousJudger
};

ForbiddenChessChainType ForbiddenMoveJudger::isAnyForbiddenChessChain(SingleChessChainStatus & status) {

}

bool ForbiddenMoveJudger::isForbiddenMove(ChessPiece piece, int x, int y) {
    constexpr int ChessChainNumber = 4;
    SingleChessChainStatus multipleDirectionChessChainStatus[ChessChainNumber] = {
        SingleChessChainStatus(piece, ChessPosition(x, y), LINE),
        SingleChessChainStatus(piece, ChessPosition(x, y), ROW),
        SingleChessChainStatus(piece, ChessPosition(x, y), ULLRDiagonal),
        SingleChessChainStatus(piece, ChessPosition(x, y), LLURDiagonal)
    };
    ForbiddenChessChainType forbiddenChessChainType4DifferentDirections[ChessChainNumber];
    for (int i = 0; i < ChessChainNumber; i++)
        forbiddenChessChainType4DifferentDirections[i] = isAnyForbiddenChessChain(multipleDirectionChessChainStatus[i]);
    unsigned aliveThreeCount = std::count_if(forbiddenChessChainType4DifferentDirections,
        forbiddenChessChainType4DifferentDirections + ChessChainNumber,
        [](ForbiddenChessChainType type) { type == ForbiddenChessChainType::AliveThree;}
    );

}