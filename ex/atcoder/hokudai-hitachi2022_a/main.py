import bisect
import collections
import copy
import enum
import functools
import heapq
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
        stats.sort_stats(pstats.SortKey.TIME, pstats.SortKey.CUMULATIVE)
        stats.reverse_order()
        stats.print_stats()
    return wrapper

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
        #self._edge = dict()
        self._edges = collections.defaultdict(list)
        self._vertices = set()
        self._trajectory = dict()

        for edge in edges:
            self._addEdge(edge)

        self._priority_queues = dict()
        self._priority_queues_memo = dict()
        for vertex in self._vertices:
            priority_queue = PriorityQueue()
            memo = set()
            memo.add(vertex)
            for edge in self._edges[vertex]:
                priority_queue.enque(self._trajectory[(vertex, edge.v())])
                memo.add(edge.v())

            self._priority_queues[vertex] = priority_queue
            self._priority_queues_memo[vertex] = memo

    def _addEdge(self, edge):
        u, v, d = edge.u(), edge.v(), edge.d()
        edge_v_u = Edge(v, u, d)
        #self._edge[(u, v)] = edge
        #self._edge[(v, u)] = edge_v_u
        self._edges[u].append(edge)
        self._edges[v].append(edge_v_u)

        self._vertices.add(u)
        self._vertices.add(v)

        self._trajectory[(u, v)] = Trajectory([u, v], d)
        self._trajectory[(v, u)] = Trajectory([v, u], d)
        if self._trajectory.get((u, u)) is None:
            self._trajectory[(u, u)] = Trajectory([u, u], 0)
        if self._trajectory.get((v, v)) is None:
            self._trajectory[(v, v)] = Trajectory([v, v], 0)

    def getTrajectory(self, start, goal):
        # TODO: use mid-point

        #log.debug('start={}, goal={}'.format(start, goal))

        if self._trajectory.get((start, goal)):
            return self._trajectory[(start, goal)]
        #elif self._trajectory.get((goal, start)):
        #    reversed_trajectory = self._trajectory[(goal, start)].getReversedTrajectory()
        #    self._trajectory[(start, goal)] = reversed_trajectory
        #    return reversed_trajectory

        priority_queue = self._priority_queues[start]
        memo = self._priority_queues_memo[start]

        goalFound = False
        while not priority_queue.empty():
            trajectory = priority_queue.deque()

            for edge in self._edges[trajectory.getEndVertex()]:
                edge_v = edge.v()
                if edge_v in memo:
                    continue
                #if self._trajectory.get((edge_v, goal)):
                #    new_trajectory = trajectory + self._trajectory[(edge_v, goal)]
                #    self._trajectory[(start, goal)] = new_trajectory
                #    goalFound = True
                #    break

                #log.debug('trajectory={}'.format(trajectory))
                #log.debug('edge={}'.format(edge))
                new_trajectory = trajectory.createNewTrajectory(edge.v(), edge.d())
                self._trajectory[(start, edge_v)] = new_trajectory

                priority_queue.enque(new_trajectory)
                memo.add(edge_v)

                if edge_v == goal:
                    goalFound = True

            if goalFound:
                break

        return self._trajectory[(start, goal)]

    def getEdges(self, vertex):
        return self._edges[vertex]

    def getAdjacentVertices(self, vertex):
        vertices = list()
        for edge in self._edges[vertex]:
            vertices.append((edge.v(), edge.d()))
        return vertices

    def getDistance(self, vertex1, vertex2):
        trajectory = self.getTrajectory(vertex1, vertex2)
        if trajectory:
            return trajectory.getDistance()
        return None

