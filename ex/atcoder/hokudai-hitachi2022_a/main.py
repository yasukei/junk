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
        self._initialState = None
        self._currentState = None
        self._context = context
        self._transitions = dict()

    def addTransition(self, src, dst, event):
        self._transitions[src, event] = dst

    def setInitialState(self, initialState):
        self._initialState = initialState

    def start(self):
        self._currentState = self._initialState
        self._currentState.onEntry(self._context)

    def onEvent(self, event):
        dst = self._transitions.get((self._currentState, event))
        if dst == None:
            return

        log.debug('{} exit'.format(type(self._currentState).__name__))
        self._currentState.onExit(self._context)
        log.debug('{} entry'.format(type(dst).__name__))
        dst.onEntry(self._context)
        self._currentState = dst

    def onAction(self):
        return self._currentState.onAction(self._context)

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

    @property
    def u(self):
        return self._u

    @property
    def v(self):
        return self._v

    @property
    def d(self):
        return self._d

class Trajectory:
    def __init__(self, vertices, distance):
        self._vertices = vertices
        self._distance = distance

    def __lt__(self, other):
        return self._distance < other._distance

    @property
    def vertices(self):
        return self._vertices

    @property
    def distance(self):
        return self._distance

class Graph:
    Nv_MAX = 2000

    def __init__(self, edges=[]):
        self._adjacentVertices = collections.defaultdict(list)
        self._trajectoryMatrix = [[None for _ in range(Graph.Nv_MAX + 1)] for _ in range(Graph.Nv_MAX + 1)]
        for edge in edges:
            self._addEdge(edge)

    def _addEdge(self, edge):
        u, v, d = edge.u, edge.v, edge.d
        self._adjacentVertices[u].append((v, d))
        self._adjacentVertices[v].append((u, d))

        self._trajectoryMatrix[u][v] = Trajectory([u, v], d)
        self._trajectoryMatrix[v][u] = Trajectory([v, u], d)

    def getAdjacentVertices(self, vertex):
        return self._adjacentVertices[vertex]

    def getDistance(self, vertex1, vertex2):
        if self._trajectoryMatrix[vertex1][vertex2] is None:
            raise NotImplemented()
        return self._trajectoryMatrix[vertex1][vertex2].distance

    #def getTrajectory(self, startVertex, endVertex):
    #    trajectory = self._trajectoryMatrix[startVertex][endVertex]
    #    if trajectory is not None:
    #        return trajectory

    #    # TODO


# -----------------------------------------------------------------------------
# Worker
# -----------------------------------------------------------------------------
class WorkerEvent(enum.Enum):
    MOVED = enum.auto()
    EXECUTED = enum.auto()
    COMPLETED = enum.auto()

class Worker:
    def __init__(self, initialVertex, maxWorkload, jobTypes, graph, jobAdmin):
        self._currentVertex = initialVertex
        self._maxWorkload = maxWorkload
        self._jobTypes = jobTypes
        self._graph = graph
        self._jobAdmin = jobAdmin

        self._eventQueue = collections.deque()
        self._trajectory = None
        self._trajectoryIndex = None
        self._remainingDistance = None
        self._targetJob = None
        self._currentTime = 0
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
        return ' '.join(list(map(str, [self._currentVertex, self._maxWorkload, len(self._jobTypes), *self._jobTypes])))

    def getAction(self, time):
        if not self._started:
            self._sm.start()
            self._started = True

        self._currentTime = time
        while len(self._eventQueue) > 0:
            event = self._eventQueue.popleft()
            self._sm.onEvent(event)
        return self._sm.onAction()

