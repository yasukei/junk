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
// Edge, Trajectory, PriorityQueue, Graph
// -----------------------------------------------------------------------------
static const int VERTEX_NONE = -1;

class Edge {
public:
	Edge(int u, int v, int d)  :
		_u(u),
		_v(v),
		_d(d)
	{}

	std::string toString() const {
		return "u=" + std::to_string(_u)
				+ ", v=" + std::to_string(_v)
				+ ", d=" + std::to_string(_d);
	}

	int u() const { return _u; }
	int v() const { return _v; }
	int d() const { return _d; }

private:
	int _u;
	int _v;
	int _d;
};

class Trajectory {
public:
	Trajectory(const std::vector<int>& vertices, int distance) :
		_vertices(vertices),
		_distance(distance)
	{}
	Trajectory(const Trajectory& original, int additionalVertex, int additionalDistance) :
		_vertices(original._vertices),
		_distance(original._distance)
	{
		_vertices.emplace_back(additionalVertex);
		_distance += additionalDistance;
	}

	std::string toString() const {
		return "distance=" + std::to_string(_distance)
				+ ", vertices=" + to_string(_vertices);
	}

	const std::vector<int>& getVertices() const { return _vertices; }
	int getStartVertex() const { return _vertices.front(); }
	int getEndVertex() const { return _vertices.back(); }
	int getDistance() const { return _distance; }

private:
	std::vector<int> _vertices;
	int _distance;
};

class PriorityQueue {
public:
	PriorityQueue() :
		_priorityQueue()
	{}

	bool empty() const {
		return _priorityQueue.empty();
	}

	void enque(std::shared_ptr<Trajectory>& trajectoryPtr) {
		_priorityQueue.push(trajectoryPtr);
	}

	std::shared_ptr<Trajectory> deque() {
		std::shared_ptr<Trajectory> trajectoryPtr = _priorityQueue.top();
		_priorityQueue.pop();
		return trajectoryPtr;
	}

	struct Compare {
		bool operator()(const std::shared_ptr<Trajectory>& a, const std::shared_ptr<Trajectory>& b) const {
			return a->getDistance() > b->getDistance();
		}
	};

private:
	std::priority_queue<
		std::shared_ptr<Trajectory>,
		std::vector<std::shared_ptr<Trajectory>>,
		PriorityQueue::Compare> _priorityQueue;
};

static int getVertexPairHash(int vertex1, int vertex2) {
	return vertex1 << 16 | vertex2;
}

class Graph {
public:
	Graph(int Nv, int Ne, const std::vector<Edge>& edges) :
		_trajectory(),
		_priorityQueues(),
		_priorityQueuesMemo(),
		_edges(),
		_Nv(Nv),
		_Ne(Ne)
	{
		std::unordered_set<int> vertices;
		vertices.reserve(Nv);
		_edges.reserve(Ne * 2);

		for(const auto& edge : edges) {
			int u = edge.u();
			int v = edge.v();
			int d = edge.d();

			_edges.emplace(u, edge);
			_edges.emplace(v, Edge(v, u, d));

			vertices.emplace(u);
			vertices.emplace(v);
		}

		_trajectory.reserve(Nv * Nv);
		for(const auto& edge : edges) {
			int u = edge.u();
			int v = edge.v();
			int d = edge.d();

			_trajectory.emplace(getVertexPairHash(u, v), std::make_shared<Trajectory>(std::vector<int>{u, v}, d));
			_trajectory.emplace(getVertexPairHash(v, u), std::make_shared<Trajectory>(std::vector<int>{v, u}, d));
			_trajectory.emplace(getVertexPairHash(u, u), std::make_shared<Trajectory>(std::vector<int>{u}, 0));
			_trajectory.emplace(getVertexPairHash(v, v), std::make_shared<Trajectory>(std::vector<int>{v}, 0));
		}

		for(const auto& vertex : vertices) {
			PriorityQueue priorityQueue;
			std::unordered_set<int> memo;
			memo.reserve(vertices.size());

			memo.emplace(vertex);
			auto result = _edges.equal_range(vertex);
			for(auto& it = result.first; it != result.second; it++) {
				const Edge& edge = it->second;
				priorityQueue.enque(_trajectory[getVertexPairHash(vertex, edge.v())]);
				memo.emplace(edge.v());
			}
			_priorityQueues[vertex] = std::move(priorityQueue);
			_priorityQueuesMemo[vertex] = std::move(memo);
		}
	}

	std::string toString() {
		return format("Nv=%d, Ne=%d", _Nv, _Ne);
	}

	const Trajectory& getTrajectory(int start, int goal) {
		//Log_debug("start=%d, goal=%d\n", start, goal);
		int startgoal = getVertexPairHash(start, goal);
		if(_trajectory.find(startgoal) != _trajectory.end()) {
			return *_trajectory[startgoal];
		}

		PriorityQueue& priorityQueue = _priorityQueues[start];
		std::unordered_set<int>& memo = _priorityQueuesMemo[start];

		bool goalFound = false;
		while(!goalFound) {
			std::shared_ptr<Trajectory> trajectory = priorityQueue.deque();

			auto result = _edges.equal_range(trajectory->getEndVertex());
			for(auto& it = result.first; it != result.second; it++) {
				const Edge& edge = it->second;
				int edgeV = edge.v();
				if(memo.find(edgeV) != memo.end()) {
					continue;
				}
				memo.emplace(edgeV);

				auto start_edgeV = getVertexPairHash(start, edgeV);
				_trajectory.emplace(start_edgeV, std::make_shared<Trajectory>(*trajectory, edgeV, edge.d()));
				priorityQueue.enque(_trajectory[start_edgeV]);

				if(edgeV == goal) {
					goalFound = true;
				}
			}
		}
		//Log_debug(format("loadFactor=%f, maxLoadFactor=%f, bucketCount=%llu, maxBucketCount=%llu, size=%llu",
		//			_trajectory.load_factor(), _trajectory.max_load_factor(), _trajectory.bucket_count(), _trajectory.max_bucket_count(), _trajectory.size()));
		return *_trajectory[startgoal];
	}

	int getDistance(int vertex1, int vertex2) {
		return getTrajectory(vertex1, vertex2).getDistance();
	}

