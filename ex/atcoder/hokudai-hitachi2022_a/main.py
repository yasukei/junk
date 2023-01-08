import bisect
import collections
import enum
import functools
import logging as log
import math
import queue
import random
import sys
import time

# -----------------------------------------------------------------------------
# Utility
# -----------------------------------------------------------------------------
import pstats
import cProfile

def execution_speed_lib(func):
    def wrapper(*args, **kwargs):
        pr = cProfile.Profile()
        pr.runcall(func, *args, **kwargs)

        stats = pstats.Stats(pr)
        stats.print_stats()
    return wrapper

# -----------------------------------------------------------------------------
# Statemachine
# -----------------------------------------------------------------------------
class State:
    def onEntry(self, context=None):
        pass
    def onAction(self, context=None):
        return None
    def onExit(self, context=None):
        pass

class Statemachine:
    def __init__(self, context=None):
        self._initial_state = None
        self._current_state = None
        self._context = context
        self._transitions = dict()

    def addTransition(self, src, dst, event):
        self._transitions[src, event] = dst

    def setInitialState(self, initial_state):
        self._initial_state = initial_state

    def start(self):
        self._current_state = self._initial_state
        self._current_state.onEntry(self._context)

    def onEvent(self, event):
        dst = self._transitions.get((self._current_state, event))
        if dst == None:
            return

        self._current_state.onExit(self._context)
        dst.onEntry(self._context)
        self._current_state = dst

    def onAction(self):
        return self._current_state.onAction(self._context)

# -----------------------------------------------------------------------------
# Graph
# -----------------------------------------------------------------------------
class Edge:
    def __init__(self, u, v, d):
        self._u = u
        self._v = v
        self._d = d

    def __str__(self):
        return 'u={}, v={}, d={}, '.format(
                    self._u,
                    self._v,
                    self._d)

    def u(self):
        return self._u

    def v(self):
        return self._v

    def d(self):
        return self._d

class Graph:
    def __init__(self, edges):
        self._edge = dict()
        self._edges = collections.defaultdict(list)
        self._vertices = set()
        self._trajectory = dict()

        for edge in edges:
            self._addEdge(edge)

    def _addEdge(self, edge):
        u, v, d = edge.u(), edge.v(), edge.d()
        edge_v_u = Edge(v, u, d)
        self._edge[(u, v)] = edge
        self._edge[(v, u)] = edge_v_u
        self._edges[u].append(edge)
        self._edges[v].append(edge_v_u)

        self._vertices.add(u)
        self._vertices.add(v)

        self._trajectory[(u, v)] = Trajectory([edge])
        self._trajectory[(v, u)] = Trajectory([edge_v_u])
        if self._trajectory.get((u, u)) is None:
            self._trajectory[(u, u)] = Trajectory([Edge(u, u, 0)])
        if self._trajectory.get((v, v)) is None:
            self._trajectory[(v, v)] = Trajectory([Edge(v, v, 0)])

    def getTrajectory(self, start, goal):
        if self._trajectory.get((start, goal)):
            return self._trajectory[(start, goal)]

        #log.debug('start={}, goal={}'.format(start, goal))
        pr_queue = queue.PriorityQueue()
        queued = set()
        queued.add(start)
        for edge in self._edges[start]:
            pr_queue.put(self._trajectory[(start, edge.v())])
            queued.add(edge.v())

        goalFound = False
        while not pr_queue.empty():
            trajectory = pr_queue.get()

            for edge in self._edges[trajectory.getEndVertex()]:
                edge_v = edge.v()
                if edge_v in queued:
                    continue

                #log.debug('trajectory={}'.format(trajectory))
                #log.debug('edge={}'.format(edge))
                new_trajectory = trajectory.createNewTrajectory(edge)
                self._trajectory[(start, edge_v)] = new_trajectory
                #reversed_trajectory = self._createReversedTrajectory(new_trajectory)
                #self._trajectory[(edge_v, start)] = reversed_trajectory

                pr_queue.put(new_trajectory)
                queued.add(edge_v)

                if edge_v == goal:
                    goalFound = True

            if goalFound:
                break

        return self._trajectory[(start, goal)]

    def _createReversedTrajectory(self, trajectory):
        reversed_edges = [self._edge[(edge.v(), edge.u())] for edge in reversed(trajectory.getEdges())]
        return Trajectory(reversed_edges)

    def getEdges(self, vertex):
        return self._edges[vertex]

    def getAdjacentVertices(self, vertex):
        vertices = list()
        for edge in self._edges[vertex]:
            vertices.append((edge.v(), edge.d()))
        return vertices

    def getDistance(self, vertex1, vertex2):
        if not self._trajectory.get((vertex1, vertex2)):
            raise NotImplemented()
        return self._trajectory[(vertex1, vertex2)].getDistance()

