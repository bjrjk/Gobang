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

int main() {
    testGetContiguousZeroCount1();
    testGetContiguousZeroCount2();
    testGetContiguousZeroCountNonRotate();
}