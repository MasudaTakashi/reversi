#include "move.h"

using namespace std;

const static IntPair directions[8] = {
	{ -1, -1 }, {  0, -1 }, { 1, -1 },
	{ -1,  0 },             { 1,  0 },
	{ -1,  1 }, {  0,  1 }, { 1,  1 }
};

extern int board[8][8];
extern vector<pair<int, IntPair> > history;

// ある一方向は裏返せるか
static int can_flip(const int side, IntPair square, const IntPair direction){
	assert(board[square.first][square.second] == 0);
	int n = 0;
	do {
		square.first  += direction.first;
		square.second += direction.second;
		n++;
		if (square.first  < 0 || square.first >= 8 || square.second < 0 || square.second >= 8)
		return 0;
	} while (board[square.first][square.second] == -side);
	if (board[square.first][square.second] == 0) return 0;
	if (n <= 1) return 0;
	return 1;
}



// 石を置くことができるか
int is_legal_move(const int side, const IntPair square){
	//外かどうか
	if (square.first  < 0 || square.first  >= 8) return 0;
	if (square.second < 0 || square.second >= 8) return 0;
	//石が置かれてないか
	int i;
	if (board[square.first][square.second] != 0) return 0;
	//石の配置的におけるかどうか
	for (i = 0; i < 8; i++) {
		if (can_flip(side, square, directions[i])) return 1;
	}
	return 0;
}




int place_disk(const int side, const IntPair square){
	assert(is_legal_move(side, square));
	int n = 0, i;
	for (i = 0; i < 8; i++) {
		const IntPair dir = directions[i];
		// 石を裏返せない方向は無視
		if (!can_flip(side, square, dir)) continue;
		// 石を裏返していく
		int x = square.first  + dir.first;
		int y = square.second + dir.second;
		while (board[x][y] == -side) {
			board[x][y] = side;
			n++;
			x += dir.first;
			y += dir.second;
		}
	}
	board[square.first][square.second] = side;
	assert(n > 0); //石を置くからには少なくとも1つはひっくり返る
	return n;
}



int generate_all_legal_moves(const int side, vector<IntPair> &legal_moves){
	int x, y;
	int nmoves = 0;  // number of generated moves
	int nempty = 0;  // number of empty squares
	for (x = 0; x < 8; x++) {
		for (y = 0; y < 8; y++) {
			if (board[x][y] != 0) continue;
			nempty++;
			IntPair square;
			square.first = x; square.second = y;
			if (!is_legal_move(side, square)) continue;
			assert(nmoves < 60);
			legal_moves.push_back(square);
			nmoves++;
		}
	}
	// おける箇所がない時は終わり
	if (nempty == 0) return -1;
	return nmoves;
}

int count_stone(const int side){
	int i, j, cnt = 0;
	for(i = 0; i < 8; i++){
		for(j = 0; j < 8; j++){
			if(board[i][j] == side) cnt++;
		}
	}
	return cnt;
}




timer::timer(){
	start = clock();
}

timer::~timer(){
	clock_t end = clock();
	double time = (double)(end-start) / CLOCKS_PER_SEC;
	cout << "time: " << time << "sec" << endl;
}

double timer::showTime(){
	clock_t end = clock();
	double time = (double)(end-start) / CLOCKS_PER_SEC;
	return time;
}



int max_node(const int side, const int me, int depth, const int max_depth, int in_pass, IntPair square, IntPair &best_square, int alpha, int beta){
	// 深さ優先探索で行き着いたら
	if(depth == max_depth){
		best_square = square;
		return eval_func(me);
	}

	// MAX Node（子ノード）の探索
	// 探索準備
	int i;
	IntPair best_move;
	vector<IntPair> legal_moves;
	int nmoves =  generate_all_legal_moves(side, legal_moves);

	if(nmoves > 0){
		for(i = 0; i < (int)legal_moves.size(); i++){
			update_board(side, legal_moves[i]);
			int child = min_node(-side, me, depth+1, max_depth, 0, legal_moves[i], best_move, alpha, beta);
			undo_board();
			if(child > alpha){
				alpha = child;
				best_square = legal_moves[i];
				if(alpha >= beta) break; // αカット
			}
		}
	}
	else {
		if(in_pass){ // 直前がパスかつ自分も打てない→終局。評価関数返す。
			best_square = square;
			return eval_func(me);
		} else { // 直前はパスではない
			return min_node(-side, me, depth, max_depth, 1, square, best_square, alpha, beta);
		}
	}
	return alpha;
}

