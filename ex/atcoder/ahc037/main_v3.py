import logging

#logging.basicConfig(filename='main.log', filemode='w', level=logging.DEBUG, format='%(levelname)s:%(filename)s %(lineno)04d: %(message)s')
logger = logging.getLogger(__name__)

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

def make_soda_v3(AB):
    operations = []
    cost = 0

    A = [ elem[0] for elem in AB ]
    A.sort()
    B = [ elem[1] for elem in AB ]
    B.sort()

    S = set()
    S.add((0, 0))
    prev_a = 0
    prev_b = 0
    for i in range(len(AB)):
        cost += A[i] - prev_a + B[i] - prev_b
        operations.append(f"{prev_a} {prev_b} {A[i]} {B[i]}")
        S.add((A[i], B[i]))
        prev_a = A[i]
        prev_b = B[i]

    for a, b in AB:
        reusables = list(filter(lambda elem: elem[0] <= a and elem[1] <= b, S))
        reusables.sort(key=lambda elem: get_distance(elem, (a, b)))
        p = reusables[0]

        cost += a - p[0] + b - p[1]
        operations.append(f"{p[0]} {p[1]} {a} {b}")

    return operations, cost

def print_answer(operations):
    print(len(operations))
    for ope in operations:
        print(ope)

operations, cost = make_soda_v3(AB)
print_answer(operations)

logger.debug(f"cost:  {cost}")
logger.debug(f"score: {get_score(N, AB, cost)}")
