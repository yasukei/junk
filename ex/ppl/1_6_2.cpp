#include <bits/stdc++.h>

class BitArray {
public:
	BitArray(size_t size) :
		_size(size),
		_array()
	{
		if(size % 8 == 0) {
			_array.resize(size / 8);
		} else {
			_array.resize(size / 8 + 1);
		}
	}

	std::string toString() {
		std::string result;
		for(size_t i = 0; i < _size / 8; i++) {
			for(size_t j = 0; j < 8; j++) {
				if(get(i*8 + j)) {
					result.push_back('1');
				} else {
					result.push_back('0');
				}
			}
			result.push_back(' ');
		}
		if(_size / 8 != _array.size()) {
			size_t i = _array.size() - 1;
			for(size_t j = 0; j < _size % 8; j++) {
				if(get(i*8 + j)) {
					result.push_back('1');
				} else {
					result.push_back('0');
				}
			}
		}
		return result;
	}

	void set(size_t index) {
		if(index >= _size) {
			throw std::out_of_range("index is out of range.");
		}

		_array[index / 8] |= 1 << (index % 8);
	}
	void reset(size_t index) {
		if(index >= _size) {
			throw std::out_of_range("index is out of range.");
		}

		_array[index / 8] &= ~(1 << (index % 8));
	}
	bool get(size_t index) {
		if(index >= _size) {
			throw std::out_of_range("index is out of range.");
		}

		uint8_t data = _array[index / 8] & (1 << (index % 8));
		return data > 0;
	}
		
private:
	size_t _size;
	std::vector<uint8_t> _array;
};

class Clock {
public:
	Clock() :
		ref_(std::chrono::steady_clock::now())
	{
		setlocale(LC_NUMERIC,"ja_JP.utf8");
	}

	void reset() {
		ref_ = std::chrono::steady_clock::now();
	}

	std::uint64_t getElapsedTimeMillisec() const {
		using namespace std::chrono;
		milliseconds elapsed = duration_cast<milliseconds>(steady_clock::now() - ref_);
		return elapsed.count();
	}
	std::uint64_t getElapsedTimeMicrosec() const {
		using namespace std::chrono;
		microseconds elapsed = duration_cast<microseconds>(steady_clock::now() - ref_);
		return elapsed.count();
	}
	std::uint64_t getElapsedTimeNanosec() const {
		using namespace std::chrono;
		nanoseconds elapsed = duration_cast<nanoseconds>(steady_clock::now() - ref_);
		return elapsed.count();
	}

private:
	std::chrono::steady_clock::time_point ref_;
};

int main() {
	Clock clock;
	const size_t allSize = 10*1000*1000;
	const size_t sampleSize = 1000*1000;
	const int repeat = 10;

	// sort target
	std::vector<int> postcodes(sampleSize);
	for(size_t i = 0; i < postcodes.size(); i++) {
		postcodes[i] = i;
	}
	std::random_device seed_gen;
	std::mt19937 engine(seed_gen());
	std::shuffle(postcodes.begin(), postcodes.end(), engine);


	// sort by BitArray
	clock.reset();
	for(int i = 0; i < repeat; i++) {
		BitArray bitArray(allSize);
		for(int& postcode : postcodes) {
			bitArray.set(postcode);
		}
	}
	printf("sort by BitArray:                %'9lu [ns]\n", clock.getElapsedTimeNanosec() / repeat);

	// sort by std::vector & std::sort
	clock.reset();
	for(int i = 0; i < repeat; i++) {
		std::vector<int> v;
		v.reserve(allSize);
		for(int& postcode : postcodes) {
			v.emplace_back(postcode);
		}
		std::sort(v.begin(), v.end());
	}
	printf("sort by std::vector & std::sort: %'9lu [ns]\n", clock.getElapsedTimeNanosec() / repeat);

	// sort by std::bitset
	clock.reset();
	for(int i = 0; i < repeat; i++) {
		std::bitset<allSize> bitset;
		for(int& postcode : postcodes) {
			bitset.set(postcode);
		}
	}
	printf("sort by std::bitset:             %'9lu [ns]\n", clock.getElapsedTimeNanosec() / repeat);

	return 0;
}
