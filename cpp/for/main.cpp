#include <iostream>
#include <vector>
#include <algorithm>

int main() {
	std::vector<int> v = {1, 2, 3, 4};

	// for by index
	for(size_t i = 0; i < v.size(); i++) {
		printf("%d ", v[i]);
	}
	printf("\n");

	// for by iterator
	for(std::vector<int>::iterator it = v.begin(); it != v.end(); it++) {
		printf("%d ", *it);
	}
	printf("\n");

	// for by std::for_each
	std::for_each(v.begin(), v.end(), [](int i) {
		printf("%d ", i);
	});
	printf("\n");

	// for by range (c++11 later)
	for(int i : v) {
		printf("%d ", i);
	}
	printf("\n");

	// Substitution
	std::for_each(v.begin(), v.end(), [](int& i) {
		i *= 2;
	});
	for(int& i : v) {
		i *= 3;
	}
	// Substitution result
	for(size_t i = 0; i < v.size(); i++) {
		printf("%d ", v[i]);
	}
	printf("\n");

	return 0;
}

