#include "move.h"

using namespace std;

int board[8][8];  // 8x8 squares: 0 -> empty; 1 -> black; -1 -> white
vector<pair<int, IntPair> > history; //指した手の記録

/**********
@todo list:
・パスが2回で終了
・終了画面

**********/



int main(int argc, char **argv){

	int human_side;

	//盤面初期化
	init_board();


	//ゲームモード選択

	/*
	//人間が遊ぶ用の説明
	cout << "select mode\n";
	cout << " 1:you start\n";
	cout << "-1:com start\n";
	cout << " 0:com vs com\n";
	*/
	//AI対戦用の説明
	cout << "select mode(AI視点)\n";
	cout << " 1:後攻\n";
	cout << "-1:先攻\n";
	cout << " 0:com vs com\n";

	cin >> human_side;

	
	int turn, eval;
	int pass_cnt = 0;
	//ゲームスタート
	for(turn = 1;; turn *= -1){
		print_board();

		//おけるところがあるか判定
		vector<IntPair> legal_moves;
		const int nmoves = generate_all_legal_moves(turn, legal_moves);
		if (nmoves == -1) break;     // no empty square
		if (nmoves ==  0) { // pass
			printf("turn = %d, move = Pass\n", turn);
			pass_cnt++;
			if(pass_cnt == 2) break;
			continue;
		}

		pass_cnt = 0;
		IntPair move;
		if (turn == human_side) { //人間が打つ
			while (1) {
				//説明表示
				char stone;
				if(human_side == 1) stone = 'o';
				else stone = 'x';
				cout << "Where?" << "enemy:" << stone << ") ";
				string buf;
				cin >> buf;

				//「待った」
				if(buf == "wait"){
					if(history.size() < 2){
						cout << "cannot wait\n";
						continue;
					}
					cout << "待ったあああああああああああああああああああああ\n";
					undo_board();
					undo_board();
					print_board();
					continue;
				}

				//正しい入力か確認
				move.first  = buf[0] - 'a';
				move.second = buf[1] - '1';
				if (is_legal_move(turn, move)) break;
				cout << "ﾅﾆｿﾚ＼ｲﾐﾜｶﾝﾅｲ!／\n\n";
			}
		} else { //comが打つ
			timer timer1;
			if(history.size() == 0){
				move = legal_moves[0];  // とりあえず動かせる場所に打つ
			}
			else {
				int depth = (history.size() < 46) ? 7 : 13;
				eval =  max_node(turn, turn, 0, depth, pass_cnt, history.back().second, move, -10000, 10000); // ミニマックス
			}
			printf("nmoves:%d, eval:%d\n", (int)legal_moves.size(), eval);
			printf("turn = %d, move = %c%c\n", turn, 'a' + move.first, '1' + move.second);
		}
		//実際に打つ
		update_board(turn, move);
	}

	//ゲーム終了
	int me = count_stone(-human_side);
	int enemy = count_stone(human_side);

	cout << "you:" << me <<", enemy:" << enemy << endl; 
	if(me > enemy){
		cout << "You Win!!!\n\n";
	} else if(me == enemy){
		cout << "Draw!\n\n";
	} else {
		cout << "You Lose...\n\n";
	}

	return 0;
}