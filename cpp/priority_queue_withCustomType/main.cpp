#include <iostream>
#include <queue>

class MyClass {
public:
	MyClass(int num) :
		num_(num)
	{}

	std::string toString() const {
		return std::to_string(num_);
	}

	bool operator<(const MyClass& rhs) const {
		return num_ > rhs.num_;
	}

	//struct Compare {
	//	bool operator()(const MyClass& a, const MyClass& b) const {
	//		return a.num_ > b.num_;
	//	}
	//};

private:
	int num_;
};

int main() {
	std::priority_queue<MyClass> que;
	//std::priority_queue<MyClass, std::vector<MyClass>, MyClass::Compare> que;

	que.push(MyClass(3));
	que.push(MyClass(1));
	que.push(MyClass(2));

	std::cout << que.top().toString() << std::endl;
	que.pop();

	std::cout << que.top().toString() << std::endl;
	que.pop();

	std::cout << que.top().toString() << std::endl;
	que.pop();

	return 0;
}