class Trajectory:
    def __init__(self, vertices, distance):
        self._vertices = vertices
        #self._end_vertex = vertices[-1]
        self._distance = distance

    def __lt__(self, other):
        return self._distance < other._distance

    def __add__(self, other):
        assert False
        # TODO
        vertices = self._edges + other._edges
        return Trajectory(edges, self._distance + other._distance)

    def __str__(self):
        s1 = 'vertices={}'.format(self._vertices)
        s2 = 'distance={}'.format(self._distance)
        return '\n'.join([s1, s2])

    def getDistance(self):
        return self._distance

    def getStartVertex(self):
        return self._vertices[0]

    def getEndVertex(self):
        return self._vertices[-1]
        #return self._end_vertex

    def getVertices(self):
        return self._vertices

    #def getReversedTrajectory(self):
    #    reversed_vertices = [vertex for vertex in reversed(self._vertices)]
    #    return Trajectory(reversed_vertices, self._distance)

    def createNewTrajectory(self, vertex, distance):
        return Trajectory(self._vertices + [vertex], self._distance + distance)

class PriorityQueue:
    def __init__(self):
        self._heap = list()

    def empty(self):
        return len(self._heap) == 0

    def enque(self, item):
        heapq.heappush(self._heap, item)

    def deque(self):
        return heapq.heappop(self._heap)

    def getHeadItem(self):
        return self._heap[0]

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

    def getWorkerId(self):
        return self._worker_id

    def getInitialVertex(self):
        return self._initial_vertex

    def getWorkload(self):
        return self._workload

    def getJobTypes(self):
        return self._job_types

    def makeSchedule(self, Tmax, graph, job_admin):
        schedule = Schedule(Tmax, self._initial_vertex)
        def currentTime():
            return schedule.getTailJobEndTime()

        Method.makeScheduleByDP(graph, job_admin, schedule, self._job_types, self._workload)
        #Method.makeHighRewardSchedule(graph, job_admin, schedule, self._job_types, self._workload)
        #Method.makeGreedyScheduleInAllBlanks(graph, job_admin, schedule, self._job_types, self._workload)

        schedule.fillNecessaryMoveInAllBlanks(graph)
        schedule.fillStayInAllBlanks()
        return schedule.makeActionStrings()

def filterAcceptableJobs(jobs, job_types, additional_condition=None):
    if additional_condition:
        for job in jobs:
            if job.getJobType() in job_types and not job.isReserved() and additional_condition(job):
                yield job
    else:
        for job in jobs:
            if job.getJobType() in job_types and not job.isReserved():
                yield job

def canMoveAndExecute(graph, current_time, current_vertex, job, workload, end_time, end_vertex=None):
    if job.getEndTime() <= current_time:
        return False

    given_time = end_time - current_time
    execution_time = job.getExecutionTime(workload)
    if given_time < execution_time:
        return False

    travel_time_before = graph.getDistance(current_vertex, job.getVertex())
    if travel_time_before is None or given_time < travel_time_before + execution_time:
        return False

    reward = job.getExpectedReward(current_time + travel_time_before, workload)
    if reward == 0:
        return False

    if end_vertex:
        travel_time_after = graph.getDistance(job.getVertex(), end_vertex)
        if travel_time_after is None or given_time < travel_time_before + execution_time + travel_time_after:
            return False
    return True