	static Graph* createGraphFromStdin() {
		int Nv, Ne;

		std::cin >> Nv >> Ne;
		std::vector<Edge> edges;
		for(int i = 0; i < Ne; i++) {
			int u, v, d;
			std::cin >> u >> v >> d;
			edges.emplace_back(u, v, d);
		}
		return new Graph(Nv, Ne, std::move(edges));
	}

private:
	std::unordered_map<int, std::shared_ptr<Trajectory>> _trajectory;
	std::unordered_map<int, PriorityQueue> _priorityQueues;
	std::unordered_map<int, std::unordered_set<int>> _priorityQueuesMemo;

	std::unordered_multimap<int, Edge> _edges;
	int _Nv;
	int _Ne;
};

// -----------------------------------------------------------------------------
// Weather
// -----------------------------------------------------------------------------
class Weather {
public:
	Weather(int Tmax, int Tweather, int Nweather, const std::vector<std::vector<double>>& tpm, const std::vector<int>& limitConstants) :
		_Tmax(Tmax),
		_Tweather(Tweather),
		_Nweather(Nweather),
		_tpm(tpm),
		_limitConstants(limitConstants),
		_forecasts(Tmax / Tweather, std::vector<double>(Tweather)),
		_currentWeather()
	{}

	std::string toString() {
		std::string result = format("Tweather=%d, Nweather=%d\n", _Tweather, _Nweather);

		result += "  tpm:\n";
		for(int i = 0; i < _Nweather; i++) {
			result += "    ";
			for(int j = 0; j < _Nweather; j++) {
				result += format("%03lf, ", _tpm[i][j]);
			}
			result += "\n";
		}

		result += "  limitConstants: " + to_string(_limitConstants) + "\n";
		result += "  forecasts:";
		for(size_t i = 0; i < _forecasts.size(); i++) {
			result += "\n    ";
			for(size_t j = 0; j < _forecasts[i].size(); j++) {
				result += format("%03lf, ", _forecasts[i][j]);
			}
		}
		return result;
	}

	void readWeatherForecastFromStdin(int time) {
		if(((time - 1) % _Tweather) != 0) {
			return;
		}

		for(size_t timeIndex = (time - 1) / _Tweather; timeIndex < _forecasts.size(); timeIndex++) {
			int t;
			std::cin >> t;
			for(int i = 0; i < _Tweather; i++) {
				std::cin >> _forecasts[timeIndex][i];
			}
		}
	}
	void readCurrentWeatherFromStdin() {
		std::cin >> _currentWeather;
	}

	static Weather* createWeatherFromStdin(int Tmax) {
		int Tweather, Nweather;
		std::cin >> Tweather >> Nweather;

		std::vector<std::vector<double>> tpm(Nweather); // transition probability matrix
		for(int i = 0; i < Nweather; i++) {
			tpm.resize(Nweather);

			for(int j = 0; j < Nweather; j++) {
				std::cin >> tpm[i][j];
			}
		}

		std::vector<int> limitConstants(Nweather);
		for(int i = 0; i < Nweather; i++) {
			std::cin >> limitConstants[i];
		}

		return new Weather(Tmax, Tweather, Nweather, std::move(tpm), std::move(limitConstants));
	}

private:
	int _Tmax;
	int _Tweather;
	int _Nweather;
	std::vector<std::vector<double>> _tpm; // transition probability matrix
	std::vector<int> _limitConstants;
	std::vector<std::vector<double>> _forecasts;
	int _currentWeather;
};

// -----------------------------------------------------------------------------
// RewardFunction, Job, JobAdmin
// -----------------------------------------------------------------------------
class RewardFunction{
public:
	RewardFunction(const std::vector<int>& rewards) {
		size_t size = rewards.size() / 2;
		_ts.resize(size);
		_ys.resize(size);
		_peakValue = 0;
		for(size_t i = 0; i < size; i++) {
			_ts[i] = rewards[2*i];
			_ys[i] = rewards[2*i+1];

			if(_ys[i] > _peakValue) {
				_peakTime = _ts[i];
				_peakValue = _ys[i];
			}
		}

		_cumulativeSum.resize(_ts.back() + 2, 0.0);
		for(int t = _ts.front(); t < _ts.back() + 1; t++) {
			_cumulativeSum[t+1] = _cumulativeSum[t] + call(t);
		}
	}

	std::string toString() const {
		std::string result = "[ ";
		for(size_t i = 0; i < _ts.size(); i++) {
			result += "t=" + std::to_string(_ts[i]) + ", y=" + std::to_string(_ys[i]) + ", ";
		}
		result += "]";
		return result;
	}

	int operator()(int t) const {
		return call(t);
	}

	int getStartTime() const { return _ts.front() + 1; }
	int getEndTime() const { return _ts.back(); }
	int getDuration() const { return getEndTime() - getStartTime(); }
	int getPeakTime() const { return _peakTime; }
	int getPeakValue() const { return _peakValue; }
	double getTotalReward(int startTime, int endTime) const {
		return _cumulativeSum[endTime] - _cumulativeSum[startTime];
	}

private:
	std::vector<int> _ts;
	std::vector<int> _ys;
	int _peakValue;
	int _peakTime;
	std::vector<double> _cumulativeSum;

	double call(int t) const {
		if(t < _ts.front()) {
			return _ys.front();
		} else if(t >= _ts.back()) {
			return _ys.back();
		} else {
			auto it = std::upper_bound(_ts.begin(), _ts.end(), t);
			size_t index = std::distance(_ts.begin(), it);
			int tPrev = _ts[index-1];
			double yPrev = _ys[index-1];
			int tNext = _ts[index];
			double yNext = _ys[index];
			return (yNext - yPrev) * (t - tPrev) / (tNext - tPrev) + yPrev;
		}
	}
};

class Job {
public:
	Job(int jobId, int jobType, int numofTasks, int vertex,
			double penalty, double weatherDependency, bool mandatory,
			const std::vector<int>& rewards, const std::vector<int>& priorJobIds) :
		_jobId(jobId),
		_jobType(jobType),
		_numofTasks(numofTasks),
		_vertex(vertex),
		_penalty(penalty),
		_weatherDependency(weatherDependency),
		_mandatory(mandatory),
		_rewardFunction(rewards),
		_priorJobIds(priorJobIds),
		_allPriorJobIds(),
		_reserved(false),
		_remainingTasks(numofTasks)
	{}

