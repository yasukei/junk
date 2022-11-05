#include <bits/stdc++.h>
typedef long long ll;

class Clock {
public:
	Clock() :
		Clock(false)
	{}
	Clock(bool printWhenDestruct) :
		ref_(std::chrono::steady_clock::now()),
		printWhenDestruct_(printWhenDestruct)
	{
		setlocale(LC_NUMERIC,"ja_JP.utf8");
	}
	~Clock() {
		if(printWhenDestruct_) {
			print();
		}
	}

	std::uint64_t getElapsedTimeMillisec() const {
		return getElapsedTime<std::chrono::milliseconds>();
	}
	std::uint64_t getElapsedTimeMicrosec() const {
		return getElapsedTime<std::chrono::microseconds>();
	}
	std::uint64_t getElapsedTimeNanosec() const {
		return getElapsedTime<std::chrono::nanoseconds>();
	}

	void reset() {
		ref_ = std::chrono::steady_clock::now();
	}
	void print() {
		printf("time: %'9lu\n", getElapsedTimeNanosec());
	}

private:
	std::chrono::steady_clock::time_point ref_;
	bool printWhenDestruct_;

	template<typename T>
	std::uint64_t getElapsedTime() const {
		T elapsed = std::chrono::duration_cast<T>(std::chrono::steady_clock::now() - ref_);
		return elapsed.count();
	}
};

class Board {
public:
	Board() {
		for(size_t i = 0; i < board_.size(); i++) {
			board_[i].fill(0);
		}
	}

	Board(const Board& rhs) :
		board_(rhs.board_)
	{}

	bool put(char candy, int pos) {
		bool result = false;
		int counter = 0;
		for(size_t i = 0; i < board_.size(); i++) {
			for(size_t j = 0; j < board_[i].size(); j++) {
				if(board_[i][j] == 0) {
					counter++;
					if(counter == pos) {
						board_[i][j] = candy;
						result = true;
						goto lExit;
					}
				}
			}	
		}
lExit:
		return result;
	}
	void move(char move) {
		switch(move) {
			case 'F': forward(); break;
			case 'B': backward(); break;
			case 'R': right(); break;
			case 'L': left(); break;
		}
	}
	int getScore() const {
		int score = 0;
		std::array<std::array<char, 10>, 10> temp(board_);

		std::list<std::pair<int, int>> list;
		int d[3] = {0, 0, 0};

		for(size_t i = 0; i < temp.size(); i++) {
			for(size_t j = 0; j < temp[i].size(); j++) {
				if(temp[i][j] == 0) continue;

				int kind = temp[i][j];
				int num = 1;
				list.push_back(std::make_pair(i, j));
				temp[i][j] = 0;
				while(!list.empty()) {
					std::pair p = list.front();
					list.pop_front();

					int x = std::get<0>(p);
					int y = std::get<1>(p);
					if(x-1 >= 0 && temp[x-1][y] == kind) { list.push_back(std::make_pair(x-1, y)); temp[x-1][y] = 0; num++; }
					if(x+1 < 10 && temp[x+1][y] == kind) { list.push_back(std::make_pair(x+1, y)); temp[x+1][y] = 0; num++; }
					if(y-1 >= 0 && temp[x][y-1] == kind) { list.push_back(std::make_pair(x, y-1)); temp[x][y-1] = 0; num++; }
					if(y+1 < 10 && temp[x][y+1] == kind) { list.push_back(std::make_pair(x, y+1)); temp[x][y+1] = 0; num++; }
				}
				score += num*num;
				d[kind-1] += num;
			}
		}

		ll dAll = 0;
		for(int i = 0; i < 3; i++) {
			dAll += d[i]*d[i];
		}
		if(dAll == 0) return 0;

		return (1000LL*1000LL*score) / dAll;
	}
	void print() const {
		for(size_t i = 0; i < board_.size(); i++) {
			for(size_t j = 0; j < board_[i].size(); j++) {
				printf("%d ", board_[i][j]);
			}
			printf("\n");
		}
	}

private:
	std::array<std::array<char, 10>, 10> board_;

