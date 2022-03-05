#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <list>
#include <unordered_map>
#include <tuple>
#include <algorithm>
#include <functional>

// A book of Cracking the Coding Interview

// Problem 1.1
bool __isUnique(std::string s)
{
	if(s.size() <= 1)
	{
		return true;
	}

	std::sort(s.begin(), s.end());
	char prev = s[0];
	for(int i = 1; i < s.size(); i++)
	{
		if(prev == s[i])
		{
			return false;
		}
		prev = s[i];
	}
	return true;
}

// Problem 1.2
void __reverse(char* str)
{
	size_t len = strlen(str);

	for(int i = 0; i < len / 2; i++)
	{
		char temp = str[i];
		str[i] = str[len - 1 - i];
		str[len - 1 - i] = temp;
	}
}

// Problem 1.3
bool __isAnagram(std::string s1, std::string s2)
{
	std::sort(s1.begin(), s1.end());
	std::sort(s2.begin(), s2.end());

	return s1 == s2;
}

// Proglem 1.4
void __replaceSpace(char* s, int len)
{
	int numofSpaces = 0;

	for(int i = 0; i < len; i++)
	{
		if(s[i] == ' ')
		{
			numofSpaces++;
		}
	}
	s[len + 2*numofSpaces] = '\0';

	for(int i = len - 1; i >= 0; i--)
	{
		if(s[i] == ' ')
		{
			s[i + 2*numofSpaces]     = '0';
			s[i + 2*numofSpaces - 1] = '2';
			s[i + 2*numofSpaces - 2] = '%';
			numofSpaces--;
		}
		else
		{
			s[i + 2*numofSpaces] = s[i];
		}
	}
}

// Problem 1.5
std::string __compressString(std::string org)
{
	std::string compressed = "";
	char current;
	int count;

	current = org[0];
	count = 1;
	for(int i = 1; i < org.size(); i++)
	{
		if(current != org[i])
		{
			compressed.push_back(current);
			compressed.push_back(std::to_string(count)[0]);
			current = org[i];
			count = 1;
		}
		else
		{
			count++;
		}
	}
	compressed.push_back(current);
	compressed.push_back(std::to_string(count)[0]);

	if(compressed.size() < org.size())
	{
		return compressed;
	}
	return org;
}

// Problem 1.6
void __rotate90(std::vector<std::vector<int> >& m)
{
	int temp;

	for(auto i = 0; i < m.size() / 2; i++)
	{
		for(auto j = i; j < m.size() - 1 - i; j++)
		{
			temp = m[i][j];
			m[i][j] = m[j][m.size() - 1 - i];
			m[j][m.size() - 1 - i] = m[m.size() - 1 - i][m.size() - 1 - j];
			m[m.size() - 1 - i][m.size() - 1 - j] = m[m.size() - 1 - j][i];
			m[m.size() - 1 - j][i] = temp;
		}
	}
}

void __printMatrix(std::vector<std::vector<int> > m)
{
	for(int i = 0; i < m.size(); i++)
	{
		for(int j = 0; j < m[0].size(); j++)
		{
			printf(" %2d", m[i][j]);
		}
		printf("\n");
	}
}

// Proglem 1.7
void __fillZero(std::vector<std::vector<int> >& m, int row, int col)
{
	for(int i = 0; i < m.size(); i++)
	{
		m[i][col] = 0;
	}
	for(int i = 0; i < m[row].size(); i++)
	{
		m[row][i] = 0;
	}
}

void __fillZero(std::vector<std::vector<int> >& m)
{
	std::vector<std::tuple<int, int> > zeroPos;

	for(int i = 0; i < m.size(); i++)
	{
		for(int j = 0; j < m[i].size(); j++)
		{
			if(m[i][j] == 0)
			{
				zeroPos.push_back(std::make_tuple(i, j));
			}
		}
	}

	for(int i = 0; i < zeroPos.size(); i++)
	{
		__fillZero(m, std::get<0>(zeroPos[i]), std::get<1>(zeroPos[i]));
	}
}

