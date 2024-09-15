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
        for i in range(N):
            x, y = map(int, input().split())
            P.append((x, y))
            #logger.debug(f"P[{i}]: ({x}, {y})")

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

class Coordinate:
    def __init__(self, g):
        P = g.P()
        self.P = g.P()
        all_x = sorted([ x for x, y in P ])
        all_y = sorted([ y for x, y in P ])
        self.min_x = all_x[0]
        self.mid_x = all_x[int(len(all_x)/2)]
        self.max_x = all_x[-1]
        self.min_y = all_y[0]
        self.mid_y = all_y[int(len(all_y)/2)]
        self.max_y = all_y[-1]

        self.city_coords_near_top_left = sorted(P, key=lambda elem: get_distance(elem, (self.min_x, self.min_y)))
        self.city_coords_near_top_mid = sorted(P, key=lambda elem: get_distance(elem, (self.mid_x, self.min_y)))
        self.city_coords_near_top_right = sorted(P, key=lambda elem: get_distance(elem, (self.max_x, self.min_y)))
        self.city_coords_near_mid_left = sorted(P, key=lambda elem: get_distance(elem, (self.min_x, self.mid_y)))
        self.city_coords_near_mid_mid = sorted(P, key=lambda elem: get_distance(elem, (self.mid_x, self.mid_y)))
        self.city_coords_near_mid_right = sorted(P, key=lambda elem: get_distance(elem, (self.max_x, self.mid_y)))
        self.city_coords_near_bot_left = sorted(P, key=lambda elem: get_distance(elem, (self.min_x, self.max_y)))
        self.city_coords_near_bot_mid = sorted(P, key=lambda elem: get_distance(elem, (self.mid_x, self.max_y)))
        self.city_coords_near_bot_right = sorted(P, key=lambda elem: get_distance(elem, (self.max_x, self.max_y)))

        city_by_coord = {}
        for city, coord in enumerate(P):
            city_by_coord[coord] = city
        #logger.debug(f"top left city:  {city_by_coord[self.city_coords_near_top_left[0]]}, {self.city_coords_near_top_left[0]}")
        #logger.debug(f"top mid city:   {city_by_coord[self.city_coords_near_top_mid[0]]}, {self.city_coords_near_top_mid[0]}")
        #logger.debug(f"top right city: {city_by_coord[self.city_coords_near_top_right[0]]}, {self.city_coords_near_top_right[0]}")
        #logger.debug(f"mid left city:  {city_by_coord[self.city_coords_near_mid_left[0]]}, {self.city_coords_near_mid_left[0]}")
        #logger.debug(f"mid mid city:   {city_by_coord[self.city_coords_near_mid_mid[0]]}, {self.city_coords_near_mid_mid[0]}")
        #logger.debug(f"mid right city: {city_by_coord[self.city_coords_near_mid_right[0]]}, {self.city_coords_near_mid_right[0]}")
        #logger.debug(f"bot left city:  {city_by_coord[self.city_coords_near_bot_left[0]]}, {self.city_coords_near_bot_left[0]}")
        #logger.debug(f"bot mid city:   {city_by_coord[self.city_coords_near_bot_mid[0]]}, {self.city_coords_near_bot_mid[0]}")
        #logger.debug(f"bot right city: {city_by_coord[self.city_coords_near_bot_right[0]]}, {self.city_coords_near_bot_right[0]}")

        self.cities_near_top_left = [ city_by_coord[coord] for coord in self.city_coords_near_top_left ]
        self.cities_near_top_mid = [ city_by_coord[coord] for coord in self.city_coords_near_top_mid ]
        self.cities_near_top_right = [ city_by_coord[coord] for coord in self.city_coords_near_top_right ]
        self.cities_near_mid_left = [ city_by_coord[coord] for coord in self.city_coords_near_mid_left ]
        self.cities_near_mid_mid = [ city_by_coord[coord] for coord in self.city_coords_near_mid_mid ]
        self.cities_near_mid_right = [ city_by_coord[coord] for coord in self.city_coords_near_mid_right ]
        self.cities_near_bot_left = [ city_by_coord[coord] for coord in self.city_coords_near_bot_left ]
        self.cities_near_bot_mid = [ city_by_coord[coord] for coord in self.city_coords_near_bot_mid ]
        self.cities_near_bot_right = [ city_by_coord[coord] for coord in self.city_coords_near_bot_right ]

    def get_cities_near_top_left(self):
        return self.cities_near_top_left
    def get_cities_near_top_mid(self):
        return self.cities_near_top_mid
    def get_cities_near_top_right(self):
        return self.cities_near_top_right

    def get_cities_near_mid_left(self):
        return self.cities_near_mid_left
    def get_cities_near_mid_mid(self):
        return self.cities_near_mid_mid
    def get_cities_near_mid_right(self):
        return self.cities_near_mid_right

    def get_cities_near_bot_left(self):
        return self.cities_near_bot_left
    def get_cities_near_bot_mid(self):
        return self.cities_near_bot_mid
    def get_cities_near_bot_right(self):
        return self.cities_near_bot_right

    def get_coordinate(self, city):
        return self.P[city]

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

    def move_by_path(self, path):
        for dst in path:
            self.move(dst)

    def get_B(self):
        return self.B

    def get_path(self, start_city):
        path = [start_city]
        for record in self.records:
            if 's' in record:
                pass
            else:
                _, city = record.split()
                path.append(int(city))

        return path

    def print(self):
        for record in self.records:
            print(record)

    def get_score(self):
        return self.sig_counter

    def get_numof_moves(self):
        return self.mv_counter

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