class Method:
    @staticmethod
    def makeGreedyScheduleInAllBlanks(graph, job_admin, schedule, job_types, workload):
        for blank in schedule.getBlankTime():
            Method.makeGreedySchedule(graph, job_admin, schedule, job_types, workload, blank)

    @staticmethod
    def makeGreedySchedule(graph, job_admin, schedule, job_types, workload, blank):
        log.debug('blank, start_time={}, end_time={}'.format(blank.getStartTime(), blank.getEndTime()))
        log.debug('blank, start_vertex={}, end_vertex={}'.format(blank.getStartVertex(), blank.getEndVertex()))

        current_time = blank.getStartTime()
        current_vertex = blank.getStartVertex()

        while True:
            best_rating = 0
            best_job = None

            def additionalCondition(job):
                return canMoveAndExecute(graph, current_time, current_vertex, job, workload, blank.getEndTime(), blank.getEndVertex())

            for job in filterAcceptableJobs(job_admin.getOpenJobs(), job_types, additionalCondition):
                travel_time_before = graph.getDistance(current_vertex, job.getVertex())
                reward = job.getExpectedReward(current_time + travel_time_before, workload)
                execution_time = job.getExecutionTime(workload)
                #log.debug('current_time={}, travel_time={}, reward={}, execution_time={}'.format(current_time, travel_time, reward, execution_time))
                rating = reward / (travel_time_before + execution_time)

                if rating > best_rating:
                    best_rating = rating
                    best_job = job

            if best_job:
                travel_time = graph.getDistance(current_vertex, best_job.getVertex())
                execute = TimeSlot.Execute(current_time + travel_time, best_job, workload)
                schedule += execute
                best_job.reserve()
                current_time = execute.getEndTime()
                current_vertex = execute.getEndVertex()
            else:
                break

    @staticmethod
    def makeScheduleByDP(graph, job_admin, schedule, job_types, workload):
        # TODO: treat all jobs considering with prior jobs

        all_jobs = list(job_admin.getOpenJobs())
        all_jobs.sort(key=lambda job: job.getRewardFunction().getPeak()[1], reverse=True)
        log.debug('all_jobs={}'.format(len(all_jobs)))
        all_jobs = all_jobs[:100]
        log.debug('all_jobs={}'.format(len(all_jobs)))
        numof_jobs = len(all_jobs)
        Tmax = schedule.getTmax()
        initial_vertex = schedule.getVertex(1)

        dp = [[[None, None, None] for _ in range(numof_jobs)] for _ in range(Tmax + 1)]
        dp[0][0][0] = 0      # total reward
        dp[0][0][1] = set()  # done job ids
        dp[0][0][2] = list() # Timeslots
        for t in range(1, Tmax + 1):
            for i in range(numof_jobs):
                log.debug('t={}, i={}'.format(t, i))

                if i == 0:
                    dp[t][i][0] = dp[t-1][i][0]
                    dp[t][i][1] = dp[t-1][i][1]
                    dp[t][i][2] = dp[t-1][i][2]
                else:
                    max_reward = 0
                    done_job_ids = None
                    timeslots = None
                    previous_vertex = None

                    #job_i = job_admin.getJobByJobId(i)
                    job_i = all_jobs[i]
                    job_i_execution_time = job_i.getExecutionTime(workload)
                    job_i_reward = job_i.getExpectedReward(t - job_i_execution_time, workload)

                    if job_i_reward > 0:
                        for j in range(numof_jobs):
                            if j == 0:
                                j_vertex = initial_vertex
                            else:
                                #j_vertex = job_admin.getJobByJobId(j).getVertex()
                                j_vertex = all_jobs[j].getVertex()
                            distance = graph.getDistance(j_vertex, job_i.getVertex())

                            j_time = t - distance - job_i_execution_time
                            if j_time < 1:
                                continue
                            if dp[j_time][j][0] is None:
                                continue
                            if i in dp[j_time][j][1]:
                                continue

                            temp = job_i_reward + dp[j_time][j][0]
                            if temp > max_reward:
                                max_reward = temp
                                done_job_ids = dp[j_time][j][1]
                                timeslots = dp[j_time][j][2]
                                previous_vertex = j_vertex

                    if dp[t-1][i][0] is None:
                        if max_reward > 0:
                            dp[t][i][0] = max_reward
                            dp[t][i][1] = done_job_ids | {i}
                            trajectory = graph.getTrajectory(previous_vertex, job_i.getVertex())
                            distance = trajectory.getDistance()
                            dp[t][i][2] = timeslots \
                                            + [TimeSlot.Move(t - distance - job_i_execution_time, trajectory, graph), \
                                               TimeSlot.Execute(t - job_i_execution_time, job_i, workload)]
                    else:
                        if max_reward > dp[t-1][i][0]:
                            dp[t][i][0] = max_reward
                            dp[t][i][1] = done_job_ids | {i}
                            trajectory = graph.getTrajectory(previous_vertex, job_i.getVertex())
                            distance = trajectory.getDistance()
                            dp[t][i][2] = timeslots \
                                            + [TimeSlot.Move(t - distance - job_i_execution_time, trajectory, graph), \
                                               TimeSlot.Execute(t - job_i_execution_time, job_i, workload)]
                        else:
                            dp[t][i][0] = dp[t-1][i][0]
                            dp[t][i][1] = dp[t-1][i][1]
                            dp[t][i][2] = dp[t-1][i][2]
                #log.debug('dp[{}][{}], [0]={}, [1]={}, [2]={}'.format(t, i, dp[t][i][0], dp[t][i][1], dp[t][i][2]))

        best_reward = 0
        best_job_timeslots = None
        for i in range(numof_jobs):
            if dp[Tmax][i][0] > best_reward:
                best_reward = dp[Tmax][i][0]
                best_job_timeslots = dp[Tmax][i][2]

        for ts in best_job_timeslots:
            schedule += ts


    @staticmethod
    def makeHighRewardSchedule(graph, job_admin, schedule, job_types, workload, blank):
        mandatory_jobs = list()

        current_time = blank.getStartTime()
        current_vertex = blank.getStartVertex()

        for high_reward_job in filterAcceptableJobs(job_admin.getAllJobsInHighRewardOrder(), job_types):

            time_to_execute = high_reward_job.getOptimalTimeToExecute(workload)
            schedule += TimeSlot.Execute(time_to_execute, high_reward_job, workload)
            high_reward_job.reserve()

            for prior_job in job_admin.getPriorJobs(high_reward_job):
                for blank in schedule.getBlankTime():
                    blank.getStartTime()
                    prior_job.getStartTime()
                    overlapped_interval = getOverlappedInterval(blank.getStartTime(), blank.getEndTime(), prior_job.getStartTime(), prior_job.getEndTime())
                    if overlapped_interval is None:
                        continue
                    if overlapped_interval.getLength() < prior_job.getExecutionTime(workload):
                        continue
                    log.debug('blank_start={}, blank_end={}, job_start={}, job_end={}'.format(blank.getStartTime(), blank.getEndTime(), prior_job.getStartTime(), prior_job.getEndTime()))
                    log.debug('interval_start={}, interval_end={}'.format(overlapped_interval.getStart(), overlapped_interval.getEnd()))

                    travel_time_before = graph.getDistance(blank.getStartVertex(), prior_job.getVertex())
                    if blank.getDuration() < prior_job.getExecutionTime(workload) + travel_time_before:
                        continue

                    travel_time_after = graph.getDistance(prior_job.getVertex(), blank.getEndVertex())
                    if blank.getDuration() < prior_job.getExecutionTime(workload) + travel_time_before + travel_time_after:
                        continue

                    if blank.getStartTime() + travel_time_before <= overlapped_interval.getStart():
                        time_to_execute = overlapped_interval.getStart()
                    else:
                        time_to_execute = blank.getStartTime() + travel_time_before

                    schedule += TimeSlot.Execute(time_to_execute, prior_job, workload)
                    prior_job.reserve()
                    break
                else:
                    assert False
            break

