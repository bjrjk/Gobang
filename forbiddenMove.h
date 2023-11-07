#pragma once
#include "gobang.h"
#include "grid.hpp"

class ForbiddenMoveJudger {
private:
    ChessboardGrid & grid;
public:
    ForbiddenMoveJudger(ChessboardGrid & grid): grid(grid) {}
    bool isForbiddenMove(ChessPiece, int, int);
};