class Trajectory:
    def __init__(self, edges):
        for i in range(1, len(edges)):
            assert edges[i-1].v() == edges[i].u()

        self._edges = edges
        self._distance = sum([edge.d() for edge in edges])

    def __lt__(self, other):
        return self._distance < other._distance

    def __str__(self):
        s1 = ''.join(list(map(str, self._edges)))
        s2 = 'distance={}'.format(self._distance)
        return '\n'.join([s1, s2])

    def getDistance(self):
        return self._distance

    def getStartVertex(self):
        return self._edges[0].u()

    def getEndVertex(self):
        return self._edges[-1].v()

    def getEdges(self):
        return self._edges

    def createNewTrajectory(self, edge):
        edges = [edge for edge in self._edges]
        edges.append(edge)
        return Trajectory(edges)

# -----------------------------------------------------------------------------
# Worker
# -----------------------------------------------------------------------------
class Worker:
    def __init__(self, worker_id, initial_vertex, workload, job_types):
        self._worker_id = worker_id
        self._initial_vertex = initial_vertex
        self._workload = workload
        self._job_types = job_types

    def __str__(self):
        return 'worker_id={}, vertex={}, workload={}, job_types={}'.format(
                    self._worker_id,
                    self._initial_vertex,
                    self._workload,
                    self._job_types)

    def makeSchedule(self, Tmax, graph, job_admin):
        schedule = Schedule(Tmax, self._initial_vertex)
        def currentTime():
            return schedule.getTailJobEndTime()

        while currentTime() <= Tmax:
            added = Method.makeGreedySchedule(graph, job_admin, schedule, self._job_types, self._workload)
            #log.debug('currentTime: {}'.format(currentTime()))
            #log.debug(schedule)
            if not added:
                stay_time = min(5, Tmax - currentTime() + 1)
                schedule += TimeSlot.StayAction(currentTime(), schedule.getTailJobEndVertex(), stay_time)
                continue

        schedule.fillStayActionInBlankTime()
        return schedule.makeActionStrings()

class Method:
    @staticmethod
    def makeGreedySchedule(graph, job_admin, schedule, job_types, workload):
        def currentTime():
            return schedule.getTailJobEndTime()
        rating_list = list()
        others_list = list()

        jobs = job_admin.getOpenJobs()
        for job in jobs:
            if job.getJobType() not in job_types:
                continue

            trajectory = graph.getTrajectory(schedule.getTailJobEndVertex(), job.getVertex())
            travel_time = trajectory.getDistance()
            reward, execute_time = job.getExpectedReward(currentTime() + travel_time, workload)
            if reward == 0:
                continue
            #log.debug('currentTime={}, travel_time={}, reward={}, execute_time={}'.format(currentTime(), travel_time, reward, execute_time))
            rating = reward / (travel_time + execute_time)

            rating_list.append(rating)
            others_list.append((trajectory, job))

        if len(rating_list) == 0:
            return False

        best_index = rating_list.index(max(rating_list))
        trajectory, job = others_list[best_index]
        job_admin.reserveJob(job)

        schedule += TimeSlot.MoveAction(currentTime(), trajectory)

        task_list = _makeTaskListForExecuteAction(job, currentTime(), workload)
        schedule += TimeSlot.ExecuteAction(currentTime(), schedule.getTailJobEndVertex(), job.getJobId(), task_list)
        job_admin.notifyDoneJob(job)
        job_admin.advanceTime()
        return True

    @staticmethod
    def makeHighRewardSchedule(graph, job_admin, schedule, job_types, workload):
        pass