# -----------------------------------------------------------------------------
# Schedule
# -----------------------------------------------------------------------------
class Schedule:
    START_TIME = 1

    def __init__(self, Tmax, initial_vertex):
        self._Tmax = Tmax
        self._initial_vertex = initial_vertex
        self._time_slots = list()

    def __str__(self):
        s = ['Schedule:']
        for ts in self._time_slots:
            s.append(str(ts))
        return '\n'.join(s)

    def getTmax(self):
        return self._Tmax

    def __iadd__(self, new_time_slot):
        self.assignTimeSlot(new_time_slot)
        return self

    def assignTimeSlot(self, new_time_slot):
        log.debug('new_time_slot: {}'.format(new_time_slot))
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
            log.debug('fail to assign')
            log.debug(self)
            assert False

        self._time_slots.insert(index, new_time_slot)
        log.debug('updated schedule: {}'.format(self))

    def getTailJobEndTime(self):
        if len(self._time_slots) > 0:
            return self._time_slots[-1].getEndTime()
        return Schedule.START_TIME

    def getTailJobEndVertex(self):
        if len(self._time_slots) > 0:
            return self._time_slots[-1].getEndVertex()
        return self._initial_vertex

    def getVertex(self, current_time):
        vertex = self._initial_vertex
        for ts in self._time_slots:
            if current_time < ts.getStartTime():
                break
            elif current_time == ts.getStartTime():
                vertex = ts.getStartVertex()
                break
            elif current_time < ts.getEndTime():
                # When the timeslot is 'move', the vertex during moving is hard to determine at most
                vertex = None
                break
            vertex = ts.getEndVertex()
        return vertex

    def getBlankTime(self):
        blanks = list()
        cursor = Schedule.START_TIME
        start_vertex = self._initial_vertex
        for ts in self._time_slots:
            if cursor < ts.getStartTime():
                blanks.append(Blank(cursor, ts.getStartTime(), start_vertex, ts.getStartVertex()))
            cursor = ts.getEndTime()
            start_vertex = ts.getEndVertex()
        if cursor <= self._Tmax:
            blanks.append(Blank(cursor, self._Tmax+1, start_vertex, None))
        return blanks

    def fillNecessaryMoveInAllBlanks(self, graph):
        for blank in self.getBlankTime():
            if blank.getStartVertex() == blank.getEndVertex() or blank.getEndVertex() is None:
                continue
            trajectory = graph.getTrajectory(blank.getStartVertex(), blank.getEndVertex())
            self += TimeSlot.Move(blank.getStartTime(), trajectory, graph)

    def fillStayInAllBlanks(self):
        for blank in self.getBlankTime():
            self += TimeSlot.Stay(blank.getStartTime(), blank.getStartVertex(), blank.getDuration())

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
    def Stay(start_time, vertex, stay_time):
        return TimeSlot(start_time, stay_time, vertex, vertex, StayAction(stay_time))

    @staticmethod
    def Move(start_time, trajectory, graph):
        return TimeSlot(start_time, trajectory.getDistance(), trajectory.getStartVertex(), trajectory.getEndVertex(), MoveAction(trajectory, graph))

    @staticmethod
    def Execute(start_time, job, workload):
        task_list = job.getTaskListToExecute(workload)
        return TimeSlot(start_time, len(task_list), job.getVertex(), job.getVertex(), ExecuteAction(job.getJobId(), task_list))

