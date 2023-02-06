#include <bits/stdc++.h>

// -----------------------------------------------------------------------------
// Utility
// -----------------------------------------------------------------------------
#define LOG_ENABLED

#ifdef LOG_ENABLED
#define Log_log(message, level) Log::log(std::to_string(__LINE__) + std::string("@") + std::string(__func__) + std::string(", ") + (message) + std::string("\n"))
#define Log_debug(message) Log_log((message), 0)
#define Log_info(message)  Log_log((message), 1)

class Log {
public:
	static void log(const std::string& message) {
		Log::_ofstream << message;
	}

private:
	static std::ofstream _ofstream;
};
std::ofstream Log::_ofstream("debug.log");

#else // LOG_ENABLED
#define Log_log(message, level)
#define Log_debug(message)
#define Log_info(message)

#endif // LOG_ENABLED

template <typename ... Args>
std::string format(const std::string& fmt, Args ... args )
{
	size_t len = std::snprintf( nullptr, 0, fmt.c_str(), args ... );
	std::vector<char> buf(len + 1);
	std::snprintf(&buf[0], len + 1, fmt.c_str(), args ... );
	return std::string(&buf[0], &buf[0] + len);
}

std::string to_string(const std::vector<int>& v) {
	std::string result = "size=" + std::to_string(v.size()) + ", [ ";
	for(auto& elem : v) {
		result += std::to_string(elem) + ", ";
	}
	result += "]";
	return result;
}

std::string to_string(const std::vector<std::string>& v) {
	std::string result = "size=" + std::to_string(v.size()) + ", [ ";
	for(auto& elem : v) {
		result += elem + ", ";
	}
	result += "]";
	return result;
}

class Clock {
public:
	Clock() :
		ref_(std::chrono::steady_clock::now())
	{
		setlocale(LC_NUMERIC,"ja_JP.utf8");
	}

	void reset() {
		ref_ = std::chrono::steady_clock::now();
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

private:
	std::chrono::steady_clock::time_point ref_;
};

// -----------------------------------------------------------------------------
// Edge, Vertex, PriorityQueue, Graph
// -----------------------------------------------------------------------------
class Edge {
public:
	Edge(uint16_t u, uint16_t v, uint32_t w)  :
		_u(u),
		_v(v),
		_w(w)
	{}

	bool operator==(const Edge& rhs) const {
		return _u == rhs._u && _v == rhs._v && _w == rhs._w;
	}

	std::string toString() const {
		return format("u: %d, v: %d, w: %d", _u, _v, _w);
	}

	uint16_t u() const { return _u; }
	uint16_t v() const { return _v; }
	uint32_t w() const { return _w; }

	struct HashFunction {
		size_t operator()(const Edge& self) const {
			return (self._u << 16) | self._v;
		}
	};

private:
	uint16_t _u;
	uint16_t _v;
	uint32_t _w;
};

typedef std::unordered_set<Edge, Edge::HashFunction> UnorderedSetofEdge;

class Vertex {
public:
	Vertex(int x, int y)  :
		_x(x),
		_y(y)
	{}

	std::string toString() const {
		return format("x: %d, y: %d", _x, _y);
	}

	int x() const { return _x; }
	int y() const { return _y; }

private:
	int _x;
	int _y;
};

struct PriorityQueueItem {
	uint32_t distance;
	uint32_t vertex;

	PriorityQueueItem(uint32_t distance, uint32_t vertex) :
		distance(distance),
		vertex(vertex)
	{}

	struct Compare {
		bool operator()(const PriorityQueueItem& a, PriorityQueueItem& b) const {
			return a.distance > b.distance;
		}
	};
};

class PriorityQueue {
public:
	PriorityQueue() :
		_priorityQueue()
	{}

	bool empty() const {
		return _priorityQueue.empty();
	}

	void enque(const PriorityQueueItem& item) {
		_priorityQueue.push(item);
	}

