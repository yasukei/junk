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
	Edge(uint16_t id, uint16_t u, uint16_t v, uint32_t w)  :
		_id(id),
		_u(u),
		_v(v),
		_w(w)
	{}

	bool operator==(const Edge& rhs) const {
		return _id == rhs._id;
	}

	std::string toString() const {
		return format("id: %d, u: %d, v: %d, w: %d", _id, _u, _v, _w);
	}

	uint16_t id() const { return _id; }
	uint16_t u() const { return _u; }
	uint16_t v() const { return _v; }
	uint32_t w() const { return _w; }

	Edge getReversedEdge() const {
		return Edge(_id, _v, _u, _w);
	}

	struct HashFunction {
		size_t operator()(const Edge& self) const {
			return self._id;
		}
	};

private:
	uint16_t _id;
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

//class Trajectory {
//public:
//	Trajectory(uint32_t distance, const std::vector<uint16_t>& vertices) :
//		_distance(distance),
//		_vertices(vertices)
//	{}
//	Trajectory(const Trajectory& trajectory, uint32_t distance, uint16_t vertex) :
//		_distance(trajectory._distance),
//		_vertices(trajectory._vertices)
//	{
//		_distance += distance;
//		_vertices.emplace_back(vertex);
//	}
//
//	uint32_t getDistance() { return _distance; }
//	uint16_t getStartVertex() { return _vertices.front(); }
//	uint16_t getEndVertex() { return _vertices.back(); }
//
//	struct Compare {
//		bool operator()(const Trajectory& a, const Trajectory& b) const {
//			return a._distance > b._distance;
//		}
//	};
//
//private:
//	uint32_t _distance;
//	std::vector<uint16_t> _vertices;
//};
//
//class PriorityQueue {
//public:
//	PriorityQueue() :
//		_queue()
//	{}
//
//	bool empty() {
//		return _queue.empty();
//	}
//
//	void enque(uint32_t distance, const std::vector<uint16_t>& vertices) {
//		_queue.emplace(distance, vertices);
//	}
//	void enque(const Trajectory& trajectory, uint32_t distance, uint16_t vertex) {
//		_queue.emplace(trajectory, distance, vertex);
//	}
//
//	Trajectory deque() {
//		Trajectory trajectory = _queue.top();
//		_queue.pop();
//		return trajectory;
//	}
//
//private:
//	std::priority_queue<Trajectory, std::vector<Trajectory>, Trajectory::Compare> _queue;
//};

struct PriorityQueueItem {
	uint32_t distance;
	uint32_t vertex;

	PriorityQueueItem() :
		distance(),
		vertex()
	{}
	PriorityQueueItem(uint32_t distance, uint32_t vertex) :
		distance(distance),
		vertex(vertex)
	{}

	struct Compare {
		bool operator()(const PriorityQueueItem& a, const PriorityQueueItem& b) const {
			return a.distance > b.distance;
		}
	};
};

class PriorityQueue {
public:
	PriorityQueue() :
		_queue()
	{}

	bool empty() {
		return _queue.empty();
	}

	void enque(const PriorityQueueItem& item) {
		_queue.push(item);
	}

	PriorityQueueItem deque() {
		PriorityQueueItem item = _queue.top();
		_queue.pop();
		return item;
	}

private:
	std::priority_queue<PriorityQueueItem, std::vector<PriorityQueueItem>, PriorityQueueItem::Compare> _queue;
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
	Graph(const Graph& graph, const UnorderedSetofEdge& edgesToDrop) :
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
#if 0
		// debug
		{
			std::ofstream outputfile("matrix.csv");
			for(size_t i = 0; i < _N; i++) {
				for(size_t j = 0; j < _N; j++) {
					outputfile << _distanceMatrix[i][j] << ", ";
				}
				outputfile << std::endl;
			}
		}
#endif

		//return __makeRandomSchedule(D, K);
		return __makeScheduleInMaxDistanceOrder(D, K);
		//return __makeAverageSchedule(D, K);
		//return __makeFastestSchedule(D, K);
	}

private:
	const uint16_t _N;
	const uint16_t _M;
	const std::vector<Edge> _originalEdges;

	UnorderedSetofEdge _availableEdges;
	std::vector<std::vector<Edge>> _edgesOnVertex;
