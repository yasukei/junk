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

class DistanceMatrix {
public:
	DistanceMatrix()
	{}

	void initialize(const std::vector<std::vector<Edge>>& edgesOnVertex) {
		const size_t N = edgesOnVertex.size();

		_distanceMatrix.resize(N);
		for(auto& row : _distanceMatrix) {
			row.resize(N, UNREACHABLE_DISTANCE);
		}
		for(size_t i = 0; i < N; i++) {
			_distanceMatrix[i][i] = 0;
		}

		for(uint16_t vertex = 0; vertex < N; vertex++) {
			__initializeDistanceMatrix(vertex, edgesOnVertex);
		}

		uint64_t temp = 0;
		for(size_t i = 0; i < N; i++) {
			for(size_t j = i + 1; j < N; j++) {
				temp += _distanceMatrix[i][j];
			}
		}
		_totalDistance = temp * 2;
	}

	uint32_t getValue(size_t i, size_t j) { return _distanceMatrix[i][j]; }
	uint64_t getTotalDistance() { return _totalDistance; }

	std::vector<int> getVerticesInMaxDistanceAscendingOrder() const {
		struct Temp {
			uint32_t maxDistance;
			uint32_t vertex;

			struct Compare {
				bool operator()(const Temp& a, const Temp& b) const {
					return a.maxDistance < b.maxDistance;
				}
			};
		};

		const size_t N = _distanceMatrix.size();
		std::vector<Temp> temp(N);
		for(size_t i = 0; i < N; i++) {
			auto it = _distanceMatrix[i].begin();
			std::advance(it, N);
			temp[i].maxDistance = *std::max_element(_distanceMatrix[i].begin(), it);
			temp[i].vertex = i;
		}
		std::sort(temp.begin(), temp.end(), Temp::Compare());

		std::vector<int> result(N);
		for(size_t i = 0; i < N; i++) {
			result[i] = temp[i].vertex;
		}
		return result;
	}

	void saveAs(const std::string& filePath="matrix.csv") {
		std::ofstream outputfile(filePath);

		for(size_t i = 0; i < _distanceMatrix.size(); i++) {
			for(size_t j = 0; j < _distanceMatrix.size(); j++) {
				outputfile << _distanceMatrix[i][j] << ", ";
			}
			outputfile << std::endl;
		}
	}

private:
	std::vector<std::vector<uint32_t>> _distanceMatrix;
	uint64_t _totalDistance;

	bool __updateDistanceMatrix(int vertex1, int vertex2, uint32_t distance) {
		if(_distanceMatrix[vertex1][vertex2] > distance) {
			_distanceMatrix[vertex1][vertex2] = distance;
			return true;
		}
		return false;
	}

	void __initializeDistanceMatrix(uint16_t vertex, const std::vector<std::vector<Edge>>& edgesOnVertex) {
		static PriorityQueue priorityQueue;

		priorityQueue.enque(PriorityQueueItem(0, vertex));

		while(!priorityQueue.empty()) {
			PriorityQueueItem item = priorityQueue.deque();
			for(const Edge& edge : edgesOnVertex[item.vertex]) {
				if(__updateDistanceMatrix(vertex, edge.v(), item.distance + edge.w())) {
					priorityQueue.enque(PriorityQueueItem(item.distance + edge.w(), edge.v()));
				}
			}
		}
	}

};

class EdgesNormalizer {
public:
	EdgesNormalizer()
	{}

	UnorderedSetofEdge normalize(const UnorderedSetofEdge& edges) {
		UnorderedSetofEdge normalizedEdges;
		uint16_t normalizedIndex = 0;

		normalizedEdges.reserve(edges.size());
		_map.clear();
		_rmap.clear();

		for(const Edge& edge : edges) {
			if(_map.find(edge.u()) == _map.end()) {
				_map[edge.u()] = normalizedIndex;
				_rmap[normalizedIndex] = edge.u();
				normalizedIndex++;
			}
			if(_map.find(edge.v()) == _map.end()) {
				_map[edge.v()] = normalizedIndex;
				_rmap[normalizedIndex] = edge.v();
				normalizedIndex++;
			}
			normalizedEdges.emplace(edge.id(), _map[edge.u()], _map[edge.v()], edge.w());
		}
		return normalizedEdges;
	}
	UnorderedSetofEdge denormalize(const UnorderedSetofEdge& edges) {
		UnorderedSetofEdge denormalizedEdges;

		for(const Edge& edge : edges) {
			denormalizedEdges.emplace(edge.id(), _rmap[edge.u()], _rmap[edge.v()], edge.w());
		}
		return denormalizedEdges;
	}

private:
	std::unordered_map<uint16_t, uint16_t> _map;
	std::unordered_map<uint16_t, uint16_t> _rmap;
};