// Problem 1.8
bool __isRotatedString(std::string s1, std::string s2)
{
	std::string s = s1 + s1.substr(0, s1.size() - 1);

	auto pos = s.find(s2);
	return pos != std::string::npos;
}

// Problem 2.1
void __removeDuplicated(std::list<int>& ls)
{
	ls.sort();
	auto useless = std::unique(ls.begin(), ls.end());
	ls.erase(useless, ls.end());
}

void __printList(const std::list<int>& ls)
{
	for(auto l : ls)
	{
		printf("%d, ", l);
	}
	printf("\n");
}

// Problem 2.2
void __printXFromTail(std::list<int> ls, int k)
{
	int fromHead = ls.size() - k;

	int i = 0;
	for(auto l : ls)
	{
		if(i == fromHead)
		{
			printf("%d\n", l);
			break;
		}
		i++;
	}
}

// Problem 2.4
void __sortLessThanX(std::list<int>& ls, int x)
{
	auto it = ls.begin();
	while(it != ls.end())
	{
		auto next = std::next(it);
		if(*it < x)
		{
			ls.erase(it);
			ls.push_front(*it);
		}
		it = next;
	}
}

// Problem 2.5
std::list<int> __addByList(std::list<int> ls1, std::list<int> ls2)
{
	std::list<int> answer;

	auto it1 = ls1.begin();
	auto it2 = ls2.begin();
	int carryUp = 0;
	while(it1 != ls1.end() || it2 != ls2.end())
	{
		int val1 = (it1 != ls1.end() ? *it1 : 0);
		int val2 = (it2 != ls2.end() ? *it2 : 0);
		int sum = val1 + val2 + carryUp;
		if(sum >= 10)
		{
			carryUp = 1;
			sum = sum % 10;
		}
		else
		{
			carryUp = 0;
		}
		answer.push_back(sum);
		it1 = (it1 != ls1.end() ? std::next(it1) : it1);
		it2 = (it2 != ls2.end() ? std::next(it2) : it2);
	}
	if(carryUp > 0)
	{
		answer.push_back(carryUp);
	}

	return answer;
}

// Problem 3.3
class SetOfStacks
{
public:
	void push(int value);
	int pop(void);
	int popAt(int index);

private:
	std::unordered_map<uint32_t, std::list<int>> setofStacks;
	uint32_t currentIndex;

#define STACK_MAXSIZE 32
};

void SetOfStacks::push(int value)
{
	if(setofStacks[currentIndex].size() == STACK_MAXSIZE)
	{
		currentIndex++;
	}

	setofStacks[currentIndex].push_back(value);
}

int SetOfStacks::pop(void)
{
	if(setofStacks[currentIndex].size() == 0)
	{
		return -1;
	}

	int ret = setofStacks[currentIndex].back();
	setofStacks[currentIndex].pop_back();

	while(setofStacks[currentIndex].size() == 0 && currentIndex > 0)
	{
		currentIndex--;
	}
	return ret;
}

int SetOfStacks::popAt(int index)
{
	if(setofStacks[index].size() == 0)
	{
		return -1;
	}

	int ret = setofStacks[index].back();
	setofStacks[index].pop_back();

	while(setofStacks[currentIndex].size() == 0 && currentIndex > 0)
	{
		currentIndex--;
	}
	return ret;
}

// Problem 3.4
void __printHanoiSub(std::list<int> list)
{
	for(int i = 0; i < list.size(); i++)
	{
		printf("%d ", list.front());
		list.push_back(list.front());
		list.pop_front();
	}
}

void __solveHanoiSub(int n, std::list<int>& first, std::list<int>& second, std::list<int>& third, std::function<void(void)> printFunc)
{
	if(n == 1)
	{
		third.push_back(first.back());
		first.pop_back();
		printFunc();
	}
	else
	{
		__solveHanoiSub(n - 1, first, third, second, printFunc);
		third.push_back(first.back());
		first.pop_back();
		printFunc();
		__solveHanoiSub(n - 1, second, first, third, printFunc);
	}
}