	PriorityQueueItem deque() {
		PriorityQueueItem item = _priorityQueue.top();
		_priorityQueue.pop();
		return item;
	}

private:
	std::priority_queue<
		PriorityQueueItem,
		std::vector<PriorityQueueItem>,
		PriorityQueueItem::Compare> _priorityQueue;
};

#define UNREACHABLE_DISTANCE (1e+9)

class Graph {
public:
	Graph(uint16_t N, uint16_t M, const std::vector<Edge>& edges) :
		_N(N),
		_M(M),
		_originalEdges(edges),
		_availableEdges(edges.begin(), edges.end()),
		_edgesOnVertex(N),
		_distanceMatrix(),
		_totalDistance()
	{
		__initializeEdgesOnVertex();
		__initializeDistance();
	}
	Graph(const Graph& graph, UnorderedSetofEdge edgesToDrop) :
		_N(graph._N),
		_M(graph._M),
		_originalEdges(graph._originalEdges),
		_availableEdges(graph._availableEdges),
		_edgesOnVertex(graph._N),
		_distanceMatrix(),
		_totalDistance()
	{
		for(const Edge& edge : edgesToDrop) {
			_availableEdges.erase(edge);
		}

		__initializeEdgesOnVertex();
		__initializeDistance();
	}

	std::vector<int> makeSchedule(int D, int K) {
		//return __makeRandomSchedule(D, K);
		return __makeFastestSchedule(D, K);
	}

private:
	uint16_t _N;
	uint16_t _M;
	std::vector<Edge> _originalEdges;

	UnorderedSetofEdge _availableEdges;
	std::vector<std::vector<Edge>> _edgesOnVertex;
	std::vector<std::vector<uint32_t>> _distanceMatrix;
	uint64_t _totalDistance;

	void __initializeEdgesOnVertex() {
		for(const Edge& edge : _availableEdges) {
			uint16_t u = edge.u();
			uint16_t v = edge.v();
			uint32_t w = edge.w();

			_edgesOnVertex[u-1].emplace_back(u, v, w);
			_edgesOnVertex[v-1].emplace_back(v, u, w);
		}
	}

	bool __updateDistanceMatrix(int vertex1, int vertex2, double distance) {
		if(_distanceMatrix[vertex1-1][vertex2-1] > distance) {
			_distanceMatrix[vertex1-1][vertex2-1] = distance;
			return true;
		}
		return false;
	}

	void __initializeDistanceMatrix(int vertex) {
		static PriorityQueue priorityQueue;

		priorityQueue.enque(PriorityQueueItem(0, vertex));

		while(!priorityQueue.empty()) {
			PriorityQueueItem item = priorityQueue.deque();

			for(const Edge& edge : _edgesOnVertex[item.vertex-1]) {
				if(__updateDistanceMatrix(vertex, edge.v(), item.distance + edge.w())) {
					priorityQueue.enque(PriorityQueueItem(item.distance + edge.w(), edge.v()));
				}
			}
		}
	}

	void __initializeDistance() {
		_distanceMatrix.resize(_N);
		for(auto& row : _distanceMatrix) {
			row.resize(_N, UNREACHABLE_DISTANCE);
		}
		for(size_t i = 0; i < _distanceMatrix.size(); i++) {
			_distanceMatrix[i][i] = 0;
		}

		for(int vertex = 1; vertex <= _N; vertex++) {
			__initializeDistanceMatrix(vertex);
		}

		uint64_t temp = 0;
		for(size_t i = 0; i < _distanceMatrix.size(); i++) {
			for(size_t j = i + 1; j < _distanceMatrix.size(); j++) {
				temp += _distanceMatrix[i][j];
			}
		}
		_totalDistance = temp * 2;
	}

