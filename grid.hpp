#pragma once
#include <cstring>
#include <bitset>
#include <cstdint>

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
    bool operator [](std::size_t pos) const {
        assert(pos < bitsetSize);
        return BitsetWithGivenSize::operator[](pos);
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
    void reset() = delete;
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
};

#undef BitsetWithGivenSize

class ChessboardGrid {
private:
    ChessboardLineBinaryGrid<15> lineGrids[15];
    ChessboardLineBinaryGrid<15> rowGrids[15];
    ChessboardLineBinaryGrid<15> ULLRDiagonalGrids[29], LLURDiagonalGrids[29];
public:
    ChessboardGrid() {
        for (int i = 0; i < 15; i++) {
            ULLRDiagonalGrids[i].resizeAndSet(i + 1);
            ULLRDiagonalGrids[28 - i].resizeAndSet(i + 1);
            LLURDiagonalGrids[i].resizeAndSet(i + 1);
            LLURDiagonalGrids[28 - i].resizeAndSet(i + 1);
        }
    }
};