def _makeTaskListForExecuteAction(job, current_time, workload):
    task_list = list()
    while True:
        numof_tasks = job.takeTasks(current_time + len(task_list), workload)
        if numof_tasks == 0:
            break
        task_list.append(numof_tasks)
    return task_list

class Schedule:
    START_TIME = 1

    def __init__(self, Tmax, initial_vertex):
        self._Tmax = Tmax
        self._time_slots = list()
        self._initial_vertex = initial_vertex

    def __str__(self):
        s = ['Schedule:']
        for ts in self._time_slots:
            s.append(str(ts))
        return '\n'.join(s)

    def __iadd__(self, new_time_slot):
        self.assignTimeSlot(new_time_slot)
        return self

    def assignTimeSlot(self, new_time_slot):
        index = None

        current = 1
        for i, ts in enumerate(self._time_slots):
            if current <= new_time_slot.getStartTime() and new_time_slot.getEndTime() <= ts.getStartTime():
                index = i
                break
            else:
                current = ts.getEndTime()
        if self.getTailJobEndTime() <= new_time_slot.getStartTime() and new_time_slot.getEndTime() <= (self._Tmax + 1):
            index = len(self._time_slots)

        if index is None:
            assert False

        self._time_slots.insert(index, new_time_slot)

    def getTailJobEndTime(self):
        if len(self._time_slots) > 0:
            return self._time_slots[-1].getEndTime()
        return Schedule.START_TIME

    def getTailJobEndVertex(self):
        if len(self._time_slots) > 0:
            return self._time_slots[-1].getEndVertex()
        return self._initial_vertex

    def getBlankTime(self):
        # returns a list of tuple (start_time, end_time, start_vertex, end_vertex)
        blanks = list()
        cursor = Schedule.START_TIME
        start_vertex = self._initial_vertex
        for ts in self._time_slots:
            if cursor < ts.getStartTime():
                blanks.append((cursor, ts.getStartTime(), start_vertex, ts.getStartVertex))
            cursor = ts.getEndTime()
            start_vertex = ts.getEndVertex()
        if cursor <= self._Tmax:
            blanks.append((cursor, self._Tmax+1, start_vertex, start_vertex))
        return blanks

    def fillStayActionInBlankTime(self):
        for start_time, end_time, start_vertex, _ in self.getBlankTime():
            duration = end_time - start_time
            stay_slot = TimeSlot.StayAction(start_time, start_vertex, duration)
            self.assignTimeSlot(stay_slot)

    def makeActionStrings(self):
        action = list()
        for ts in self._time_slots:
            action += ts.makeActionStrings()
        return action

class TimeSlot:
    # [start, end)
    # start <= time < end
    # end - start == duration
    def __init__(self, start_time, duration, start_vertex, end_vertex, action):
        self._start_time = start_time
        self._duration = duration
        self._start_vertex = start_vertex
        self._end_vertex = end_vertex
        self._action = action

    def __str__(self):
        return 'start={:03}, end={:03}, action=\'{}\''.format(
                    self.getStartTime(),
                    self.getEndTime(),
                    self._action)

    def getStartTime(self):
        return self._start_time

    def getEndTime(self):
        return self._start_time + self._duration

    def getDuration(self):
        return self._duration

    def getStartVertex(self):
        return self._start_vertex

    def getEndVertex(self):
        return self._end_vertex

    def makeActionStrings(self):
        return self._action.makeActionStrings()

    @staticmethod
    def StayAction(start_time, current_vertex, stay_time):
        return TimeSlot(start_time, stay_time, current_vertex, current_vertex, StayAction(stay_time))

    @staticmethod
    def MoveAction(start_time, trajectory):
        return TimeSlot(start_time, trajectory.getDistance(), trajectory.getStartVertex(), trajectory.getEndVertex(), MoveAction(trajectory))

    @staticmethod
    def ExecuteAction(start_time, current_vertex, job_id, task_list):
        return TimeSlot(start_time, len(task_list), current_vertex, current_vertex, ExecuteAction(job_id, task_list))

