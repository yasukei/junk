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
		using namespace std::chrono;
		milliseconds elapsed = duration_cast<milliseconds>(steady_clock::now() - ref_);
		return elapsed.count();
	}
	std::uint64_t getElapsedTimeMicrosec() const {
		using namespace std::chrono;
		microseconds elapsed = duration_cast<microseconds>(steady_clock::now() - ref_);
		return elapsed.count();
	}
	std::uint64_t getElapsedTimeNanosec() const {
		using namespace std::chrono;
		nanoseconds elapsed = duration_cast<nanoseconds>(steady_clock::now() - ref_);
		return elapsed.count();
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
};

class Board {
public:
	Board() {
		for(int i = 0; i < 10; i++) {
			for(int j = 0; j < 10; j++) {
				board_[i][j] = 0;
			}
		}
		space_ = 100;
	}

	Board(const Board& self) {
		std::memcpy(this->board_, self.board_, sizeof(int)*100);
		this->space_ = self.space_;
	}

	bool put(int candy, int pos) {
		bool result = false;
		int counter = 0;
		for(int i = 0; i < 10; i++) {
			for(int j = 0; j < 10; j++) {
				if(board_[i][j] == 0) {
					counter++;
					if(counter == pos) {
						board_[i][j] = candy;
						space_--;
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
	int getSpace() const {
		return space_;
	}
	int getScore() const {
		int score = 0;
		int temp[10][10];
		std::memcpy(temp, board_, sizeof(int)*100);

		std::list<std::pair<int, int>> list;
		int d[3] = {0, 0, 0};

		for(int i = 0; i < 10; i++) {
			for(int j = 0; j < 10; j++) {
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
		return (1000LL*1000LL*score) / dAll;
	}
	void print() const {
		for(int i = 0; i < 10; i++) {
			for(int j = 0; j < 10; j++) {
				printf("%d ", board_[i][j]);
			}
			printf("\n");
		}
	}


private:
	int board_[10][10];
	int space_;

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

std::random_device seed_gen;
std::default_random_engine engine(seed_gen());
int f[100];
const char moves[4] = {'F', 'B', 'R', 'L'};

void recurse(const Board& b, int index, int* p, int depth, int& bestScore, int& bestMoveIndex) {
	std::vector<Board> boards(4, b);

	if(depth == 1) {
		for(int i = 0; i < 4; i++) {
			boards[i].move(moves[i]);

			if(boards[i].getScore() > bestScore) {
				bestScore = boards[i].getScore();
				bestMoveIndex = i;
			}
		}
		return;
	}

	for(int i = 0; i < 4; i++) {
		boards[i].move(moves[i]);
		boards[i].put(f[index], p[0]);

		int score = 0;
		int moveIndex = 0;
		recurse(boards[i], index+1, &p[1], depth-1, score, moveIndex);
		if(score > bestScore) {
			bestScore = score;
			bestMoveIndex = i;
		}
	}
}

int main() {
#if 0
	printf("Board:     %lu\n", sizeof(Board));
	return 0;
#endif
	Clock clock;

	int d[3] = {0, 0, 0};
	for(size_t i = 0; i < 100; i++) {
		std::cin >> f[i];
		d[f[i]-1] += 1;
	}
	int mvp;
	if(d[0] >= d[1] && d[0] >= d[2]) {
		mvp = 1;
	} else if(d[1] >= d[0] && d[1] >= d[2]) {
		mvp = 2;
	} else {
		mvp = 3;
	}

	Board board;

	for(int i = 0; i < 100; i++) {
		int p;
		std::cin >> p;

		board.put(f[i], p);

		std::uniform_int_distribution<> dist(1, 99-std::min(98, i));
		int depth = 2;
		std::vector<int> predict(depth);
		for(int j = 0; j < depth; j++) {
			predict[j] = dist(engine);
		}

		int score = 0;
		int moveIndex = 0;
		recurse(board, i+1, predict.data(), depth, score, moveIndex);
		board.move(moves[moveIndex]);
		printf("%c\n", moves[moveIndex]); fflush(stdout);
	}

#if 1
	board.print();
	printf("\n");
	printf("mvp:   %d\n", mvp);
	printf("Score: %'d\n", board.getScore());
	clock.print();
#endif

	return 0;
}

