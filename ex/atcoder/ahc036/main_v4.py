from collections import namedtuple
import copy
import heapq
import inspect
import logging
import time

#logging.basicConfig(filename='main.log', filemode='w', level=logging.DEBUG)
logger = logging.getLogger(__name__)

def Location():
    frame = inspect.currentframe().f_back
    return frame.f_code.co_name, frame.f_lineno

class Clock:
    def __init__(self):
        self.start_time = time.perf_counter()

    def get_elapsed_time_millisec(self):
        return (time.perf_counter() - self.start_time) * 1000

    def debug_print(self, msg=''):
        logger.debug(f"elapsed time: {self.get_elapsed_time_millisec():.3f} [ms], {msg}")

class QueueItem:
    def __init__(self, path):
        self.path = path

    def __lt__(self, other):
        return len(self.path) < len(other.path)

    def get_path(self):
        return self.path

class PriorityQueue:
    def __init__(self):
        self.heap = []

    def is_empty(self):
        return len(self.heap) == 0

    def enque(self, path):
        heapq.heappush(self.heap, QueueItem(path))

    def deque(self):
        return heapq.heappop(self.heap).get_path()

    def get_head(self):
        return self.heap[0].get_path()

class NearestPathFinder:
    def __init__(self, N, G, P):
        self.N = N
        self.G = G
        self.P = P

    def find_path(self, src, dst):
        def _recurse(src, dst, path, visited):
            current = path[-1]
            if current == dst:
                return True

            adj_cities = sorted(self.G[current], key=lambda adj_city: get_distance(self.P[adj_city], self.P[dst]))
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
        visited = [ False ] * self.N
        visited[src] = True
        result = _recurse(src, dst, path, visited)
        if not result:
            return None

        return path

class ShortestPathFinder:
    def __init__(self, G):
        self.G = G
        self.N = len(G)

        self.priority_queues = [ None for _ in range(self.N) ]
        self.visited = [ None for _ in range(self.N) ]

        self.cache = {}

    def _init(self, src):
        self.priority_queues[src] = PriorityQueue()
        self.priority_queues[src].enque([src])
        self.visited[src] = [ False for _ in range(self.N) ]
        self.visited[src][src] = True

    def find_path(self, src, dst):
        if (src, dst) in self.cache:
            #logger.debug(f"cache used, src: {src}, dst: {dst}")
            return self.cache[(src, dst)]
        #if (dst, src) in self.cache:
        #    return list(reversed(self.cache[(dst, src)]))

        if not self.priority_queues[src]:
            self._init(src)

        found = False
        queue = self.priority_queues[src]
        while not queue.is_empty():
            path = queue.deque()

            for city in self.G[ path[-1] ]:
                if self.visited[src][city]:
                    continue

                self.visited[src][city] = True
                new_path = path + [city]
                queue.enque(new_path)
                self.cache[(src, city)] = new_path
                if city == dst:
                    found = True
            if found:
                return self.cache[(src, dst)]
        return None

def find_all_paths(path_finder, start, t):
    paths = []
    src = start
    for dst in t:
        path = path_finder.find_path(src, dst)
        if not path:
            return None
        paths.append(path)
        src = dst
    return paths

def get_tuple_of_city1_city2_by_ascending_order(city1, city2):
    if city1 < city2:
        return (city1, city2)
    else:
        return (city2, city1)