def get_first_city_existing_in_G(cities, G):
    for city in cities:
        if len(G[city]) != 0:
            return city
    return None

def create_A_along_most_visited_cities(G, start_city, La, paths_stats, coordinate):
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

    path_finder = ShortestPathFinder(G)
    top_left = get_first_city_existing_in_G(coordinate.get_cities_near_top_left(), G)
    top_mid = get_first_city_existing_in_G(coordinate.get_cities_near_top_mid(), G)
    top_right = get_first_city_existing_in_G(coordinate.get_cities_near_top_right(), G)
    mid_left = get_first_city_existing_in_G(coordinate.get_cities_near_mid_left(), G)
    mid_mid = get_first_city_existing_in_G(coordinate.get_cities_near_mid_mid(), G)
    mid_right = get_first_city_existing_in_G(coordinate.get_cities_near_mid_right(), G)
    bot_left = get_first_city_existing_in_G(coordinate.get_cities_near_bot_left(), G)
    bot_mid = get_first_city_existing_in_G(coordinate.get_cities_near_bot_mid(), G)
    bot_right = get_first_city_existing_in_G(coordinate.get_cities_near_bot_right(), G)
    routes = [
            # X
            (top_left, bot_right),
            (top_right, bot_left),

            # angled square
            (top_mid, mid_right),
            (mid_right, bot_mid),
            (bot_mid, mid_left),
            (mid_left, top_mid),

            # +
            (top_mid, bot_mid),
            (mid_left, mid_right),

            # outer square
            (top_left, bot_left),
            (bot_left, bot_right),
            (bot_right, top_right),
            (top_right, top_left),
            ]
    for route in routes:
        #logger.debug(f"add route: {coordinate.get_coordinate(route[0])}, {coordinate.get_coordinate(route[1])}")
        path = path_finder.find_path(route[0], route[1])
        for city in path:
            if len(A) == La:
                break
            A.append(city)
        if len(A) == La:
            break

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

def path_is_in_B(path, B):
    length = len(path)
    for i in range(len(B) - length + 1):
        if B[i:i+length] == path:
            return True

    r_path = list(reversed(path))
    for i in range(len(B) - length + 1):
        if B[i:i+length] == r_path:
            return True

    return False



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
                #logger.debug(f"  dst_city: {dst_city} is adjacent")
                if not dst_city in recorder.get_B():
                    #recorder.signal(1, A.index(dst_city), 0)
                    recorder.signal(1, A.index(dst_city), g.Lb()-1)
                recorder.move(dst_city)
            else:
                #logger.debug(f"  dst_city: {dst_city} is NOT adjacent")
                shortcut_path = shortcut_path_dict[(current, dst_city)][1:]
                dst_city_indices = get_indices_of_the_city_in_A(dst_city, A)
                #logger.debug(f"  shortcut_path:    {shortcut_path}")
                #logger.debug(f"  dst_city_indices: {dst_city_indices}")

                if path_is_in_B(shortcut_path, recorder.get_B()):
                    recorder.move_by_path(shortcut_path)
                else:
                    found = False
                    for dst_city_index in dst_city_indices:
                        if dst_city_index+len(shortcut_path) <= len(A) and A[dst_city_index:dst_city_index+len(shortcut_path)] == list(reversed(shortcut_path)):
                            recorder.signal(len(shortcut_path), dst_city_index, 0)
                            recorder.move_by_path(shortcut_path)
                            found = True
                        elif dst_city_index+1-len(shortcut_path) >= 0 and A[dst_city_index+1-len(shortcut_path):dst_city_index+1] == shortcut_path:
                            recorder.signal(len(shortcut_path), dst_city_index+1-len(shortcut_path), 0)
                            recorder.move_by_path(shortcut_path)
                            found = True
                        if found:
                            break

                    if not found:
                        #print(*A)
                        #recorder.print()
                        raise Exception('Operation not found.')

            current = dst_city

    return recorder

def main():
    clock = Clock()
    g = Global()
    g.read_input()
    coordinate = Coordinate(g)

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

        A = create_A_along_most_visited_cities(G, START_CITY, La, paths_stats, coordinate)
        recorder = create_operation_records(g, A, G)
        logger.info(f"score:         {recorder.get_score()}")
        logger.info(f"elapsed time:  {clock.get_elapsed_time_millisec()}")

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
