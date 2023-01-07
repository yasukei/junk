import bisect
import collections
import enum
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
        return 'u={}, v={}, d={}'.format(self._u, self._v, self._d)

    def u(self):
        return self._u

    def v(self):
        return self._v

    def d(self):
        return self._d

class Trajectory:
    def __init__(self, vertices, distance):
        self._vertices = vertices
        self._distance = distance

    def __lt__(self, other):
        return self._distance < other._distance

    def getVertices(self):
        return self._vertices

    def getDistance(self):
        return self._distance

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

        self._trajectory[(u, v)] = Trajectory2([edge])
        self._trajectory[(v, u)] = Trajectory2([edge_v_u])
        self._trajectory[(u, u)] = Trajectory2([Edge(u, u, 0)])
        self._trajectory[(v, v)] = Trajectory2([Edge(v, v, 0)])

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
                newTrajectory = trajectory.createNewTrajectory(edge)
                self._trajectory[(start, edge_v)] = newTrajectory

                pr_queue.put(newTrajectory)
                queued.add(edge_v)

                if edge_v == goal:
                    goalFound = True

            if goalFound:
                break

        return self._trajectory[(start, goal)]

    def _createReversedTrajectory(self, trajectory):
        reversed_edges = [self._edge[(edge.v(), edge.u())] for edge in reversed(trajectory.getEdges())]
        return Trajectory2(reversed_edges)

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

class Trajectory2:
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
        return Trajectory2(edges)

# -----------------------------------------------------------------------------
# Worker
# -----------------------------------------------------------------------------
class WorkerEvent(enum.Enum):
    MOVED = enum.auto()
    EXECUTED = enum.auto()
    NO_JOB = enum.auto()
    WAKEUP = enum.auto()

class Worker:
    def __init__(self, worker_id, initial_vertex, max_workload, job_types, graph, job_admin):
        self._worker_id = worker_id
        self._initial_vertex = initial_vertex
        self._current_vertex = initial_vertex
        self._max_workload = max_workload
        self._job_types = job_types
        self._graph = graph
        self._job_admin = job_admin

        self._event_queue = collections.deque()
        self._trajectory = None
        self._trajectory_index = None
        self._remaining_distance = None
        self._target_job = None
        self._current_time = 0
        self._started = False
        sm = Statemachine(self)
        moving = WorkerState_Moving()
        executing = WorkerState_Executing()
        idle = WorkerState_Idle()
        sm.addTransition(moving,    executing, WorkerEvent.MOVED)
        sm.addTransition(moving,    idle,      WorkerEvent.NO_JOB)
        sm.addTransition(executing, moving,    WorkerEvent.EXECUTED)
        sm.addTransition(idle,      moving,    WorkerEvent.WAKEUP)
        sm.setInitialState(moving)
        self._sm = sm

    def __str__(self):
        return f"worker_id={self._worker_id}, vertex={self._initial_vertex}, max_workload={self._max_workload}, job_types={self._job_types}"

    def makePlan(self, Tmax):
        plan = list()
        def currentTime():
            return len(plan) + 1
        current_vertex = self._initial_vertex

        rating_list = list()
        others_list = list()

        while len(plan) < Tmax:
            log.debug('len(plan)={}'.format(len(plan)))
            rating_list.clear()
            others_list.clear()

            jobs = self._job_admin.getOpenJobs()
            for job in jobs:
                if job.getJobType() not in self._job_types:
                    continue

                trajectory = self._graph.getTrajectory(current_vertex, job.getVertex())
                travel_time = trajectory.getDistance()
                reward, execute_time = job.getExpectedReward(currentTime() + travel_time, self._max_workload)
                if reward == 0:
                    continue
                #log.debug('currentTime={}, travel_time={}, reward={}, execute_time={}'.format(currentTime(), travel_time, reward, execute_time))
                rating = reward / (travel_time + execute_time)

                rating_list.append(rating)
                others_list.append((trajectory, job))

            if len(rating_list) == 0:
                stay_time = min(5, Tmax - currentTime() + 1)
                plan += self._makeStayAction(stay_time)
                continue

            best_index = rating_list.index(max(rating_list))
            trajectory, job = others_list[best_index]
            self._job_admin.reserveJob(job)

            plan += self._makeMoveAction(trajectory)
            current_vertex = trajectory.getEndVertex()

            plan += self._makeExecuteAction(job, currentTime(), self._max_workload)
            self._job_admin.notifyDoneJob(job)
            self._job_admin.advanceTime()

        return plan
            
    def _makeStayAction(self, stay_time):
        return ['stay'] * stay_time

    def _makeMoveAction(self, trajectory):
        action = list()
        for edge in trajectory.getEdges():
            move = ['move {}'.format(edge.v())] * edge.d()
            action += move
        return action

        # TODO: faster?
        #return ['move {}'.format(edge.v()) for _ in range(edge.d()) for edge in trajectory.getEdges()]

    def _makeExecuteAction(self, job, current_time, workload):
        task_list = list()
        while True:
            numof_tasks = job.takeTasks(current_time + len(task_list), workload)
            if numof_tasks == 0:
                break
            task_list.append(numof_tasks)

        action = ['execute {} {}'.format(job.getJobId(), numof_tasks) for numof_tasks in task_list]
        return action

    def getAction(self, current_time):
        if not self._started:
            self._sm.start()
            self._started = True

        self._current_time = current_time
        while len(self._event_queue) > 0:
            event = self._event_queue.popleft()
            self._sm.onEvent(event)
        return self._sm.onAction()

