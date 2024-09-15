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

def get_distance(a, b):
    return (a[0] - b[0]) ** 2 + (a[1] - b[1]) ** 2

def get_midpoints(a, b, num):
    midpoints = []
    for i in range(num):
        x = round((b[0] - a[0]) * (i+1) / (num+1) + a[0])
        y = round((b[1] - a[1]) * (i+1) / (num+1) + a[1])
        midpoints.append((x, y))
    return midpoints

def make_soda_v5(AB):
    operations = []
    cost = 0

    AB.sort(key=lambda elem: get_distance(elem, (0, 0)))

    S = set()
    S.add((0, 0))

    for a, b in AB:
        reusables = list(filter(lambda elem: elem[0] <= a and elem[1] <= b, S))
        reusables.sort(key=lambda elem: get_distance(elem, (a, b)))
        p = reusables[0]

        for mid in get_midpoints(p, (a, b), 4):
            cost += mid[0] - p[0] + mid[1] - p[1]
            operations.append(f"{p[0]} {p[1]} {mid[0]} {mid[1]}")
            S.add(mid)
            p = mid
        cost += a - p[0] + b - p[1]
        operations.append(f"{p[0]} {p[1]} {a} {b}")
        S.add((a, b))

    return operations, cost

def print_answer(operations):
    print(len(operations))
    for ope in operations:
        print(ope)

operations, cost = make_soda_v5(AB)
print_answer(operations)

logger.debug(f"cost:  {cost}")
logger.debug(f"score: {get_score(N, AB, cost)}")
logger.debug(f"time:  {clock.get_elapsed_time_millisec()}")

