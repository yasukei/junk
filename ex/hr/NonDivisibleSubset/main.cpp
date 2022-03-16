#include <bits/stdc++.h>

#if 0
#define PRINTF printf
#else
#define PRINTF
#endif

static std::bitset<2*1000*1000*1000> isDivisible;
static int K;
static std::vector<int> maxSubset;

static void solve(int k, std::vector<int>& v)
{
	int counter;
	std::vector<std::vector<int>> temp;
	temp.resize(v.size());

	for(int i = 0; i < v.size(); ++i)
	{
		counter = 0;
		for(int j = 0; j < v.size(); ++j)
		{
			if(i == j)
			{
				continue;
			}

			if((v[i] + v[j]) % k != 0)
			{
				counter++;
			}
		}
		temp[counter].push_back(v[i]);
	}

#if 1
	for(int i = 0; i < temp.size(); ++i)
	{
		printf("temp[%d]:", i);
		for(int j = 0; j < temp[i].size(); ++j)
		{
			printf(" %d", temp[i][j]);
		}
		printf("\n");
	}
#endif

	v.clear();
	for(int i = 0; i < temp.size(); ++i)
	//for(int i = temp.size() - 1 ; i >= 0; --i)
	{
		if(temp[i].empty())
		{
			continue;
		}

		for(int j = 0; j < temp[i].size(); j++)
		{
			v.push_back(temp[i][j]);
		}
	}
}

static void initialize(int k, const std::vector<int>& v)
{
	K = k;

	for(int i = 0; i < v.size(); ++i)
	{
		for(int j = i + 1; j < v.size(); ++j)
		{
			int value = v[i] + v[j];
			if(value % K == 0)
			{
				isDivisible.set(value);
			}
		}
	}
}

static bool __isDivisible(int value)
{
#if 0
	return value % K == 0;
#else
	return isDivisible[value];
#endif
}

static bool isNotDivisibleValueForTheSubset(int value, const int* subset, int subsetSize)
{
	PRINTF("subsetSize: [%d]\n", subsetSize);

    for(int i = 0; i < subsetSize; ++i)
    {
		PRINTF("\t\t\t\t\tsubset[i]: [%d], value: [%d]\n", subset[i], value);

		if(__isDivisible(subset[i] + value))
		{
			return false;
        }
    }
    return true;
}

static void makeSubset(const int* motherSet, int motherSetSize, int* subset, int subsetSize, int& maxSubsetSize)
{
	for(int i = motherSetSize - 1; i >= 0; --i)
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
			maxSubset.clear();
			for(int i = 0; i < subsetSize + 1; i++)
			{
				maxSubset.push_back(subset[i]);
			}
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
        std::cin >> S[i];
    }
	std::sort(S.rbegin(), S.rend());
	initialize(k, S);
	//solve(k, S);

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
	for(int i = 0; i < maxSubset.size(); ++i)
	{
		printf("%d\n", maxSubset[i]);
	}
    
    return 0;
}