class Graph {
public:
	Graph(const std::vector<Edge>& edges) :
		_vertices(),
		_edges(edges.begin(), edges.end()),
		_edgesOnVertex(),
		_distanceMatrix()
	{
		for(const Edge& edge : edges) {
			_vertices.emplace(edge.u());
			_vertices.emplace(edge.v());
		}
		__initializeEdgesOnVertex();
		_distanceMatrix.initialize(_edgesOnVertex);
	}
	Graph(const Graph& graph, const UnorderedSetofEdge& edgesToDrop) :
		_vertices(graph._vertices),
		_edges(graph._edges),
		_edgesOnVertex(),
		_distanceMatrix()
	{
		for(const Edge& edge : edgesToDrop) {
			_edges.erase(edge);
		}

		__initializeEdgesOnVertex();
		_distanceMatrix.initialize(_edgesOnVertex);
	}

	std::vector<int> makeSchedule(int D, int K, const Clock& clock) {
		return __makeScheduleWithPartitioning(D, K, clock);
		//return __makeScheduleInMaxDistanceOrder(D, K);
	}

private:
	std::unordered_set<uint16_t> _vertices;
	UnorderedSetofEdge _edges;
	std::vector<std::vector<Edge>> _edgesOnVertex;

	DistanceMatrix _distanceMatrix;

	void __initializeEdgesOnVertex() {
		_edgesOnVertex.resize(_vertices.size());
		for(const Edge& edge : _edges) {
			uint16_t u = edge.u();
			uint16_t v = edge.v();

			_edgesOnVertex[u].emplace_back(edge);
			_edgesOnVertex[v].emplace_back(edge.getReversedEdge());
		}
	}

	uint64_t __calcScore(int D, const std::vector<UnorderedSetofEdge>& dropEdgesForEachDay) {
		double score = 0.0;
		double N = _vertices.size();

		for(const UnorderedSetofEdge& edgesToDrop : dropEdgesForEachDay) {
			if(edgesToDrop.size() == 0) {
				continue;
			}

			Graph graph(*this, edgesToDrop);
			uint64_t diff = graph._distanceMatrix.getTotalDistance() - _distanceMatrix.getTotalDistance();
			double fk = diff / (N * (N-1.0));
			score += fk;
			//Log_debug(format("diff:                 %18lu", diff));
			//Log_debug(format("fk:                               %10.3lf", fk));
			Log_debug(format("Score:                            %10.3lf", score));
		}
		score *= 1000.0 / D;
		return (uint64_t)std::round(score);
	}

	UnorderedSetofEdge __getEdgesWithinGivenVertices(const std::unordered_set<uint16_t>& vertices) {
		UnorderedSetofEdge edges;

		for(const uint16_t& vertex : vertices) {
			for(const Edge& edge : _edgesOnVertex[vertex]) {
				if(vertices.find(edge.u()) == vertices.end()) {
					continue;
				}
				if(vertices.find(edge.v()) == vertices.end()) {
					continue;
				}
				edges.emplace(edge);
			}
		}
		Log_debug(format("vertices.size(): %u", vertices.size()));
		Log_debug(format("edges.size():    %u", edges.size()));

		return edges;
	}

	static std::vector<UnorderedSetofEdge> __makeScheduleInMaxDistanceOrder(int D, const Graph& graph) {
		std::vector<int> verticesInMaxDistanceAscending = graph._distanceMatrix.getVerticesInMaxDistanceAscendingOrder();
		std::vector<UnorderedSetofEdge> dropEdgesForEachDay(D);
		UnorderedSetofEdge alreadyDropped;
		int day = 1;

		for(const uint16_t& vertex : verticesInMaxDistanceAscending) {
			for(const Edge& edge : graph._edgesOnVertex[vertex]) {
				if(alreadyDropped.find(edge) != alreadyDropped.end()) {
					continue;
				}
				dropEdgesForEachDay[day-1].emplace(edge);
				alreadyDropped.emplace(edge);
				day++;
				if(day > D) {
					day = 1;
				}
			}
		}

		return dropEdgesForEachDay;
	}

	std::vector<UnorderedSetofEdge> __exploreBestSchedule(uint32_t D, uint32_t K, const UnorderedSetofEdge& edges, const Clock& clock) {
		Graph baseGraph(std::vector<Edge>(edges.begin(), edges.end()));

		std::vector<int> verticesInMaxDistanceAscending = baseGraph._distanceMatrix.getVerticesInMaxDistanceAscendingOrder();
		std::vector<UnorderedSetofEdge> dropEdgesForEachDay(D);
		UnorderedSetofEdge alreadyDropped;

		for(const uint16_t& vertex : verticesInMaxDistanceAscending) {
			for(const Edge& edge : baseGraph._edgesOnVertex[vertex]) {
				if(alreadyDropped.find(edge) != alreadyDropped.end()) {
					continue;
				}

				uint64_t bestScore = UINT64_MAX;
				size_t bestIndex;
				for(size_t i = 0; i < dropEdgesForEachDay.size(); i++) {
					UnorderedSetofEdge& dropEdges = dropEdgesForEachDay[i];

					if(dropEdges.size() == K) {
						continue;
					}

					dropEdges.emplace(edge);
					Graph graph(baseGraph, dropEdges);
					dropEdges.erase(edge);

					uint64_t totalDistance = graph._distanceMatrix.getTotalDistance();
					if(bestScore > totalDistance) {
						bestScore = totalDistance;
						bestIndex = i;
					}
				}
				dropEdgesForEachDay[bestIndex].emplace(edge);
				alreadyDropped.emplace(edge);
			}
			if(clock.getElapsedTimeMillisec() > 5500) {
				Log_debug("break!");
				break;
			}
		}
		Log_debug(format("current elapsed time: %llu[ms]", clock.getElapsedTimeMillisec()));

		return dropEdgesForEachDay;
	}