	std::string toString() const {
		std::string result = format("jobId=%d, jobType=%d, numofTasks=%d, vertex=%d, penalty=%lf, weatherDependency=%lf, mandatory=%d",
									_jobId, _jobType, _numofTasks, _vertex, _penalty, _weatherDependency, _mandatory);
		result += format("\nrewardFunction=%s", _rewardFunction.toString().c_str());
		result += format("\npriorJobIds=%s", to_string(_priorJobIds).c_str());
		return result;
	}

	int getJobId() const { return _jobId; }
	int getJobType() const { return _jobType; }
	int getNumofTasks() const { return _numofTasks; }
	int getVertex() const { return _vertex; }

	double getPenalty() { return _penalty; }
	double getWeatherDependency() { return _weatherDependency; }
	bool isMandatory() { return _mandatory; }

	int getStartTime() const { return _rewardFunction.getStartTime(); }
	int getEndTime() const { return _rewardFunction.getEndTime(); }
	int getRewardPeakTime() const { return _rewardFunction.getPeakTime(); }
	int getRewardPeakValue() const { return _rewardFunction.getPeakValue(); }

	const std::vector<int>& getPriorJobIds() const { return _priorJobIds; }
	void setAllPriorJobIds(const std::set<int>& allPriorJobIds) {
		_allPriorJobIds = allPriorJobIds;
	}
	const std::set<int>& getAllPriorJobIds() { return _allPriorJobIds; }

	int getExecutionTime(int workload) const {
		return std::ceil((double)_numofTasks / (double)workload);
	}

	double getExpectedReward(int startTime, int workload) const {
		if(startTime < getStartTime()) {
			return 0.0;
		}

		int endTime = startTime + getExecutionTime(workload);
		if(getEndTime() < endTime) {
			return 0.0;
		}

		if(_numofTasks < workload) {
			workload = _numofTasks;
		}

		return _rewardFunction.getTotalReward(startTime, endTime) * ((double)_numofTasks)  / ((double)getExecutionTime(workload));
	}

	void reserve() {
		assert(!_reserved);
		_reserved = true;
	}
	bool isReserved() const { return _reserved; }
	void updateRemainingTasks(int remainingTasks) {
		_remainingTasks = remainingTasks;
	}
	int getRemainingTasks() { return _remainingTasks; }

	std::vector<int> getTaskListToExecute(int workload) const {
		std::vector<int> taskList;
		int remainingTasks = _numofTasks;
		while(remainingTasks > 0) {
			int numofTasks = std::min(workload, remainingTasks);
			taskList.emplace_back(numofTasks);
			remainingTasks -= numofTasks;
		}
		return taskList;
	}

private:
	int _jobId;
	int _jobType;
	int _numofTasks;
	int _vertex;

	double _penalty;
	double _weatherDependency;
	bool _mandatory;

	RewardFunction _rewardFunction;

	std::vector<int> _priorJobIds;
	std::set<int> _allPriorJobIds;

	bool _reserved;
	int _remainingTasks;
};

class JobAdmin {
public:
	JobAdmin(const std::vector<std::shared_ptr<Job>>& jobs) :
		_allJobs(jobs)
	{
		for(auto& job : _allJobs) {
			_job.emplace(job->getJobId(), job);
		}
		for(auto& job : _allJobs) {
			if(!job->getPriorJobIds().empty()) {
				job->setAllPriorJobIds(getAllPriorJobIds(job));
			}
		}

		std::sort(_allJobs.begin(), _allJobs.end(), JobAdmin::rewardDescendingOrder);
	}

	std::string toString() const {
		return format("Njob=%d", _allJobs.size());
	}

	const std::vector<std::shared_ptr<Job>>& getAllJobs() const { return _allJobs; }
	const std::shared_ptr<Job> getJob(int jobId) { return _job[jobId]; }

	std::vector<std::shared_ptr<Job>> getOpenJobsInHighRewardOrder(const std::vector<int>& jobTypes, size_t size=0) const {
		std::vector<std::shared_ptr<Job>> result;

		if(size == 0) {
			size = SIZE_MAX;
		}

		for(auto& job : _allJobs) {
			if(result.size() >= size) {
				break;
			}

			if(std::find(jobTypes.begin(), jobTypes.end(), job->getJobType()) == jobTypes.end()) {
				continue;
			}
			if(!job->getPriorJobIds().empty() || job->isReserved()) {
				continue;
			}
			result.emplace_back(job);
		}
		return result;
	}

	std::vector<std::shared_ptr<Job>> getHighRewardJobs(size_t size) {
		std::vector<std::shared_ptr<Job>> result;
		std::set<int> memo;

		for(auto& job : _allJobs) {
			std::set<int> allPriorJobIds = job->getAllPriorJobIds();
			std::set<int> remainingPriorJobIds;

			std::set_difference(
					allPriorJobIds.begin(), allPriorJobIds.end(),
					memo.begin(), memo.end(),
					std::inserter(remainingPriorJobIds, remainingPriorJobIds.end()));
			if(result.size() + 1 + remainingPriorJobIds.size() > size) {
				break;
			}

			result.emplace_back(job);
			memo.emplace(job->getJobId());
			for(int priorJobId : remainingPriorJobIds) {
				result.emplace_back(_job[priorJobId]);
				memo.emplace(priorJobId);
			}
		}

		return result;
	}

	void readJobInfoFromStdin() {
		int Nselected;
		std::cin >> Nselected;

		for(int i = 0; i < Nselected; i++) {
			int jobId, remainingTasks;
			std::cin >> jobId >> remainingTasks;

			_job[jobId]->updateRemainingTasks(remainingTasks);
		}
	}

	static bool rewardDescendingOrder(const std::shared_ptr<Job>& job1, const std::shared_ptr<Job>& job2) {
		return job1->getRewardPeakValue() > job2->getRewardPeakValue();
	}

	static JobAdmin* createJobAdminFromStdinForA() {
		return createJobAdminFromStdin(false);
	}
	static JobAdmin* createJobAdminFromStdinForB() {
		return createJobAdminFromStdin(true);
	}

private:
	std::vector<std::shared_ptr<Job>> _allJobs;
	std::unordered_map<int, std::shared_ptr<Job>> _job;