class StayAction:
    def __init__(self, stay_time):
        self._stay_time = stay_time

    def __str__(self):
        return 'stay {} times'.format(self._stay_time)
        
    def makeActionStrings(self):
        return ['stay'] * self._stay_time

class MoveAction:
    def __init__(self, trajectory):
        self._trajectory = trajectory

    def __str__(self):
        return 'move {} -> {}'.format(
                    self._trajectory.getStartVertex(),
                    self._trajectory.getEndVertex())
        
    def makeActionStrings(self):
        action = list()
        for edge in self._trajectory.getEdges():
            move = ['move {}'.format(edge.v())] * edge.d()
            action += move
        return action

class ExecuteAction:
    def __init__(self, job_id, task_list):
        self._job_id = job_id
        self._task_list = task_list

    def __str__(self):
        return 'execute {}'.format(self._job_id)
        
    def makeActionStrings(self):
        action = ['execute {} {}'.format(self._job_id, numof_tasks) for numof_tasks in self._task_list]
        return action

# -----------------------------------------------------------------------------
# Job
# -----------------------------------------------------------------------------
class Job:
    def __init__(self, jobId, job_type, numof_tasks, vertex, rewards, prior_job_ids):
        self._job_id = jobId
        self._job_type = job_type
        self._remaining_tasks = numof_tasks
        self._vertex = vertex
        self._reward_func = RewardFunction(rewards)
        self._gained_reward = 0
        self._prior_job_ids = prior_job_ids

    def __str__(self):
        l1 = 'jobId: {}, job_type: {}, numofRemainingTasks: {}, vertex: {}'.format(
                self._job_id,
                self._job_type,
                self._remaining_tasks,
                self._vertex)
        l2 = 'rewards: {}'.format(self._reward_func)
        l3 = 'numofPriorJobIds: {}, prior_job_ids: {}'.format(
                len(self._prior_job_ids),
                self._prior_job_ids)
        return '\n'.join([l1, l2, l3])

    def getJobId(self):
        return self._job_id

    def getJobType(self):
        return self._job_type

    def isDone(self):
        return self._remaining_tasks == 0

    def isOpen(self, current_time):
        if current_time < self._reward_func.getStartTime() or self._reward_func.getEndTime() <= current_time or self._remaining_tasks == 0:
            return False
        return True

    def getRewardPeak(self):
        peak_time, peak_value = self._reward_func.getPeak()
        return peak_time, peak_value

    def getVertex(self):
        return self._vertex

    def getPriorJobIds(self):
        return self._prior_job_ids

    def getGainedReward(self):
        if self._remaining_tasks > 0:
            return 0
        return int(self._gained_reward)

    def getExpectedReward(self, start_time, workload_per_time):
        execute_time = math.ceil(self._remaining_tasks / workload_per_time)
        end_time = start_time + execute_time - 1
        if not self.isOpen(start_time) or not self.isOpen(end_time):
            return 0, 0

        if self._remaining_tasks < workload_per_time:
            workload_per_time = self._remaining_tasks
        reward = self._reward_func.getTotalReward(start_time, end_time) * workload_per_time
        return reward, execute_time

    def takeTasks(self, current_time, numof_taking):
        reward, _ = self.getExpectedReward(current_time, numof_taking)
        if reward <= 0:
            return 0

        if numof_taking > self._remaining_tasks:
            numof_taking = self._remaining_tasks
        self._remaining_tasks -= numof_taking
        self._gained_reward += numof_taking * reward
        return numof_taking

    def notifyDoneJob(self, done_job):
        self._prior_job_ids.remove(done_job)