	uint64_t __calcScore(int D, std::vector<UnorderedSetofEdge> dropEdgesForEachDay) {
		double score = 0.0;
		double N = _N;

		for(const UnorderedSetofEdge& edgesToDrop : dropEdgesForEachDay) {
			if(edgesToDrop.size() == 0) {
				continue;
			}

			Graph graph(*this, edgesToDrop);
			uint64_t diff = graph._totalDistance - _totalDistance;
			double fk = diff / (N * (N-1.0));
			score += fk;
			Log_debug(format("graph._totalDistance: %18lu", graph._totalDistance));
			Log_debug(format("_totalDistance:       %18lu", _totalDistance));
			Log_debug(format("diff:                 %18lu", diff));
			Log_debug(format("fk:                               %10.3lf", fk));
			Log_debug(format("Score:                            %10.3lf", score));
		}
		score *= 1000.0 / D;
		return (uint64_t)std::round(score);
	}

	std::vector<int> __makeRandomSchedule(int D, int K) {
		std::vector<int> bestSchedule;
		uint64_t bestScore = UINT64_MAX;

		std::random_device seed_gen;
		//std::default_random_engine engine(seed_gen());
		std::default_random_engine engine(0);
		std::uniform_int_distribution<> dist(1, D);

		for(int i = 0; i < 5; i++) {
			std::vector<int> schedule(_M);
			std::vector<UnorderedSetofEdge> dropEdgesForEachDay(D);
			std::vector<int> remaining(D, K);
			for(size_t i = 0; i < schedule.size(); i++) {
				while(true) {
					int day = dist(engine);
					if(remaining[day-1] > 0) {
						remaining[day-1]--;
						schedule[i] = day;
						dropEdgesForEachDay[day-1].emplace(_originalEdges[i]);
						break;
					}
				}
			}
			uint64_t score = __calcScore(D, dropEdgesForEachDay);
			if(score < bestScore) {
				bestScore = score;
				std::swap(schedule, bestSchedule);
			}
		}
		Log_debug(format("Final score: %lu", bestScore));
		return bestSchedule;
	}

	std::vector<int> __makeFastestSchedule(int D, int K) {
		std::vector<int> schedule;
		std::vector<UnorderedSetofEdge> dropEdgesForEachDay(D);
		size_t remainingAll = _M;
		size_t remainingInADay;
		int day = 1;

		while(remainingAll > 0) {
			remainingInADay = K;
			while(remainingAll > 0 && remainingInADay > 0) {
				schedule.emplace_back(day);
				dropEdgesForEachDay[day-1].emplace(_originalEdges[schedule.size()-1]);
				remainingAll--;
				remainingInADay--;
			}
			day++;
		}

		uint64_t score = __calcScore(D, dropEdgesForEachDay);
		Log_debug(format("Final score: %lu", score));
		return schedule;
	}

};

// -----------------------------------------------------------------------------
// Main
// -----------------------------------------------------------------------------
class Main {
public:
	Main() {}

	void readInput() {
		std::cin >> _N >> _M >> _D >> _K;

		// Edge
		std::vector<Edge> edges;
		edges.reserve(_M);
		for(int i = 0; i < _M; i++) {
			int u, v, w;
			std::cin >> u >> v >> w;
			edges.emplace_back(u, v, w);
		}

		// Coordinate
		//std::vector<Vertex> vertexCoordinates;
		//vertexCoordinates.reserve(_N);
		//for(int i = 0; i < _N; i++) {
		//	int x, y;
		//	std::cin >> x >> y;
		//	vertexCoordinates.emplace_back(x, y);
		//}

		_graph = new Graph(_N, _M, std::move(edges));

		Log_info(format("N: %d, M: %d, D: %d, K: %d", _N, _M, _D, _K));
		//Log_info(_graph->toString());
	}

	void run() {
		std::vector<int> schedule = _graph->makeSchedule(_D, _K);
		uint64_t elapsedTimeMillisec = _clock.getElapsedTimeMillisec();

		// Output
		for(int& r : schedule) {
			std::cout << r << " ";
		}
		std::cout << std::endl;

		Log_info(format("Elapsed Time: %lu[ms]", elapsedTimeMillisec));
	}

private:
	int _N;;
	int _M;;
	int _D;;
	int _K;;
	Graph* _graph;

	// debug
	Clock _clock;
};

int main() {
	Main main;
	main.readInput();
	main.run();

	return 0;
}

