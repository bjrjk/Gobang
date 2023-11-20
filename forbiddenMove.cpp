#include <algorithm>
#include "forbiddenMove.h"

ForbiddenChessChainType ForbiddenMoveJudger::isAnyForbiddenChessChain(SingleChessChainStatus & status) {
    return ForbiddenChessChainType::None;
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

#define CheckForbiddenChessChain(TYPE, COUNT, CONDITION) \
        do { \
            unsigned TYPE##Count = std::count_if(forbiddenChessChainType4DifferentDirections, \
                forbiddenChessChainType4DifferentDirections + ChessChainNumber, \
                [](ForbiddenChessChainType type) { return (CONDITION); } \
            ); \
            if (TYPE##Count >= COUNT) return true; \
        } while(0)

    CheckForbiddenChessChain(Three, 2, type == ForbiddenChessChainType::AliveThree);
    CheckForbiddenChessChain(Four, 2, type == ForbiddenChessChainType::AliveFour || type == ForbiddenChessChainType::OneSideBlockingFour);
    CheckForbiddenChessChain(Long, 1, type == ForbiddenChessChainType::LongContiguous);

#undef CheckForbiddenChessChain

    return false;
}