class JobAdmin:
    def __init__(self, jobs):
        self._all_jobs = set()
        self._job_dict = dict()
        self._open_jobs = set()
        self._open_jobs_foreach_vertex = collections.defaultdict(list)
        self._reseved_jobs = set()
        self._hidden_jobs = set()
        self._done_jobs = set()
        self._subsequent_job_ids = collections.defaultdict(list)
        self._next_open_jobs = list()

        for job in jobs:
            self._addJob(job)

        self._all_jobs_in_high_reward_order = list(self._all_jobs)
        self._all_jobs_in_high_reward_order.sort(key=lambda job: job.getRewardPeak()[1], reverse=True)
        for job in self._all_jobs_in_high_reward_order:
            peak_time, peak_value = job.getRewardPeak()

    def __str__(self):
        return 'all={}, open={}, hidden={}, done={}'.format(
                len(self._all_jobs),
                len(self._open_jobs),
                len(self._hidden_jobs),
                len(self._done_jobs))

    def _addJob(self, job):
        self._all_jobs.add(job)
        self._job_dict[job.getJobId()] = job

        if len(job.getPriorJobIds()) > 0:
            self._hidden_jobs.add(job)
            for priorJobId in job.getPriorJobIds():
                self._subsequent_job_ids[priorJobId].append(job.getJobId())
        else:
            self._addOpenJob(job)

    # TODO: support finding high peak job
    def _addOpenJob(self, job):
        self._open_jobs.add(job)
        self._open_jobs_foreach_vertex[job.getVertex()].append(job)

    def _removeOpenJob(self, job):
        self._open_jobs.remove(job)
        self._open_jobs_foreach_vertex[job.getVertex()].remove(job)

    def _addReservedJob(self, job):
        self._reseved_jobs.add(job)

    def _removeReservedJob(self, job):
        self._reseved_jobs.remove(job)

    def getOpenJobs(self):
        return self._open_jobs

    def getJobByJobId(self, job_id):
        return self._job_dict[job_id]

    def getJobsByVertex(self, vertex, job_types):
        result = list()
        for job in self._open_jobs_foreach_vertex[vertex]:
            if job.getJobType() in job_types:
                result.append(job)
        return result

    def getAllJobsInHighRewardOrder(self):
        return self._all_jobs_in_high_reward_order

    def reserveJob(self, job):
        self._removeOpenJob(job)
        self._addReservedJob(job)

    def cancelReservedJob(self, job):
        self._removeReservedJob(job)
        self._addOpenJob(job)

    def notifyDoneJob(self, done_job):
        # TODO: make JobAdmin remember call history along time series
        if done_job in self._reseved_jobs:
            self._removeReservedJob(done_job)
        elif done_job in self._open_jobs:
            self._removeOpenJob(done_job)
        else:
            return

        self._done_jobs.add(done_job)

        for subsequentJobId in self._subsequent_job_ids[done_job.getJobId()]:
            subsequentJob = self._job_dict[subsequentJobId]
            subsequentJob.notifyDoneJob(done_job.getJobId())
            if len(subsequentJob.getPriorJobIds()) == 0:
                self._next_open_jobs.append(subsequentJob)

    def advanceTime(self):
        for job in self._next_open_jobs:
            self._hidden_jobs.remove(job)
            self._addOpenJob(job)
        self._next_open_jobs.clear()

    def saveJobGraph(self):
        body = []
        for job in self._all_jobs:
            s = '{} [label="{}", color=orange, style=filled]'.format(job.getJobId(), job.getJobId())
            body.append(s)

        for job in self._all_jobs:
            for prior_job_id in job.getPriorJobIds():
                s = '{} -> {}'.format(prior_job_id, job.getJobId())
                body.append(s)

        text = 'digraph g {\n' + '\n'.join(body) + '\n}'
        with open('graph.dot', 'w') as f:
            f.write(text)

        # Execute the following command in terminal
        #   dot -T graph.dot png graph.png

class RewardFunction:
    def __init__(self, rewards):
        self._t = rewards[::2]
        self._y = rewards[1::2]
        self._peak_value = max(self._y)
        self._peak_time = self._t[self._y.index(self._peak_value)]
        self._cache = dict()

        self._cumulative_sum = [0] * (self._t[-1]+2)
        for t in range(self._t[0], self._t[-1]+1):
            self._cumulative_sum[t+1] = self._cumulative_sum[t] + self._callWithoutCache(t)

    def __str__(self):
        return ''.join(['t={}, y={},'.format(t, y) for t, y in zip(self._t, self._y)])

    #@functools.lru_cache(maxsize=None)
    def __call__(self, t):
        if self._cache.get(t):
            return self._cache[t]

        y = self._callWithoutCache(t)
        self._cache[t] = y
        return y

    def _callWithoutCache(self, t):
        if t < self._t[0]:
            return self._y[0]
        elif t >= self._t[-1]:
            return self._y[-1]
        else:
            i = bisect.bisect_left(self._t, t)
            t_prev = self._t[i-1]
            y_prev = self._y[i-1]
            t_next = self._t[i]
            y_next = self._y[i]
            return (y_next - y_prev) * (t - t_prev) / (t_next - t_prev) + y_prev

    def getStartTime(self):
        return self._t[0] + 1

    def getEndTime(self):
        return self._t[-1]

    def getDuration(self):
        return self.getEndTime() - self.getStartTime()

    def getPeak(self):
        return self._peak_time, self._peak_value

    def getTotalReward(self, start_time, end_time):
        return self._cumulative_sum[end_time+1] - self._cumulative_sum[start_time]