class PathsStats:
    def __init__(self, G, paths):
        self._analyze_cities(G, paths)
        self._analyze_paths(G, paths)

    def _analyze_cities(self, G, paths):
        numof_visited_city = {}
        for city, adjs in enumerate(G):
            if len(adjs) == 0:
                continue
            numof_visited_city[city] = 0
        for i, path in enumerate(paths):
            for j, city in enumerate(path):
                if i != 0 and j == 0:
                    continue
                numof_visited_city[city] += 1
        cities_visited_specific_times = {}
        for city, num in numof_visited_city.items():
            if num in cities_visited_specific_times:
                cities_visited_specific_times[num].append(city)
            else:
                cities_visited_specific_times[num] = [city]

        logger.debug('cities_visited_specific_times')
        for item in sorted(cities_visited_specific_times.items(), key=lambda item: item[0], reverse=True):
            logger.debug(f"numof_visited: {item[0]}, cities: {item[1]}")

        self.cities_visited_specific_times = cities_visited_specific_times

    def _analyze_paths(self, G, paths):
        numof_visited_path = {}
        for city, adjs in enumerate(G):
            if len(adjs) == 0:
                continue
            for adj in adjs:
                tpl = get_tuple_of_city1_city2_by_ascending_order(city, adj)
                numof_visited_path[tpl] = 0
        for path in paths:
            for i, city in enumerate(path):
                if i == 0:
                    continue
                tpl = get_tuple_of_city1_city2_by_ascending_order(path[i-1], path[i])
                numof_visited_path[tpl] += 1
        paths_visited_specific_times = {}
        #G_with_freq = [ [] for _ in range(len(G)) ]
        for path, num in numof_visited_path.items():
            if num in paths_visited_specific_times:
                paths_visited_specific_times[num].append(path)
            else:
                paths_visited_specific_times[num] = [path]
            #G_with_freq[path[0]].append((path[1], num))
            #G_with_freq[path[1]].append((path[0], num))

        logger.debug("paths_visited_specific_times")
        for item in sorted(paths_visited_specific_times.items(), key=lambda item: item[0], reverse=True):
            logger.debug(f"numof_visited: {item[0]}, paths: {item[1]}")

        #for row in G_with_freq:
        #    row.sort(key=lambda elem: elem[1], reverse=True)
        #logger.debug("G_with_freq")
        #for i, row in enumerate(G_with_freq):
        #    logger.debug(f"G_with_freq[{i}]: {row}")

        self.paths_visited_specific_times = paths_visited_specific_times

    def get_cities_visited_specific_times(self, times):
        if times in self.cities_visited_specific_times:
            return self.cities_visited_specific_times[times]
        else:
            return []

    def get_paths_visited_specific_times(self, times):
        if times in self.paths_visited_specific_times:
            return self.paths_visited_specific_times[times]
        else:
            return []

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

class Global:
    def read_input(self):
        N, M, T, La, Lb = map(int, input().split())

        u_v = []
        G = [ [] for _ in range(N) ]
        for _ in range(M):
            u, v = map(int, input().split())
            u_v.append((u, v))
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
        self._u_v = u_v
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
    def u_v(self):
        return self._u_v
    def G(self):
        return self._G
    def t(self):
        return self._t
    def P(self):
        return self._P

class Coordinate:
    def __init__(self, g):
        pass

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

class Graph:
    def __init__(self, g):
        self.g = g
        self.M = g.M()
        self.G = copy.deepcopy(g.G())

    def getG(self):
        return self.G

    def copy(self):
        return copy.deepcopy(self)

    def remove_city(self, city):
        for adj in self.G[city]:
            self.G[adj].remove(city)
            self.M -= 1
        self.G[city].clear()

    def remove_road(self, road):
        if road[1] in self.G[road[0]]:
            self.G[road[0]].remove(road[1])
            self.G[road[1]].remove(road[0])
            self.M -= 1

    def get_adjacent_cities(self, city):
        return self.G[city]

    def print_graph_info(self):
        G = self.G
        M = self.M

        N = self.g.N()
        T = self.g.T()
        La = self.g.La()
        Lb = self.g.Lb()
        u_v = self.g.u_v()
        t = self.g.t()
        P = self.g.P()

        print('*** Graph Info begin ***')
        print(N, M, T, La, Lb)

        for u, v in u_v:
            if u in G[v]:
                print(u, v)

        print(' '.join(map(str, t)))
        for city in range(N):
            print(P[city][0], P[city][1])
        print('*** Graph Info end ***')