void __solveHanoi(void)
{
	std::list<int> first, second, third;

	auto printFunc = [&first, &second, &third]()
	{
		printf("1st: "); __printHanoiSub(first);  printf("\n");
		printf("2nd: "); __printHanoiSub(second); printf("\n");
		printf("3rd: "); __printHanoiSub(third);  printf("\n");
		printf("\n");
	};

	first.push_back(4);
	first.push_back(3);
	first.push_back(2);
	first.push_back(1);

	printFunc();
	__solveHanoiSub(first.size(), first, second, third, printFunc);
}

// Problem 3.5
class MyQueue
{
public:
	void enque(int value);
	int deque(void);

private:
	std::list<int> stack1, stack2;
};

void MyQueue::enque(int value)
{
	stack1.push_back(value);
}

int MyQueue::deque(void)
{
	if(stack1.size() == 0)
	{
		return -1;
	}

	while(stack1.size() != 0)
	{
		stack2.push_back(stack1.back());
		stack1.pop_back();
	}

	int ret = stack2.back();
	stack2.pop_back();

	while(stack2.size() != 0)
	{
		stack1.push_back(stack2.back());
		stack2.pop_back();
	}

	return ret;
}

// Problem 3.6
void __sortStackSub(std::list<int>& stack1, std::list<int>& stack2, std::list<int>& stack3, std::function<void()> printStack)
{
	if(!stack1.empty())
	{
		stack3.push_back(stack1.back());
		stack1.pop_back();
		printStack();

		while(!stack1.empty())
		{
			if(stack3.back() > stack1.back())
			{
				stack2.push_back(stack3.back());
				stack3.pop_back();
				printStack();

				stack3.push_back(stack1.back());
				stack1.pop_back();
				printStack();
			}
			else
			{
				stack2.push_back(stack1.back());
				stack1.pop_back();
				printStack();
			}
		}
		__sortStackSub(stack2, stack1, stack3, printStack);
	}
}

void __printStack(std::list<int> stack)
{
	for(int i = 0; i < stack.size(); i++)
	{
		printf("%d ", stack.front());
		stack.push_back(stack.front());
		stack.pop_front();
	}
}

void __sortStack(void)
{
	std::list<int> stack1, stack2, stack3;

	auto printStack = [&stack1, &stack2, &stack3]()
	{
		printf("stack1: "); __printStack(stack1);  printf("\n");
		printf("stack2: "); __printStack(stack2); printf("\n");
		printf("stack3: "); __printStack(stack3);  printf("\n");
		printf("\n");
	};

	stack1.push_back(1);
	stack1.push_back(2);
	stack1.push_back(3);
	printStack();

	__sortStackSub(stack1, stack2, stack3, printStack);
	while(!stack3.empty())
	{
		stack1.push_back(stack3.back());
		stack3.pop_back();
		printStack();
	}
}

// Problem 3.7
class AnimalShelter
{
	struct Animal
	{
		enum
		{
			Dog,
			Cat,
		};
	};
public:
	void enque(int value);
	int dequeAny(void);
	int dequeDog(void);
	int dequeCat(void);

private:
	std::list<int> _queue;
};

void AnimalShelter::enque(int animal)
{
	_queue.push_back(animal);
}

int AnimalShelter::dequeAny(void)
{
	int ret = _queue.front();
	_queue.pop_front();
	return ret;
}

int AnimalShelter::dequeDog(void)
{
	int num = 0;

	while(_queue.front() != Animal::Dog)
	{
		int temp = _queue.front();
		_queue.pop_front();
		_queue.push_back(temp);
		num++;
	}

	int ret = _queue.front();
	_queue.pop_front();
	for(int i = 0; i < num; i++)
	{
		int temp = _queue.back();
		_queue.pop_back();
		_queue.push_front(temp);
	}
	return ret;
}

int AnimalShelter::dequeCat(void)
{
	int num = 0;

	while(_queue.front() != Animal::Cat)
	{
		int temp = _queue.front();
		_queue.pop_front();
		_queue.push_back(temp);
		num++;
	}

	int ret = _queue.front();
	_queue.pop_front();
	for(int i = 0; i < num; i++)
	{
		int temp = _queue.back();
		_queue.pop_back();
		_queue.push_front(temp);
	}
	return ret;
}

