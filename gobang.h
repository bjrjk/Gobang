#pragma once
#include <cassert>
#include <algorithm>

constexpr int SIZE = 15; //棋盘边长
constexpr int DIAGONAL_SIZE = 29; //棋盘单向对角线个数

//二维数组中的数值代表棋盘中摆放的棋子，每种数值代表一种棋子
enum ChessPiece {
	NOT_EXIST = -1, //该位置不存在（数组越界）
	EMPTY = 0, //该位置为空
	PIECE_START,
	BOT = PIECE_START, //该位置为机器人的棋子
	PLAYER, //该位置为人类的棋子
	PIECE_END = PLAYER
};

const ChessPiece ChessPieceAdversaryMapper[] = {
	NOT_EXIST,
	PLAYER,
	BOT
};

enum ChessboardLineType {
	LINE, // 行
	ROW, // 列
	ULLRDiagonal, // 左上右下对角线 Upper Left - Lower Right
	LLURDiagonal, // 左下右上对角线 Lower Left - Upper Right
	LINE_TYPE_END = LLURDiagonal
};

struct ChessPosition { //用来表示棋子位置的数据结构
	int x, y;
	ChessPosition(int x, int y): x(x), y(y) {}
	ChessPosition(): ChessPosition(0, 0) {}
};

class ChessboardLine {
	ChessboardLineType type;
	int x, y; // 棋盘中第一优先靠左、第二优先靠上元素的横纵坐标
	int _size;
public:
	ChessboardLine(ChessboardLineType type, int x, int y): type(type) {
		switch (type) {
			case LINE: {
				this->x = x;
				this->y = 0;
				break;
			}
			case ROW: {
				this->x = 0;
				this->y = y;
				break;
			}
			case ULLRDiagonal: {
				int shift = std::min(x, y);
				this->x = x - shift;
				this->y = y - shift;
				break;
			}
			case LLURDiagonal: {
				int shift = std::min(SIZE - 1 - x, y);
				this->x = x + shift;
				this->y = y - shift;
				break;
			}
		}
		_size = NOT_EXIST;
	}
	inline uint64_t getUniqueID() const {
		switch (type) {
			case LINE:
			{
				return this->x;
			}
			case ROW:
			{
				return this->y;
			}
			case ULLRDiagonal:
			{
				return (!this->x) * (SIZE - 1 - this->y) + (!!this->x) * (SIZE - 1 + this->x);
			}
			case LLURDiagonal:
			{
				return this->x + this->y;
			}
		}
		assert(false);
		return NOT_EXIST;
	}
	inline ChessboardLineType getType() const {
		return this->type;
	}
	int size() {
		if (_size != NOT_EXIST) return _size;
		switch (type) {
			case LINE:
			case ROW: {
				return _size = SIZE;
			}
			case ULLRDiagonal: {
				return _size = std::min(SIZE - x, SIZE - y);
			}
			case LLURDiagonal: {
				if (y == 0)
					return _size = x + 1;
				else // x == SIZE
					return _size = SIZE - y;
			}
		}
		assert(false);
		return NOT_EXIST;
	}
	int i(int index) const {
		switch (type) {
			case LINE: {
				return x;
			}
			case ROW: {
				return index;
			}
			case ULLRDiagonal: {
				return x + index;
			}
			case LLURDiagonal: {
				return x - index;
			}
		}
		assert(false);
		return NOT_EXIST;
	}
	int j(int index) const {
		switch (type) {
			case LINE: {
				return index;
			}
			case ROW: {
				return y;
			}
			case ULLRDiagonal:
			case LLURDiagonal:
			{
				return y + index;
			}
		}
		assert(false);
		return NOT_EXIST;
	}
	int getIndex(int i, int j) const {
		switch (type) {
			case LINE: {
				assert(this->x == i);
				return j;
			}
			case ROW: {
				assert(this->y == j);
				return i;
			}
			case ULLRDiagonal: {
				assert(i - this->x == j - this->y);
				return j - this->y;
			}
			case LLURDiagonal: {
				assert(this->x - i == j - this->y);
				return j - this->y;
			}
		}
		assert(false);
		return NOT_EXIST;
	}
};

struct SingleChessChainStatus {
	// Input
	const ChessPiece chessType; // 所查找的（我方）棋子类型
	const ChessPosition dropPosition; // 我方落子的位置
	const ChessboardLineType lineType; // 查找的棋盘线方向
	// Output
	ChessboardLine chessboardLine; // 棋盘线数据结构
	int64_t selfLeftmostIndex, selfRightmostIndex; // 以我方落子的位置为中心，我方连续棋子的最高、最低位置。该连续可以不是紧密连续，中间可以有空位，但是不能被对方棋子所插入
	uint64_t selfChessCount; // 我方连续棋子的个数
	int64_t adversaryLeftAdjacentIndex, adversaryRightAdjacentIndex; // 超出我方连续棋子外的、与我方连续棋子最近的对方棋子的左右侧位置
	SingleChessChainStatus(ChessPiece chessType, ChessPosition dropPosition, ChessboardLineType lineType):
		chessType(chessType), dropPosition(dropPosition), lineType(lineType), chessboardLine(lineType, dropPosition.x, dropPosition.y) {}
};
