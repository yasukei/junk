import collections
import copy
import heapq
import inspect
import logging
import time

#logging.basicConfig(filename='main.log', filemode='w', level=logging.DEBUG, format='%(levelname)s:%(filename)s %(lineno)04d: %(message)s')
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
    def __init__(self, item):
        self.item = item

    def __lt__(self, other):
        return len(self.item) < len(other.item)

    def get_item(self):
        return self.item

class PriorityQueue:
    def __init__(self):
        self.heap = []

    def is_empty(self):
        return len(self.heap) == 0

    def enque(self, item):
        heapq.heappush(self.heap, QueueItem(item))

    def deque(self):
        return heapq.heappop(self.heap).get_item()

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
            return self.cache[(src, dst)]

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
        numof_visited_of_the_city = {}
        for city, adjs in enumerate(G):
            if len(adjs) == 0:
                continue
            numof_visited_of_the_city[city] = 0
        for i, path in enumerate(paths):
            for j, city in enumerate(path):
                if i != 0 and j == 0:
                    continue
                numof_visited_of_the_city[city] += 1
        cities_visited_specific_times = collections.defaultdict(list)
        for city, num in numof_visited_of_the_city.items():
            cities_visited_specific_times[num].append(city)

        #logger.debug('cities_visited_specific_times')
        #for item in sorted(cities_visited_specific_times.items(), key=lambda item: item[0], reverse=True):
        #    logger.debug(f"numof_visited: {item[0]}, cities: {item[1]}")

        self.numof_visited_of_the_city = numof_visited_of_the_city
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
        paths_visited_specific_times = collections.defaultdict(list)
        for path, num in numof_visited_path.items():
            paths_visited_specific_times[num].append(path)

        #logger.debug("paths_visited_specific_times")
        #for item in sorted(paths_visited_specific_times.items(), key=lambda item: item[0], reverse=True):
        #    logger.debug(f"numof_visited: {item[0]}, paths: {item[1]}")

        self.paths_visited_specific_times = paths_visited_specific_times

    def get_numof_visited_of_the_city(self, city):
        return self.numof_visited_of_the_city[city]

    def get_cities_visited_specific_times(self, times):
        return self.cities_visited_specific_times[times]

    def get_paths_visited_specific_times(self, times):
        return self.paths_visited_specific_times[times]

def get_distance(a, b):
    return (a[0] - b[0]) ** 2 + (a[1] - b[1]) ** 2

class Global:
    def read_input(self):
        N, M, T, La, Lb = map(int, input().split())

        u_v = []
        G = [ set() for _ in range(N) ]
        for _ in range(M):
            u, v = map(int, input().split())
            u_v.append((u, v))
            G[u].add(v)
            G[v].add(u)

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

class Recorder:
    RECORD_LIMIT = 10 ** 5

    def __init__(self, g, A):
        self.N = g.N()
        self.La = g.La()
        self.Lb = g.Lb()
        self.A = A
        self.B = [ -1 for _ in range(g.Lb()) ]
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

        for i in range(l):
            self.B[Pb + i] = self.A[Pa + i]

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

    def get_B(self):
        return self.B

    def get_path(self, start_city):
        path = [start_city]
        for record in self.records:
            if 's' in record:
                continue

            _, city = record.split()
            path.append(int(city))
        return path

    def print(self):
        for record in self.records:
            print(record)

    def get_score(self):
        return self.sig_counter

    def get_numof_records(self):
        return self.sig_counter + self.mv_counter

def get_M(G):
    M = 0
    counted = set()
    for city, adjs in enumerate(G):
        for adj in adjs:
            road = (min(city, adj), max(city, adj))
            if road not in counted:
                M += 1
                counted.add(road)
    return M

def print_graph_info(g, G):
    N = g.N()
    M = get_M(G)
    T = g.T()
    La = g.La()
    Lb = g.Lb()
    u_v = g.u_v()
    t = g.t()
    P = g.P()

    print('*** Graph Info begin ***')
    print(N, M, T, La, Lb)

    for u, v in u_v:
        if u in G[v]:
            print(u, v)

    print(' '.join(map(str, t)))
    for city in range(N):
        print(P[city][0], P[city][1])
    print('*** Graph Info end ***')

def remove_city(G, city):
    for adj in G[city]:
        G[adj].remove(city)
    G[city].clear()

def remove_road(G, road):
    if road[1] in G[road[0]]:
        G[road[0]].remove(road[1])
        G[road[1]].remove(road[0])

def update_G_with_removing_no_visited_cities_and_roads(G, paths_stats):
    no_visited_cities = paths_stats.get_cities_visited_specific_times(0)
    no_visited_paths = paths_stats.get_paths_visited_specific_times(0)
    #logger.debug(f"len(no_visited_cities): {len(no_visited_cities)}")
    #logger.debug(f"len(no_visited_paths): {len(no_visited_paths)}")
    for city in no_visited_cities:
        remove_city(G, city)
    for road in no_visited_paths:
        remove_road(G, road)

def create_A_along_most_visited_cities(G, start_city, La, paths_stats):
    def recurse(A, visited):
        current = A[-1]
        adj_cities = sorted(G[current], key=lambda adj_city: paths_stats.get_numof_visited_of_the_city(adj_city), reverse=True)
        for adj_city in adj_cities:
            if visited[adj_city]:
                continue
            A.append(adj_city)
            visited[adj_city] = True
            recurse(A, visited)

    A = [start_city]
    visited = [ False ] * len(G)
    visited[start_city] = True
    recurse(A, visited)
    while len(A) < La:
        A.append(0)

    #logger.debug(f"A: {A}")
    return A