	std::set<int> getAllPriorJobIds(const std::shared_ptr<Job>& job) {
		std::set<int> result;

		std::function<void(const std::shared_ptr<Job>& job)> recurse = [&](const std::shared_ptr<Job>& job) {
			for(int priorJobId : job->getPriorJobIds()) {
				recurse(_job[priorJobId]);
				
				auto it = std::find(result.begin(), result.end(), priorJobId);
				if(it == result.end()) {
					result.emplace(priorJobId);
				}
			}
		};
		recurse(job);
		return result;
	}

	static JobAdmin* createJobAdminFromStdin(bool problemB) {
		int Njob;
		std::vector<std::shared_ptr<Job>> jobs;
		std::vector<int> rewards;
		std::vector<int> priorJobIds;

		std::cin >> Njob;
		for(int i = 0; i < Njob; i++) {
			int jobId, jobType, Ntask, vertex;
			std::cin >> jobId >> jobType >> Ntask >> vertex;

			double penalty = 0.0;
			double weatherDependency = 0.0;
			bool mandatory = false;
			if(problemB) {
				std::cin >> penalty >> weatherDependency >> mandatory;
			}

			int Nreward;
			std::cin >> Nreward;
			rewards.clear();
			for(int j = 0; j < Nreward; j++) {
				int t, y;
				std::cin >> t >> y;
				rewards.emplace_back(t);
				rewards.emplace_back(y);
			}

			int Ndepend;
			std::cin >> Ndepend;
			priorJobIds.clear();
			for(int j = 0; j < Ndepend; j++) {
				int priorJobId;
				std::cin >> priorJobId;
				priorJobIds.emplace_back(priorJobId);
			}
			jobs.emplace_back(std::make_shared<Job>(jobId, jobType, Ntask, vertex, penalty, weatherDependency, mandatory, rewards, priorJobIds));
		}

		return new JobAdmin(std::move(jobs));
	}

};

// -----------------------------------------------------------------------------
// Action, TimeSlot, Schedule
// -----------------------------------------------------------------------------
class Action {
public:
	virtual void notifyScheduleFixed() = 0;
	virtual std::vector<std::string> makeActionStrings() = 0;
	virtual std::string toString() = 0;
};

class StayAction : public Action {
public:
	StayAction(int stayTime) :
		_stayTime(stayTime)
	{}

	void notifyScheduleFixed()
	{}
	std::string toString() {
		return format("stay %d times", _stayTime);
	}
	std::vector<std::string> makeActionStrings() {
		return std::vector<std::string>(_stayTime, "stay");
	}

private:
	int _stayTime;
};

class MoveAction : public Action {
public:
	MoveAction(const Trajectory& trajectory, Graph& graph) :
		_trajectory(trajectory),
		_graph(graph)
	{}

	void notifyScheduleFixed()
	{}
	std::string toString() {
		return format("move %d -> %d", _trajectory.getStartVertex(), _trajectory.getEndVertex());
	}
	std::vector<std::string> makeActionStrings() {
		std::vector<std::string> result;

		const std::vector<int>& vertices = _trajectory.getVertices();
		size_t vertices_size = vertices.size();
		for(size_t i = 1; i < vertices_size; i++) {
			size_t distance = _graph.getDistance(vertices[i-1], vertices[i]);
			for(size_t j = 0; j < distance; j++) {
				result.emplace_back(format("move %d", vertices[i]));
			}
		}
		return result;
	}

private:
	const Trajectory& _trajectory;
	Graph& _graph;
};

class ExecuteAction : public Action {
public:
	ExecuteAction(Job& job, int workload) :
		_job(job),
		_workload(workload)
	{}

	void notifyScheduleFixed() {
		_job.reserve();
	}
	std::string toString() {
		return format("execute %d", _job.getJobId());
	}
	std::vector<std::string> makeActionStrings() {
		std::vector<int> taskList = _job.getTaskListToExecute(_workload);
		std::vector<std::string> result;

		for(size_t i = 0; i < taskList.size(); i++) {
			result.emplace_back(format("execute %d %d", _job.getJobId(), taskList[i]));
		}
		return result;
	}

private:
	Job& _job;
	int _workload;
};

class NoAction : public Action {
public:
	NoAction()
	{}

	void notifyScheduleFixed()
	{}
	std::string toString() {
		return "No action";
	}
	std::vector<std::string> makeActionStrings() {
		return std::vector<std::string>(1, "No action");
	}

private:
};

class TimeSlot {
public:
	TimeSlot(int startTime, int endTime, int startVertex, int endVertex, std::shared_ptr<Action> action) :
		_startTime(startTime),
		_endTime(endTime),
		_startVertex(startVertex),
		_endVertex(endVertex),
		_action(action)
	{
		assert(startTime < endTime);
	}

	std::string toString() const {
		return format("start=%03d, end=%03d, action={%s}", _startTime, _endTime, _action->toString().c_str());
	}

	bool operator<(const TimeSlot& rhs) const {
		return _startTime < rhs._startTime;
	}

	int getStartTime() const { return _startTime; }
	int getEndTime() const { return _endTime; }
	int getDuration() const { return _endTime - _startTime; }
	int getStartVertex() const { return _startVertex; }
	int getEndVertex() const { return _endVertex; }
	void notifyScheduleFixed() const { _action->notifyScheduleFixed(); }
	std::vector<std::string> makeActionStrings() const { return _action->makeActionStrings(); }

	static TimeSlot Stay(int startTime, int vertex, int stayTime) {
		return TimeSlot(startTime,
						startTime + stayTime,
						vertex,
						vertex,
						std::make_shared<StayAction>(stayTime));
	}
	static TimeSlot Move(int startTime, const Trajectory& trajectory, Graph& graph) {
		return TimeSlot(startTime,
						startTime + trajectory.getDistance(),
						trajectory.getStartVertex(),
						trajectory.getEndVertex(),
						std::make_shared<MoveAction>(trajectory, graph));
	}
	static TimeSlot Execute(int startTime, Job& job, int workload) {
		return TimeSlot(startTime,
						startTime + job.getExecutionTime(workload),
						job.getVertex(),
						job.getVertex(),
						std::make_shared<ExecuteAction>(job, workload));
	}
	static TimeSlot Blank(int startTime, int endTime, int startVertex, int endVertex) {
		return TimeSlot(startTime,
						endTime,
						startVertex,
						endVertex,
						std::make_shared<NoAction>());
	}

private:
	int _startTime;
	int _endTime;
	int _startVertex;
	int _endVertex;
	std::shared_ptr<Action> _action;
};

