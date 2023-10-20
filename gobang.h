#pragma once

//二维数组中的数值代表棋盘中摆放的棋子，每种数值代表一种棋子
enum ChessPiece {
	NOT_EXIST = -1, //该位置不存在（数组越界）
	EMPTY = 0, //该位置为空
	PIECE_START,
	BOT = PIECE_START, //该位置为机器人的棋子
	PLAYER, //该位置为人类的棋子
	PIECE_END = PLAYER
};

const static int SIZE = 15; //棋盘边长