def get_indices_of_the_city_in_A(city, A):
    return [ i for i, _ in enumerate(A) if A[i] == city ]

def create_G_including_cities_within_one_signal(G, A, Lb):
    new_G = copy.deepcopy(G)
    shortcut_path_dict = {}
    for city, adjs in enumerate(G):
        for adj in adjs:
            shortcut_path_dict[(city, adj)] = [city, adj]

            for i in get_indices_of_the_city_in_A(adj, A):
                path = [city, adj]
                for j in range(1, Lb):
                    if i + j >= len(A):
                        break
                    if A[i + j] not in G[ A[i + j - 1] ]:
                        break
                    new_G[city].add(A[i + j])
                    path.append(A[i + j])
                    shortcut_path_dict[(city, A[i + j])] = [*path]

                path = [city, adj]
                for j in range(1, Lb):
                    if i - j < 0:
                        break
                    if A[i - j] not in G[ A[i - j + 1] ]:
                        break
                    new_G[city].add(A[i - j])
                    path.append(A[i - j])
                    shortcut_path_dict[(city, A[i - j])] = [*path]
    return new_G, shortcut_path_dict

    #new_G = copy.deepcopy(G)
    #for city, _ in enumerate(new_G):
    #    if len(new_G[city]) == 0:
    #        continue

    #    for i in get_indices_of_the_city_in_A(city, A):
    #        for j in range(1, Lb + 1):
    #            if i + j < len(A) and A[i + j] in G[ A[i + j - 1] ]:
    #                new_G[city].add(A[i + j])
    #            else:
    #                break
    #        for j in range(1, Lb + 1):
    #            if i - j >= 0 and A[i - j] in G[ A[i - j + 1] ]:
    #                new_G[city].add(A[i - j])
    #            else:
    #                break
    #return new_G

def create_operation_records(g, A, G):
    new_G, shortcut_path_dict = create_G_including_cities_within_one_signal(G, A, g.Lb())
    path_finder = ShortestPathFinder(new_G)
    paths = find_all_paths(path_finder, 0, g.t())

    #for city, adjs in enumerate(new_G):
    #    logger.debug(f"new_G[{city}]: {adjs}")

    recorder = Recorder(g, A)
    for xth, path in enumerate(paths):
        #logger.debug(f"xth: {xth}, path: {path}")
        current = path[0]
        path = path[1:]
        for dst_city in path:
            if dst_city in G[current]:
                #logger.debug(f"dst_city: {dst_city} is adjacent")
                if not dst_city in recorder.get_B():
                    recorder.signal(1, A.index(dst_city), 0)
                recorder.move(dst_city)
            else:
                #logger.debug(f"dst_city: {dst_city} is NOT adjacent")
                shortcut_path = shortcut_path_dict[(current, dst_city)][1:]
                dst_city_indices = get_indices_of_the_city_in_A(dst_city, A)

                found = False
                for dst_city_index in dst_city_indices:
                    if dst_city_index+len(shortcut_path) < len(A) and A[dst_city_index:dst_city_index+len(shortcut_path)] == list(reversed(shortcut_path)):
                        recorder.signal(len(shortcut_path), dst_city_index, 0)
                        for elem in shortcut_path:
                            recorder.move(elem)
                        found = True
                    elif dst_city_index+1-len(shortcut_path) >= 0 and A[dst_city_index+1-len(shortcut_path):dst_city_index+1] == shortcut_path:
                        recorder.signal(len(shortcut_path), dst_city_index+1-len(shortcut_path), 0)
                        for elem in shortcut_path:
                            recorder.move(elem)
                        found = True
                    if found:
                        break

                if not found:
                    raise Exception('Operation not found.')

            current = dst_city

    return recorder

def main():
    clock = Clock()
    g = Global()
    g.read_input()

    START_CITY = 0
    EXEC_TIME_LIMIT_MILLISEC = 3000
    G = g.G()
    t = g.t()
    La = g.La()

    best_A = None
    best_recorder = None

    path_finder = ShortestPathFinder(G)
    paths = find_all_paths(path_finder, START_CITY, t)

    iteration = 5
    for i in range(iteration):
        logger.info(f"iteration {i}")
        paths_stats = PathsStats(G, paths)
        update_G_with_removing_no_visited_cities_and_roads(G, paths_stats)

        A = create_A_along_most_visited_cities(G, START_CITY, La, paths_stats)

        recorder = create_operation_records(g, A, G)
        logger.info(f"score:        {recorder.get_score()}")
        logger.info(f"elapsed time: {clock.get_elapsed_time_millisec()}")

        if not best_A or best_recorder.get_score() > recorder.get_score():
            best_A = A
            best_recorder = recorder

        if i == 0:
            first_exec_time = clock.get_elapsed_time_millisec()
        if clock.get_elapsed_time_millisec() > EXEC_TIME_LIMIT_MILLISEC - first_exec_time:
            break

        paths = [ recorder.get_path(START_CITY) ]

    print(*best_A)
    best_recorder.print()

if __name__ == '__main__':
    main()