class Schedule {
public:
	static int START_TIME;

	Schedule(int Tmax, int initialVertex) :
		_Tmax(Tmax),
		_initialVertex(initialVertex)
	{}

	std::string toString() const {
		std::string result = format("Schedule: size=%d\n", _timeSlots.size());
		for(auto& timeSlot : _timeSlots) {
			result += timeSlot.toString() + "\n";
		}
		return result;
	}

	void operator+=(const TimeSlot& timeSlot) {
		assignTimeSlot(timeSlot);
	}

	int getTmax() const { return _Tmax; }
	int getInitialVertex() const { return _initialVertex; }

	void assignTimeSlot(const TimeSlot& newTimeslot) {
		auto it = _timeSlots.upper_bound(newTimeslot);

		if(it == _timeSlots.end()) {
			if(_timeSlots.size() > 0) {
				it = std::prev(_timeSlots.end());
				if((*it).getEndTime() <= newTimeslot.getStartTime()) {
					_timeSlots.emplace(newTimeslot);
					return;
				}
				assert(false);
			}
			_timeSlots.emplace(newTimeslot);
			return;
		}

		if(it == _timeSlots.begin()) {
			if(newTimeslot.getEndTime() <= (*it).getStartTime()) {
				_timeSlots.emplace(newTimeslot);
				return;
			}
			assert(false);
		}

		auto prev = std::prev(it);
		if((*prev).getEndTime() <= newTimeslot.getStartTime() && newTimeslot.getEndTime() <= (*it).getStartTime()) {
			_timeSlots.emplace(newTimeslot);
			return;
		}
		assert(false);
	}

	std::vector<TimeSlot> getBlankTimeSlots() {
		std::vector<TimeSlot> result;
		
		int currentTime = Schedule::START_TIME;
		int currentVertex = _initialVertex;

		for(auto& timeSlot : _timeSlots) {
			if(currentTime < timeSlot.getStartTime()) {
				result.emplace_back(TimeSlot::Blank(currentTime, timeSlot.getStartTime(), currentVertex, timeSlot.getStartVertex()));
			}
			currentTime = timeSlot.getEndTime();
			currentVertex = timeSlot.getEndVertex();
		}
		if(currentTime <= _Tmax) {
			result.emplace_back(TimeSlot::Blank(currentTime, _Tmax+1, currentVertex, VERTEX_NONE));
		}

		return result;
	}

	void fillNecessaryMoveInAllBlankTimeSlots(Graph& graph) {
		for(auto& blankTimeSlot : getBlankTimeSlots()) {
			int startVertex = blankTimeSlot.getStartVertex();
			int endVertex = blankTimeSlot.getEndVertex();

			if(startVertex == endVertex || endVertex == VERTEX_NONE) {
				continue;
			}
			*this += TimeSlot::Move(blankTimeSlot.getStartTime(), graph.getTrajectory(startVertex, endVertex), graph);
		}
	}

	void fillStayInAllBlankTimeSlots() {
		for(auto& blankTimeSlot : getBlankTimeSlots()) {
			int startTime = blankTimeSlot.getStartTime();
			int startVertex = blankTimeSlot.getStartVertex();
			int duration = blankTimeSlot.getDuration();

			*this += TimeSlot::Stay(startTime, startVertex, duration);
		}
	}

	void notifyScheduleFixed() {
		for(auto& timeSlot : _timeSlots) {
			timeSlot.notifyScheduleFixed();
		}
	}

	std::vector<std::string> makeActionStrings() {
		std::vector<std::string> result;

		for(auto& timeSlot : _timeSlots) {
			std::vector<std::string> actionStrings = timeSlot.makeActionStrings();
			result.insert(result.end(), actionStrings.begin(), actionStrings.end());
		}
		return result;
	}

	void makeJobScheduleAndActionStrings(std::vector<int>& jobSchedule, std::vector<std::string>& actionStrings) {
		jobSchedule.clear();
		actionStrings.clear();

		jobSchedule.resize(_Tmax);
		actionStrings = std::move(makeActionStrings());

		int start = 0;
		char dummyCharArray[16];
		int jobId;
		int dummyInt;
		for(size_t i = 0; i < actionStrings.size(); i++) {
			if(actionStrings[i].find("execute") != std::string::npos) {
				sscanf(actionStrings[i].c_str(), "%s %d %d", dummyCharArray, &jobId, &dummyInt);
				for(size_t j = start; j <= i; j++) {
					jobSchedule[j] = jobId;
				}
				start = i + 1;
			}
		}
	}

private:
	int _Tmax;
	int _initialVertex;
	std::set<TimeSlot> _timeSlots;
};

int Schedule::START_TIME = 1;

// -----------------------------------------------------------------------------
// Worker, WorkerManager
// -----------------------------------------------------------------------------
class Worker {
public:
	Worker(int workerId, int initialVertex, int workload, const std::vector<int>& jobTypes) :
		_workerId(workerId),
		_initialVertex(initialVertex),
		_workload(workload),
		_jobTypes(jobTypes)
	{}

	std::string toString() const {
		return "workerId=" + std::to_string(_workerId)
				+ ", initialVertex=" + std::to_string(_initialVertex)
				+ ", workload=" + std::to_string(_workload)
				+ ", jobTypes=" + to_string(_jobTypes);
	}

	int getWorkerId() const { return _workerId; }
	int getInitialVertex() const { return _initialVertex; }
	int getWorkload() const { return _workload; }
	const std::vector<int>& getJobTypes() const { return _jobTypes; }

	Schedule makeSchedule(int Tmax, Graph& graph, JobAdmin& jobAdmin) const {
		Schedule schedule(Tmax, _initialVertex);

		makeGreedySchedule(schedule, graph, jobAdmin, _jobTypes, _workload);
		//makeScheduleByDP(schedule, graph, jobAdmin, _jobTypes, _workload);
		Log_debug(format("%s", schedule.toString().c_str()));
		schedule.fillNecessaryMoveInAllBlankTimeSlots(graph);
		Log_debug(format("%s", schedule.toString().c_str()));
		schedule.fillStayInAllBlankTimeSlots();
		Log_debug(format("%s", schedule.toString().c_str()));

		return schedule;
	}

