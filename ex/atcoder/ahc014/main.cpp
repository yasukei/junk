#include <bits/stdc++.h>
typedef long long ll;

class Clock {
public:
	Clock() :
		Clock(false)
	{}
	Clock(bool printWhenDestruct) :
		ref_(std::chrono::steady_clock::now()),
		printWhenDestruct_(printWhenDestruct)
	{
		setlocale(LC_NUMERIC,"ja_JP.utf8");
	}
	~Clock() {
		if(printWhenDestruct_) {
			print();
		}
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

	void reset() {
		ref_ = std::chrono::steady_clock::now();
	}
	void print() {
		printf("time: %'9lu\n", getElapsedTimeNanosec());
	}

private:
	std::chrono::steady_clock::time_point ref_;
	bool printWhenDestruct_;
};

class Point {
public:
	Point() {}
	Point(int x, int y) : x_(x), y_(y) {}
	Point(const Point& p) : x_(p.x_), y_(p.y_) {}

	int x() const { return x_; }
	int y() const { return y_; }

	Point operator=(const Point& rhs) {
		this->x_ = rhs.x_;
		this->y_ = rhs.y_;
		return *this;
	}    
	bool operator==(const Point& rhs) const {
		return this->x_ == rhs.x_ && this->y_ == rhs.y_;
	}    
	bool operator!=(const Point& rhs) const {
		return !(*this == rhs);
	}    
	Point operator+(const Point& rhs) const {
		return Point(this->x_ + rhs.x_, this->y_ + rhs.y_);
	}    
	Point operator-(const Point& rhs) const {
		return Point(this->x_ - rhs.x_, this->y_ - rhs.y_);
	}    

	static int getDistance(const Point& p1, const Point& p2) {
		int x = p1.x_ - p2.x_;
		int y = p1.y_ - p2.y_;
		return x*x + y*y; // WARN: sqrt is omitted
	}

	struct HashFunction {
		std::size_t operator()(const Point& point) const {
			return point.x_ + (point.y_ << 8);
		}
	};

private:
	std::int16_t x_;
	std::int16_t y_;
};

class Edge {
public:
	Edge() {}
	Edge(const Point& p1, const Point& p2) : p1_(p1), p2_(p2) {}
	Edge(int x1, int y1, int x2, int y2) : Edge(Point(x1, y1), Point(x2, y2)) {}

	Point p1() const { return p1_; }
	Point p2() const { return p2_; }

	void set(const Point& p1, const Point& p2) {
		p1_ = p1;
		p2_ = p2;
	}

	bool is45DegreeSlope() const {
		int slope;
		bool result = getSlope(slope);
		return result && (slope == 1 || slope == -1);
	}
	bool isOverlapped(const Edge& rhs) const {
		if(this->isVertical() || rhs.isVertical()) {
			int this_max = std::max(this->p1_.y(), this->p2_.y());
			int this_min = std::min(this->p1_.y(), this->p2_.y());
			int rhs_max = std::max(rhs.p1_.y(), rhs.p2_.y());
			int rhs_min = std::min(rhs.p1_.y(), rhs.p2_.y());
			return this->isVertical() && rhs.isVertical()
			&& this_max > rhs_min && rhs_max > this_min;
		}
		int s1, s2, i1, i2;
		if(this->getSlope(s1) && rhs.getSlope(s2) && s1 == s2
		&& this->getIntercept(i1) && rhs.getIntercept(i2) && i1 == i2) {
			int this_max = std::max(this->p1_.x(), this->p2_.x());
			int this_min = std::min(this->p1_.x(), this->p2_.x());
			int rhs_max = std::max(rhs.p1_.x(), rhs.p2_.x());
			int rhs_min = std::min(rhs.p1_.x(), rhs.p2_.x());
			return this_max > rhs_min && rhs_max > this_min;
		}
		return false;
	}
	bool isOnEdge(const Point& p) const {
		if(isVertical()) {
			int min = std::min(p1_.y(), p2_.y());
			int max = std::max(p1_.y(), p2_.y());
			return p.x() == p1_.x() && min <= p.y() && p.y() <= max;
		}
		if(isHorizontal()) {
			int min = std::min(p1_.x(), p2_.x());
			int max = std::max(p1_.x(), p2_.x());
			return p.y() == p1_.y() && min <= p.x() && p.x() <= max;
		}
		int slope, intercept;
		if(!getSlope(slope) || !getIntercept(intercept)) return false;
		int min = std::min(p1_.x(), p2_.x());
		int max = std::max(p1_.x(), p2_.x());
		return (p.x() * slope + intercept) == p.y() && min <= p.x() && p.x() <= max;

	}
	bool isVertical() const { return p1_.x() == p2_.x(); }
	bool isHorizontal() const { return p1_.y() == p2_.y(); }
	bool getSlope(int& slope) const {
		// WARN: it assumes the slope is only -1 or 1
		if(isVertical()) return false;
		int y = p1_.y() - p2_.y();
		if(y == 0) {
			slope = 0;
			return true;
		}
		int x = p1_.x() - p2_.x();
		if(std::abs(x) != std::abs(y)) return false;
		slope = (x > 0 && y > 0) || (x < 0 && y < 0) ? 1 : -1;
		return true;
	}
	bool getIntercept(int& intercept) const {
		int slope;
		bool result = getSlope(slope);
		if(!result) return false;
		intercept = p1_.y() - slope * p1_.x();
		return true;
	}

