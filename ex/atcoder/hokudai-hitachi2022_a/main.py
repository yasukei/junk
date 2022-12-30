import collections
import enum
import logging as log
import queue
import random
import sys
import time

import numpy as np

# -----------------------------------------------------------------------------
# Utility
# -----------------------------------------------------------------------------

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

        log.debug('{} exit'.format(type(self._current_state).__name__))
        self._current_state.onExit(self._context)
        log.debug('{} entry'.format(type(dst).__name__))
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
        return ' '.join(list(map(str, [self.u, self.v, self.d])))

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
    Nv_MAX = 2000

    def __init__(self, edges=[]):
        self._adjacent_vertices = collections.defaultdict(list)
        self._trajectory_matrix = [[None for _ in range(Graph.Nv_MAX + 1)] for _ in range(Graph.Nv_MAX + 1)]
        for edge in edges:
            self._addEdge(edge)

    def _addEdge(self, edge):
        u, v, d = edge.u(), edge.v(), edge.d()
        self._adjacent_vertices[u].append((v, d))
        self._adjacent_vertices[v].append((u, d))

        self._trajectory_matrix[u][v] = Trajectory([u, v], d)
        self._trajectory_matrix[v][u] = Trajectory([v, u], d)

    def getAdjacentVertices(self, vertex):
        return self._adjacent_vertices[vertex]

    def getDistance(self, vertex1, vertex2):
        if self._trajectory_matrix[vertex1][vertex2] is None:
            raise NotImplemented()
        return self._trajectory_matrix[vertex1][vertex2].getDistance()


# -----------------------------------------------------------------------------
# Worker
# -----------------------------------------------------------------------------
class WorkerEvent(enum.Enum):
    MOVED = enum.auto()
    EXECUTED = enum.auto()
    COMPLETED = enum.auto()

class Worker:
    def __init__(self, initial_vertex, max_workload, job_types, graph, job_admin):
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
        done = WorkerState_Done()
        sm.addTransition(moving,    executing, WorkerEvent.MOVED)
        sm.addTransition(moving,    done,      WorkerEvent.COMPLETED)
        sm.addTransition(executing, moving,    WorkerEvent.EXECUTED)
        sm.setInitialState(moving)
        self._sm = sm

    def __str__(self):
        return ' '.join(list(map(str, [self._current_vertex, self._max_workload, len(self._job_types), *self._job_types])))

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
        trajectory = Trajectory([context._current_vertex], 0)
        pr_queue = queue.PriorityQueue()
        pr_queue.put(trajectory)
        sweeped = set()
        sweeped.add(context._current_vertex)

        jobFound = False
        while not pr_queue.empty():
            trajectory = pr_queue.get()
            jobs = context._job_admin.getJobsByVertex(trajectory.getVertices()[-1], context._job_types)
            if len(jobs) > 0:
                for job in jobs:
                    if job.getExpectedReward(context._current_time + trajectory.getDistance(), context._max_workload) > 0:
                        jobFound = True
                        break
                if jobFound:
                    break
            else:
                for vertex, distance in context._graph.getAdjacentVertices(trajectory.getVertices()[-1]):
                    if vertex in sweeped:
                        continue
                    new_trajectory = Trajectory([*trajectory.getVertices(), vertex], trajectory.getDistance() + distance)
                    pr_queue.put(new_trajectory)
                    sweeped.add(vertex)

        if not jobFound:
            context._event_queue.append(WorkerEvent.COMPLETED)
            return

        if trajectory.getDistance() == 0:
            context._event_queue.append(WorkerEvent.MOVED)
            return

        context._trajectory = trajectory
        context._trajectory_index = 1
        context._remaining_distance = context._graph.getDistance(
                context._trajectory.getVertices()[context._trajectory_index],
                context._trajectory.getVertices()[context._trajectory_index-1])

    def onAction(self, context):
        action = 'move {}'.format(context._trajectory.getVertices()[context._trajectory_index])
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

