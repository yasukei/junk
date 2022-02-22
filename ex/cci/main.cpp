#include <stdio.h>
#include <string.h>
#include <string>
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

int main(void)
{
	int no = 104;

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

		default:
			printf("invalid no: [%d]\n", no);
			break;
	}

	return 0;
}