	bool operator==(const Edge& rhs) const {
		return this->p1_ == rhs.p1_ && this->p2_ == rhs.p2_;
	}

	struct HashFunction {
		std::size_t operator()(const Edge& edge) const {
			static Point::HashFunction hashFunc;
			return hashFunc(edge.p1_) + (hashFunc(edge.p2_) << 16);
		}
	};

private:
	Point p1_;
	Point p2_;

};

class EdgeSet {
public:
	EdgeSet() {}
	EdgeSet(const Edge& edge) {
		insert(edge);
	}

	void insert(const Edge& edge) {
		if(edge.isHorizontal()) {
			int x = std::min(edge.p1().x(), edge.p2().x());
			int y = edge.p1().y();
			int size = std::abs(edge.p1().x() - edge.p2().x());
			for(int i = 0; i < size; i++) {
				edgeSet_.insert(Edge(Point(x + i, y), Point(x + i + 1, y)));
			}
		} else if(edge.isVertical()) {
			int x = edge.p1().x();
			int y = std::min(edge.p1().y(), edge.p2().y());
			int size = std::abs(edge.p1().y() - edge.p2().y());
			for(int i = 0; i < size; i++) {
				edgeSet_.insert(Edge(Point(x, y + i), Point(x, y + i + 1)));
			}
		} else {
			int x = std::min(edge.p1().x(), edge.p2().x());
			int y = (x == edge.p1().x()) ? edge.p1().y() : edge.p2().y();
			int size = std::abs(edge.p1().x() - edge.p2().x());
			int slope;

			if(!edge.getSlope(slope)) assert(false); // should nerver come
			for(int i = 0; i < size; i++) {
				edgeSet_.insert(Edge(Point(x + i, y + i * slope), Point(x + i + 1, y + (i + 1) * slope)));
			}
		}
	}
	void clear() {
		edgeSet_.clear();
	}

	bool isOverlapped(const Edge& edge) const {
		EdgeSet edgeSet(edge);

		return isOverlapped(edgeSet);
	}
	bool isOverlapped(const EdgeSet& rhs) const {
		for(auto it = rhs.edgeSet_.cbegin(); it != rhs.edgeSet_.cend(); it++) {
			auto result = edgeSet_.find(*it);
			if(result != edgeSet_.end()) {
				return true;
			}
		}
		return false;
	}

	void foreachEdge(std::function<bool(const Edge& edge)> func) {
		for(auto it = edgeSet_.cbegin(); it != edgeSet_.cend(); it++) {
			if(!func(*it)) return;
		}
	}

	bool operator==(const EdgeSet& rhs) const {
		return this->edgeSet_ == rhs.edgeSet_;
	}

	struct HashFunction {
		std::size_t operator()(const EdgeSet& edgeSet) const {
			static Edge::HashFunction hasuFunc;

			std::size_t hash = edgeSet.edgeSet_.size();
			if(!edgeSet.edgeSet_.empty()) {
				hash += hasuFunc(*edgeSet.edgeSet_.cbegin());
			}
			return hash;
		}
	};

private:
	std::unordered_set<Edge, Edge::HashFunction> edgeSet_;
};

class Rectangle {
public:
	Rectangle() {}
	Rectangle(const Point& p1, const Point& p2, const Point& p3, const Point& p4) :
		p1_(p1),
		p2_(p2),
		p3_(p3),
		p4_(p4)
   	{}