class WorkerState_Executing(State):
    def onEntry(self, context):
        jobs = context._job_admin.getJobsByVertex(context._current_vertex, context._job_types)
        for job in jobs:
            if job.getExpectedReward(context._current_time, context._max_workload) > 0:
                context._target_job = job
                return

        context._event_queue.append(WorkerEvent.EXECUTED)

    def onAction(self, context):
        targetJob = context._target_job

        if targetJob.getExpectedReward(context._current_time, context._max_workload) == 0:
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

class WorkerState_Done(State):
    def onAction(self, context):
        action = 'stay'
        return action

# -----------------------------------------------------------------------------
# Job
# -----------------------------------------------------------------------------
class Job:
    def __init__(self, jobId, job_type, numof_tasks, vertex, reward_control_points=[], prior_job_ids=[]):
        self._job_id = jobId
        self._job_type = job_type
        self._remaining_tasks = numof_tasks
        self._vertex = vertex
        self._reward_control_points = reward_control_points
        self._reward_func = RewardFunction(reward_control_points)
        self._gained_reward = 0
        self._prior_job_ids = prior_job_ids

    def __str__(self):
        l1 = 'jobId: {}, job_type: {}, numofRemainingTasks: {}, vertex: {}'.format(
                self._job_id,
                self._job_type,
                self._remaining_tasks,
                self._vertex)
        l2 = 'numofControlPoints: {}, controlPoints: {}'.format(
                int(len(self._reward_control_points)/2),
                [str(rcp) for rcp in self._reward_control_points])
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

    def getVertex(self):
        return self._vertex

    def getPriorJobIds(self):
        return self._prior_job_ids

    def getGainedReward(self):
        if self._remaining_tasks > 0:
            return 0
        return int(self._gained_reward)

    def getExpectedReward(self, current_time, numof_tasks_to_be_executed):
        if self._remaining_tasks > 0 and self._reward_func(current_time) > 0:
            if self._remaining_tasks < numof_tasks_to_be_executed:
                numof_tasks_to_be_executed = self._remaining_tasks
            return self._reward_func(current_time) * numof_tasks_to_be_executed
        return 0

    def takeTasks(self, current_time, numof_taking):
        reward = self.getExpectedReward(current_time, numof_taking)
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
        self._open_jobs_foreach_vertex = collections.defaultdict(list)
        self._numof_open_jobs = 0
        self._hidden_jobs = dict()
        self._numof_hidden_jobs = 0
        self._subsequent_job_ids = collections.defaultdict(list)

    def __str__(self):
        l1 = 'numof all jobs:    {}'.format(len(self._all_jobs))
        l2 = 'numof open jobs:   {}'.format(self._numof_open_jobs)
        l3 = 'numof hidden jobs: {}'.format(self._numof_hidden_jobs)
        return '\n'.join([l1, l2, l3])

    def addJob(self, job):
        self._all_jobs[job.getJobId()] = job

        if len(job.getPriorJobIds()) > 0:
            self._hidden_jobs[job.getJobId()] = job
            self._numof_hidden_jobs += 1
            for priorJobId in job.getPriorJobIds():
                self._subsequent_job_ids[priorJobId].append(job.getJobId())
        else:
            self._open_jobs_foreach_vertex[job.getVertex()].append(job)
            self._numof_open_jobs += 1

    def getJobByJobId(self, job_id):
        return self._all_jobs[job_id]

    def getJobsByVertex(self, vertex, job_types):
        result = []
        for job in self._open_jobs_foreach_vertex[vertex]:
            if job.getJobType() in job_types:
                result.append(job)
        return result

    def notifyDoneJob(self, done_job):
        if self._open_jobs_foreach_vertex.get(done_job.getVertex()) is None:
            return

        del self._open_jobs_foreach_vertex[done_job.getVertex()]
        self._numof_open_jobs -= 1

        for subsequentJobId in self._subsequent_job_ids[done_job.getJobId()]:
            subsequentJob = self._hidden_jobs[subsequentJobId]
            subsequentJob.notifyDoneJob(done_job.getJobId())
            if len(subsequentJob.getPriorJobIds()) == 0:
                del self._hidden_jobs[subsequentJob.getJobId()]
                self._numof_hidden_jobs -= 1
                self._open_jobs_foreach_vertex[subsequentJob.getVertex()] = subsequentJob

