https://atcoder.jp/contests/hokudai-hitachi2022/tasks/hokudai_hitachi2022_a

[x] high peak of reward function
[x] TimeSlot has current vertex
[x] use heapq instead of priority queue
[x] making plan for multi workers
[ ] Policy / Strategy
	  [x] Max reward (greedy at current time)
      [ ] Reward peak
	  [ ] Prior task hunter
	  [ ] ?
[x] prioritize larger workload worker
[ ] separate graph two or more regions for getting faster getTrajectory()


# A default

# cpp
33067122854 (greedy)
33786554072 (greedy 2 times)
35275399292 (DP. 20s)
35329678021 (DP. 20s)
34514331982 (DP. 100 jobs limited. 0.65s)

## python
33067122854 (greedy)
35275399292 (DP. 1m53s)
35266931893 (DP. len(open jobs) / 2 limited. 37s)
35026467028 (DP. len(open jobs) / 4 limited. 11s)
34376902804 (DP. 100 jobs limited. 8s)

# A max

# cpp
753628794762 (greedy)
809659250397 (greedy 2 times)
805617108785 (DP. 100jobs limited. 19s)

## python
727095045891 (greedy)
764437612328 (greedy, workers sorted in workload order)

# reference
[](https://www.jstage.jst.go.jp/article/isciesci/64/6/64_200/_pdf/-char/ja)

# B
## New features
[ ] Mandatory job
[ ] Output trustee jobs beforehand
[ ] Output workers' schedule and actions
[ ] change workers' schedule
[ ] stochastic transition matrix of weather
[ ] Weather forecast for each 10 seconds such as 1, 11, 21, etc..

## Functionality
[ ] Read Problem B inputs
[ ] Write Problem B outputs
[ ] Proceed mandatory jobs
[ ] consider Weather at initial state
[ ] consider Weather at runtime
[ ] reschedule at runtime
[ ] calculate Problem B score