	void set(const Point& p1, const Point& p2, const Point& p3, const Point& p4) {
		p1_ = p1;
		p2_ = p2;
		p3_ = p3;
		p4_ = p4;
	}

	Point p1() const { return p1_; }
	Point p2() const { return p2_; }
	Point p3() const { return p3_; }
	Point p4() const { return p4_; }
	Edge e1() const { return Edge(p1_.x(), p1_.y(), p2_.x(), p2_.y()); }
	Edge e2() const { return Edge(p2_.x(), p2_.y(), p3_.x(), p3_.y()); }
	Edge e3() const { return Edge(p3_.x(), p3_.y(), p4_.x(), p4_.y()); }
	Edge e4() const { return Edge(p4_.x(), p4_.y(), p1_.x(), p1_.y()); }

	bool isOnRectangle(const Point& p) const {
		return e1().isOnEdge(p)
			|| e2().isOnEdge(p)
			|| e3().isOnEdge(p)
			|| e4().isOnEdge(p);
	}

	bool operator==(const Rectangle& rhs) const {
		return this->p1_ == rhs.p1_
				&& this->p2_ == rhs.p2_
				&& this->p3_ == rhs.p3_
				&& this->p4_ == rhs.p4_;
	}

	struct HashFunction {
		std::size_t operator()(const Rectangle& rectangle) const {
			return (rectangle.p1_.x() << 0)
					+ (rectangle.p1_.y() << 4)
					+ (rectangle.p2_.x() << 8)
					+ (rectangle.p2_.y() << 12)
					+ (rectangle.p3_.x() << 16)
					+ (rectangle.p3_.y() << 20)
					+ (rectangle.p4_.x() << 24)
					+ (rectangle.p4_.y() << 28);
		}
	};

private:
	Point p1_;
	Point p2_;
	Point p3_;
	Point p4_;
};

class Geometry {
public:
	Geometry(int N) :
		N_(N)
	{}

	int calcScore(int x, int y) const {
		int c = (N_ - 1) / 2;
		int temp1 = x - c;
		int temp2 = y - c;
		return temp1*temp1 + temp2*temp2 + 1;
	}

	void findCandidateRectanglesForAllPoints(
		const std::unordered_set<Point, Point::HashFunction>& existingPoints,
		std::unordered_set<Rectangle, Rectangle::HashFunction>& candidateRectangles) const {
		Rectangle rect;

		for(auto it = existingPoints.cbegin(); it != existingPoints.cend(); it++) {
			for(auto jt = std::next(it); jt != existingPoints.cend(); jt++) {
				for(auto kt = std::next(jt); kt != existingPoints.cend(); kt++) {
					if(!tryToMakeRectangle(*it, *jt, *kt, rect)) continue;
					if(!withinGrid(rect)) continue;
					if(hasPointOnTheRectangle(rect, existingPoints)) continue;

					candidateRectangles.insert(rect);
				}
			}
		}
	}

	void findAdditionalCandidateRectanglesForNewRectangle(
		const Rectangle& newRectangle,
		const std::unordered_set<Point, Point::HashFunction>& existingPoints,
		const EdgeSet& existingEdges,
		std::unordered_set<Rectangle, Rectangle::HashFunction>& candidateRectangles) const {

		// Sanitizing existing candidate rectangles
		for(auto it = candidateRectangles.begin(); it != candidateRectangles.end(); ) {
			if(isOverlappedWithExistingEdge(*it, existingEdges)) {
				it = candidateRectangles.erase(it);
			} else {
				it++;
			}
		}
		for(auto it = candidateRectangles.begin(); it != candidateRectangles.end(); ) {
			if(isOverlappedWithExistingPoints((*it).p4(), existingPoints)) {
				it = candidateRectangles.erase(it);
			} else {
				it++;
			}
		}
		for(auto it = candidateRectangles.begin(); it != candidateRectangles.end(); ) {
			if(hasPointOnTheRectangle((*it), existingPoints)) {
				it = candidateRectangles.erase(it);
			} else {
				it++;
			}
		}

		// Find additional candidate rectangles
		Rectangle rect;
		for(auto it = existingPoints.cbegin(); it != existingPoints.cend(); it++) {
			for(auto jt = std::next(it); jt != existingPoints.cend(); jt++) {
				if(!tryToMakeRectangle(*it, *jt, newRectangle.p4(), rect)) continue;
				if(!withinGrid(rect)) continue;
				if(hasPointOnTheRectangle(rect, existingPoints)) continue;
				if(isOverlappedWithExistingEdge(rect, existingEdges)) continue;

				candidateRectangles.insert(rect);
			}
		}
	}

private:
	int N_;