class ControlPoint:
    def __init__(self, t, y):
        self._t = t
        self._y = y

    def __str__(self):
        return 't={}, y={}'.format(self._t, self._y)

    def t(self):
        return self._t

    def y(self):
        return self._y

class RewardFunction:
    def __init__(self, control_points=[]):
        self._control_points = control_points

    def __call__(self, t):
        if t < self._control_points[0].t():
            return self._control_points[0].y()
        elif t >= self._control_points[-1].t():
            return self._control_points[-1].y()
        else:
            i = 1
            while t > self._control_points[i].t():
                i += 1
            t_prev = self._control_points[i-1].t()
            y_prev = self._control_points[i-1].y()
            t_next = self._control_points[i].t()
            y_next = self._control_points[i].y()
            return (y_next - y_prev) * (t - t_prev) / (t_next - t_prev) + y_prev

# -----------------------------------------------------------------------------
# main
# -----------------------------------------------------------------------------
def main():
    # Time
    T, = input().split()
    T = int(T)

    # Graph
    Nv, Ne = input().split()
    Nv, Ne  = int(Nv), int(Ne)
    edges = []
    for _ in range(Ne):
        u, v, d = input().split()
        edge = Edge(int(u), int(v), int(d))
        edges.append(edge)
    graph = Graph(edges)

    # Worker
    Nworker, = input().split()
    Nworker = int(Nworker)
    workers = []
    jobAdmin = JobAdmin()
    for _ in range(Nworker):
        v, Lmax, Njobtype, *jobTypes = input().split()
        v, Lmax, Njobtype  = int(v), int(Lmax), int(Njobtype)
        jobTypes = list(map(int, jobTypes))
        worker = Worker(v, Lmax, jobTypes, graph, jobAdmin)
        workers.append(worker)

    # Job
    Njob, = input().split()
    Njob = int(Njob)
    for _ in range(Njob):
        jobId, jobType, Ntask, vertex = input().split()
        jobId, jobType, Ntask, vertex = int(jobId), int(jobType), int(Ntask), int(vertex)
        Nreward, *rewards = input().split()
        Nreward = int(Nreward)
        rewards = list(map(int, rewards))
        Ndepend, *priorJobIds = input().split()
        Ndepend = int(Ndepend)
        priorJobIds = list(map(int, priorJobIds))
        control_points = [ControlPoint(rewards[2*i], rewards[2*i+1]) for i in range(Nreward)]
        job = Job(jobId, jobType, Ntask, vertex, control_points, priorJobIds)
        jobAdmin.addJob(job)

    # Output
    for t in range(1, T+1):
        for worker in workers:
            action = worker.getAction(t)
            log.debug(f"time: {t:03}, action: {action}")
            print(action, flush=True)

    # Score
    score, = input().split()

    # Debug info
    #print(T, file=sys.stderr)
    #print(Nv, Ne, file=sys.stderr)
    #for edge in edges:
    #    print(edge, file=sys.stderr)
    #print(Nworker, file=sys.stderr)
    #for worker in workers:
    #    print(worker, file=sys.stderr)
    #print(Njob, file=sys.stderr)
    #for job in jobs:
    #    print(job, file=sys.stderr)

    #timeStart = time.perf_counter()
    #a = np.ones((400, 400))
    #np.dot(a, a)
    #timeEnd = time.perf_counter()
    #result = timeEnd - timeStart
    #print(result, file=sys.stderr)

if __name__ == '__main__':
    log.basicConfig(filename='debug.log', filemode='w', level=log.DEBUG)

    if len(sys.argv) == 1:
        main()
    else:
        print('sys.argv: ' + str(sys.argv))

