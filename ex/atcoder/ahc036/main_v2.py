import copy
import logging

#logging.basicConfig(filename='main.log', filemode='w', level=logging.DEBUG)
logger = logging.getLogger(__name__)

def get_distance(a, b):
    return (a[0] - b[0]) ** 2 + (a[1] - b[1]) ** 2

N, M, T, La, Lb = map(int, input().split())
logger.debug(f"N: {N}, M: {M}, T: {T}, La: {La}, Lb: {Lb}")

G = [ [] for _ in range(N) ]
for _ in range(M):
    u, v = map(int, input().split())
    G[u].append(v)
    G[v].append(u)

t = list(map(int, input().split()))
logger.debug(f"t: {t}")

P = []
for _ in range(N):
    x, y = map(int, input().split())
    P.append((x, y))

def make_A(A, visited):
    current = A[-1]

    adj_cities = sorted(G[current], key=lambda adj_city: get_distance(P[adj_city], P[current]), reverse=True)
    for adj_city in adj_cities:
        if visited[adj_city]:
            continue
        visited[adj_city] = True
        A.append(adj_city)
        make_A(A, visited)

A = [0]
visited = [ False ] * N
visited[0] = True
make_A(A, visited)
while len(A) < La:
    A.append(0)
print(*A)
logger.debug(f"A: {A}")

def make_new_G(G, A, Lb):
    new_G = copy.deepcopy(G)
    for u, _ in enumerate(new_G):
        i = A.index(u)
        for j in range(1, Lb + 1):
            if i + j < len(A) and A[i + j] in G[ A[i + j - 1] ]:
                new_G[u].append(A[i + j])
            else:
                break
        for j in range(1, Lb + 1):
            if i - j >= 0 and A[i - j] in G[ A[i - j + 1] ]:
                new_G[u].append(A[i - j])
            else:
                break
    return new_G

new_G = make_new_G(G, A, Lb)

def recurse(src, dst, path, visited):
    current = path[-1]
    logger.debug(f"src: {src}, dst: {dst}, path: {path}")
    if current == dst:
        return True

    #adj_cities = sorted(G[current], key=lambda adj_city: get_distance(P[adj_city], P[dst]))
    adj_cities = sorted(new_G[current], key=lambda adj_city: get_distance(P[adj_city], P[dst]))
    for adj_city in adj_cities:
        if visited[adj_city]:
            continue
        visited[adj_city] = True
        path.append(adj_city)
        result = recurse(src, dst, path, visited)
        if result:
            return True
        path.pop()
    return False

OPE_LIMIT = 10 ** 5
ope_counter = 0

src = 0
for dst in t:
    path = [src]
    visited = [ False ] * N
    visited[src] = True
    result = recurse(src, dst, path, visited)
    if not result:
        raise

    logger.debug(f"path: {path}")
    current = path[0]
    path = path[1:]
    for city in path:
        logger.debug(f"current: {current}")
        logger.debug(f"city:    {city}")
        if city in G[current]:
            print('s', 1, A.index(city), 0)
            print('m', city)
        else:
            index1 = A.index(current)
            index2 = A.index(city)
            if index1 < index2:
                print('s', index2 - index1, index1 + 1, 0)
                for i in range(index2 - index1):
                    print('m', A[index1 + i + 1])
            else:
                print('s', index1 - index2, index2, 0)
                for i in range(index1 - index2):
                    print('m', A[index1 - i - 1])
        current = city
        #ope_counter += 2
        #if ope_counter > OPE_LIMIT:
        #    raise
    src = dst
    #break

