#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <list>
#include <tuple>
#include <algorithm>

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

int main(void)
{
	int no = 205;

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
		default:
			printf("invalid no: [%d]\n", no);
			break;
	}

	return 0;
}
