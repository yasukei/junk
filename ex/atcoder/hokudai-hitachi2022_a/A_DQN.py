import collections
import copy
import enum
import logging as log
import queue
import random
import sys
import time

import numpy as np

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

    def getAdjacentVertices(self, vertex):
        return self._adjacent_vertices[vertex]

# -----------------------------------------------------------------------------
# Worker
# -----------------------------------------------------------------------------
class Worker:
    def __init__(self, worker_id, initial_vertex, max_workload, job_types, graph, job_admin):
        self._worker_id = worker_id
        self._initial_vertex = initial_vertex
        self._max_workload = max_workload
        self._job_types = job_types

        self._graph = graph
        self._job_admin = job_admin

        self.reset()

    def __str__(self):
        return f"worker_id={self._worker_id}, vertex={self._initial_vertex}, max_workload={self._max_workload}, job_types={self._job_types}"

    def reset(self):
        self._u = self._initial_vertex
        self._v = self._initial_vertex
        self._d = 0
        self._action_history = []
        
        state = (self._u, self._v, self._d)
        return state

    def act(self, action, current_time):
        reward = 0
        action_record = 'stay'

        if action == Action.STAY:
            pass

        elif Action.MOVE1 <= action and action <= Action.MOVE4:
            if self._d != 0:
                if action == Action.MOVE1:
                    action_record = 'move {}'.format(self._v)
                    self._d -= 1
                else:
                    # invalid move
                    reward = -1
            else:
                vertices = self._graph.getAdjacentVertices(self._u)
                if action - Action.MOVE1 < len(vertices):
                    self._v, self._d = vertices[action - Action.MOVE1]
                    action_record = 'move {}'.format(self._v)
                    self._d -= 1
                else:
                    # non-existence move
                    reward = -1
            if self._u != self._v and self._d == 0:
                self._u = self._v

        elif Action.EXECUTE1 <= action and action <= Action.EXECUTE3:
            if self._d != 0:
                # trying to execute during moving
                reward = -1
            else:
                jobs = self._job_admin.getJobsByVertex(self._u, self._job_types)
                if action - Action.EXECUTE1 < len(jobs):
                    job = jobs[action - Action.EXECUTE1]
                    if job.getExpectedReward(current_time, self._max_workload) > 0:
                        tasks = job.takeTasks(current_time, self._max_workload)
                        action_record = 'execute {} {}'.format(job.getJobId(), tasks)
                        reward = job.getGainedReward()
                        if reward > 0:
                            self._job_admin.notifyDoneJob(job)
                    else:
                        # there is no jobs to execute
                        reward = -1
                else:
                    # invalid job specification
                    reward = -1
        else:
            # invalud Action
            assert False

        self._action_history.append(action_record)
        state = (self._u, self._v, self._d)
        return state, reward

    def get_action_history(self):
        return self._action_history

# -----------------------------------------------------------------------------
# Job
# -----------------------------------------------------------------------------
class Job:
    def __init__(self, jobId, job_type, numof_tasks, vertex, reward_control_points=[], prior_job_ids=[]):
        self._job_id = jobId
        self._job_type = job_type
        self._numof_tasks = numof_tasks
        self._vertex = vertex
        self._reward_control_points = reward_control_points
        self._reward_func = RewardFunction(reward_control_points)
        self._prior_job_ids = prior_job_ids
        self.reset()

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

    def reset(self):
        self._remaining_tasks = self._numof_tasks
        self._gained_reward = 0
        self._current_prior_job_ids = copy.copy(self._prior_job_ids)

    def getJobId(self):
        return self._job_id

    def getJobType(self):
        return self._job_type

    def isDone(self):
        return self._remaining_tasks == 0

    def getVertex(self):
        return self._vertex

    def getPriorJobIds(self):
        return self._current_prior_job_ids

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
        self._current_prior_job_ids.remove(done_job)

