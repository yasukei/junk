#include <bits/stdc++.h>

static int K;

#if 0
#define PRINTF printf
#else
#define PRINTF
#endif

static std::unordered_map<int, bool> cache;

void initialize(int k, std::vector<int> v)
{
	K = k;

	for(int i = 0; i < v.size(); i++)
	{
		for(int j = i + 1; j < v.size(); j++)
		{
			int value = v[i] + v[j];
			bool result = value % K == 0;
			cache.insert(std::make_pair(value, result));
		}
	}
}

bool isDivisible(int value)
{
#if 1
	return value % K == 0;
#else
	auto it = cache.find(value);
	return it->second;
#endif
}

bool isNotDivisibleValueForTheSubset(int value, const int* subset, int subsetSize)
{
	PRINTF("subsetSize: [%d]\n", subsetSize);

    for(int i = 0; i < subsetSize; i++)
    {
		PRINTF("\t\t\t\t\tsubset[i]: [%d], value: [%d]\n", subset[i], value);
#if 0
		if((subset[i] + value) % K == 0)
#else
		if(isDivisible(subset[i] + value))
#endif
		{
			return false;
        }
    }
    return true;
}

void makeSubset(const int* motherSet, int motherSetSize, int* subset, int subsetSize, int& maxSubsetSize)
{
	for(int i = motherSetSize - 1; i >= 0; i--)
	{
		if(maxSubsetSize > i + subsetSize)
		{
			break;
		}
		if(!isNotDivisibleValueForTheSubset(motherSet[i], subset, subsetSize))
		{
			continue;
		}

		subset[subsetSize] = motherSet[i];
		if(subsetSize + 1 > maxSubsetSize)
		{
			maxSubsetSize = subsetSize + 1;
			PRINTF("\t\t\t\t\t\t\t\t\t\tmaxSubsetSize: [%d]\n", maxSubsetSize);
		}
		makeSubset(motherSet, i, subset, subsetSize + 1, maxSubsetSize);
	}
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
	std::sort(S.rbegin(), S.rend());
	initialize(k, S);

	std::vector<int> subset;
	subset.resize(n);

    int answer = 0;
   	if(k == 1)
	{
		answer = 1;
	}
	else
	{
		makeSubset(S.data(), S.size(), subset.data(), 0, answer);
	}

    std::cout << answer << std::endl;
    
    return 0;
}