	static bool acceptableJob(const Job& job, const std::vector<int>& jobTypes) {
		auto it = std::find(jobTypes.begin(), jobTypes.end(), job.getJobType());
		return it != jobTypes.end() && !job.isReserved();
	}

	static bool canMoveAndExecute(const Job& job, Graph& graph, int workload, int startTime, int endTime, int startVertex, int endVertex) {
		if(job.getEndTime() <= startTime) { 
			return false;
		}

		int givenTime = endTime - startTime;
		int executionTime = job.getExecutionTime(workload);
		if(givenTime < executionTime) {
			return false;
		}

		int travelTimeBefore = graph.getDistance(startVertex, job.getVertex());
		if(givenTime < travelTimeBefore + executionTime) {
			return false;
		}

		double reward = job.getExpectedReward(startTime + travelTimeBefore, workload);
		if(reward == 0.0) {
			return false;
		}

		if(endVertex != VERTEX_NONE) {
			int travelTimeAfter = graph.getDistance(job.getVertex(), endVertex);
			if(givenTime < travelTimeBefore + executionTime + travelTimeAfter) {
				return false;
			}
		}

		return true;
	}

	static void makeGreedySchedule(Schedule& schedule, Graph& graph, JobAdmin& jobAdmin, const std::vector<int>& jobTypes, int workload) {
		int currentTime = Schedule::START_TIME;
		int currentVertex = schedule.getInitialVertex();
		int endTime = schedule.getTmax() + 1;

		std::vector<std::shared_ptr<Job>> openJobs = jobAdmin.getOpenJobsInHighRewardOrder(jobTypes);

		while(true) {
			double bestRating = 0.0;
			Job* bestJob = nullptr;

			for(auto& job : openJobs) {
				if(job->isReserved()) {
					continue;
				}
				if(!canMoveAndExecute(*job, graph, workload, currentTime, endTime, currentVertex, VERTEX_NONE)) {
					continue;
				}

				int travelTime = graph.getDistance(currentVertex, job->getVertex());
				double reward = job->getExpectedReward(currentTime + travelTime, workload);
				int executionTime = job->getExecutionTime(workload);
				double rating = reward / (travelTime + executionTime);

				if(rating > bestRating) {
					bestRating = rating;
					bestJob = &(*job);
				}

				for(auto& nextJob : openJobs) {
					if(nextJob == job) {
						continue;
					}
					if(nextJob->isReserved()) {
						continue;
					}
					if(!canMoveAndExecute(*nextJob, graph, workload, currentTime + travelTime + executionTime, endTime, job->getVertex(), VERTEX_NONE)) {
						continue;
					}
					int travelTime2 = graph.getDistance(job->getVertex(), nextJob->getVertex());
					double reward2 = nextJob->getExpectedReward(currentTime + travelTime + executionTime + travelTime2, workload);
					int executionTime2 = nextJob->getExecutionTime(workload);
					rating = (reward + reward2) / (travelTime + executionTime + travelTime2 + executionTime2);
					if(rating > bestRating) {
						bestRating = rating;
						bestJob = &(*job);
					}
				}
			}

			if(bestJob != nullptr) {
				int travelTime = graph.getDistance(currentVertex, bestJob->getVertex());
				TimeSlot execute = TimeSlot::Execute(currentTime + travelTime, *bestJob, workload);
				currentTime = execute.getEndTime();
				currentVertex = execute.getEndVertex();
				bestJob->reserve();
				schedule += execute;
			} else {
				break;
			}
		}
	}

	struct DpElement {
		double totalReward;
		std::set<int> doneJobIds;
		std::set<TimeSlot> timeSlots;

		DpElement() :
			totalReward(0.0),
			doneJobIds(),
			timeSlots()
		{}
		// TODO: get more efficient such as sharing objects using pointer, etc.
		DpElement(double totalReward, const std::set<int>& doneJobIds, const std::set<TimeSlot>& timeSlots) :
			totalReward(totalReward),
			doneJobIds(doneJobIds),
			timeSlots(timeSlots)
		{}
	};

	static void makeScheduleByDP(Schedule& schedule, Graph& graph, JobAdmin& jobAdmin, const std::vector<int>& jobTypes, int workload) {
		int Tmax = schedule.getTmax();
		int initialVertex = schedule.getInitialVertex();

		std::vector<std::shared_ptr<Job>> allJobs = jobAdmin.getOpenJobsInHighRewardOrder(jobTypes, 100);
		//auto allJobs = jobAdmin.getHighRewardJobs(100);
		for(auto& job : allJobs) {
			Log_debug(job->toString());
		}

		std::vector<std::vector<DpElement*>> dp(Tmax+1);
		for(auto& dpRow : dp) {
			dpRow.resize(allJobs.size()+1, nullptr);
		}
		dp[0][0] = new DpElement();

		for(int t = 1; t <= Tmax; t++) {
			Log_debug(format("t=%d", t));
			for(size_t i = 0; i <= allJobs.size(); i++) {
				if(i == 0) {
					dp[t][i] = dp[t-1][i];
					continue;
				}

				double maxReward = 0.0;
				std::set<int>* doneJobIds;
				std::set<TimeSlot>* timeSlots;

				std::shared_ptr<Job> job_i = allJobs[i-1];
				int job_i_executiontime = job_i->getExecutionTime(workload);
				double job_i_reward = job_i->getExpectedReward(t - job_i_executiontime, workload);
				const std::set<int>& allPriorJobIds = job_i->getAllPriorJobIds();

				if(job_i_reward > 0.0) {
					for(size_t j = 0; j <= allJobs.size(); j++) {
						int j_vertex;
						if(j == 0) {
							j_vertex = initialVertex;
						} else {
							j_vertex = allJobs[j-1]->getVertex();
						}
						int distance = graph.getDistance(j_vertex, job_i->getVertex());

						int j_time = t - distance - job_i_executiontime;
						if(j_time < 1) {
							continue;
						}
						if(dp[j_time][j] == nullptr) {
							continue;
						}
						DpElement* dpElement = dp[j_time][j];
						if(dpElement->doneJobIds.find(i) != dpElement->doneJobIds.end()) {
							continue;
						}
						if(!std::includes(
									dpElement->doneJobIds.begin(), dpElement->doneJobIds.end(),
									allPriorJobIds.begin(), allPriorJobIds.end())) {
							continue;
						}

						double reward = job_i_reward + dpElement->totalReward;
						if(reward > maxReward) {
							maxReward = reward;
							doneJobIds = &(dpElement->doneJobIds);
							timeSlots = &(dpElement->timeSlots);
						}
					}
				}

				if(dp[t-1][i] == nullptr) {
					if(maxReward > 0.0) {
						DpElement* dpElement = new DpElement(maxReward, *doneJobIds, *timeSlots);
						dpElement->doneJobIds.emplace(i);
						dpElement->timeSlots.emplace(
							TimeSlot::Execute(t - job_i_executiontime, *job_i, workload));
						dp[t][i] = dpElement;
					}
				} else {
					if(maxReward > dp[t-1][i]->totalReward) {
						DpElement* dpElement = new DpElement(maxReward, *doneJobIds, *timeSlots);
						dpElement->doneJobIds.emplace(i);
						dpElement->timeSlots.emplace(
							TimeSlot::Execute(t - job_i_executiontime, *job_i, workload));
						dp[t][i] = dpElement;
					} else {
						dp[t][i] = dp[t-1][i];
					}
				}
			}
		}

		double bestReward = 0.0;
		std::set<TimeSlot>* bestTimeSlots;
		for(size_t i = 0; i <= allJobs.size(); i++) {
			if(dp[Tmax][i] == nullptr) {
				continue;
			}
			Log_debug(format("i=%03d, totalReward=%lf", i, dp[Tmax][i]->totalReward));
			if(dp[Tmax][i]->totalReward > bestReward) {
				bestReward = dp[Tmax][i]->totalReward;
				bestTimeSlots = &(dp[Tmax][i]->timeSlots);
			}
		}
		Log_debug(format("bestReward=%lf", bestReward));
		for(auto& timeSlot : *bestTimeSlots) {
			schedule += timeSlot;
		}
		schedule.notifyScheduleFixed();
	}

private:
	int _workerId;
	int _initialVertex;
	int _workload;
	std::vector<int> _jobTypes;
};

