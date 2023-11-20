#pragma once
#include <cstring>
#include <bitset>
#include <cstdint>
#include <cassert>
#include <functional>
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
    static uint64_t countOnesForRawValue(uint64_t value, int start = 0, int end = 63) { // [start, end] in range [0, 63]
        uint64_t mask, result;
        if (end - start + 1 == 64) mask = -1;
        else mask = ((1ull << (end - start + 1)) - 1) << start;
        asm volatile (
            "popcnt %[maskedValue], %[result]\n"
            : [result] "=r" (result)
            : [maskedValue] "r" (value & mask)
        );
        return result;
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
    // Left indicate maximum index (63); Right indicate minimum index (0).
    uint64_t getContiguousZeroCount(uint64_t position, uint64_t * leftOnePosition = NULL, uint64_t * rightOnePosition = NULL) const {
        assert(position < bitsetSize);
        if ((*this)[position]) {
            if (leftOnePosition) *leftOnePosition = position + 1;
            if (rightOnePosition) *rightOnePosition = position;
            return 0;
        }
        uint64_t value = this->to_ullong();
        uint64_t leftZeroCount, rightZeroCount;
        asm volatile ( // amd64 architecture
            "movq %[position], %%rcx\n"
            "rorq %%cl, %[value]\n"
            "tzcntq %[value], %[leftZeroCount]\n"
            "lzcntq %[value], %[rightZeroCount]\n"
            : [leftZeroCount] "=&r" (leftZeroCount), [rightZeroCount] "=&r" (rightZeroCount)
            : [value] "r" (value), [position] "rm" (position)
            : "rcx"
        );
        if (leftZeroCount == 64 && rightZeroCount == 64) {
            if (leftOnePosition) *leftOnePosition = bitsetSize;
            if (rightOnePosition) *rightOnePosition = -1;
            return bitsetSize;
        } else {
            if ((int64_t) position + (int64_t) leftZeroCount >= 64) leftZeroCount -= 64 - bitsetSize;
            if ((int64_t) position - (int64_t) rightZeroCount < 0) rightZeroCount -= 64 - bitsetSize;
        }
        if (leftOnePosition) *leftOnePosition = (position + leftZeroCount) % bitsetSize;
        if (rightOnePosition) *rightOnePosition = (position - rightZeroCount - 1 + bitsetSize) % bitsetSize;
        return leftZeroCount + rightZeroCount;
    }
    uint64_t getContiguousZeroCountNonRotate(uint64_t position, uint64_t * leftOnePosition = NULL, uint64_t * rightOnePosition = NULL) const {
        uint64_t leftOnePositionValue, rightOnePositionValue;
        getContiguousZeroCount(position, &leftOnePositionValue, &rightOnePositionValue);
        if (rightOnePositionValue > position) rightOnePositionValue = -1;
        if (leftOnePositionValue < position) leftOnePositionValue = bitsetSize;
        if (leftOnePosition) *leftOnePosition = leftOnePositionValue;
        if (rightOnePosition) *rightOnePosition = rightOnePositionValue;
        return (int64_t) leftOnePositionValue - (int64_t) rightOnePositionValue - 1;
    }
    uint64_t getContiguousOneCountNonRotate(uint64_t position, uint64_t * leftZeroPosition = NULL, uint64_t * rightZeroPosition = NULL) {
        uint64_t result;
        this->flip();
        result = getContiguousZeroCountNonRotate(position, leftZeroPosition, rightZeroPosition);
        this->flip();
        return result;
    }
    uint64_t findFirstZeroAscendingNonRotate(uint64_t position) const {
        uint64_t value = this->to_ullong(), trailingActualOneCount;
        value = ~(value >> position);
        asm volatile ( // amd64 architecture
            "tzcntq %[value], %[trailingZeroCount]\n"
            : [trailingZeroCount] "=r" (trailingActualOneCount)
            : [value] "r" (value)
        );
        position += trailingActualOneCount;
        if (position >= bitsetSize) position = bitsetSize;
        return position;
    }
    uint64_t countOnes(int start = 0, int end = 63) const {
        assert(start <= end);
        uint64_t value = this->to_ullong();
        return this->countOnesForRawValue(value, start, end);
    }
    uint64_t countZeros(int start = 0, int end = 63) const {
        assert(start <= end);
        uint64_t value = this->to_ullong();
        return this->countOnesForRawValue(~value, start, end);
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

    ChessboardLineBinaryGrid<SIZE> grids[PIECE_END + 1][SIZEOF_ENUMCLASS(ChessboardLineType)][DIAGONAL_SIZE];
public:
    ChessboardGrid() {
        for (int k = EMPTY; k <= PIECE_END; k++) {
            for (int i = 0; i < SIZE; i++) {
                grids[k][C2MI(ChessboardLineType::ULLRDiagonal)][i].resizeAndSet(i + 1);
                grids[k][C2MI(ChessboardLineType::ULLRDiagonal)][DIAGONAL_SIZE - 1 - i].resizeAndSet(i + 1);
                grids[k][C2MI(ChessboardLineType::LLURDiagonal)][i].resizeAndSet(i + 1);
                grids[k][C2MI(ChessboardLineType::LLURDiagonal)][DIAGONAL_SIZE - 1 - i].resizeAndSet(i + 1);
            }
        }
    }
    ChessPiece get(uint64_t x, uint64_t y) const {
        assert(x < SIZE && y < SIZE);
        if (!grids[BOT][C2MI(ChessboardLineType::LINE)][x][y]) return BOT;
        else if (!grids[PLAYER][C2MI(ChessboardLineType::LINE)][x][y]) return PLAYER;
        else return EMPTY;
    }
    void set(int x, int y, ChessPiece value) {
        constexpr int ChessboardLineCount = 4;
		ChessboardLine ChessboardLineArr[ChessboardLineCount] = {
			ChessboardLine(ChessboardLineType::LINE, x, 0), // 行
			ChessboardLine(ChessboardLineType::ROW, 0, y), // 列
			ChessboardLine(ChessboardLineType::ULLRDiagonal, x, y), // 左上-右下对角线
			ChessboardLine(ChessboardLineType::LLURDiagonal, x, y) // 右上-左下对角线
		};
        switch (value) {
            case EMPTY: {
                for (int i = 0; i < ChessboardLineCount; i++) {
                    grids[EMPTY][C2MI(ChessboardLineArr[i].getType())][ChessboardLineArr[i].getUniqueID()]
                        .set(ChessboardLineArr[i].getIndex(x, y));
                    grids[BOT][C2MI(ChessboardLineArr[i].getType())][ChessboardLineArr[i].getUniqueID()]
                        .set(ChessboardLineArr[i].getIndex(x, y));
                    grids[PLAYER][C2MI(ChessboardLineArr[i].getType())][ChessboardLineArr[i].getUniqueID()]
                        .set(ChessboardLineArr[i].getIndex(x, y));
                }
                break;
            }
            case BOT: {
                for (int i = 0; i < ChessboardLineCount; i++) {
                    grids[EMPTY][C2MI(ChessboardLineArr[i].getType())][ChessboardLineArr[i].getUniqueID()]
                        .reset(ChessboardLineArr[i].getIndex(x, y));
                    grids[BOT][C2MI(ChessboardLineArr[i].getType())][ChessboardLineArr[i].getUniqueID()]
                        .reset(ChessboardLineArr[i].getIndex(x, y));
                    grids[PLAYER][C2MI(ChessboardLineArr[i].getType())][ChessboardLineArr[i].getUniqueID()]
                        .set(ChessboardLineArr[i].getIndex(x, y));
                }
                break;
            }
            case PLAYER: {
                for (int i = 0; i < ChessboardLineCount; i++) {
                    grids[EMPTY][C2MI(ChessboardLineArr[i].getType())][ChessboardLineArr[i].getUniqueID()]
                        .reset(ChessboardLineArr[i].getIndex(x, y));
                    grids[BOT][C2MI(ChessboardLineArr[i].getType())][ChessboardLineArr[i].getUniqueID()]
                        .set(ChessboardLineArr[i].getIndex(x, y));
                    grids[PLAYER][C2MI(ChessboardLineArr[i].getType())][ChessboardLineArr[i].getUniqueID()]
                        .reset(ChessboardLineArr[i].getIndex(x, y));
                }
                break;
            }
            default:
                break;
        }
    }
    void lambdaForTraverseChessboardLine(ChessboardLine &line, std::function<void (ChessPiece, int, int, ChessPiece, ChessPiece)> const & lambda) {
        // void lambda(ChessPiece currentPiece, int count, int position, ChessPiece leftOutOfBoundPiece, ChessPiece rightOutOfBoundPiece);
        int lineTypedIndex = C2MI(line.getType());
        uint64_t uniqueID = line.getUniqueID();
        uint64_t bitsetSize = line.size();
        auto &emptyGrids = grids[EMPTY][lineTypedIndex][uniqueID], &botGrids = grids[BOT][lineTypedIndex][uniqueID], &playerGrids = grids[PLAYER][lineTypedIndex][uniqueID];
        auto getPleceInChessboardLine = [&] (int pos) {
                if (!grids[BOT][lineTypedIndex][uniqueID][pos]) return BOT;
                else if (!grids[PLAYER][lineTypedIndex][uniqueID][pos]) return PLAYER;
                else return EMPTY;
        };
        for (uint64_t i = emptyGrids.findFirstZeroAscendingNonRotate(0); i < bitsetSize; i = emptyGrids.findFirstZeroAscendingNonRotate(i)) {
            ChessPiece currentPiece = getPleceInChessboardLine(i);
            uint64_t contiguousChessCount, leftOnePosition, rightOnePosition;
            contiguousChessCount = grids[currentPiece][lineTypedIndex][uniqueID].getContiguousZeroCountNonRotate(i, &leftOnePosition, &rightOnePosition);
            ChessPiece leftOutOfBoundPiece, rightOutOfBoundPiece;
            if (leftOnePosition == bitsetSize) leftOutOfBoundPiece = NOT_EXIST;
            else leftOutOfBoundPiece = getPleceInChessboardLine(leftOnePosition);
            if (rightOnePosition == -1) rightOnePosition = NOT_EXIST;
            else rightOutOfBoundPiece = getPleceInChessboardLine(rightOnePosition);
            lambda(currentPiece, contiguousChessCount, i, leftOutOfBoundPiece, rightOutOfBoundPiece);
            i = leftOnePosition;
        }
    }
    void getSingleChessChainStatus(SingleChessChainStatus & status) {
        assert(status.chessType == BOT || status.chessType == PLAYER);
        assert(this->get(status.dropPosition.x, status.dropPosition.y) != ChessPieceAdversaryMapper[status.chessType]);
        uint64_t lineUniqueID = status.chessboardLine.getUniqueID();
        uint64_t chessIndex = status.chessboardLine.getIndex(status.dropPosition.x, status.dropPosition.y);
        auto &emptyGrids = grids[EMPTY][C2MI(status.lineType)][lineUniqueID],
            &selfGrids = grids[status.chessType][C2MI(status.lineType)][lineUniqueID],
            &adversaryGrids = grids[ChessPieceAdversaryMapper[status.chessType]][C2MI(status.lineType)][lineUniqueID];
        adversaryGrids.getContiguousOneCountNonRotate(chessIndex,
            reinterpret_cast<uint64_t *>(&status.adversaryLeftAdjacentIndex),
            reinterpret_cast<uint64_t *>(&status.adversaryRightAdjacentIndex)
        );
        assert(status.adversaryLeftAdjacentIndex - 1 >= 0 && status.adversaryRightAdjacentIndex + 1 < emptyGrids.size());
        emptyGrids.getContiguousOneCountNonRotate(status.adversaryLeftAdjacentIndex - 1, NULL, reinterpret_cast<uint64_t *>(&status.selfLeftmostIndex));
        if (emptyGrids.getContiguousOneCountNonRotate(status.adversaryRightAdjacentIndex + 1, reinterpret_cast<uint64_t *>(&status.selfRightmostIndex), NULL) == 0)
            status.selfRightmostIndex = status.adversaryRightAdjacentIndex + 1;
        assert(emptyGrids.countZeros(status.selfRightmostIndex, status.selfLeftmostIndex) == selfGrids.countZeros(status.selfRightmostIndex, status.selfLeftmostIndex));
        status.selfChessCount = selfGrids.countZeros(status.selfRightmostIndex, status.selfLeftmostIndex);
    }
};
