import logging

#logging.basicConfig(filename='main.log', filemode='w', level=logging.DEBUG, format='%(levelname)s:%(filename)s %(lineno)04d: %(message)s')
logger = logging.getLogger(__name__)

N = int(input())
AB = []
for _ in range(N):
    a, b = list(map(int, input().split()))
    AB.append((a, b))

cost = 0

print(N)
for a, b in AB:
    cost += a + b
    print(f"0 0 {a} {b}")

def get_score(N, AB, cost):
    L = max(map(lambda elem: max(elem), AB))
    return int((10 ** 6) * N * L / (1 + cost))

logger.debug(f"cost:  {cost}")
logger.debug(f"score: {get_score(N, AB, cost)}")