	bool withinGrid(const Rectangle& rect) const {
		return isInRange(rect.p1().x(), 0, N_ - 1) && isInRange(rect.p1().y(), 0, N_ - 1)
				&& isInRange(rect.p2().x(), 0, N_ - 1) && isInRange(rect.p2().y(), 0, N_ - 1)
				&& isInRange(rect.p3().x(), 0, N_ - 1) && isInRange(rect.p3().y(), 0, N_ - 1)
				&& isInRange(rect.p4().x(), 0, N_ - 1) && isInRange(rect.p4().y(), 0, N_ - 1);
	}
	static bool isInRange(int value, int lowerLimit, int upperLimit) {
		return lowerLimit <= value && value <= upperLimit;
	}

	static bool isOnStraightLine(const Point& p1, const Point& p2, const Point& p3) {
		return Edge(p1, p2).isOnEdge(p3)
				|| Edge(p2, p3).isOnEdge(p1)
				|| Edge(p3, p1).isOnEdge(p2);
	}

	static bool tryToMakeRectangle(Point p1, Point p2, Point p3, Rectangle& out) {
		int p1_p2 = Point::getDistance(p1, p2);
		int p2_p3 = Point::getDistance(p2, p3);
		int p3_p1 = Point::getDistance(p3, p1);

		// let p1_p3 be the diagonal line of rectangle
		int max = std::max({p1_p2, p2_p3, p3_p1});
		if(max == p1_p2) {
			std::swap(p2, p3);
		} else if(max == p2_p3) {
			std::swap(p2, p1);
		}

		if(p1.x() == p2.x() && p3.y() == p2.y()) {
			out.set(p1, p2, p3, Point(p3.x(), p1.y()));
			return !isOnStraightLine(p1, p2, p3);
		} else if(p3.x() == p2.x() && p1.y() == p2.y()) {
			out.set(p1, p2, p3, Point(p1.x(), p3.y()));
			return !isOnStraightLine(p1, p2, p3);
		} else {
			Edge e12(p1, p2);
			Edge e32(p3, p2);
			if(e12.is45DegreeSlope() && e32.is45DegreeSlope()) {
				out.set(p1, p2, p3, (p1 - p2) + p3);
				return !isOnStraightLine(p1, p2, p3);
			}
		}

		return false;
	}

	static bool hasPointOnTheRectangle(const Rectangle& rect, const std::unordered_set<Point, Point::HashFunction>& existingPoints) {
#if 0
		static EdgeSet edgeSet;
		edgeSet.clear();
		edgeSet.insert(rect.e1());
		edgeSet.insert(rect.e2());
		edgeSet.insert(rect.e3());
		edgeSet.insert(rect.e4());

		bool result = false;

		auto hasPointOnRectanglesEdge = [&](const Edge& edge) -> bool {
			bool continueForeach = true;
			auto isExistingPoint = [&](const Point& point) -> bool {
				if(point == rect.p1()
						|| point == rect.p2()
						|| point == rect.p3()) {
						//|| point == rect.p4())
					return false;
				}
				auto it = existingPoints.find(point);
				if(it == existingPoints.end()) {
					return false;
				}
				return true;
			};

			if(isExistingPoint(edge.p1())
				|| isExistingPoint(edge.p2())) {
				result = true;
				continueForeach = false;
			}
			return continueForeach;
		};
		edgeSet.foreachEdge(hasPointOnRectanglesEdge);
		return result;
#else
		for(auto it = existingPoints.cbegin(); it != existingPoints.cend(); it++) {
			if(*it == rect.p1()
					|| *it == rect.p2()
					|| *it == rect.p3()) {
					//|| *it == rect.p4()) 
				continue;
			}
			if(rect.isOnRectangle(*it)) {
				return true;
			}
		}
		return false;
#endif
	}

