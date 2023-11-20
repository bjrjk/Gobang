#include <cstdio>
#include <cstdint>
#include <cassert>
#include "grid.hpp"

using namespace std;

void testGetContiguousZeroCount1() {
    ChessboardLineBinaryGrid<15> grid;
    uint64_t leftOnePosition, rightOnePosition, result;

    grid.set();
    grid.flip(0);
    grid.flip(1);
    grid.flip(2);

    result = grid.getContiguousZeroCount(1, &leftOnePosition, &rightOnePosition);
    printf("%lld %lld %lld\n", result, leftOnePosition, rightOnePosition);
    assert(result == 3 && leftOnePosition == 3 && rightOnePosition == 14);
    
    grid.set();
    grid.flip(0);
    grid.flip(3);
    grid.flip(4);
    grid.flip(5);
    grid.flip(6);
    grid.flip(7);
    grid.flip(14);

    result = grid.getContiguousZeroCount(0, &leftOnePosition, &rightOnePosition);
    printf("%lld %lld %lld\n", result, leftOnePosition, rightOnePosition);
    assert(result == 2 && leftOnePosition == 1 && rightOnePosition == 13);

    result = grid.getContiguousZeroCount(4, &leftOnePosition, &rightOnePosition);
    printf("%lld %lld %lld\n", result, leftOnePosition, rightOnePosition);
    assert(result == 5 && leftOnePosition == 8 && rightOnePosition == 2);

    grid.set();
    grid.flip(0);
    grid.flip(1);
    grid.flip(2);
    grid.flip(13);
    grid.flip(14);

    result = grid.getContiguousZeroCount(1, &leftOnePosition, &rightOnePosition);
    printf("%lld %lld %lld\n", result, leftOnePosition, rightOnePosition);
    assert(result == 5 && leftOnePosition == 3 && rightOnePosition == 12);
}

void testGetContiguousZeroCount2() {
    ChessboardLineBinaryGrid<32> grid(15);
    uint64_t leftOnePosition, rightOnePosition, result;

    grid.set();
    grid.flip(0);
    grid.flip(1);
    grid.flip(2);

    result = grid.getContiguousZeroCount(1, &leftOnePosition, &rightOnePosition);
    printf("%lld %lld %lld\n", result, leftOnePosition, rightOnePosition);
    assert(result == 3 && leftOnePosition == 3 && rightOnePosition == 14);

    grid.set();
    grid.flip(0);
    grid.flip(3);
    grid.flip(4);
    grid.flip(5);
    grid.flip(6);
    grid.flip(7);
    grid.flip(14);

    result = grid.getContiguousZeroCount(0, &leftOnePosition, &rightOnePosition);
    printf("%lld %lld %lld\n", result, leftOnePosition, rightOnePosition);
    assert(result == 2 && leftOnePosition == 1 && rightOnePosition == 13);

    result = grid.getContiguousZeroCount(4, &leftOnePosition, &rightOnePosition);
    printf("%lld %lld %lld\n", result, leftOnePosition, rightOnePosition);
    assert(result == 5 && leftOnePosition == 8 && rightOnePosition == 2);

    grid.set();
    grid.flip(0);
    grid.flip(1);
    grid.flip(2);
    grid.flip(13);
    grid.flip(14);

    result = grid.getContiguousZeroCount(1, &leftOnePosition, &rightOnePosition);
    printf("%lld %lld %lld\n", result, leftOnePosition, rightOnePosition);
    assert(result == 5 && leftOnePosition == 3 && rightOnePosition == 12);

    result = grid.getContiguousZeroCount(14, &leftOnePosition, &rightOnePosition);
    printf("%lld %lld %lld\n", result, leftOnePosition, rightOnePosition);
    assert(result == 5 && leftOnePosition == 3 && rightOnePosition == 12);
}

void testGetContiguousZeroCountNonRotate() {
    ChessboardLineBinaryGrid<32> grid(15);
    uint64_t leftOnePosition, rightOnePosition, result;

    grid.set();
    grid.flip(0);
    grid.flip(1);
    grid.flip(2);
    grid.flip(13);
    grid.flip(14);

    result = grid.getContiguousZeroCountNonRotate(1, &leftOnePosition, &rightOnePosition);
    printf("%lld %lld %lld\n", result, leftOnePosition, rightOnePosition);
    assert(result == 3 && leftOnePosition == 3 && rightOnePosition == -1);

    result = grid.getContiguousZeroCountNonRotate(14, &leftOnePosition, &rightOnePosition);
    printf("%lld %lld %lld\n", result, leftOnePosition, rightOnePosition);
    assert(result == 2 && leftOnePosition == 15 && rightOnePosition == 12);
}