class WorkerState_Moving(State):
    def onEntry(self, context):
        #trajectory = Trajectory([context._current_vertex], 0)
        pr_queue = queue.PriorityQueue()
        #pr_queue.put(trajectory)
        sweeped = set()
        sweeped.add(context._current_vertex)
        #sweeped.add(context._current_vertex)
        for edge in context._graph.getEdges(context._current_vertex):
            pr_queue.put(context._graph.findTrajectory(context._current_vertex, edge.v()))
            sweeped.add(edge.v())

        job_list = list()
        reward_rating_list = list()
        trajectory_list = list()
        # TODO: add jobs of current vertex

        while not pr_queue.empty():
            trajectory = pr_queue.get()
            jobs = context._job_admin.getJobsByVertex(trajectory.getEndVertex(), context._job_types)
            if len(jobs) > 0:
                for job in jobs:
                    #log.debug(str(job))
                    reward, time_to_execute = job.getExpectedReward(context._current_time + trajectory.getDistance(), context._max_workload)
                    if reward > 0:
                        job_list.append(job)
                        reward_rating_list.append(reward / (trajectory.getDistance() + time_to_execute))
                        trajectory_list.append(trajectory)
                        break

            #for vertex, distance in context._graph.getAdjacentVertices(trajectory.getVertices()[-1]):
            for edge in context._graph.getEdges(trajectory.getEndVertex()):
                if edge.v() in sweeped:
                    continue
                #new_trajectory = Trajectory([*trajectory.getVertices(), vertex], trajectory.getDistance() + distance)
                #print('trajectory: {}'.format(trajectory), file=sys.stderr)
                #print('edge: {}'.format(edge), file=sys.stderr)
                new_trajectory = trajectory.createNewTrajectory(edge)
                pr_queue.put(new_trajectory)
                sweeped.add(edge.v())

            if len(job_list) > 30:
                break

        if len(job_list) == 0:
            #log.debug('sweeped={}'.format(str(sweeped)))
            context._event_queue.append(WorkerEvent.NO_JOB)
            return

        best = reward_rating_list.index(max(reward_rating_list))
        context._target_job = job_list[best]
        trajectory = trajectory_list[best]

        if trajectory.getDistance() == 0:
            context._event_queue.append(WorkerEvent.MOVED)
            return

        context._trajectory = trajectory
        context._trajectory_index = 0
        #context._remaining_distance = context._graph.getDistance(
        #        context._trajectory.getVertices()[context._trajectory_index],
        #        context._trajectory.getVertices()[context._trajectory_index-1])
        context._remaining_distance = context._trajectory.getEdges()[context._trajectory_index].d()

    def onAction(self, context):
        action = 'move {}'.format(context._trajectory.getEdges()[context._trajectory_index]).v()
        context._remaining_distance -= 1
        if context._remaining_distance == 0:
            context._current_vertex = context._trajectory.getVertices()[context._trajectory_index]
            context._trajectory_index += 1
            if context._trajectory_index < len(context._trajectory.getVertices()):
                context._remaining_distance = context._graph.getDistance(
                        context._trajectory.getVertices()[context._trajectory_index],
                        context._trajectory.getVertices()[context._trajectory_index-1])
            else:
                context._event_queue.append(WorkerEvent.MOVED)
        return action

        #action = 'move {}'.format(context._trajectory.getVertices()[context._trajectory_index])
        #context._remaining_distance -= 1
        #if context._remaining_distance == 0:
        #    context._current_vertex = context._trajectory.getVertices()[context._trajectory_index]
        #    context._trajectory_index += 1
        #    if context._trajectory_index < len(context._trajectory.getVertices()):
        #        context._remaining_distance = context._graph.getDistance(
        #                context._trajectory.getVertices()[context._trajectory_index],
        #                context._trajectory.getVertices()[context._trajectory_index-1])
        #    else:
        #        context._event_queue.append(WorkerEvent.MOVED)
        #return action

