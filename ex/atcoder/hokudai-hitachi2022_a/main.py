import collections
import enum
import logging as log
import math
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
        assert u != v
        assert d > 0

        self._u = u
        self._v = v
        self._d = d

    def __str__(self):
        return 'u={}, v={}, d={}'.format(self._u, self._v, self._d)

    def __eq__(self, other):
        if self._d != other._d:
            return false
        result1 = self._u == other._u and self._v == other._v
        result2 = self._u == other._v and self._v == other._u
        return result1 or result2

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
    MATRIX_SIZE = 2001

    def __init__(self, edges):
        self._adjacent_vertices = collections.defaultdict(list)
        self._trajectory_matrix = [[None for _ in range(Graph.MATRIX_SIZE)] for _ in range(Graph.MATRIX_SIZE)]
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

        job_list = list()
        reward_rating_list = list()
        trajectory_list = list()
        while not pr_queue.empty():
            trajectory = pr_queue.get()
            jobs = context._job_admin.getJobsByVertex(trajectory.getVertices()[-1], context._job_types)
            if len(jobs) > 0:
                for job in jobs:
                    #log.debug(str(job))
                    reward, time_to_execute = job.getExpectedReward(context._current_time + trajectory.getDistance(), context._max_workload)
                    if reward > 0:
                        job_list.append(job)
                        reward_rating_list.append(reward / (trajectory.getDistance() + time_to_execute))
                        trajectory_list.append(trajectory)
                        break

            for vertex, distance in context._graph.getAdjacentVertices(trajectory.getVertices()[-1]):
                if vertex in sweeped:
                    continue
                new_trajectory = Trajectory([*trajectory.getVertices(), vertex], trajectory.getDistance() + distance)
                pr_queue.put(new_trajectory)
                sweeped.add(vertex)

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
        time_to_execute = math.ceil(self._remaining_tasks / workload_per_time)
        end_time = start_time + time_to_execute - 1
        if not self.isOpen(start_time) or not self.isOpen(end_time):
            return 0, 0

        if self._remaining_tasks < workload_per_time:
            workload_per_time = self._remaining_tasks
        reward = self._reward_func.getTotalReward(start_time, end_time) * workload_per_time
        return reward, time_to_execute

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
        self._open_jobs_foreach_vertex = collections.defaultdict(list)
        self._hidden_jobs = dict()
        self._done_jobs = list()
        self._subsequent_job_ids = collections.defaultdict(list)
        self._next_open_jobs = []

    def __str__(self):
        numof_open_jobs = 0
        for open_jobs in self._open_jobs_foreach_vertex.values():
            numof_open_jobs += len(open_jobs)

        return f"all={len(self._all_jobs)}, open={numof_open_jobs}, hidden={len(self._hidden_jobs)}, done={len(self._done_jobs)}"

    def addJob(self, job):
        self._all_jobs[job.getJobId()] = job

        if len(job.getPriorJobIds()) > 0:
            self._hidden_jobs[job.getJobId()] = job
            for priorJobId in job.getPriorJobIds():
                self._subsequent_job_ids[priorJobId].append(job.getJobId())
        else:
            self._open_jobs_foreach_vertex[job.getVertex()].append(job)

    def getJobByJobId(self, job_id):
        return self._all_jobs[job_id]

    def getJobsByVertex(self, vertex, job_types):
        result = []
        for job in self._open_jobs_foreach_vertex[vertex]:
            if job.getJobType() in job_types:
                result.append(job)
        return result

    def notifyDoneJob(self, done_job):
        if not done_job in self._open_jobs_foreach_vertex[done_job.getVertex()]:
            return

        self._open_jobs_foreach_vertex[done_job.getVertex()].remove(done_job)
        self._done_jobs.append(done_job)

        for subsequentJobId in self._subsequent_job_ids[done_job.getJobId()]:
            subsequentJob = self._hidden_jobs[subsequentJobId]
            subsequentJob.notifyDoneJob(done_job.getJobId())
            if len(subsequentJob.getPriorJobIds()) == 0:
                self._next_open_jobs.append(subsequentJob)

    def advanceTime(self):
        for job in self._next_open_jobs:
            del self._hidden_jobs[job.getJobId()]
            self._open_jobs_foreach_vertex[job.getVertex()].append(job)
        self._next_open_jobs.clear()

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

    def getTotalReward(self, start_time, end_time):
        reward = 0
        for t in range(start_time, end_time+1):
            reward += self(t)
        return reward

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
        edges = []
        for _ in range(self._Ne):
            u, v, d = input().split()
            edge = Edge(int(u), int(v), int(d))
            edges.append(edge)
        self._graph = Graph(edges)

        # Worker
        Nworker, = input().split()
        self._Nworker = int(Nworker)
        self._workers = []
        self._jobAdmin = JobAdmin()
        for i in range(self._Nworker):
            v, Lmax, Njobtype, *jobTypes = input().split()
            v, Lmax, Njobtype  = int(v), int(Lmax), int(Njobtype)
            jobTypes = list(map(int, jobTypes))
            worker = Worker(i, v, Lmax, jobTypes, self._graph, self._jobAdmin)
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
            control_points = [ControlPoint(rewards[2*i], rewards[2*i+1]) for i in range(Nreward)]
            job = Job(jobId, jobType, Ntask, vertex, control_points, priorJobIds)
            self._jobAdmin.addJob(job)

        # log basic info
        log.debug(f"Tmax:      {self._Tmax}")
        log.debug(f"Nv:        {self._Nv}")
        log.debug(f"Ne:        {self._Ne}")
        log.debug(f"Nworker:   {self._Nworker}")
        for i in range(self._Nworker):
            log.debug(f"             {self._workers[i]}")
        log.debug(f"Njob:      {self._Njob}")
        log.debug(f"             {self._jobAdmin}")

    def run(self):
        # Output
        for t in range(1, self._Tmax+1):
            for i in range(self._Nworker):
                action = self._workers[i].getAction(t)
                log.debug(f"time={t:03}, worker={i}, action={action}")
                print(action, flush=True)
            self._jobAdmin.advanceTime()

        # Score
        score, = input().split()
        self._score = int(score)
        end_time = time.perf_counter()
        log.debug(f"time:       {end_time - self._start_time}")
        log.debug(f"job:        {self._jobAdmin}")
        log.debug(f"score:      {self._score}")

# -----------------------------------------------------------------------------
# main
# -----------------------------------------------------------------------------
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

