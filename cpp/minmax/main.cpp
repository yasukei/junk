#include <iostream>
#include <vector>
#include <algorithm>

int main() {
	std::vector<int> v = {1, 2, 3, 4};
	decltype(v)::iterator it;

	int min = v[0];
	int max = v[0];
	for(int i : v) {
		if(i < min) min = i;
		if(i > max) max = i;
	}
	printf("min: %d\n", min);
	printf("max: %d\n", max);

	it = std::min_element(v.begin(), v.end());
	printf("min: %d\n", *it);

	it = std::max_element(v.begin(), v.end());
	printf("max: %d\n", *it);

	return 0;
}

