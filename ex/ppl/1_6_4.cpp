#include <bits/stdc++.h>

std::vector<int> makeSample(int n, int k) {
	std::random_device seed_gen;
	std::default_random_engine engine(seed_gen());

	std::vector<int> result;
	int remaining = n;
	for(int i = 0; i < n; i++) {
		std::uniform_int_distribution<> dist(0, remaining);
		if(dist(engine) % remaining < k) {
			result.emplace_back(i);
			k--;
		}
		remaining--;
	}
	return result;
}

int main() {
	std::vector<int> v = makeSample(10, 5);
	for(int& i : v) {
		printf("%d, ", i);
	}
	printf("\n");

	return 0;
}