class WorkerState_Moving(State):
    def onEntry(self, context):
        trajectory = Trajectory([context._currentVertex], 0)
        pr_queue = queue.PriorityQueue()
        pr_queue.put(trajectory)
        sweeped = set()
        sweeped.add(context._currentVertex)

        jobFound = False
        while not pr_queue.empty():
            trajectory = pr_queue.get()
            jobs = context._jobAdmin.getJobsByVertex(trajectory.vertices[-1], context._jobTypes)
            if len(jobs) > 0:
                for job in jobs:
                    if job.getExpectedReward(context._currentTime + trajectory.distance, context._maxWorkload) > 0:
                        jobFound = True
                        break
                if jobFound:
                    break
            else:
                for vertex, distance in context._graph.getAdjacentVertices(trajectory.vertices[-1]):
                    if vertex in sweeped:
                        continue
                    new_trajectory = Trajectory([*trajectory.vertices, vertex], trajectory.distance + distance)
                    pr_queue.put(new_trajectory)
                    sweeped.add(vertex)

        if not jobFound:
            context._eventQueue.append(WorkerEvent.COMPLETED)
            return

        if trajectory.distance == 0:
            context._eventQueue.append(WorkerEvent.MOVED)
            return

        context._trajectory = trajectory
        context._trajectoryIndex = 1
        context._remainingDistance = context._graph.getDistance(
                context._trajectory.vertices[context._trajectoryIndex],
                context._trajectory.vertices[context._trajectoryIndex-1])

    def onAction(self, context):
        action = 'move {}'.format(context._trajectory.vertices[context._trajectoryIndex])
        context._remainingDistance -= 1
        if context._remainingDistance == 0:
            context._currentVertex = context._trajectory.vertices[context._trajectoryIndex]
            context._trajectoryIndex += 1
            if context._trajectoryIndex < len(context._trajectory.vertices):
                context._remainingDistance = context._graph.getDistance(
                        context._trajectory.vertices[context._trajectoryIndex],
                        context._trajectory.vertices[context._trajectoryIndex-1])
            else:
                context._eventQueue.append(WorkerEvent.MOVED)
        return action

class WorkerState_Executing(State):
    def onEntry(self, context):
        jobs = context._jobAdmin.getJobsByVertex(context._currentVertex, context._jobTypes)
        for job in jobs:
            if job.getExpectedReward(context._currentTime, context._maxWorkload) > 0:
                context._targetJob = job
                return

        context._eventQueue.append(WorkerEvent.EXECUTED)

    def onAction(self, context):
        targetJob = context._targetJob

        if targetJob.getExpectedReward(context._currentTime, context._maxWorkload) == 0:
            context._eventQueue.append(WorkerEvent.EXECUTED)
            action = 'stay'
            return action

        numofTakingTasks = targetJob.takeTasks(context._currentTime, context._maxWorkload)
        action = 'execute {} {}'.format(targetJob.jobId, numofTakingTasks)
        if targetJob.isDone:
            context._jobAdmin.notifyDoneJob(targetJob)
            context._eventQueue.append(WorkerEvent.EXECUTED)
        return action

    def onExit(self, context):
        context._targetJob = None

class WorkerState_Done(State):
    def onAction(self, context):
        action = 'stay'
        return action