class StayAction:
    def __init__(self, stay_time):
        self._stay_time = stay_time

    def __str__(self):
        return 'stay {} times'.format(self._stay_time)
        
    def makeActionStrings(self):
        return ['stay'] * self._stay_time

class MoveAction:
    def __init__(self, trajectory, graph):
        self._trajectory = trajectory
        self._graph = graph

    def __str__(self):
        return 'move {} -> {}'.format(
                    self._trajectory.getStartVertex(),
                    self._trajectory.getEndVertex())
        
    def makeActionStrings(self):
        action = list()
        vertices = self._trajectory.getVertices()
        for i in range(1, len(vertices)):
            #move = ['move {}'.format(edge.v())] * edge.d()
            move = ['move {}'.format(vertices[i])] * self._graph.getDistance(vertices[i-1], vertices[i])
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

class Blank:
    def __init__(self, start_time, end_time, start_vertex, end_vertex):
        self._start_time = start_time
        self._end_time = end_time
        self._start_vertex = start_vertex
        self._end_vertex = end_vertex

    def getStartTime(self):
        return self._start_time

    def getEndTime(self):
        return self._end_time

    def getStartVertex(self):
        return self._start_vertex

    def getEndVertex(self):
        return self._end_vertex

    def getDuration(self):
        return self._end_time - self._start_time

class Interval:
    def __init__(self, start, end):
        self._start = start
        self._end = end

    def getStart(self):
        return self._start

    def getEnd(self):
        return self._end

    def getLength(self):
        return self._end - self._start

def getOverlappedInterval(start1, end1, start2, end2):
    if end1 < start2 or end2 < start1:
        return None

    if start1 < start2:
        if end1 < end2:
            return Interval(start2, end1)
        else:
            return Interval(start2, end2)
    else:
        if end2 < end1:
            return Interval(start1, end2)
        else:
            return Interval(start1, end1)

