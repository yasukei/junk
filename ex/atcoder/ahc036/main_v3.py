from collections import namedtuple
import copy
import logging
import time

#logging.basicConfig(filename='main.log', filemode='w', level=logging.DEBUG)
logger = logging.getLogger(__name__)

class Clock:
    def __init__(self):
        self.start_time = time.perf_counter()

    def get_elapsed_time_millisec(self):
        return (time.perf_counter() - self.start_time) * 1000

def get_distance(a, b):
    return (a[0] - b[0]) ** 2 + (a[1] - b[1]) ** 2

def get_distance_vertically_emphasized(a, b):
    c = (a[0], a[1] * 1000)
    d = (b[0], b[1] * 1000)
    return get_distance(c, d)

def get_distance_horizontally_emphasized(a, b):
    c = (a[0] * 1000, a[1])
    d = (b[0] * 1000, b[1])
    return get_distance(c, d)

def find_path(N, G, P, src, dst):
    def _recurse(src, dst, path, visited):
        current = path[-1]
        if current == dst:
            return True

        adj_cities = sorted(G[current], key=lambda adj_city: get_distance(P[adj_city], P[dst]))
        for adj_city in adj_cities:
            if visited[adj_city]:
                continue
            visited[adj_city] = True
            path.append(adj_city)
            result = _recurse(src, dst, path, visited)
            if result:
                return True
            path.pop()
        return False

    path = [src]
    visited = [ False ] * N
    visited[src] = True
    result = _recurse(src, dst, path, visited)
    if not result:
        return None

    return path

class Global:
    def read_input(self):
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

        self._N = N
        self._M = M
        self._T = T
        self._La = La
        self._Lb = Lb
        self._G = G
        self._t = t
        self._P = P

        logger.debug(f"N: {N}, M: {M}, T: {T}, La: {La}, Lb: {Lb}")
        logger.debug(f"t: {t}")

    def N(self):
        return self._N
    def M(self):
        return self._M
    def T(self):
        return self._T
    def La(self):
        return self._La
    def Lb(self):
        return self._Lb
    def G(self):
        return self._G
    def t(self):
        return self._t
    def P(self):
        return self._P

class Recorder:
    RECORD_LIMIT = 10 ** 5

    def __init__(self, g):
        self.N = g.N()
        self.La = g.La()
        self.Lb = g.Lb()
        self.sig_counter = 0
        self.mv_counter = 0
        self.records = []

    def _isRecordable(self):
        return self.sig_counter + self.mv_counter <= Recorder.RECORD_LIMIT

    def signal(self, l, Pa, Pb):
        if l < 1 or self.Lb < l:
            raise Exception(f"l is out of bounds. l: {l}, Pa: {Pa}, Pb: {Pb}, La: {self.La}, Lb: {self.Lb}")
        if Pa < 0 or self.La - l < Pa:
            raise Exception(f"Pa is out of bounds. l: {l}, Pa: {Pa}, Pb: {Pb}, La: {self.La}, Lb: {self.Lb}")
        if Pb < 0 or self.Lb - l < Pb:
            raise Exception(f"Pb is out of bounds. l: {l}, Pa: {Pa}, Pb: {Pb}, La: {self.La}, Lb: {self.Lb}")


        self.records.append(f"s {l} {Pa} {Pb}")
        self.sig_counter += 1

        if not self._isRecordable():
            raise Exception("Record limit exceeded.")

    def move(self, v):
        if v < 0 or self.N <= v:
            raise Exception("v is out of bounds. v: {v}, N: {self.N}")

        self.records.append(f"m {v}")
        self.mv_counter += 1

        if not self._isRecordable():
            raise Exception("Record limit exceeded.")

    def print(self):
        for record in self.records:
            print(record)

    def get_score(self):
        return self.sig_counter

    def get_numof_records(self):
        return self.sig_counter + self.mv_counter