# -----------------------------------------------------------------------------
# Job
# -----------------------------------------------------------------------------
class Job:
    def __init__(self, jobId, jobType, numofTasks, vertex, reward_control_points=[], priorJobIds=[]):
        self._jobId = jobId
        self._jobType = jobType
        self._numofRemainingTasks = numofTasks
        self._vertex = vertex
        self._reward_control_points = reward_control_points
        self._reward_func = RewardFunction(reward_control_points)
        self._gainedReward = 0
        self._priorJobIds = priorJobIds

    def __str__(self):
        l1 = 'jobId: {}, jobType: {}, numofRemainingTasks: {}, vertex: {}'.format(
                self._jobId,
                self._jobType,
                self._numofRemainingTasks,
                self._vertex)
        l2 = 'numofControlPoints: {}, controlPoints: {}'.format(
                int(len(self._reward_control_points)/2),
                [str(rcp) for rcp in self._reward_control_points])
        l3 = 'numofPriorJobIds: {}, priorJobIds: {}'.format(
                len(self._priorJobIds),
                self._priorJobIds)
        return '\n'.join([l1, l2, l3])

    @property
    def jobId(self):
        return self._jobId

    @property
    def jobType(self):
        return self._jobType

    @property
    def isDone(self):
        return self._numofRemainingTasks == 0

    @property
    def vertex(self):
        return self._vertex

    @property
    def priorJobIds(self):
        return self._priorJobIds

    @property
    def gainedReward(self):
        if self._numofRemainingTasks > 0:
            return 0
        return int(self._gainedReward)

    def getExpectedReward(self, time, numofTasksToBeExecuted):
        if self._numofRemainingTasks > 0 and self._reward_func(time) > 0:
            if self._numofRemainingTasks < numofTasksToBeExecuted:
                numofTasksToBeExecuted = self._numofRemainingTasks
            return self._reward_func(time) * numofTasksToBeExecuted
        return 0

    def takeTasks(self, time, numofTaking):
        reward = self.getExpectedReward(time, numofTaking)
        if reward <= 0:
            return 0

        if numofTaking > self._numofRemainingTasks:
            numofTaking = self._numofRemainingTasks
        self._numofRemainingTasks -= numofTaking
        self._gainedReward += numofTaking * reward
        return numofTaking

    def notifyDoneJob(self, doneJob):
        self._priorJobIds.remove(doneJob)

class JobAdmin:
    def __init__(self):
        self._dictByJobId = dict()
        self._openJobDictByVertex = collections.defaultdict(list)
        self._numofOpenJob = 0
        self._hiddenJobDictByJobId = dict()
        self._numofHiddenJob = 0
        self._subsequentJobIds = collections.defaultdict(list)

    def __str__(self):
        l1 = 'numof all jobs:    {}'.format(len(self._dictByJobId))
        l2 = 'numof open jobs:   {}'.format(self._numofOpenJob)
        l3 = 'numof hidden jobs: {}'.format(self._numofHiddenJob)
        return '\n'.join([l1, l2, l3])

    def addJob(self, job):
        self._dictByJobId[job.jobId] = job

        if len(job.priorJobIds) > 0:
            self._hiddenJobDictByJobId[job.jobId] = job
            self._numofHiddenJob += 1
            for priorJobId in job.priorJobIds:
                self._subsequentJobIds[priorJobId].append(job.jobId)
        else:
            self._openJobDictByVertex[job.vertex].append(job)
            self._numofOpenJob += 1

    def getJobByJobId(self, jobId):
        return self._dictByJobId[jobId]

    def getJobsByVertex(self, vertex, jobTypes):
        result = []
        for job in self._openJobDictByVertex[vertex]:
            if job.jobType in jobTypes:
                result.append(job)
        return result

    def notifyDoneJob(self, doneJob):
        del self._openJobDictByVertex[doneJob.vertex]
        self._numofOpenJob -= 1

        for subsequentJobId in self._subsequentJobIds[doneJob.jobId]:
            subsequentJob = self._hiddenJobDictByJobId[subsequentJobId]
            subsequentJob.notifyDoneJob(doneJob.jobId)
            if len(subsequentJob.priorJobIds) == 0:
                del self._hiddenJobDictByJobId[subsequentJob.jobId]
                self._numofHiddenJob -= 1
                self._openJobDictByVertex[subsequentJob.vertex] = subsequentJob

class ControlPoint:
    def __init__(self, t, y):
        self._t = t
        self._y = y

    def __str__(self):
        return 't={}, y={}'.format(self._t, self._y)

    @property
    def t(self):
        return self._t

    @property
    def y(self):
        return self._y

class RewardFunction:
    def __init__(self, control_points=[]):
        self._control_points = control_points

    def __call__(self, t):
        if t < self._control_points[0].t:
            return self._control_points[0].y
        elif t >= self._control_points[-1].t:
            return self._control_points[-1].y
        else:
            i = 1
            while t > self._control_points[i].t:
                i += 1
            t_prev = self._control_points[i-1].t
            y_prev = self._control_points[i-1].y
            t_next = self._control_points[i].t
            y_next = self._control_points[i].y
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

