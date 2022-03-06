#include <bits/stdc++.h>

#define DEBUG
#ifdef DEBUG
#define PRINTF printf
#else // DEBUG
#define PRINTF
#endif // DEBUG

bool isNotDivisibleSubset(const std::vector<int>& subset, int k)
{
    for(auto it1 = subset.begin(); it1 != subset.end(); it1++)
    {
        for(auto it2 = std::next(it1); it2 != subset.end(); it2++)
        {
            if((*it1 + *it2) % k == 0)
            {
                return false;
            }
        }
    }
    return true;
}

bool makeSubset(const std::vector<int>& motherSet, int motherSetSize, int depth, std::vector<int>& subset, std::vector<std::vector<int>>& subsetQueue, int k)
{
	bool found = false;

	for(int i = motherSetSize - 1; !found && i >= 0; i--)
	{
		subset.push_back(motherSet[i]);

		if(depth == 1)
		{
			found = isNotDivisibleSubset(subset, k);
			if(found)
			{
				subsetQueue.push_back(subset);
			}
		}
		else
		{
			found = makeSubset(motherSet, i, depth - 1, subset, subsetQueue, k);
		}
		subset.pop_back();
	}
	return found;
}

void printSubset(const std::vector<int>& subset)
{
	for(int i = 0; i < subset.size(); i++)
	{
		PRINTF("%d ", subset[i]);
	}
	PRINTF("\n");
}

void printSubsetQueue(const std::vector<std::vector<int>>& subsetQueue)
{
	PRINTF("[subsetQueue]\n");
	PRINTF("size: [%lu]\n", subsetQueue.size());
	for(int i = 0; i < subsetQueue.size(); i++)
	{
		printSubset(subsetQueue[i]);
	}
	PRINTF("\n");
}

int binarySearch(int searchLeft, int searchRight, const std::vector<int>& motherSet, std::vector<int>& subset, std::vector<std::vector<int>>& subsetQueue, int k)
{
	int searchIndex = searchLeft + (searchRight - searchLeft) / 2;
	int result1 = 0;

	PRINTF("left, index, right: %d, %d, %d\n", searchLeft, searchIndex, searchRight);

	subsetQueue.clear();
	bool found = makeSubset(motherSet, motherSet.size(), searchIndex, subset, subsetQueue, k);
	//printSubsetQueue(subsetQueue);

	if(found)
	{
		result1 = subsetQueue[0].size();
	}

	int result2 = 0;
	if(searchLeft < searchRight)
	{
		if(found)
		{
			result2 = binarySearch(searchIndex + 1, searchRight, motherSet, subset, subsetQueue, k);
		}
		else
		{
			result2 = binarySearch(searchLeft, searchIndex - 1, motherSet, subset, subsetQueue, k);
		}
	}

	return result1 > result2 ? result1 : result2;
}

int main(void)
{
    int n, k;
    std::vector<int> S;
    
    std::cin >> n >> k;
	S.resize(n);
    for(int i = 0; i < n; i++)
    {
        int temp;
        std::cin >> S[i];
    }

	std::vector<int> subset;
	std::vector<std::vector<int>> subsetQueue;

    int answer;
   	if(k == 1)
	{
		answer = 1;
	}
	else
	{
		answer = binarySearch(2, S.size(), S, subset, subsetQueue, k);
	}

    std::cout << answer << std::endl;
    
    return 0;
}