class AFactory:
    @staticmethod
    def make_A_by_choosing_far_city(g, tweak=0):
        N = g.N()
        G = g.G()
        P = g.P()
        La = g.La()
        Lb = g.Lb()

        def recurse(A, visited):
            current = A[-1]
            oldest = current
            for i in range(1, Lb-tweak):
                if i >= len(A):
                    break
                if A[-1-i] not in G[ A[-i] ]:
                    break
                oldest = A[-1-i]

            #adj_cities = sorted(G[current], key=lambda adj_city: get_distance(P[adj_city], P[current]), reverse=True)
            adj_cities = sorted(G[current], key=lambda adj_city: get_distance(P[adj_city], P[oldest]), reverse=True)
            for adj_city in adj_cities:
                if visited[adj_city]:
                    continue
                visited[adj_city] = True
                A.append(adj_city)
                recurse(A, visited)

        A = [0]
        visited = [ False ] * N
        visited[0] = True
        recurse(A, visited)
        while len(A) < La:
            A.append(0)
        return A

    @staticmethod
    def make_A_by_clutering(g):
        N = g.N()
        G = g.G()
        P = g.P()
        La = g.La()
        Lb = g.Lb()

        city_finder = {}
        for city, p in enumerate(P):
            city_finder[p] = city

        A = []
        visited = [ False ] * N
        new_P = sorted(P, key=lambda p: get_distance(p, (0, 0)))
        for p in new_P:
            first_city = city_finder[p]
            if visited[first_city]:
                continue

            A.append(first_city)
            visited[first_city] = True
            current = first_city
            for _ in range(1, Lb):
                adj_cities = sorted(G[current], key=lambda adj_city: get_distance(P[adj_city], P[first_city]), reverse=True)
                adj_cities = [ city for city in adj_cities if not visited[city] ]
                if len(adj_cities) == 0:
                    break

                A.append(adj_cities[0])
                visited[adj_cities[0]] = True
                current = adj_cities[0]

        while len(A) < La:
            A.append(0)
        return A

class OperatorV2:
    def __init__(self, g):
        self.g = g

    def _make_G_including_cities_within_one_signal(self, G, A, Lb):
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

    def perform(self, A, recorder):
        N = self.g.N()
        G = self.g.G()
        Lb = self.g.Lb()
        t = self.g.t()
        P = self.g.P()
        new_G = self._make_G_including_cities_within_one_signal(G, A, Lb)

        src = 0
        for dst in t:
            path = [src]
            path = find_path(N, new_G, P, src, dst)
            if not path:
                raise

            logger.debug(f"path: {path}")
            current = path[0]
            path = path[1:]
            for city in path:
                logger.debug(f"current: {current}")
                logger.debug(f"city:    {city}")
                if city in G[current]:
                    recorder.signal(1, A.index(city), 0)
                    recorder.move(city)
                else:
                    index1 = A.index(current)
                    index2 = A.index(city)
                    if index1 < index2:
                        recorder.signal(index2 - index1, index1 + 1, 0)
                        for i in range(index2 - index1):
                            recorder.move(A[index1 + i + 1])
                    else:
                        recorder.signal(index1 - index2, index2, 0)
                        for i in range(index1 - index2):
                            recorder.move(A[index1 - i - 1])
                current = city
            src = dst

clock = Clock()
g = Global()
g.read_input()

Config = namedtuple('Config', ['name', 'A', 'operator', 'recorder'])

configs = [
        Config('1', AFactory.make_A_by_choosing_far_city(g), OperatorV2(g), Recorder(g)),
        Config('2', AFactory.make_A_by_choosing_far_city(g, 1), OperatorV2(g), Recorder(g)),
        Config('3', AFactory.make_A_by_choosing_far_city(g, 2), OperatorV2(g), Recorder(g)),
        ]

best_config = None

for config in configs:
    name = config.name
    A = config.A
    operator = config.operator
    recorder = config.recorder

    operator.perform(A, recorder)

    logger.debug(f"elapsed time:  {clock.get_elapsed_time_millisec():.3f} [ms]")
    logger.debug(f"numof_records: {recorder.get_numof_records()}")
    logger.debug(f"score:         {recorder.get_score()}")

    if not best_config or best_config.recorder.get_score() > recorder.get_score():
        best_config = config
        logger.debug(f"best is updated: {name}")


print(*best_config.A)
best_config.recorder.print()
