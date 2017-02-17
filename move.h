#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <ctime>


using namespace std;

typedef struct intpair
{
	int first;
	int second;
} IntPair;


/***************************************
ルール系
***************************************/

// 盤面のリセット
void init_board();
// 盤面の出力
void print_board();

// 石を数える
int count_stone(const int side);

/*
***************
石を置くのが適切かどうか
***************
引数：
	int side	自分のターン、石のこと
	IntPair square	対象としているマス
戻り値：
	int型
	1	置ける
	0	置けない
*/
int is_legal_move(const int side, const IntPair square);


/*
***************
実際に石を置く処理
***************
引数：
	int side	自分のターン、石のこと
	IntPair square	対象としているマス
戻り値：
	int型
	1以上の整数	裏返った数	
*/
int place_disk(const int side, const IntPair square);



/*
***************
石の置ける箇所の候補を求める
***************
引数：
	int side	自分のターン、石のこと
	vector<IntPair> legal_moves	置ける箇所の結果格納
戻り値：
	int型
	-1 		盤面に石が全て置かれていて置けない
	 0以上	置ける箇所の数
*/
int generate_all_legal_moves(const int side, vector<IntPair> &legal_moves);


class timer{
	clock_t start;
public:
	//計測開始
	timer();
	//計測終了、開始から終了まで出力
	~timer();
	//計測時間表示
	double showTime();
};





/***************************************
探索系
***************************************/

/*
***************
ミニマックス法
***************
引数：
	int side	自分のターン、石のこと
	int depth	今いる深さ
	int max_depth	潜る深さ
	int in_pass	直前の手がパスかどうか
	IntPair square	最後に打たれた手
	IntPair best_square	最良手
*/
int max_node(const int side, const int me, int depth, const int max_depth, int in_pass, IntPair square, IntPair &best_square, int alpha, int beta);
int min_node(const int side, const int me, int depth, const int max_depth, int in_pass, IntPair square, IntPair &best_square, int alpha, int beta);

/*
***************
評価関数
***************
引数：
	int side	自分のターン、石のこと（毎度最初にmax_node()をよぶため）
戻り値：
	int型
	整数値	盤面の評価値
*/
int eval_func(const int side);

/*
***************
盤面の記録
***************
引数：
	int side	自分のターン、石のこと
	IntPair square	新しい手
戻り値：
	なし
*/
void update_board(const int side, IntPair square);

/*
***************
盤面の記録のうち、最後のものをリセット（待った等に用いる）
***************
引数：
	なし
戻り値：
	なし
*/
void undo_board();