class JobAdmin:
    def __init__(self):
        self._all_jobs = dict()
        self.reset()

    def __str__(self):
        numof_open_jobs = 0
        for open_jobs in self._open_jobs_foreach_vertex.values():
            numof_open_jobs += len(open_jobs)

        return f"all={len(self._all_jobs)}, open={numof_open_jobs}, hidden={len(self._hidden_jobs)}, done={len(self._done_jobs)}"

    def reset(self):
        self._open_jobs_foreach_vertex = collections.defaultdict(list)
        self._hidden_jobs = dict()
        self._done_jobs = list()
        self._subsequent_job_ids = collections.defaultdict(list)
        self._next_open_jobs = []

        for job in self._all_jobs.values():
            job.reset()
            self._screenJob(job)

    def addJob(self, job):
        self._all_jobs[job.getJobId()] = job
        self._screenJob(job)

    def _screenJob(self, job):
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

# -----------------------------------------------------------------------------
# Environment
# -----------------------------------------------------------------------------
class Environment:
    def __init__(self):
        self._start_time = time.perf_counter()
        #self._reset()

    def readInput(self):
        # Time
        Tmax, = input().split()
        self._Tmax = int(Tmax)
        self._Tcurrent = 1

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
            control_points = [ControlPoint(rewards[2*i], rewards[2*i+1]) for i in range(Nreward)]
            job = Job(jobId, jobType, Ntask, vertex, control_points, priorJobIds)
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

    def reset(self):
        self._done = False
        self._Tcurrent = 1
        self._job_admin.reset()

        states = []
        for worker in self._workers:
            state = worker.reset()
            states.append(state)
        reward = 0
        return tuple(states), reward, self._done

    def step(self, action):
        if self._Tcurrent > self._Tmax:
            return None, None, self._done
        elif self._Tcurrent == self._Tmax:
            self._done = True

        states = [self._Tcurrent]
        rewards = []
        for i, worker in enumerate(self._workers):
            state, reward = worker.act(action[i], self._Tcurrent)
            states.append(state)
            rewards.append(reward)

        self._Tcurrent += 1
        self._job_admin.advanceTime()
        return tuple(states), sum(rewards), self._done

    def run(self):
        actions = []
        for worker in self._workers:
            actions.append(worker.get_action_history())

        # Output
        for t in range(self._Tmax):
            for i in range(len(self._workers)):
                action = actions[i][t]
                log.debug(f"time={t+1:03}, worker={i}, action={action}")
                print(action, flush=True)

        # Score
        score, = input().split()
        self._score = int(score)
        end_time = time.perf_counter()
        log.debug(f"time:       {end_time - self._start_time}")
        log.debug(f"job:        {self._job_admin}")
        log.debug(f"score:      {self._score}")

class Action(enum.IntEnum):
    STAY = 0
    MOVE1 = 1
    MOVE2 = 2
    MOVE3 = 3
    MOVE4 = 4
    EXECUTE1 = 5
    EXECUTE2 = 6
    EXECUTE3 = 7

# -----------------------------------------------------------------------------
# Agent
# -----------------------------------------------------------------------------
class Agent:
    def __init__(self):
        pass

    def get_action(self, state):
        actions = [
                Action.STAY,
                Action.MOVE1,
                Action.MOVE2,
                Action.MOVE3,
                Action.MOVE4,
                Action.EXECUTE1,
                Action.EXECUTE2,
                Action.EXECUTE3,
                ]
        return [random.choice(actions)]

# -----------------------------------------------------------------------------
# main
# -----------------------------------------------------------------------------
def main():
    env = Environment()
    env.readInput()

    agent = Agent()

    numof_episode = 10

    for i in range(numof_episode):
        log.debug('episode {} start'.format(i))
        start_time = time.perf_counter()
        state = env.reset()

        done = False
        while not done:
            action = agent.get_action(state)
            log.debug('action: {}'.format(action))
            state, reward, done = env.step(action)
            log.debug('state: {}, reward: {}, done: {}'.format(state, reward, done))
        end_time = time.perf_counter()
        log.debug('episode {} end. elapsed time: {}'.format(i, end_time - start_time))

    log.debug('env.run()')
    env.run()

if __name__ == '__main__':
    log.basicConfig(filename='debug.log', filemode='w', level=log.DEBUG)

    if len(sys.argv) == 1:
        main()
    else:
        print('sys.argv: ' + str(sys.argv))