int main(void)
{
	int no = 306;

	switch(no)
	{
		case 101:
			{
				std::string s;
				s = "abac";
				printf("[%s] is %s\n", s.c_str(), __isUnique(s) ? "unique" : "not unique");
				s = "abAc";
				printf("[%s] is %s\n", s.c_str(), __isUnique(s) ? "unique" : "not unique");
			}
			break;
		case 102:
			{
				char str[32];
				char reversed[32];
				strcpy(str, "abcde");
				strcpy(reversed, str);
				__reverse(reversed);
				printf("%s is reversed to %s\n", str, reversed);
			}
			break;
		case 103:
			{
				std::string s1 = "management";
				std::string s2 = "nemetamang";
				printf("[%s] and [%s] is %s\n", s1.c_str(), s2.c_str(), __isAnagram(s1, s2) ? "anagram" : "not anagram");
			}
			break;
		case 104:
			{
				char str1[32];
				char str2[32];
				strcpy(str1, "   ");
				strcpy(str2, str1);
				__replaceSpace(str2, strlen(str2));
				printf("[%s] is converted to [%s]\n", str1, str2);
			}
			break;
		case 105:
			{
				std::string s1 = "aabcccccaaa";
				std::string s2 = __compressString(s1);
				printf("[%s] is converted to [%s]\n", s1.c_str(), s2.c_str());
			}
			break;
		case 106:
			{
				//std::vector<std::vector<int> > m = { {1} };
				//std::vector<std::vector<int> > m = { {1, 2}, {3, 4} };
				//std::vector<std::vector<int> > m = { {1, 2, 3}, {4, 5, 6}, {7, 8, 9} };
				//std::vector<std::vector<int> > m = { {1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}, {13, 14, 15, 16} };
				std::vector<std::vector<int> > m = { {1, 2, 3, 4, 5}, {6, 7, 8, 9, 10}, {11, 12, 13, 14, 15}, {16, 17, 18, 19, 20}, {21, 22, 23, 24, 25} };
				printf("[original]\n");
				__printMatrix(m);
				__rotate90(m);
				printf("[rotated by 90]\n");
				__printMatrix(m);
			}
			break;
		case 107:
			{
				std::vector<std::vector<int> > m = { {0, 2, 3, 4, 5}, {6, 7, 8, 9, 10}, {11, 12, 13, 14, 15}, {16, 17, 18, 0, 20}, {21, 22, 23, 24, 25} };
				printf("[original]\n");
				__printMatrix(m);
				__fillZero(m);
				printf("[filled by 0]\n");
				__printMatrix(m);
			}
			break;
		case 108:
			{
				std::string s1 = "erbottlewat";
				std::string s2 = "waterbottle";
				printf("s1: [%s]\n", s1.c_str());
				printf("s2: [%s]\n", s2.c_str());
				printf("%s\n", __isRotatedString(s1, s2) ? "is rotated" : "is not rotated");
			}
			break;
		case 201:
			{
				std::list<int> ls = {1, 2, 2, 3, 4, 1};
				printf("[original]\n");
				__printList(ls);
				__removeDuplicated(ls);
				printf("[duplication removed]\n");
				__printList(ls);
			}
			break;
		case 202:
			{
				std::list<int> ls = {1, 2, 3};
				__printXFromTail(ls, 1);
			}
			break;
		case 204:
			{
				std::list<int> ls = {1, 2, 2, 3, 4, 0};
				printf("[original]\n");
				__printList(ls);
				__sortLessThanX(ls, 2);
				printf("[sorted]\n");
				__printList(ls);
			}
			break;
		case 205:
			{
				std::list<int> ls1 = {9, 9, 9};
				std::list<int> ls2 = {1};
				std::list<int> ls3 = __addByList(ls1, ls2);
				__printList(ls3);
			}
			break;
		case 304:
			{
				__solveHanoi();
			}
			break;
		case 306:
			{
				__sortStack();
			}
			break;
		default:
			printf("invalid no: [%d]\n", no);
			break;
	}

	return 0;
}