# -----------------------------------------------------------------------------
# Environment
# -----------------------------------------------------------------------------
class Environment:
    def __init__(self):
        self._start_time = time.perf_counter()

    def readInput(self):
        # Time
        Tmax, = input().split()
        self._Tmax = int(Tmax)

        # Graph
        Nv, Ne = input().split()
        self._Nv = int(Nv)
        self._Ne = int(Ne)
        edges = list()
        for _ in range(self._Ne):
            u, v, d = input().split()
            edge = Edge(int(u), int(v), int(d))
            edges.append(edge)
        self._graph = Graph(edges)

        # Worker
        Nworker, = input().split()
        self._Nworker = int(Nworker)
        self._workers = list()
        for i in range(self._Nworker):
            v, Lmax, Njobtype, *jobTypes = input().split()
            v, Lmax, Njobtype  = int(v), int(Lmax), int(Njobtype)
            jobTypes = list(map(int, jobTypes))
            worker = Worker(i, v, Lmax, jobTypes)
            self._workers.append(worker)

        # Job
        Njob, = input().split()
        self._Njob = int(Njob)
        jobs = list()
        for _ in range(self._Njob):
            jobId, jobType, Ntask, vertex = input().split()
            jobId, jobType, Ntask, vertex = int(jobId), int(jobType), int(Ntask), int(vertex)
            Nreward, *rewards = input().split()
            Nreward = int(Nreward)
            rewards = list(map(int, rewards))
            Ndepend, *priorJobIds = input().split()
            Ndepend = int(Ndepend)
            priorJobIds = list(map(int, priorJobIds))
            job = Job(jobId, jobType, Ntask, vertex, rewards, priorJobIds)
            jobs.append(job)
        self._job_admin = JobAdmin(jobs)

        # log basic info
        log.debug(f"Tmax:      {self._Tmax}")
        log.debug(f"Nv:        {self._Nv}")
        log.debug(f"Ne:        {self._Ne}")
        log.debug(f"Nworker:   {self._Nworker}")
        for i in range(self._Nworker):
            log.debug(f"             {self._workers[i]}")
        log.debug(f"Njob:      {self._Njob}")
        log.debug(f"             {self._job_admin}")
        if log.getLogger().isEnabledFor(log.DEBUG):
            self._job_admin.saveJobGraph()

    def run(self):
        # Output
        schedules = []
        for i in range(self._Nworker):
            schedules.append(self._workers[i].makeSchedule(self._Tmax, self._graph, self._job_admin))

        for t in range(self._Tmax):
            for i in range(self._Nworker):
                action = schedules[i][t]
                log.debug(f"time={t+1:03}, worker={i}, action={action}")
                print(action, flush=True)

        # Score
        score, = input().split()
        self._score = int(score)
        end_time = time.perf_counter()
        log.debug(f"time:       {end_time - self._start_time}")
        log.debug(f"job:        {self._job_admin}")
        log.debug(f"score:      {self._score}")

# -----------------------------------------------------------------------------
# main
# -----------------------------------------------------------------------------
#@execution_speed_lib
def main():
    env = Environment()
    env.readInput()
    env.run()

if __name__ == '__main__':
    log.basicConfig(filename='debug.log', filemode='w', level=log.DEBUG)

    if len(sys.argv) == 1:
        main()
    else:
        print('sys.argv: ' + str(sys.argv))

