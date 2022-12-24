import sys
import random
import numpy as np

class Edge:
    def __init__(self, u, v, d):
        self.u = u
        self.v = v
        self.d = d

    def __str__(self):
        return ' '.join(list(map(str, [self.u, self.v, self.d])))

class Worker:
    def __init__(self, initPosition, workload, jobTypes=[]):
        self.initPosition = initPosition
        self.workload = workload
        self.jobTypes = jobTypes

    def __str__(self):
        return ' '.join(list(map(str, [self.initPosition, self.workload, len(self.jobTypes), *self.jobTypes])))

class Job:
    def __init__(self, jobId, jobType, numofTasks, position, rewards=[], priorJobIds=[]):
        self.jobId = jobId
        self.jobType = jobType
        self.numofTasks = numofTasks
        self.position = position
        self.rewards = rewards
        self.priorJobIds = priorJobIds

    def __str__(self):
        l1 = ' '.join(list(map(str, [self.jobId, self.jobType, self.numofTasks, self.position])))
        l2 = ' '.join(list(map(str, [int(len(self.rewards)/2), *self.rewards])))
        l3 = ' '.join(list(map(str, [len(self.priorJobIds), *self.priorJobIds])))
        return '\n'.join([l1, l2, l3])

def main():
    # Time
    T, = input().split()
    T = int(T)

    # Geometry
    Nv, Ne = input().split()
    Nv, Ne  = int(Nv), int(Ne)
    edges = []
    for _ in range(Ne):
        u, v, d = input().split()
        edge = Edge(int(u), int(v), int(d))
        edges.append(edge)

    # Worker
    Nworker, = input().split()
    Nworker = int(Nworker)
    workers = []
    for _ in range(Nworker):
        v, Lmax, Njobtype, *jobTypes = input().split()
        v, Lmax, Njobtype  = int(v), int(Lmax), int(Njobtype)
        worker = Worker(v, Lmax, jobTypes)
        workers.append(worker)

    # Job
    Njob, = input().split()
    Njob = int(Njob)
    jobs = []
    for _ in range(Njob):
        jobId, jobType, Ntask, position = input().split()
        jobId, jobType, Ntask, position = int(jobId), int(jobType), int(Ntask), int(position)
        Nreward, *rewards = input().split()
        Nreward = int(Nreward)
        Ndepend, *priorJobIds = input().split()
        Ndepend = int(Ndepend)
        job = Job(jobId, jobType, Ntask, position, rewards, priorJobIds)
        jobs.append(job)

    # Output
    for t in range(T):
        for w in range(Nworker):
            print('stay')

    # Score
    score, = input().split()
    print('score:', score, file=sys.stderr)

    # Debug info
    print(T, file=sys.stderr)
    print(Nv, Ne, file=sys.stderr)
    for edge in edges:
        print(edge, file=sys.stderr)
    print(Nworker, file=sys.stderr)
    for worker in workers:
        print(worker, file=sys.stderr)
    print(Njob, file=sys.stderr)
    for job in jobs:
        print(job, file=sys.stderr)

if __name__ == '__main__':
    if len(sys.argv) == 1:
        main()
    else:
        print('sys.argv: ' + str(sys.argv))

