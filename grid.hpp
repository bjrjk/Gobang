#pragma once
#include <cstring>
#include <bitset>
#include <cstdint>
#include <cassert>
#include "gobang.h"

#define BitsetWithGivenSize std::bitset<BITSET_SIZE>

template <std::size_t BITSET_SIZE>
class ChessboardLineBinaryGrid: public BitsetWithGivenSize {
    static_assert(BITSET_SIZE <= sizeof(uint64_t) * 8, 
        "Programmer defined custom operations only support basic integer types!");
private:
    const uint64_t bitsetSize;
    inline void clearHighBits() {
        if (bitsetSize < BITSET_SIZE) *this &= (1ULL << bitsetSize) - 1;
    }
public:
    // Initialized with all bits set
    ChessboardLineBinaryGrid(uint64_t bitsetSize = BITSET_SIZE)
        : BitsetWithGivenSize(), bitsetSize(bitsetSize) {
        assert(bitsetSize <= BITSET_SIZE);
        this->set();
    }
    void resizeAndSet(uint64_t bitsetSize) {
        assert(bitsetSize <= BITSET_SIZE);
        *const_cast<uint64_t *>(&this->bitsetSize) = bitsetSize;
        this->set();
    }
    bool operator ==(const ChessboardLineBinaryGrid& rhs) const = delete;
    bool operator !=(const ChessboardLineBinaryGrid& rhs) const = delete;
    bool operator [](std::size_t pos) const {
        assert(pos < bitsetSize);
        return BitsetWithGivenSize::operator[](pos);
    }
    void test() = delete;
    void all() = delete;
    void any() = delete;
    void none() = delete;
    void count() = delete;
    std::size_t size() const {
        return bitsetSize;
    }
    ChessboardLineBinaryGrid& set() {
        BitsetWithGivenSize::set();
        clearHighBits();
        return *this;
    }
    ChessboardLineBinaryGrid& set(std::size_t pos, bool value = true) {
        assert(pos < bitsetSize);
        BitsetWithGivenSize::set(pos, value);
        return *this;
    }
    ChessboardLineBinaryGrid& reset() {
        BitsetWithGivenSize::reset();
        clearHighBits();
        return *this;
    }
    ChessboardLineBinaryGrid& reset(std::size_t pos) {
        assert(pos < bitsetSize);
        BitsetWithGivenSize::reset(pos);
        return *this;
    }
    ChessboardLineBinaryGrid& flip() {
        BitsetWithGivenSize::flip();
        clearHighBits();
        return *this;
    }
    ChessboardLineBinaryGrid& flip(std::size_t pos) {
        assert(pos < bitsetSize);
        BitsetWithGivenSize::flip(pos);
        return *this;
    }
    uint64_t getContiguousZeroCount(uint64_t position, uint64_t * leftOnePosition = NULL, uint64_t * rightOnePosition = NULL) {
        assert(position < bitsetSize);
        uint64_t value = this->to_ullong();
        uint64_t leftZeroCount, rightZeroCount;
        asm volatile ( // amd64 architecture
            "movq %[position], %%rcx\n"
            "rorq %%cl, %[value]\n"
            "tzcntq %[value], %[leftZeroCount]\n"
            "lzcntq %[value], %[rightZeroCount]\n"
            : [leftZeroCount] "=r" (leftZeroCount), [rightZeroCount] "=r" (rightZeroCount)
            : [value] "r" (value), [position] "rm" (position)
            : "rcx"
        );
        if ((int64_t) position + (int64_t) leftZeroCount > 64) leftZeroCount -= 64 - bitsetSize;
        if ((int64_t) position - (int64_t) rightZeroCount < 0) rightZeroCount -= 64 - bitsetSize;
        if (leftZeroCount == 64 && rightZeroCount == 64) {
            if (leftOnePosition) *leftOnePosition = -1;
            if (rightOnePosition) *rightOnePosition = -1;
            return bitsetSize;
        }
        if (leftOnePosition) *leftOnePosition = (position + leftZeroCount) % bitsetSize;
        if (rightOnePosition) *rightOnePosition = (position - rightZeroCount - 1 + bitsetSize) % bitsetSize;
        return leftZeroCount + rightZeroCount;
    }
    uint64_t getContiguousZeroCountNonRotate(uint64_t position, uint64_t * leftOnePosition = NULL, uint64_t * rightOnePosition = NULL) {
        getContiguousZeroCount(position, leftOnePosition, rightOnePosition);
        if (*rightOnePosition > position) *rightOnePosition = -1;
        if (*leftOnePosition < position) *leftOnePosition = bitsetSize;
        return (int64_t) *leftOnePosition - (int64_t) *rightOnePosition - 1;
    }
};