#define N_MAX 1000
	std::array<std::array<uint32_t, N_MAX>, N_MAX> _distanceMatrix;
	uint64_t _totalDistance;

	void __initializeEdgesOnVertex() {
		for(const Edge& edge : _availableEdges) {
			uint16_t u = edge.u();
			uint16_t v = edge.v();

			_edgesOnVertex[u-1].emplace_back(edge);
			_edgesOnVertex[v-1].emplace_back(edge.getReversedEdge());
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

	//void __initializeDistanceMatrix(uint16_t vertex) {
	//	static PriorityQueue priorityQueue;

	//	priorityQueue.enque(0, std::vector<uint16_t>{vertex});

	//	while(!priorityQueue.empty()) {
	//		Trajectory trajectory = priorityQueue.deque();
	//		for(const Edge& edge : _edgesOnVertex[trajectory.getEndVertex()-1]) {
	//			if(__updateDistanceMatrix(vertex, edge.v(), trajectory.getDistance() + edge.w())) {
	//				priorityQueue.enque(trajectory, edge.w(), edge.v());
	//			}
	//		}
	//	}
	//}

	void __initializeDistance() {
		for(auto& row : _distanceMatrix) {
			row.fill(UNREACHABLE_DISTANCE);
		}
		for(size_t i = 0; i < _N; i++) {
			_distanceMatrix[i][i] = 0;
		}

		for(uint16_t vertex = 1; vertex <= _N; vertex++) {
			__initializeDistanceMatrix(vertex);
		}

		uint64_t temp = 0;
		for(size_t i = 0; i < _N; i++) {
			for(size_t j = i + 1; j < _N; j++) {
				temp += _distanceMatrix[i][j];
			}
		}
		_totalDistance = temp * 2;

	}

	uint64_t __calcScore(int D, const std::vector<UnorderedSetofEdge>& dropEdgesForEachDay) {
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

		for(int i = 0; i < 10; i++) {
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

	std::vector<int> __makeScheduleInMaxDistanceOrder(int D, int K) {
		struct Temp {
			uint32_t maxDistance;
			uint32_t vertex;

			struct Compare {
				bool operator()(const Temp& a, const Temp& b) const {
					return a.maxDistance < b.maxDistance;
				}
			};
		};
		std::vector<Temp> v(_N);
		for(size_t i = 0; i < _N; i++) {
			auto it = _distanceMatrix[i].begin();
			std::advance(it, _N);
			v[i].maxDistance = *std::max_element(_distanceMatrix[i].begin(), it);
			v[i].vertex = i;
		}
		std::sort(v.begin(), v.end(), Temp::Compare());

		std::vector<int> schedule(_M);
		std::vector<UnorderedSetofEdge> dropEdgesForEachDay(D);
		UnorderedSetofEdge alreadyDropped;
		int day = 1;

		for(size_t i = 0; i < _N; i++) {
			uint32_t vertex = v[i].vertex;
			for(size_t j = 0; j < _edgesOnVertex[vertex].size(); j++) {
				Edge edge = _edgesOnVertex[vertex][j];
				if(alreadyDropped.find(edge) != alreadyDropped.end()) {
				//if(dropEdgesForEachDay[day-1].find(edge) != dropEdgesForEachDay[day-1].end()) {
					continue;
				}
				schedule[edge.id()] = day;
				dropEdgesForEachDay[day-1].emplace(edge);
				alreadyDropped.emplace(edge);
				day++;
				if(day > D) {
					day = 1;
				}
			}
		}

		uint64_t score = __calcScore(D, dropEdgesForEachDay);
		Log_debug(format("Final score: %lu", score));
		return schedule;
	}

	std::vector<int> __makeAverageSchedule(int D, int K) {
		std::vector<int> schedule;
		std::vector<UnorderedSetofEdge> dropEdgesForEachDay(D);
		int day = 1;

		for(int i = 0; i < _M; i++) {
			schedule.emplace_back(day);
			dropEdgesForEachDay[day-1].emplace(_originalEdges[i]);
			day++;
			if(day > D) {
				day = 1;
			}
		}

		uint64_t score = __calcScore(D, dropEdgesForEachDay);
		Log_debug(format("Final score: %lu", score));
		return schedule;
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
			edges.emplace_back(i, u, v, w);
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

		// Debug info
		std::vector<int> histogram(_D);
		for(int day : schedule) {
			histogram[day-1]++;
		}
		for(size_t i = 0; i < histogram.size(); i++) {
			Log_info(format("day%2d: %3d edges", i, histogram[i]));
		}
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

	Log_debug("[sizeof]");
	Log_debug(format("Clock:             %u", sizeof(Clock)));
	Log_debug(format("Edge:              %u", sizeof(Edge)));
	Log_debug(format("PriorityQueueItem: %u", sizeof(PriorityQueueItem)));
	Log_debug(format("PriorityQueue:     %u", sizeof(PriorityQueue)));
	Log_debug(format("Graph:             %u", sizeof(Graph)));
	Log_debug(format("Main:              %u", sizeof(Main)));
	return 0;
}

