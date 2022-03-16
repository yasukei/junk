#include <bits/stdc++.h>

//#define PRINTF printf
#define PRINTF

struct Direction
{
	enum Enum
	{
		OutOfBoarder = -1,
		North = 0,
		South,
		East,
		West,
		NorthEast,
		SouthEast,
		SouthWest,
		NorthWest,
	};
};

struct Point
{
public:
	Point(int x, int y) :
		_x(x),
		_y(y)
	{
	}

	int x() const { return _x; }
	int y() const { return _y; }

	Direction::Enum getDirection()
	{
		Direction::Enum result = Direction::OutOfBoarder;

		if(_y > 0 && _x == 0)
		{
			result = Direction::North;
		}
		else if(_y < 0 && _x == 0)
		{
			result = Direction::South;
		}
		else if(_y == 0 && _x > 0)
		{
			result = Direction::East;
		}
		else if(_y == 0 && _x < 0)
		{
			result = Direction::West;
		}
		else if(_y > 0 && _x > 0 && _y == _x)
		{
			result = Direction::NorthEast;
		}
		else if(_y < 0 && _x > 0 && -_y == _x)
		{
			result = Direction::SouthEast;
		}
		else if(_y < 0 && _x < 0 && _y == _x)
		{
			result = Direction::SouthWest;
		}
		else if(_y > 0 && _x < 0 && _y == -_x)
		{
			result = Direction::NorthWest;
		}
		return result;
	}

	int getDistanceFromOrigin()
	{
		if(getDirection() == Direction::OutOfBoarder)
		{
			return INT_MAX;
		}

		if(_y == 0)
		{
			return std::abs(_x) - 1;
		}
		return std::abs(_y) - 1;
	}

	std::string to_string()
	{
		char buf[128];
		snprintf(buf, sizeof(buf), "x: [%4d], y: [%4d], direction: [%2d], distance: [%4d]", _x, _y, getDirection(), getDistanceFromOrigin());
		return std::string(buf);
	}

private:
	int _x;
	int _y;
};

int main(void)
{
	int n, k;
	int rq, cq;

    std::cin >> n >> k;    
    std::cin >> rq >> cq;

	// Nearest obstacles
	Point north(0,          n - rq + 1);
	Point south(0,          0 - rq);
	Point  east(n - cq + 1, 0);
	Point  west(0 - cq,     0);
	int temp;
	temp = std::abs(n - cq + 1) < std::abs(n - rq + 1) ? std::abs(n - cq + 1) : std::abs(n - rq + 1); Point northEast(temp, temp);
	temp = std::abs(n - cq + 1) < std::abs(0 - rq)     ? std::abs(n - cq + 1) : std::abs(0 - rq);     Point southEast(temp, -temp);
	temp = std::abs(0 - cq)     < std::abs(0 - rq)     ? std::abs(0 - cq)     : std::abs(0 - rq);     Point southWest(-temp, -temp);
	temp = std::abs(0 - cq)     < std::abs(n - rq + 1) ? std::abs(0 - cq)     : std::abs(n - rq + 1); Point northWest(-temp, temp);

	int r;
	int c;
    for(int i = 0; i < k; ++i)
    {
        std::cin >> r >> c;

		Point newObstacle(c - cq, r - rq);
		switch(newObstacle.getDirection())
		{
			case Direction::North:
				if(newObstacle.getDistanceFromOrigin() < north.getDistanceFromOrigin()) { north = newObstacle; } break;
			case Direction::South:
				if(newObstacle.getDistanceFromOrigin() < south.getDistanceFromOrigin()) { south = newObstacle; } break;
			case Direction::East:
				if(newObstacle.getDistanceFromOrigin() < east.getDistanceFromOrigin()) { east = newObstacle; } break;
			case Direction::West:
				if(newObstacle.getDistanceFromOrigin() < west.getDistanceFromOrigin()) { west = newObstacle; } break;
			case Direction::NorthEast:
				if(newObstacle.getDistanceFromOrigin() < northEast.getDistanceFromOrigin()) { northEast = newObstacle; } break;
			case Direction::SouthEast:
				if(newObstacle.getDistanceFromOrigin() < southEast.getDistanceFromOrigin()) { southEast = newObstacle; } break;
			case Direction::SouthWest:
				if(newObstacle.getDistanceFromOrigin() < southWest.getDistanceFromOrigin()) { southWest = newObstacle; } break;
			case Direction::NorthWest:
				if(newObstacle.getDistanceFromOrigin() < northWest.getDistanceFromOrigin()) { northWest = newObstacle; } break;
			default:
				break;
		}
    }

	PRINTF("\n");
	PRINTF("north,     %s\n", north.to_string().c_str());
	PRINTF("south,     %s\n", south.to_string().c_str());
	PRINTF("east,      %s\n", east.to_string().c_str());
	PRINTF("west,      %s\n", west.to_string().c_str());
	PRINTF("northEast, %s\n", northEast.to_string().c_str());
	PRINTF("southEast, %s\n", southEast.to_string().c_str());
	PRINTF("southWest, %s\n", southWest.to_string().c_str());
	PRINTF("northWest, %s\n", northWest.to_string().c_str());

	int answer = 0;
	answer += north.getDistanceFromOrigin();
	answer += south.getDistanceFromOrigin();
	answer += east.getDistanceFromOrigin();
	answer += west.getDistanceFromOrigin();
	answer += northEast.getDistanceFromOrigin();
	answer += southEast.getDistanceFromOrigin();
	answer += southWest.getDistanceFromOrigin();
	answer += northWest.getDistanceFromOrigin();

    std::cout << answer << std::endl;

    return 0;
}

