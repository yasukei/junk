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

def make_soda_v2(AB):
    operations = []
    cost = 0

    A = [ elem[0] for elem in AB ]
    A.sort()
    B = [ elem[1] for elem in AB ]
    B.sort()

    prev = 0
    for a in A:
        cost += a - prev
        operations.append(f"{prev} 0 {a} 0")
        prev = a

    for a, b in AB:
        cost += b
        operations.append(f"{a} 0 {a} {b}")

    return operations, cost

def print_answer(operations):
    print(len(operations))
    for ope in operations:
        print(ope)

operations, cost = make_soda_v2(AB)
print_answer(operations)

logger.debug(f"cost:  {cost}")
logger.debug(f"score: {get_score(N, AB, cost)}")