	void forward() {
		for(int i = 0; i < 10; i++) {
			for(int j = 0; j < 10; j++) {
				if(board_[i][j] != 0) continue;

				int next = i + 1;
				while(next < 10 && board_[next][j] == 0) {
					next++;
				}
				if(next < 10) {
					std::swap(board_[i][j], board_[next][j]);
				}
			}
		}
	}
	void backward() {
		for(int i = 9; i >= 0; i--) {
			for(int j = 0; j < 10; j++) {
				if(board_[i][j] != 0) continue;

				int next = i - 1;
				while(next >= 0 && board_[next][j] == 0) {
					next--;
				}
				if(next >= 0) {
					std::swap(board_[i][j], board_[next][j]);
				}
			}
		}
	}
	void right() {
		for(int i = 0; i < 10; i++) {
			for(int j = 9; j >= 0; j--) {
				if(board_[i][j] != 0) continue;

				int next = j - 1;
				while(next >= 0 && board_[i][next] == 0) {
					next--;
				}
				if(next >= 0) {
					std::swap(board_[i][j], board_[i][next]);
				}
			}
		}
	}
	void left() {
		for(int i = 0; i < 10; i++) {
			for(int j = 0; j < 10; j++) {
				if(board_[i][j] != 0) continue;

				int next = j + 1;
				while(next < 10 && board_[i][next] == 0) {
					next++;
				}
				if(next < 10) {
					std::swap(board_[i][j], board_[i][next]);
				}
			}
		}
	}
};

class Searcher {
public:
	Searcher(const std::vector<int>& f) :
		f_(f),
		index_(0),
		current_()
	{}

	char order(int p) {
		//printf("order: %d\n", p);
		//int depth = index_ > 90 ? 3 : 2;
		//depth = (100 - index_ - 1) >= depth ? depth : 100 - index_ - 1;
		int depth = 2;

		current_.put((char)f_[index_], p);
		index_ += 1;
		std::pair<char, int> result = search(depth, current_, index_);
		char move = std::get<0>(result);
		current_.move(move);
		return move;
	}

	const Board& getBoard() const {
		return current_;
	}
private:
	std::vector<int> f_;
	int index_;
	Board current_;

	std::pair<char, int> search(int depth, const Board& current, int nextIndex) const {
		static const std::array<char, 4> moves = {'F', 'B', 'R', 'L'};
		char move = 'F';
		int score = 0;

		if(depth <= 1) {
			for(auto m : moves) {
				Board b(current);

				b.move(m);
				int s = b.getScore();
				if(s > score) {
					score = s;
					move = m;
				}
			}
			goto lExit;
		}

		for(auto m : moves) {
			int temp = 0;
			for(int p = 1; p < 100-nextIndex; p++) {
				Board b(current_);

				b.move(m);
				b.put(f_[nextIndex], p);
				std::pair<char, int> result = search(depth-1, b, nextIndex+1);
				temp += std::get<1>(result);
			}
			if(temp > score) {
				score = temp;
				move = m;
			}
		}
		score /= 100-nextIndex-1 > 0 ? 100-nextIndex-1 : 1;

lExit:
		//printf("depth: %d, move: %c, score: %'d, nextIndex: %d\n", depth, move, score, nextIndex);
		return std::make_pair(move, score);
	}
};

int main() {
#if 0
	printf("Board:     %lu\n", sizeof(Board));
	return 0;
#endif
	Clock clock;

	std::vector<int> f(100);
	for(size_t i = 0; i < f.size(); i++) {
		std::cin >> f[i];
	}

	Searcher searcher(f);
	for(size_t i = 0; i < f.size(); i++) {
		int p;
		std::cin >> p;

		char move = searcher.order(p);
		printf("%c\n", move);
		fflush(stdout);
	}

#if 1
	const Board& board = searcher.getBoard();
	board.print();
	printf("\n");
	printf("Score: %'d\n", board.getScore());
	clock.print();
#endif

	return 0;
}

