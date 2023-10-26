#pragma once
#include <cstring>
#include <bitset>
#include <cstdint>

#define BitsetWithGivenSize std::bitset<BITSET_SIZE>


template <std::size_t BITSET_SIZE>
class ChessboardLineBinaryGrid: public BitsetWithGivenSize {
    static_assert(BITSET_SIZE <= sizeof(uint64_t) * 8, 
        "Programmer defined custom operations only support basic integer types!");
public:
    // Initialized with all bits set
    ChessboardLineBinaryGrid(): BitsetWithGivenSize() {
        this->set();
    }
    uint64_t getContiguousZeroCount(uint64_t position, uint64_t * leftOnePosition = NULL, uint64_t * rightOnePosition = NULL) {
        assert(position < BITSET_SIZE);
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
        if ((int64_t) position - (int64_t) rightZeroCount < 0) rightZeroCount -= 64 - BITSET_SIZE;
        if (leftZeroCount == 64 && rightZeroCount == 64) {
            if (leftOnePosition) *leftOnePosition = -1;
            if (rightOnePosition) *rightOnePosition = -1;
            return BITSET_SIZE;
        }
        if (leftOnePosition) *leftOnePosition = (position + leftZeroCount) % BITSET_SIZE;
        if (rightOnePosition) *rightOnePosition = (position - rightZeroCount - 1 + BITSET_SIZE) % BITSET_SIZE;
        return leftZeroCount + rightZeroCount;
    }
};

#undef BitsetWithGivenSize 