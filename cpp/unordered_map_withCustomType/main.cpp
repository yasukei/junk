#include <set>
#include <iostream>
#include <string>

class MyClass {
public:
	MyClass(int num) :
		num_(num)
	{}

	std::string toString() const {
		return std::to_string(num_);
	}

	bool operator<(const MyClass& rhs) const {
		return num_ < rhs.num_;
	}

	//struct Compare {
	//	size_t operator()(const MyClass& a, const MyClass& b) const {
	//		return a.num_ < b.num_;
	//	}
	//};

private:
	int num_;
};

int main() {
	std::set<MyClass> mySet;
	//std::set<MyClass, MyClass::Compare> mySet;
	MyClass obj1(1);
	MyClass obj2(2);
	MyClass obj3(3);

	mySet.insert(obj1);
	mySet.insert(obj3);
	mySet.insert(obj2);

	for(const MyClass& obj : mySet) {
		std::cout << obj.toString() << std::endl;
	}

	return 0;
}