class WorkerManager {
public:
	WorkerManager(const std::vector<Worker>& workers) :
		_workers(workers),
		_scheduleChangePenalty(),
		_scheduleChangeDecayRate(),
		_scheduleKeepRewardRate()
	{}

	std::string toString() {
		std::string result = format("Nworker=%d", _workers.size());
		for(const auto& worker : _workers) {
			result += "\n  " + worker.toString();
		}
		return result;
	}

	std::vector<std::vector<std::string>> makeOutput1ForA(int Tmax, Graph& graph, JobAdmin& jobAdmin) {
		auto workloadDescendingOrder = [](const Worker& a, const Worker& b) {
			return a.getWorkload() > b.getWorkload();
		};
		std::sort(_workers.begin(), _workers.end(), workloadDescendingOrder);
		
		std::vector<std::vector<std::string>> actionStrings(_workers.size());
		for(auto& worker : _workers) {
			Schedule schedule = worker.makeSchedule(Tmax, graph, jobAdmin);
			actionStrings[worker.getWorkerId()] = schedule.makeActionStrings();
		}

		return actionStrings;
	}

	void readScheduleInfoFromStdin() {
		std::cin >> _scheduleChangePenalty >> _scheduleChangeDecayRate >> _scheduleKeepRewardRate;
	}
	void readWorkerInfoFromStdin() {
		int workerId, u, v, dFromU;

		for(auto& worker : _workers) {
			(void)worker;
			std::cin >> workerId >> u >> v >> dFromU;
		}
	}

	void writeScheduledJobIdsToStdout(int Tmax, Graph& graph, JobAdmin& jobAdmin, Weather& weather) {
		std::set<int> scheduledJobIds;
		for(auto& job : jobAdmin.getAllJobs()) {
			if(job->isMandatory()) {
				scheduledJobIds.emplace(job->getJobId());

				for(int priorJobId : job->getAllPriorJobIds()) {
					scheduledJobIds.emplace(priorJobId);
				}
			}
		}

		std::cout << scheduledJobIds.size();
		for(int jobId : scheduledJobIds) {
			std::cout << " " << jobId;
		}
		std::cout << std::endl;
	}
	void writeScheduleAndActionToStdout(int Tmax, int time, Graph& graph, JobAdmin& jobAdmin, Weather& weather) {
		static bool atFirst = true;
		if(atFirst) {
			for(size_t i = 0; i < _workers.size(); i++) {
				_schedules.emplace_back(_workers[i].makeSchedule(Tmax, graph, jobAdmin));
			}
			atFirst = false;
		}

		std::vector<int> jobSchedule;
		std::vector<std::string> actionStrings;

		if(time == 1) {
			// output all schedule

		} else {
			// output updated schedule
		}

		// output action of the time

		// TODO: here
	}

	static WorkerManager* createWorkerManagerFromStdin() {
		int Nworker;
		std::vector<Worker> workers;
		std::vector<int> jobTypes;

		std::cin >> Nworker;
		for(int i = 0; i < Nworker; i++) {
			int v, Lmax, Njobtype;
			std::cin >> v >> Lmax >> Njobtype;

			jobTypes.clear();
			for(int j = 0; j < Njobtype; j++) {
				int jobType;
				std::cin >> jobType;
				jobTypes.emplace_back(jobType);
			}
			workers.emplace_back(i, v, Lmax, jobTypes);
		}

		return new WorkerManager(std::move(workers));
	}

private:
	std::vector<Worker> _workers;
	std::vector<Schedule> _schedules;

	double _scheduleChangePenalty;
	double _scheduleChangeDecayRate;
	double _scheduleKeepRewardRate;
};

// -----------------------------------------------------------------------------
// Test
// -----------------------------------------------------------------------------
#define EQUAL(value1, value2) \
	do {\
		auto v1 = value1;\
		auto v2 = value2;\
		if((v1 != v2)) {\
			Log_debug(format("Test fail: value1=%s, value2=%s", std::to_string(v1).c_str(), std::to_string(v2).c_str()));\
		}\
	} while(false);