	void __makeScheduleByGivenVertices(
			uint32_t D,
			uint32_t K,
			const std::unordered_set<uint16_t>& vertices,
			const Clock& clock,
			std::vector<UnorderedSetofEdge>& dropEdgesForEachDay, // out
			UnorderedSetofEdge& alreadyDropped                    // out
			) {
		UnorderedSetofEdge edges = __getEdgesWithinGivenVertices(vertices);

		EdgesNormalizer edgesNormalizer;
		UnorderedSetofEdge normalizedEdges = edgesNormalizer.normalize(edges);

		std::vector<UnorderedSetofEdge> bestDropEdgesForEachDay = __exploreBestSchedule(D, K, normalizedEdges, clock);

		for(size_t i = 0; i < bestDropEdgesForEachDay.size(); i++) {
			for(const Edge& edge : edgesNormalizer.denormalize(bestDropEdgesForEachDay[i])) {
				dropEdgesForEachDay[i].emplace(edge);
			}
			for(const Edge& edge : dropEdgesForEachDay[i]) {
				alreadyDropped.emplace(edge);
			}
		}
	}

	std::vector<int> __makeScheduleWithPartitioning(uint32_t D, uint32_t K, const Clock& clock) {
		const size_t N = _vertices.size();
		std::vector<int> verticesInMaxDistanceAscending = _distanceMatrix.getVerticesInMaxDistanceAscendingOrder();

		std::vector<UnorderedSetofEdge> dropEdgesForEachDay(D);
		UnorderedSetofEdge alreadyDropped;

		const size_t numofPartitions = 7;
		std::unordered_set<size_t> targetPartition = {0};
		for(size_t i = 0; i < numofPartitions; i++) {
			if(targetPartition.find(i) == targetPartition.end()) {
				continue;
			}
			std::unordered_set<uint16_t> vertices;

			for(size_t j = N*i/numofPartitions; j < N*(i+1)/numofPartitions; j++) {
				vertices.emplace(verticesInMaxDistanceAscending[j]);
			}
			__makeScheduleByGivenVertices(D, K, vertices, clock, dropEdgesForEachDay, alreadyDropped);
		}

		std::vector<int> schedule(_edges.size());
		for(size_t i = 0; i < dropEdgesForEachDay.size(); i++) {
			for(const Edge& edge : dropEdgesForEachDay[i]) {
				schedule[edge.id()] = i + 1;
			}
		}

		uint32_t day = 1;
		for(size_t i = 0; i < N; i++) {
			uint32_t vertex = verticesInMaxDistanceAscending[i];
			for(size_t j = 0; j < _edgesOnVertex[vertex].size(); j++) {
				Edge edge = _edgesOnVertex[vertex][j];
				if(alreadyDropped.find(edge) != alreadyDropped.end()) {
					continue;
				}
				while(dropEdgesForEachDay[day-1].size() == K) {
					day++;
					if(day > D) {
						day = 1;
					}
				}
				dropEdgesForEachDay[day-1].emplace(edge);
				schedule[edge.id()] = day;
				alreadyDropped.emplace(edge);
				day++;
				if(day > D) {
					day = 1;
				}
			}
		}

#ifdef LOG_ENABLED
		uint64_t score = __calcScore(D, dropEdgesForEachDay);
		Log_debug(format("Final score: %lu", score));
#endif // LOG_ENABLED
		return schedule;
	}

	std::vector<int> __makeScheduleInMaxDistanceOrder(int D, int K) {
		const size_t N = _vertices.size();
		std::vector<int> verticesInMaxDistanceAscending = _distanceMatrix.getVerticesInMaxDistanceAscendingOrder();

		std::vector<int> schedule(_edges.size());
		std::vector<UnorderedSetofEdge> dropEdgesForEachDay(D);
		UnorderedSetofEdge alreadyDropped;
		int day = 1;

		for(size_t i = 0; i < N; i++) {
			uint32_t vertex = verticesInMaxDistanceAscending[i];
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
			edges.emplace_back(i, u-1, v-1, w);
		}

		// Coordinate
		//std::vector<Vertex> vertexCoordinates;
		//vertexCoordinates.reserve(_N);
		//for(int i = 0; i < _N; i++) {
		//	int x, y;
		//	std::cin >> x >> y;
		//	vertexCoordinates.emplace_back(x, y);
		//}

		_graph = new Graph(std::move(edges));

		Log_info(format("N: %d, M: %d, D: %d, K: %d", _N, _M, _D, _K));
	}

	void run() {
		std::vector<int> schedule = _graph->makeSchedule(_D, _K, _clock);
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

