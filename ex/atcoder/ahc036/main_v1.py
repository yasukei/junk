def get_distance(a, b):
    return (a[0] - b[0]) ** 2 + (a[1] - b[1]) ** 2

OPE_LIMIT = 10 ** 5

N, M, T, La, Lb = map(int, input().split())

G = [ [] for _ in range(N) ]
for _ in range(M):
    u, v = map(int, input().split())
    G[u].append(v)
    G[v].append(u)

t = list(map(int, input().split()))

P = []
for _ in range(N):
    x, y = map(int, input().split())
    P.append((x, y))

A = [0] * La
for i in range(La):
    if i < N:
        A[i] = i
    else:
        break
print(*A)

def recurse(src, dst, path, visited):
    current = path[-1]
    if current == dst:
        return True

    adj_cities = []
    for adj_city in G[current]:
        if visited[adj_city]:
            continue
        adj_cities.append(adj_city)

    adj_cities.sort(key=lambda adj_city: get_distance(P[adj_city], P[dst]))
    for adj_city in adj_cities:
        visited[adj_city] = True
        path.append(adj_city)
        result = recurse(src, dst, path, visited)
        if result:
            return True
        path.pop()
    return False

src = 0
ope_counter = 0
for dst in t:
    path = [src]
    visited = [ False ] * N
    visited[src] = True
    result = recurse(src, dst, path, visited)
    if not result:
        raise

    path = path[1:]
    for city in path:
        print('s', 1, city, 0)
        print('m', city)
        ope_counter += 2
        if ope_counter > OPE_LIMIT:
            raise
    src = dst

