#include <cstdio>
#include <unordered_set>

class MyClass {
public:

	bool operator==(const MyClass& rhs) const {
		return this->num_ == rhs.num_;
	}

	struct HashFunction {
		size_t operator()(const MyClass& self) const {
			return self.num_;
		}
	};

private:
	int num_;
};

int main() {
	std::unordered_set<MyClass, MyClass::HashFunction> mySets;
	MyClass obj1;

	mySets.insert(obj1);
	auto it = mySets.find(obj1);
	if(it != mySets.end()) {
		printf("found!\n");
	} else {
		printf("not found...\n");
	}

	return 0;
}