	static bool isOverlappedWithExistingPoints(const Point& point, const std::unordered_set<Point, Point::HashFunction>& existingPoints) {
		auto result = existingPoints.find(point);
		if(result != existingPoints.end()) {
			return true;
		}
		return false;
	}
	static bool isOverlappedWithExistingEdge(const Rectangle& rectangle, const EdgeSet& existingEdges) {
		if(existingEdges.isOverlapped(rectangle.e1())
			|| existingEdges.isOverlapped(rectangle.e2())
			|| existingEdges.isOverlapped(rectangle.e3())
			|| existingEdges.isOverlapped(rectangle.e4())) {
			return true;
		}
		return false;
	}
};

struct Context {
public:
	std::unordered_set<Point, Point::HashFunction> existingPoints;
	EdgeSet existingEdges;
	std::unordered_set<Rectangle, Rectangle::HashFunction> candidateRectangles;

	Context() {}

	Context(const Context& context) :
		existingPoints(context.existingPoints),
		existingEdges(context.existingEdges),
		candidateRectangles(context.candidateRectangles)
	{}
};

class Answer {
public:
	Answer() {}
	Answer(const Answer& answer) :
		rectangles_(answer.rectangles_),
		score_(answer.score_)
	{}

	void pushBack(const Rectangle& rectangle, int score) {
		rectangles_.push_back(rectangle);
		score_ += score;
	}
	Rectangle getLastRectangle() const { return rectangles_.back(); }
	int getNumofRectangles() const { return rectangles_.size(); }
	int getScore() const { return score_; }
	void print() const {
		printf("%ld\n", rectangles_.size());
		for(auto it = rectangles_.cbegin(); it != rectangles_.end(); it++) {
			const Rectangle& rect = *it;
			printf(" %d %d", rect.p4().x(), rect.p4().y());
			printf(" %d %d", rect.p1().x(), rect.p1().y());
			printf(" %d %d", rect.p2().x(), rect.p2().y());
			printf(" %d %d", rect.p3().x(), rect.p3().y());
			printf("\n");
		}
	}

private:
	std::vector<Rectangle> rectangles_;
	int score_;
};

class TreeNode {
public:
	TreeNode(Context* context) // constructor for root node
	{
		context_ = new Context(*context);
	}
	TreeNode(TreeNode* parent, const Rectangle& rectangle, int score) : // constructor for child node
		answer_(parent->answer_),
		done_(false)
	{
		answer_.pushBack(rectangle, score);
		context_ = new Context(*parent->context_);
	}
	~TreeNode() {
		if(context_ != nullptr) delete context_;
	}

	bool operator<(const TreeNode& rhs) const {
		return this->answer_.getScore() > rhs.answer_.getScore(); // the greater('>') is for Tree::scoreRanking_
	}

	void markAsDone() {
		done_ = true;

		free(context_);
		context_ = nullptr;
	}
	bool isDone() const { return done_; }
	Answer getAnswer() const { return answer_; }
	Rectangle getRectangle() const { return answer_.getLastRectangle(); }
	int getScore() const { return answer_.getScore(); }
	Context* getContext() const { return context_; }

private:
	Context* context_;
	Answer answer_;
	bool done_;

};

auto f = [](TreeNode* lhs, TreeNode* rhs) -> bool { return *lhs < *rhs; };

class Tree {
public:
	Tree(
		int N,
		const std::unordered_set<Point, Point::HashFunction>& initialPoints
		) :
		root_(nullptr),
		scoreRanking_(f),
		geometry_(N)
	{
		Context context;

		context.existingPoints = initialPoints;
		geometry_.findCandidateRectanglesForAllPoints(context.existingPoints, context.candidateRectangles);

		root_ = new TreeNode(&context);
		createChildren(root_);
	}

	void processOneNode(bool& doneAllNodes) {
		TreeNode* node = findBestScoreNodeOrAnotherToProcess();
		if(node == nullptr) {
			doneAllNodes = true;
			return;
		}

		Context* context = node->getContext();
		geometry_.findAdditionalCandidateRectanglesForNewRectangle(
				node->getRectangle(),
				context->existingPoints,
				context->existingEdges,
				context->candidateRectangles);
		createChildren(node);
		updateBestAnswer();
	}
	std::multiset<TreeNode*, decltype(f)>::iterator eraseNode(const std::multiset<TreeNode*, decltype(f)>::iterator& it) {
		TreeNode* node = *it;
		delete node;
		return scoreRanking_.erase(it);
	}