int min_node(const int side, const int me, int depth, const int max_depth, int in_pass, IntPair square, IntPair &best_square, int alpha, int beta){
	// 深さ優先探索で行き着いたら
	if(depth == max_depth){
		best_square = square;
		return eval_func(me);
	}

	// MIN Node（子ノード）の探索
	// 探索準備
	int i;
	IntPair best_move;
	vector<IntPair> legal_moves;
	int nmoves =  generate_all_legal_moves(side, legal_moves);

	if(nmoves > 0){
		for(i = 0; i < (int)legal_moves.size(); i++){
			update_board(side, legal_moves[i]);
			int child = max_node(-side, me, depth+1, max_depth, 0, legal_moves[i], best_move, alpha, beta);//@todo
			undo_board();
			if(child < beta){
				beta = child;
				best_square = legal_moves[i];
				if(alpha >= beta) break; //βカット
			}
		}
	}
	else {
		if(in_pass){ // 直前がパスかつ自分も打てない→終局。評価関数返す。
			best_square = square;
			return eval_func(me);
		} else { // 直前はパスではない
			return max_node(-side, me, depth, max_depth, 1, square, best_square, alpha, beta);
		}
	}
	return beta;
}

int eval_func(const int side){
	int i, j , value = 0;
	int board_score[8][8] = {
		{ 40, -12,  5, -1, -1,  5, -12,  40},
		{-12, -15, -3, -3, -3, -3, -15, -12},
		{  5,  -3,  0, -1, -1,  0,  -3,   5},
		{ -1,  -3, -1, -1, -1, -1,  -3,  -1},
		{ -1,  -3, -1, -1, -1, -1,  -3,  -1},
		{  5,  -3,  0, -1, -1,  0,  -3,   5},
		{-12, -15, -3, -3, -3, -3, -15, -12},
		{ 40, -12,  5, -1, -1,  5, -12,  40}};

	// 〜中盤
	if(history.size() < 48){
		vector<IntPair> legal_moves;
		int mymoves =  generate_all_legal_moves(side, legal_moves);
		int enemymoves = generate_all_legal_moves(-side, legal_moves);
		for(i = 0; i < 8; i++){
			for(j = 0; j < 8; j++){
				if(board[i][j] == side) value += board_score[i][j];	
				else if(board[i][j] == -side) value -= board_score[i][j];	
			}
		}
		if(enemymoves != 0) value = value * mymoves / enemymoves;
	}
	// 終盤
	else {
		value = count_stone(side) - count_stone(-side);
	}
	return value;
}

void update_board(const int side, IntPair square){
	place_disk(side, square);
	history.push_back(make_pair(side, square));
	return;
}

void undo_board(){
	// 最終手を無かったことに
	history.pop_back();
	// 最終手以外を初めから打ちなおす
	init_board();
	int i;
	for (i = 0; i < (int)history.size(); i++){
		place_disk(history[i].first, history[i].second);
	}
	return;
}


void init_board(){
	int x, y;
	for (y = 0; y < 8; y++)
		for (x = 0; x < 8; x++)
			board[x][y] = 0;

	// 初期位置
	board[4][3] = board[3][4] =  1;  // black disks
	board[3][3] = board[4][4] = -1;  // white disks
}


void print_board(){
	int x, y;
	puts("\n  a b c d e f g h");
	for (y = 0; y < 8; y++) {
		printf("%d ", y + 1);
		for (x = 0; x < 8; x++) {
			const int d = board[x][y];
			char c = '-';
			if (d ==  1) c = 'o';
			if (d == -1) c = 'x';
			printf("%c ", c);
		}
		putchar('\n');
	}
	putchar('\n');
}