class WorkerState_Executing(State):
    def onAction(self, context):
        targetJob = context._target_job

        reward, _ = targetJob.getExpectedReward(context._current_time, context._max_workload)
        if reward == 0:
            context._event_queue.append(WorkerEvent.EXECUTED)
            action = 'stay'
            return action

        numofTakingTasks = targetJob.takeTasks(context._current_time, context._max_workload)
        action = 'execute {} {}'.format(targetJob.getJobId(), numofTakingTasks)
        if targetJob.isDone():
            context._job_admin.notifyDoneJob(targetJob)
            context._event_queue.append(WorkerEvent.EXECUTED)
        return action

    def onExit(self, context):
        context._target_job = None

class WorkerState_Idle(State):
    def onEntry(self, context):
        self._counter = 0

    def onAction(self, context):
        self._counter += 1
        if self._counter >= 5:
            context._event_queue.append(WorkerEvent.WAKEUP)
        action = 'stay'
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
        if self._remaining_tasks == 0 or self._reward_func(current_time) == 0:
            return False
        return True

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
    def __init__(self):
        self._all_jobs = dict()
        self._open_jobs = set()
        self._open_jobs_foreach_vertex = collections.defaultdict(list)
        self._reseved_jobs = set()
        self._hidden_jobs = dict()
        self._done_jobs = list()
        self._subsequent_job_ids = collections.defaultdict(list)
        self._next_open_jobs = list()

    def __str__(self):
        return 'all={}, open={}, hidden={}, done={}'.format(
                len(self._all_jobs), len(self._open_jobs), len(self._hidden_jobs), len(self._done_jobs))

    def addJob(self, job):
        self._all_jobs[job.getJobId()] = job

        if len(job.getPriorJobIds()) > 0:
            self._hidden_jobs[job.getJobId()] = job
            for priorJobId in job.getPriorJobIds():
                self._subsequent_job_ids[priorJobId].append(job.getJobId())
        else:
            self._addOpenJob(job)

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
        return self._all_jobs[job_id]

    def getJobsByVertex(self, vertex, job_types):
        result = list()
        for job in self._open_jobs_foreach_vertex[vertex]:
            if job.getJobType() in job_types:
                result.append(job)
        return result

    def reserveJob(self, job):
        self._removeOpenJob(job)
        self._addReservedJob(job)

    def notifyDoneJob(self, done_job):
        # TODO: make JobAdmin remember call history along time series
        if done_job in self._reseved_jobs:
            self._removeReservedJob(done_job)
        if done_job in self._open_jobs:
            self._removeOpenJob(done_job)
        else:
            return

        self._done_jobs.append(done_job)

        for subsequentJobId in self._subsequent_job_ids[done_job.getJobId()]:
            subsequentJob = self._hidden_jobs[subsequentJobId]
            subsequentJob.notifyDoneJob(done_job.getJobId())
            if len(subsequentJob.getPriorJobIds()) == 0:
                self._next_open_jobs.append(subsequentJob)

    def advanceTime(self):
        for job in self._next_open_jobs:
            del self._hidden_jobs[job.getJobId()]
            self._addOpenJob(job)
        self._next_open_jobs.clear()

class RewardFunction:
    def __init__(self, rewards):
        self._t = rewards[::2]
        self._y = rewards[1::2]
        self._cache = dict()

        self._cumulative_sum = [0] * (self._t[-1]+2)
        for t in range(self._t[0], self._t[-1]+1):
            self._cumulative_sum[t+1] = self._cumulative_sum[t] + self._callWithoutCache(t)

    def __str__(self):
        return ''.join(['t={}, y={},'.format(t, y) for t, y in zip(self._t, self._y)])

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
        self._job_admin = JobAdmin()
        for i in range(self._Nworker):
            v, Lmax, Njobtype, *jobTypes = input().split()
            v, Lmax, Njobtype  = int(v), int(Lmax), int(Njobtype)
            jobTypes = list(map(int, jobTypes))
            worker = Worker(i, v, Lmax, jobTypes, self._graph, self._job_admin)
            self._workers.append(worker)

        # Job
        Njob, = input().split()
        self._Njob = int(Njob)
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
            self._job_admin.addJob(job)

        # log basic info
        log.debug(f"Tmax:      {self._Tmax}")
        log.debug(f"Nv:        {self._Nv}")
        log.debug(f"Ne:        {self._Ne}")
        log.debug(f"Nworker:   {self._Nworker}")
        for i in range(self._Nworker):
            log.debug(f"             {self._workers[i]}")
        log.debug(f"Njob:      {self._Njob}")
        log.debug(f"             {self._job_admin}")

    def run(self):
        # Output
        #for t in range(1, self._Tmax+1):
        #    for i in range(self._Nworker):
        #        action = self._workers[i].getAction(t)
        #        log.debug(f"time={t:03}, worker={i}, action={action}")
        #        print(action, flush=True)
        #    self._job_admin.advanceTime()

        # Output
        plans = []
        for i in range(self._Nworker):
            plans.append(self._workers[i].makePlan(self._Tmax))

        for t in range(self._Tmax):
            for i in range(self._Nworker):
                action = plans[i][t]
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
@execution_speed_lib
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