	void printBestAnswer() {
		TreeNode* best = nullptr;

		auto it = scoreRanking_.begin();
		if(it != scoreRanking_.end()) {
			best = *it;
		}
		printAnswer(best);
	}
	void printBestScore() {
		int score = 0;

		auto best = scoreRanking_.begin();
		if(best != scoreRanking_.end()) {
			score = (*best)->getAnswer().getScore();
		}
		printf("BestScore: %d\n", score);
	}
	void printScoreRanking(bool all) {
		printf("numofExplored:     %'ld\n", numofExplored_);
		printf("numofScoreRanking: %'ld\n", scoreRanking_.size());
		if(!all) return;

		for(auto treeNode = scoreRanking_.cbegin(); treeNode != scoreRanking_.cend(); treeNode++) {
			printf("%d  ", (*treeNode)->getAnswer().getScore());
		}
		printf("\n");
	}

private:
	TreeNode* root_;

	std::multiset<TreeNode*, decltype(f)> scoreRanking_;
	Answer bestAnswer_;

	std::unordered_set<EdgeSet, EdgeSet::HashFunction> exploredSpace_;
	int64_t numofExplored_;

	Geometry geometry_;

	TreeNode* findBestScoreNodeOrAnotherToProcess() {
		auto it = scoreRanking_.begin();
		while(it != scoreRanking_.end() && (*it)->isDone()) {
			if(it != scoreRanking_.begin()) {
				it = eraseNode(it);
			} else {
				it = std::next(it);
			}
		}
		if(it == scoreRanking_.end()) {
			return nullptr;
		}
		return *it;
	}

	void createChildren(TreeNode* parent) {
		//Clock c(true);
		Context* parentContext = parent->getContext();

		for(auto it = parentContext->candidateRectangles.cbegin(); it != parentContext->candidateRectangles.cend(); it++) {
			const Rectangle& newRectangle = *it;

			int score = geometry_.calcScore(newRectangle.p4().x(), newRectangle.p4().y());
			TreeNode* childNode = new TreeNode(parent, newRectangle, score);
			Context* childContext = childNode->getContext();
			childContext->existingPoints.insert(newRectangle.p4());
			childContext->existingEdges.insert(newRectangle.e1());
			childContext->existingEdges.insert(newRectangle.e2());
			childContext->existingEdges.insert(newRectangle.e3());
			childContext->existingEdges.insert(newRectangle.e4());
			childContext->candidateRectangles.erase(newRectangle);
			auto result = exploredSpace_.find(childContext->existingEdges);
			if(result != exploredSpace_.end()) {
				delete childNode;
				continue;
			}

			scoreRanking_.insert(childNode);
		}
		exploredSpace_.insert(std::move(parentContext->existingEdges));
		numofExplored_++;
		parent->markAsDone();
	}
	void updateBestAnswer() {
		if(scoreRanking_.cbegin() == scoreRanking_.cend()) return;

		TreeNode* rankingTop = *scoreRanking_.cbegin();
		if(rankingTop->getScore() > bestAnswer_.getScore()) {
			bestAnswer_ = rankingTop->getAnswer();
		}
	}
	void printAnswer(TreeNode* node) {
		if(node == nullptr) {
			printf("0\n");
			return;
		}

		Answer answer = node->getAnswer();
		answer.print();
	}
};

int main() {
#if 0
	printf("Point:     %lu\n", sizeof(Point));
	printf("Edge:      %lu\n", sizeof(Edge));
	printf("EdgeSet:   %lu\n", sizeof(EdgeSet));
	printf("Rectangle: %lu\n", sizeof(Rectangle));
	printf("Geometry:  %lu\n", sizeof(Geometry));
	printf("Answer:    %lu\n", sizeof(Answer));
	printf("Context :  %lu\n", sizeof(Context ));
	printf("TreeNode:  %lu\n", sizeof(TreeNode));
	printf("Tree:      %lu\n", sizeof(Tree));
	return 0;
#endif

	Clock clock;

	int N, M;
	std::cin >> N >> M;
	std::unordered_set<Point, Point::HashFunction> points;
	for(int i = 0; i < M; i++) {
		int x, y;
		std::cin >> x >> y;
		points.insert(Point(x, y));
	}

	Tree tree(N, points);
	
	while(clock.getElapsedTimeMillisec() < 1000) {
		bool doneAllNodes = false;
		tree.processOneNode(doneAllNodes);
		if(doneAllNodes) { break; }
	}
	//tree.printBestScore();
	tree.printBestAnswer();
	//tree.printScoreRanking(false);

	return 0;
}