void testFindFirstZeroAscendingNonRotate() {
    ChessboardLineBinaryGrid<32> grid(15);
    uint64_t result;

    grid.set();
    grid.flip(0);
    grid.flip(1);
    grid.flip(2);
    grid.flip(13);
    grid.flip(14);

    result = grid.findFirstZeroAscendingNonRotate(1);
    printf("%lld\n", result);
    assert(result == 1);

    result = grid.findFirstZeroAscendingNonRotate(14);
    printf("%lld\n", result);
    assert(result == 14);

    result = grid.findFirstZeroAscendingNonRotate(5);
    printf("%lld\n", result);
    assert(result == 13);
}

void testGetSingleChessChainStatus1() {
    ChessboardGrid grid;
    SingleChessChainStatus status(BOT, ChessPosition(0, 6), LINE);
    grid.set(0, 2, PLAYER);
    grid.set(0, 4, BOT);
    grid.set(0, 6, BOT);
    grid.set(0, 7, BOT);
    grid.set(0, 9, PLAYER);
    grid.getSingleChessChainStatus(status);
    printf("%lld %lld %lld %lld %lld\n", status.selfRightmostIndex, status.selfLeftmostIndex, status.selfChessCount,
        status.adversaryRightAdjacentIndex, status.adversaryLeftAdjacentIndex);
    assert(status.selfRightmostIndex == 4 && status.selfLeftmostIndex == 7 &&
        status.selfChessCount == 3 &&
        status.adversaryRightAdjacentIndex == 2 && status.adversaryLeftAdjacentIndex == 9);
}

void testGetSingleChessChainStatus2() {
    ChessboardGrid grid;
    SingleChessChainStatus status(BOT, ChessPosition(0, 6), LINE);
    grid.set(0, 5, BOT);
    grid.set(0, 6, BOT);
    grid.set(0, 14, BOT);
    grid.getSingleChessChainStatus(status);
    printf("%lld %lld %lld %lld %lld\n", status.selfRightmostIndex, status.selfLeftmostIndex, status.selfChessCount,
        status.adversaryRightAdjacentIndex, status.adversaryLeftAdjacentIndex);
    assert(status.selfRightmostIndex == 5 && status.selfLeftmostIndex == 14 &&
        status.selfChessCount == 3 &&
        status.adversaryRightAdjacentIndex == -1 && status.adversaryLeftAdjacentIndex == 15);
}

void testGetSingleChessChainStatus3() {
    ChessboardGrid grid;
    SingleChessChainStatus status(PLAYER, ChessPosition(6, 0), ROW);
    grid.set(4, 0, BOT);
    grid.set(5, 0, PLAYER);
    grid.set(6, 0, PLAYER);
    grid.set(7, 0, PLAYER);
    grid.set(8, 0, BOT);
    grid.getSingleChessChainStatus(status);
    printf("%lld %lld %lld %lld %lld\n", status.selfRightmostIndex, status.selfLeftmostIndex, status.selfChessCount,
        status.adversaryRightAdjacentIndex, status.adversaryLeftAdjacentIndex);
    assert(status.selfRightmostIndex == 5 && status.selfLeftmostIndex == 7 &&
        status.selfChessCount == 3 &&
        status.adversaryRightAdjacentIndex == 4 && status.adversaryLeftAdjacentIndex == 8);
}

void testGetSingleChessChainStatus4() {
    ChessboardGrid grid;
    SingleChessChainStatus status(PLAYER, ChessPosition(7, 7), ULLRDiagonal);
    grid.set(0, 0, BOT);
    grid.set(1, 1, PLAYER);
    grid.set(2, 2, PLAYER);
    grid.set(3, 3, PLAYER);
    grid.set(5, 5, PLAYER);
    grid.set(8, 8, BOT);
    grid.set(14, 14, BOT);
    grid.getSingleChessChainStatus(status);
    printf("%lld %lld %lld %lld %lld\n", status.selfRightmostIndex, status.selfLeftmostIndex, status.selfChessCount,
        status.adversaryRightAdjacentIndex, status.adversaryLeftAdjacentIndex);
    assert(status.selfRightmostIndex == 1 && status.selfLeftmostIndex == 5 &&
        status.selfChessCount == 4 &&
        status.adversaryRightAdjacentIndex == 0 && status.adversaryLeftAdjacentIndex == 8);
}

int main() {
    testGetContiguousZeroCount1();
    testGetContiguousZeroCount2();
    testGetContiguousZeroCountNonRotate();
    testFindFirstZeroAscendingNonRotate();
    testGetSingleChessChainStatus1();
    testGetSingleChessChainStatus2();
    testGetSingleChessChainStatus3();
    testGetSingleChessChainStatus4();
}