# -----------------------------------------------------------------------------
# Job
# -----------------------------------------------------------------------------
class Job:
    def __init__(self, jobId, job_type, numof_tasks, vertex, rewards, prior_job_ids):
        self._job_id = jobId
        self._job_type = job_type
        self._numof_tasks = numof_tasks
        self._vertex = vertex
        self._reward_function = RewardFunction(rewards)
        self._prior_job_ids = prior_job_ids

        self._reserved = False
        self._plannedStartTime = None
        self._plannedEndTime = None

    def __str__(self):
        l1 = 'jobId: {}, job_type: {}, numof_tasks: {}, vertex: {}'.format(
                self._job_id,
                self._job_type,
                self._numof_tasks,
                self._vertex)
        l2 = 'rewards: {}'.format(self._reward_function)
        l3 = 'numofPriorJobIds: {}, prior_job_ids: {}'.format(
                len(self._prior_job_ids),
                self._prior_job_ids)
        return '\n'.join([l1, l2, l3])

    def getJobId(self):
        return self._job_id

    def getJobType(self):
        return self._job_type

    def isOpen(self, current_time):
        if current_time < self._reward_function.getStartTime() or self._reward_function.getEndTime() <= current_time:
            return False
        return True

    def getStartTime(self):
        return self._reward_function.getStartTime()

    def getEndTime(self):
        return self._reward_function.getEndTime()

    def getRewardFunction(self):
        return self._reward_function
    
    def getVertex(self):
        return self._vertex

    def getPriorJobIds(self):
        return self._prior_job_ids

    def getExecutionTime(self, workload):
        return math.ceil(self._numof_tasks / workload)

    def getExpectedReward(self, start_time, workload):
        if not self.isOpen(start_time):
            return 0

        end_time = start_time + self.getExecutionTime(workload)
        if not self.isOpen(end_time - 1):
            return 0

        if self._numof_tasks < workload:
            workload = self._numof_tasks
        return self._reward_function.getTotalReward(start_time, end_time - 1) * workload

    def getOptimalTimeToExecute(self, workload):
        execution_time = self.getExecutionTime(workload)
        peak_time, peak_value = self._reward_function.getPeak()
        time_before, value_before = self._reward_function.getInflectionPointBeforePeak()
        time_after, value_after = self._reward_function.getInflectionPointAfterPeak()
        time_to_execute = 0
        reward_max = 0
        if value_before > value_after:
            time_to_execute = math.floor(peak_time - execution_time / 2)
            if time_to_execute + execution_time > self.getEndTime():
                time_to_execute = self.getEndTime() - execution_time
            while True:
                reward = self.getExpectedReward(time_to_execute, workload)
                log.debug('reward={}'.format(reward))
                if reward > reward_max:
                    reward_max = reward
                    time_to_execute -= 1
                    continue
                break
        else:
            time_to_execute = math.ceil(peak_time - execution_time / 2)
            if time_to_execute < self.getStartTime():
                time_to_execute = self.getStartTime()
            while True:
                reward = self.getExpectedReward(time_to_execute, workload)
                log.debug('reward={}'.format(reward))
                if reward > reward_max:
                    reward_max = reward
                    time_to_execute += 1
                    continue
                break
        return time_to_execute


    def reserve(self):
        if self._reserved:
            assert False

        self._reserved = True

    def isReserved(self):
        return self._reserved

    def getTaskListToExecute(self, workload):
        task_list = list()
        remaining_tasks = self._numof_tasks
        while remaining_tasks > 0:
            numof_tasks = min(workload, remaining_tasks)
            task_list.append(numof_tasks)
            remaining_tasks -= numof_tasks
        return task_list

    #def notifyDoneJob(self, done_job):
    #    self._prior_job_ids.remove(done_job)