static void TestRewardFunction() {
	std::vector<int> rewards = {5, 0, 10, 5, 15, 0};
	RewardFunction func(rewards);

	EQUAL(func(4), 0.0);
	EQUAL(func(5), 0.0);
	EQUAL(func(6), 1.0);
	EQUAL(func(7), 2.0);
	EQUAL(func(8), 3.0);
	EQUAL(func(9), 4.0);
	EQUAL(func(10), 5.0);
	EQUAL(func(11), 4.0);
	EQUAL(func(12), 3.0);
	EQUAL(func(13), 2.0);
	EQUAL(func(14), 1.0);
	EQUAL(func(15), 0.0);
	EQUAL(func(16), 0.0);

	EQUAL(func.getTotalReward(6, 7), 1.0);
	EQUAL(func.getTotalReward(6, 8), 3.0);
	EQUAL(func.getTotalReward(6, 9), 6.0);
	EQUAL(func.getTotalReward(6, 10), 10.0);
	EQUAL(func.getTotalReward(6, 11), 15.0);
	EQUAL(func.getTotalReward(6, 12), 19.0);
	EQUAL(func.getTotalReward(6, 13), 22.0);
	EQUAL(func.getTotalReward(6, 14), 24.0);
	EQUAL(func.getTotalReward(6, 15), 25.0);
}

static void TestJob() {
	int jobId = 1;
	int jobType = 2;
	int Ntask = 9;
	int vertex = 3;
	double penalty = 0.0;
	double weatherDependency = 0.0;
    bool mandatory = false;
	std::vector<int> rewards = {5, 0, 10, 5, 15, 0};
	std::vector<int> priorJobIds = {4};
	Job job(jobId, jobType, Ntask, vertex, penalty, weatherDependency, mandatory, rewards, priorJobIds);

	EQUAL(job.getStartTime(), 6);
	EQUAL(job.getEndTime(), 15);

	EQUAL(job.getExecutionTime(1), 9);
	EQUAL(job.getExecutionTime(2), 5);
	EQUAL(job.getExecutionTime(3), 3);
	EQUAL(job.getExecutionTime(4), 3);
	EQUAL(job.getExecutionTime(5), 2);
	EQUAL(job.getExecutionTime(6), 2);
	EQUAL(job.getExecutionTime(7), 2);
	EQUAL(job.getExecutionTime(8), 2);
	EQUAL(job.getExecutionTime(9), 1);
	EQUAL(job.getExecutionTime(10), 1);
	EQUAL(job.getExecutionTime(11), 1);

	EQUAL(job.getExpectedReward(5, 1), 0.0);
	EQUAL(job.getExpectedReward(6, 1), 25.0);
	EQUAL(job.getExpectedReward(7, 1), 0.0);
	EQUAL(job.getExpectedReward(5, 2), 0.0);
	EQUAL(job.getExpectedReward(6, 2), 30.0);
	EQUAL(job.getExpectedReward(7, 2), 36.0);
	EQUAL(job.getExpectedReward(8, 2), 38.0);
	EQUAL(job.getExpectedReward(9, 2), 36.0);
	EQUAL(job.getExpectedReward(10, 2), 30.0);
	EQUAL(job.getExpectedReward(11, 2), 0.0);
}

void TestAll() {
	TestRewardFunction();
	TestJob();
}

// -----------------------------------------------------------------------------
// Main
// -----------------------------------------------------------------------------
class Main {
public:
	Main() {}

	void readInputForA() {
		// Time
		std::cin >> _Tmax;

		// Graph
		_graph = Graph::createGraphFromStdin();

		// Worker
		_workerManager = WorkerManager::createWorkerManagerFromStdin();

		// Job
		_jobAdmin = JobAdmin::createJobAdminFromStdinForA();

		Log_info(format("Tmax=%d", _Tmax));
		Log_info(_graph->toString());
		Log_info(_workerManager->toString());
		Log_info(_jobAdmin->toString());
	}

	void runForA() {
		std::vector<std::vector<std::string>> actionStrings = _workerManager->makeOutput1ForA(_Tmax, *_graph, *_jobAdmin);
		uint64_t elapsedTimeMillisec = _clock.getElapsedTimeMillisec();

		// Output
		for(int t = 0; t < _Tmax; t++) {
			for(size_t i = 0; i < actionStrings.size(); i++) {
				Log_info(format("t=%03d, action=\"%s\"", t+1, actionStrings[i][t].c_str()));
				std::cout << actionStrings[i][t] << std::endl;
			}
		}

		// Score
		uint64_t score;
		std::cin >> score;

		Log_info(format("Score= %llu", score));
		Log_info(format("Time=  %llu", elapsedTimeMillisec));
	}

	void readInputForB() {
		// Time
		std::cin >> _Tmax;

		// Graph
		_graph = Graph::createGraphFromStdin();

		// Worker
		_workerManager = WorkerManager::createWorkerManagerFromStdin();

		// Job
		_jobAdmin = JobAdmin::createJobAdminFromStdinForB();

		// Weather
		_weather = Weather::createWeatherFromStdin(_Tmax);

		// Schedule
		_workerManager->readScheduleInfoFromStdin();

		// Weather forecast
		_weather->readWeatherForecastFromStdin(1);

		Log_info(format("Tmax=%d", _Tmax));
		Log_info(_graph->toString());
		Log_info(_workerManager->toString());
		Log_info(_jobAdmin->toString());
		Log_info(_weather->toString());
	}

	void runForB() {

		// Output 1
		_workerManager->writeScheduledJobIdsToStdout(_Tmax, *_graph, *_jobAdmin, *_weather);

		for(int t = 1; t <= _Tmax; t++) {
			// Input 2
			_weather->readCurrentWeatherFromStdin();
			_jobAdmin->readJobInfoFromStdin();
			_workerManager->readWorkerInfoFromStdin();
			_weather->readWeatherForecastFromStdin(t);

			// Output 2
			_workerManager->writeScheduleAndActionToStdout(_Tmax, t, *_graph, *_jobAdmin, *_weather);
		}

		uint64_t elapsedTimeMillisec = _clock.getElapsedTimeMillisec();

		// Score
		uint64_t score;
		std::cin >> score;

		Log_info(format("Score= %llu", score));
		Log_info(format("Time=  %llu", elapsedTimeMillisec));
	}

private:
	int _Tmax;
	Graph* _graph;
	WorkerManager* _workerManager;
	JobAdmin* _jobAdmin;
	Weather* _weather;

	// debug
	Clock _clock;
};

int main() {
	//TestAll();

	Main main;

#define ProblemA
#ifdef ProblemA
	main.readInputForA();
	main.runForA();
#else // ProblemA
	main.readInputForB();
	main.runForB();
#endif // ProblemA

	return 0;
}

