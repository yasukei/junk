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
	std::string result = "[ ";
	for(auto elem : v) {
		result += std::to_string(elem) + ", ";
	}
	result += "]";
	return result;
}

std::string to_string(const std::unordered_set<int>& us) {
	std::string result = "{ ";
	for(auto elem : us) {
		result += std::to_string(elem) + ", ";
	}
	result += "}";
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
const int VERTEX_NONE = -1;

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

class Graph {
public:
	Graph(const std::vector<Edge>& edges) :
		_edges(),
		_vertices(),
		_trajectory(),
		_priorityQueues(),
		_priorityQueuesMemo()
	{
		for(auto& edge : edges) {
			int u = edge.u();
			int v = edge.v();
			int d = edge.d();
			Edge edge_v_u(v, u, d);

			_edges.emplace(u, edge);
			_edges.emplace(v, edge_v_u);

			_vertices.emplace_back(u);
			_vertices.emplace_back(v);

			_trajectory.emplace(std::make_pair(u, v), std::make_shared<Trajectory>(std::vector<int>{u, v}, d));
			_trajectory.emplace(std::make_pair(v, u), std::make_shared<Trajectory>(std::vector<int>{v, u}, d));
			// TODO: use {u} instead of {u, u}
			_trajectory.emplace(std::make_pair(u, u), std::make_shared<Trajectory>(std::vector<int>{u, u}, 0));
			_trajectory.emplace(std::make_pair(v, v), std::make_shared<Trajectory>(std::vector<int>{v, v}, 0));
		}

		for(auto& vertex : _vertices) {
			PriorityQueue priorityQueue;
			std::set<int> memo;

			memo.insert(vertex);
			auto result = _edges.equal_range(vertex);
			for(auto& it = result.first; it != result.second; it++) {
				Edge& edge = it->second;
				priorityQueue.enque(_trajectory[std::make_pair(vertex, edge.v())]);
				memo.insert(edge.v());
			}
			_priorityQueues[vertex] = std::move(priorityQueue);
			_priorityQueuesMemo[vertex] = std::move(memo);
		}
	}

	const Trajectory& getTrajectory(int start, int goal) {
		//Log_debug("start=%d, goal=%d\n", start, goal);
		auto startgoal = std::make_pair(start, goal);
		if(_trajectory.count(startgoal) > 0) {
			return *_trajectory[startgoal];
		}

		PriorityQueue& priorityQueue = _priorityQueues[start];
		std::set<int>& memo = _priorityQueuesMemo[start];

		bool goalFound = false;
		while(!priorityQueue.empty() && !goalFound) {
			std::shared_ptr<Trajectory> trajectory = priorityQueue.deque();

			auto result = _edges.equal_range(trajectory->getEndVertex());
			for(auto& it = result.first; it != result.second; it++) {
				Edge& edge = it->second;
				int edgeV = edge.v();
				if(memo.count(edgeV) > 0) {
					continue;
				}

				auto start_edgeV = std::make_pair(start, edgeV);
				_trajectory.emplace(start_edgeV, std::make_shared<Trajectory>(*trajectory, edgeV, edge.d()));
				priorityQueue.enque(_trajectory[start_edgeV]);
				memo.insert(edgeV);

				if(edgeV == goal) {
					goalFound = true;
				}
			}
		}
		return *_trajectory[startgoal];
	}

	int getDistance(int vertex1, int vertex2) {
		return getTrajectory(vertex1, vertex2).getDistance();
	}

	struct VertexPairHashFunction {
		size_t operator()(const std::pair<int, int>& pair) const {
			return pair.first << 16 | pair.second;
		}
	};

private:
	std::unordered_multimap<int, Edge> _edges;
	std::vector<int> _vertices;
	std::unordered_map<std::pair<int, int>, std::shared_ptr<Trajectory>, Graph::VertexPairHashFunction> _trajectory;

	std::unordered_map<int, PriorityQueue> _priorityQueues;
	std::unordered_map<int, std::set<int>> _priorityQueuesMemo;
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

	// TODO: use cache
	int operator()(int t) const {
		return call(t);
	}

	int getStartTime() const { return _ts.front() + 1; }
	int getEndTime() const { return _ts.back(); }
	int getDuration() const { return getEndTime() - getStartTime(); }
	void getPeak(int& peakTime, int& peakValue) const {
		peakTime = _peakTime;
		peakValue = _peakValue;
	}
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
			auto it = std::lower_bound(_ts.begin(), _ts.end(), t);
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
	Job(int jobId, int jobType, int numofTasks, int vertex, const std::vector<int>& rewards, const std::vector<int>& priorJobIds) :
		_jobId(jobId),
		_jobType(jobType),
		_numofTasks(numofTasks),
		_vertex(vertex),
		_rewardFunction(rewards),
		_priorJobIds(priorJobIds),
		_reserved(false)
	{}

	std::string toString() const {
		return "jobId=" + std::to_string(_jobId)
				+ "jobType=" + std::to_string(_jobType)
				+ "numofTasks=" + std::to_string(_numofTasks)
				+ "vertex=" + std::to_string(_vertex)
				+ "rewardFunction=" + _rewardFunction.toString()
				+ "priorJobIds=" + to_string(_priorJobIds);
	}

	int getJobId() const { return _jobId; }
	int getJobType() const { return _jobType; }
	int getNumofTasks() const { return _numofTasks; }
	int getVertex() const { return _vertex; }
	int getStartTime() const { return _rewardFunction.getStartTime(); }
	int getEndTime() const { return _rewardFunction.getEndTime(); }
	const std::vector<int>& getPriorJobIds() const { return _priorJobIds; }

	int getExecutionTime(int workload) const {
		return std::ceil((double)_numofTasks / (double)workload);
	}
	bool isOpen(int time) const {
		return _rewardFunction.getStartTime() <= time && time < _rewardFunction.getEndTime();
	}

	double getExpectedReward(int startTime, int workload) const {
		if(!isOpen(startTime)) {
			return 0.0;
		}

		int endTime = startTime + getExecutionTime(workload);
		if(!isOpen(endTime)) { 
			return 0.0;
		}

		if(_numofTasks < workload) {
			workload = _numofTasks;
		}

		return _rewardFunction.getTotalReward(startTime, endTime) * (double)workload;
	}

	void reserve() {
		assert(!_reserved);
		_reserved = true;
	}
	bool isReserved() const { return _reserved; }

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
	RewardFunction _rewardFunction;
	std::vector<int> _priorJobIds;

	bool _reserved;
};

class JobAdmin {
public:
	JobAdmin(const std::vector<std::shared_ptr<Job>>& jobs) :
		_allJobs(jobs)
	{
		for(auto& job : _allJobs) {
			_job.emplace(job->getJobId(), job);

			if(job->getPriorJobIds().empty()) {
				_openJobs.emplace_back(job);
			}
		}
	}

	std::string toString() {
		int all = _allJobs.size();
		int open = _openJobs.size();
		int hidden = all - open;
		return format("all=%d, open=%d, hidden=%d", all, open, hidden);
	}

	const std::vector<std::shared_ptr<Job>>& getAllJobs() { return _allJobs; }
	const std::vector<std::shared_ptr<Job>>& getOpenJobs() { return _openJobs; }
	const Job& getJob(int jobId) { return *_job[jobId]; }

private:
	std::vector<std::shared_ptr<Job>> _allJobs;
	std::unordered_map<int, std::shared_ptr<Job>> _job;

	std::vector<std::shared_ptr<Job>> _openJobs;
};

// -----------------------------------------------------------------------------
// Action, TimeSlot, Schedule
// -----------------------------------------------------------------------------
class Action {
public:
	virtual std::vector<std::string> makeActionStrings() const = 0;
	virtual std::string toString() const = 0;
};

class StayAction : public Action {
public:
	StayAction(int stayTime) :
		_stayTime(stayTime)
	{}

	std::string toString() const {
		return format("stay %d times", _stayTime);
	}
	std::vector<std::string> makeActionStrings() const {
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

	std::string toString() const {
		return format("move %d -> %d", _trajectory.getStartVertex(), _trajectory.getEndVertex());
	}
	std::vector<std::string> makeActionStrings() const {
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
	ExecuteAction(const Job& job, int workload) :
		_job(job),
		_workload(workload)
	{}

	std::string toString() const {
		return format("execute %d", _job.getJobId());
	}
	std::vector<std::string> makeActionStrings() const {
		std::vector<int> taskList = _job.getTaskListToExecute(_workload);
		std::vector<std::string> result;

		for(size_t i = 0; i < taskList.size(); i++) {
			result.emplace_back(format("execute %d %d", _job.getJobId(), taskList[i]));
		}
		return result;
	}

private:
	const Job& _job;
	int _workload;
};

class NoAction : public Action {
public:
	NoAction()
	{}

	std::string toString() const {
		return "No action";
	}
	std::vector<std::string> makeActionStrings() const {
		return std::vector<std::string>();
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
	{}

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
	static TimeSlot Execute(int startTime, const Job& job, int workload) {
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
		std::string result = "Schedule:\n";
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

	std::vector<std::string> makeActionStrings() {
		std::vector<std::string> result;

		for(auto& timeslot : _timeSlots) {
			std::vector<std::string> actionStrings = timeslot.makeActionStrings();
			result.insert(result.end(), actionStrings.begin(), actionStrings.end());
		}
		return result;
	}

private:
	int _Tmax;
	int _initialVertex;
	std::set<TimeSlot> _timeSlots;
};

int Schedule::START_TIME = 1;

// -----------------------------------------------------------------------------
// Worker
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

	std::vector<std::string> makeSchedule(int Tmax, Graph& graph, JobAdmin& jobAdmin) const {
		Schedule schedule(Tmax, _initialVertex);

		makeGreedySchedule(schedule, graph, jobAdmin, _jobTypes, _workload);
		schedule.fillNecessaryMoveInAllBlankTimeSlots(graph);
		schedule.fillStayInAllBlankTimeSlots();
		return schedule.makeActionStrings();
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

		while(true) {
			double bestRating = 0.0;
			Job* bestJob = nullptr;

			for(auto& job : jobAdmin.getOpenJobs()) {
				if(!acceptableJob(*job, jobTypes)) {
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

private:
	int _workerId;
	int _initialVertex;
	int _workload;
	std::vector<int> _jobTypes;
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

void TestRewardFunction() {
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

void TestAll() {
	TestRewardFunction();
}

// -----------------------------------------------------------------------------
// Main
// -----------------------------------------------------------------------------
class Main {
public:
	Main() {}

	void readInput() {
		// Time
		std::cin >> _Tmax;

		// Graph
		std::cin >> _Nv >> _Ne;
		std::vector<Edge> edges;
		for(int i = 0; i < _Ne; i++) {
			int u, v, d;
			std::cin >> u >> v >> d;
			edges.emplace_back(u, v, d);
		}
		_graph = new Graph(std::move(edges));

		// Worker
		std::cin >> _Nworker;
		for(int i = 0; i < _Nworker; i++) {
			int v, Lmax, Njobtype;
			std::cin >> v >> Lmax >> Njobtype;

			std::vector<int> jobTypes;
			for(int j = 0; j < Njobtype; j++) {
				int jobType;
				std::cin >> jobType;
				jobTypes.emplace_back(jobType);
			}
			_workers.emplace_back(i, v, Lmax, jobTypes);
		}

		// Job
		std::cin >> _Njob;
		std::vector<std::shared_ptr<Job>> jobs;
		for(int i = 0; i < _Njob; i++) {
			int jobId, jobType, Ntask, vertex, Nreward;
			std::cin >> jobId >> jobType >> Ntask >> vertex >> Nreward;

			std::vector<int> rewards;
			for(int j = 0; j < Nreward; j++) {
				int t, y;
				std::cin >> t >> y;
				rewards.emplace_back(t);
				rewards.emplace_back(y);
			}

			int Ndepend;
			std::cin >> Ndepend;
			std::vector<int> priorJobIds;
			for(int j = 0; j < Ndepend; j++) {
				int priorJobId;
				std::cin >> priorJobId;
				priorJobIds.emplace_back(priorJobId);
			}
			jobs.emplace_back(std::make_shared<Job>(jobId, jobType, Ntask, vertex, rewards, priorJobIds));
		}
		_jobAdmin = new JobAdmin(std::move(jobs));

		Log_info("Tmax=      " + std::to_string(_Tmax));
		Log_info("Nv=        " + std::to_string(_Nv));
		Log_info("Ne=        " + std::to_string(_Ne));
		Log_info("Nworker=   " + std::to_string(_Nworker));
		for(int i = 0; i < _Nworker; i++) {
			Log_info("             " + _workers[i].toString());
		}
		Log_info("Njob=      " + std::to_string(_Njob));
		Log_info("             " + _jobAdmin->toString());
	}

	void run() {
		auto workloadDescendingOrder = [](const Worker& a, const Worker& b) {
			return a.getWorkload() > b.getWorkload();
		};
		std::sort(_workers.begin(), _workers.end(), workloadDescendingOrder);
		
		std::vector<std::vector<std::string>> actionStrings(_workers.size());
		for(auto& worker : _workers) {
			actionStrings[worker.getWorkerId()] = worker.makeSchedule(_Tmax, *_graph, *_jobAdmin);
		}
		uint64_t elapsedTimeMillisec = _clock.getElapsedTimeMillisec();

		// Output
		for(int t = 0; t < _Tmax; t++) {
			for(size_t i = 0; i < _workers.size(); i++) {
				Log_info(actionStrings[i][t]);
				std::cout << actionStrings[i][t] << std::endl;
				//std::cout << "stay" << std::endl;
			}
		}

		// Score
		uint64_t score;
		std::cin >> score;

		Log_info(format("Score= %llu", score));
		Log_info(format("Time=  %llu", elapsedTimeMillisec));
	}

private:
	int _Tmax;

	int _Nv;
	int _Ne;
	Graph* _graph;

	int _Nworker;
	std::vector<Worker> _workers;

	int _Njob;
	JobAdmin* _jobAdmin;

	Clock _clock;
};

int main() {
	//TestAll();

	Main main;
	main.readInput();
	main.run();

	return 0;
}