class AFactory:
    @staticmethod
    def make_simple_A(g):
        N = g.N()
        La = g.La()

        A = [ i for i in range(N) ]
        while len(A) < La:
            A.append(0)
        return A

    @staticmethod
    def make_A_frequency_prioritized(g):
        N = g.N()
        G = g.G()
        t = g.t()
        P = g.P()
        La = g.La()
        Lb = g.Lb()

        graph = Graph(g)

        for i in range(5):
            path_finder = ShortestPathFinder(graph.getG())
            clock.debug_print(Location())
            paths = find_all_paths(path_finder, 0, t)
            clock.debug_print(Location())
            paths_stats = PathsStats(graph.getG(), paths)

            not_visited_cities = paths_stats.get_cities_visited_specific_times(0)
            not_visited_paths = paths_stats.get_paths_visited_specific_times(0)
            logger.debug(f"len(not_visited_cities): {len(not_visited_cities)}")
            logger.debug(f"not_visited_cities:      {not_visited_cities}")
            logger.debug(f"len(not_visited_paths): {len(not_visited_paths)}")
            for city in not_visited_cities:
                graph.remove_city(city)
            for road in not_visited_paths:
                graph.remove_road(road)
        #graph.print_graph_info()
        
        for i in range(0):
            removing_cities = paths_stats.get_cities_visited_specific_times(i)
            removing_cities = [ city for city in removing_cities if city not in t ]
            removed_cities = []
            not_removed_cities = []
            #logger.debug(f"removing_cities: {removing_cities}")

            for removing_city in removing_cities:
                temp_graph = graph.copy()
                temp_graph.remove_city(removing_city)
                path_finder = ShortestPathFinder(temp_graph.getG())

                adj_cities = graph.get_adjacent_cities(removing_city)
                for j, _ in enumerate(adj_cities):
                    if j == 0:
                        continue
                    path = path_finder.find_path(adj_cities[0], adj_cities[j])
                    if not path:
                        not_removed_cities.append(removing_city)
                        break
                else:
                    # all adj_cities can be removed
                    graph = temp_graph
                    removed_cities.append(removing_city)
            #logger.debug(f"removed_city: {removed_cities}")
            #logger.debug(f"NOT removed_city: {not_removed_cities}")

        G = graph.getG()

        def recurse(A, visited):
            current = A[-1]
            oldest = current
            for i in range(1, Lb):
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

        #not_visited_cities = [ i for i, _ in enumerate(visited) if not visited[i] ]
        #logger.debug(f"len(not_visited_cities): {len(not_visited_cities)}")
        #logger.debug(f"not_visited_cities:      {not_visited_cities}")

        return A, G

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
            if len(new_G[u]) == 0:
                continue
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

    def perform(self, A, G, recorder):
        N = self.g.N()
        #G = self.g.G()
        Lb = self.g.Lb()
        t = self.g.t()
        P = self.g.P()
        new_G = self._make_G_including_cities_within_one_signal(G, A, Lb)

        #path_finder = NearestPathFinder(N, new_G, P)
        path_finder = ShortestPathFinder(new_G)
        src = 0
        for dst in t:
            #path = find_path(N, new_G, P, src, dst)
            path = path_finder.find_path(src, dst)
            if not path:
                raise

            logger.debug(f"path:  {path}")
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
        Config('0', AFactory.make_A_frequency_prioritized,                  OperatorV2(g), Recorder(g)),
        #Config('1', AFactory.make_A_by_choosing_far_city(g, 0), OperatorV2(g), Recorder(g)),
        #Config('2', AFactory.make_A_by_choosing_far_city(g, 1), OperatorV2(g), Recorder(g)),
        #Config('3', AFactory.make_A_by_choosing_far_city(g, 2), OperatorV2(g), Recorder(g)),
        ]

best_A = None
best_recorder = None

for config in configs:
    name = config.name
    A, G = config.A(g)
    operator = config.operator
    recorder = config.recorder

    operator.perform(A, G, recorder)

    clock.debug_print(Location())
    logger.debug(f"numof_records: {recorder.get_numof_records()}")
    logger.debug(f"score:         {recorder.get_score()}")

    if not best_A or best_recorder.get_score() > recorder.get_score():
        best_A = A
        best_recorder = recorder
        logger.debug(f"best is updated: {name}")


print(*best_A)
best_recorder.print()