#undef BitsetWithGivenSize

class ChessboardGrid {
private:
    /*
        grids[EMPTY][*][*] for Both Player & Bot chesses' occupation;
        grids[BOT][*][*] for Bot chesses' occupation;
        grids[PLAYER][*][*] for Player chesses' occupation.
        grids[*][LINE][i] for line occupation, i_{max} = 14, size of the third dimension is 15.
        grids[*][ROW][i] for row occupation, i_{max} = 14, size of the third dimension is 15.
        grids[*][ULLRDiagonal][i] for line occupation, i_{max} = 28, size of the third dimension is 29.
        grids[*][LLURDiagonal][i] for line occupation, i_{max} = 28, size of the third dimension is 29.

        0 is placed, 1 is unplaced.
    */

    ChessboardLineBinaryGrid<SIZE> grids[PIECE_END + 1][LINE_TYPE_END + 1][DIAGONAL_SIZE];
public:
    ChessboardGrid() {
        for (int k = EMPTY; k <= PIECE_END; k++) {
            for (int i = 0; i < SIZE; i++) {
                grids[k][ULLRDiagonal][i].resizeAndSet(i + 1);
                grids[k][ULLRDiagonal][DIAGONAL_SIZE - 1 - i].resizeAndSet(i + 1);
                grids[k][LLURDiagonal][i].resizeAndSet(i + 1);
                grids[k][LLURDiagonal][DIAGONAL_SIZE - 1 - i].resizeAndSet(i + 1);
            }
        }
    }
    ChessPiece get(uint64_t x, uint64_t y) const {
        assert(x < SIZE && y < SIZE);
        if (!grids[BOT][LINE][x][y]) return BOT;
        else if (!grids[PLAYER][LINE][x][y]) return PLAYER;
        else return EMPTY;
    }
    void set(int x, int y, ChessPiece value) {
        constexpr int ChessboardLineCount = 4;
		ChessboardLine ChessboardLineArr[ChessboardLineCount] = {
			ChessboardLine(LINE, x, 0), // 行
			ChessboardLine(ROW, 0, y), // 列
			ChessboardLine(ULLRDiagonal, x, y), // 左上-右下对角线
			ChessboardLine(LLURDiagonal, x, y) // 右上-左下对角线
		};
        switch (value) {
            case EMPTY: {
                for (int i = 0; i < ChessboardLineCount; i++) {
                    grids[EMPTY][ChessboardLineArr[i].getType()][ChessboardLineArr[i].getUniqueID()]
                        .set(ChessboardLineArr[i].getIndex(x, y));
                    grids[BOT][ChessboardLineArr[i].getType()][ChessboardLineArr[i].getUniqueID()]
                        .set(ChessboardLineArr[i].getIndex(x, y));
                    grids[PLAYER][ChessboardLineArr[i].getType()][ChessboardLineArr[i].getUniqueID()]
                        .set(ChessboardLineArr[i].getIndex(x, y));
                }
                break;
            }
            case BOT: {
                for (int i = 0; i < ChessboardLineCount; i++) {
                    grids[EMPTY][ChessboardLineArr[i].getType()][ChessboardLineArr[i].getUniqueID()]
                        .reset(ChessboardLineArr[i].getIndex(x, y));
                    grids[BOT][ChessboardLineArr[i].getType()][ChessboardLineArr[i].getUniqueID()]
                        .reset(ChessboardLineArr[i].getIndex(x, y));
                    grids[PLAYER][ChessboardLineArr[i].getType()][ChessboardLineArr[i].getUniqueID()]
                        .set(ChessboardLineArr[i].getIndex(x, y));
                }
                break;
            }
            case PLAYER: {
                for (int i = 0; i < ChessboardLineCount; i++) {
                    grids[EMPTY][ChessboardLineArr[i].getType()][ChessboardLineArr[i].getUniqueID()]
                        .reset(ChessboardLineArr[i].getIndex(x, y));
                    grids[BOT][ChessboardLineArr[i].getType()][ChessboardLineArr[i].getUniqueID()]
                        .set(ChessboardLineArr[i].getIndex(x, y));
                    grids[PLAYER][ChessboardLineArr[i].getType()][ChessboardLineArr[i].getUniqueID()]
                        .reset(ChessboardLineArr[i].getIndex(x, y));
                }
                break;
            }
        }
    }
};
