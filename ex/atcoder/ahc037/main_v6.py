import logging
import time

#logging.basicConfig(filename='main.log', filemode='w', level=logging.DEBUG, format='%(levelname)s:%(filename)s %(lineno)04d: %(message)s')
logger = logging.getLogger(__name__)

class Clock:
    def __init__(self):
        self.start_time = time.perf_counter()

    def get_elapsed_time_millisec(self):
        return (time.perf_counter() - self.start_time) * 1000
clock = Clock()

N = int(input())
AB = []
for _ in range(N):
    a, b = list(map(int, input().split()))
    AB.append((a, b))

def get_score(N, AB, cost):
    L = max(map(lambda elem: max(elem), AB))
    return round((10 ** 6) * N * L / (1 + cost))

def get_distance(p1, p2):
    return (p1[0] - p2[0]) ** 2 + (p1[1] - p2[1]) ** 2

def get_midpoints(start, end, num):
    midpoints = []
    for i in range(num):
        x = round((end[0] - start[0]) * (i+1) / (num+1) + start[0])
        y = round((end[1] - start[1]) * (i+1) / (num+1) + start[1])
        midpoints.append((x, y))
    return midpoints

def connect(start, end, num, operations, S):
    cost = 0
    p = start
    for mid in get_midpoints(p, end, num):
        cost += mid[0] - p[0] + mid[1] - p[1]
        operations.append(f"{p[0]} {p[1]} {mid[0]} {mid[1]}")
        S.add(mid)
        p = mid
    cost += end[0] - p[0] + end[1] - p[1]
    operations.append(f"{p[0]} {p[1]} {end[0]} {end[1]}")
    S.add(end)
    return cost

def make_soda_v6(AB):
    operations = []
    cost = 0

    AB.sort(key=lambda elem: get_distance(elem, (0, 0)))

    priors = []
    for a, b in AB:
        if a == 0 and b == 0:
            priors.append((a, b))
        elif a == 0:
            priors.append((a, b))
        elif b == 0:
            priors.append((a, b))

    S = set()
    S.add((0, 0))
    for a, b in priors:
        cost += connect((0, 0), (a, b), 4, operations, S)

    for a, b in AB:
        if a == 0 or b == 0:
            continue
        reusables = list(filter(lambda elem: elem[0] <= a and elem[1] <= b, S))
        reusables.sort(key=lambda elem: get_distance(elem, (a, b)))
        p = reusables[0]

        cost += connect(p, (a, b), 4, operations, S)

    return operations, cost

def print_answer(operations):
    print(len(operations))
    for ope in operations:
        print(ope)

operations, cost = make_soda_v6(AB)
print_answer(operations)

logger.debug(f"cost:  {cost}")
logger.debug(f"score: {get_score(N, AB, cost)}")
logger.debug(f"time:  {clock.get_elapsed_time_millisec()}")