class JobAdmin:
    def __init__(self, jobs):
        self._all_jobs = set()
        self._job_dict = dict()
        self._open_jobs = set()
        self._open_jobs_foreach_vertex = collections.defaultdict(list)
        self._hidden_jobs = set()
        self._done_jobs = set()
        self._subsequent_job_ids = collections.defaultdict(list)

        for job in jobs:
            self._addJob(job)

        self._all_jobs_in_high_reward_order = list(self._all_jobs)
        self._all_jobs_in_high_reward_order.sort(key=lambda job: job.getRewardFunction().getPeak()[1], reverse=True)
        for job in self._all_jobs_in_high_reward_order:
            peak_time, peak_value = job.getRewardFunction().getPeak()
            #log.debug('job_id={}, peak_time={}, peak_value={}'.format(job.getJobId(), peak_time, peak_value))

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

    def _addOpenJob(self, job):
        self._open_jobs.add(job)
        self._open_jobs_foreach_vertex[job.getVertex()].append(job)

    def _removeOpenJob(self, job):
        self._open_jobs.remove(job)
        self._open_jobs_foreach_vertex[job.getVertex()].remove(job)

    def getAllJobs(self):
        return self._all_jobs

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

    def getPriorJobs(self, job):
        # retrun a list of prior jobs in topological sort order
        prior_jobs = list()
        visited_job_ids = set()
        JobAdmin._getPriorJobs(job, self._job_dict, visited_job_ids, prior_jobs)
        return prior_jobs

    @staticmethod
    def _getPriorJobs(job, job_dict, visited_job_ids, result):
        visited_job_ids.add(job.getJobId())

        for prior_job_id in job._prior_job_ids:
            if prior_job_id in visited_job_ids:
                continue
            prior_job = job_dict[prior_job_id]
            JobAdmin._getPriorJobs(prior_job, job_dict, visited_job_ids, result)
            result.append(prior_job)

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
        self._peak_index = self._y.index(self._peak_value)
        self._peak_time = self._t[self._peak_index]

        self._cumulative_sum = [0] * (self._t[-1]+2)
        for t in range(self._t[0], self._t[-1]+1):
            self._cumulative_sum[t+1] = self._cumulative_sum[t] + self._callWithoutCache(t)

    def __str__(self):
        return ''.join(['t={}, y={},'.format(t, y) for t, y in zip(self._t, self._y)])

    @functools.lru_cache(maxsize=None)
    def __call__(self, t):
        return self._callWithoutCache(t)

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

    def getInflectionPointBeforePeak(self):
        return self._t[self._peak_index - 1], self._y[self._peak_index - 1]

    def getInflectionPointAfterPeak(self):
        return self._t[self._peak_index + 1], self._y[self._peak_index + 1]

    def getTotalReward(self, start_time, end_time):
        return self._cumulative_sum[end_time+1] - self._cumulative_sum[start_time]

# -----------------------------------------------------------------------------
# Main
# -----------------------------------------------------------------------------
class Main:
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
        log.info(f"Tmax:      {self._Tmax}")
        log.info(f"Nv:        {self._Nv}")
        log.info(f"Ne:        {self._Ne}")
        log.info(f"Nworker:   {self._Nworker}")
        for i in range(self._Nworker):
            log.info(f"             {self._workers[i]}")
        log.info(f"Njob:      {self._Njob}")
        log.info(f"             {self._job_admin}")
        if log.getLogger().isEnabledFor(log.DEBUG):
            self._job_admin.saveJobGraph()

    def run(self):
        # Output
        schedules = [None] * self._Nworker
        self._workers.sort(key=lambda worker: worker.getWorkload(), reverse=True)
        for worker in self._workers:
            schedules[worker.getWorkerId()] = worker.makeSchedule(self._Tmax, self._graph, self._job_admin)

        for t in range(self._Tmax):
            for i in range(self._Nworker):
                action = schedules[i][t]
                log.info(f"time={t+1:03}, worker={i}, action={action}")
                print(action, flush=True)
        end_time = time.perf_counter()

        # Score
        score, = input().split()
        self._score = int(score)
        log.info(f"time:       {end_time - self._start_time}")
        log.info(f"job:        {self._job_admin}")
        log.info(f"score:      {self._score}")

# -----------------------------------------------------------------------------
# main
# -----------------------------------------------------------------------------
@execution_speed_lib
def main():
    main = Main()
    main.readInput()
    main.run()

if __name__ == '__main__':
    log.basicConfig(filename='debug.log', filemode='w', format='%(lineno)d@%(funcName)s: %(message)s', level=log.DEBUG)
    #log.basicConfig(filename='debug.log', filemode='w', format='%(lineno)d@%(funcName)s: %(message)s', level=log.INFO)

    if len(sys.argv) == 1:
        main()
    else:
        print('sys.argv: ' + str(sys.argv))

