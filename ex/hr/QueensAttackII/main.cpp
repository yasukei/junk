#include <bits/stdc++.h>

#define printf

int main(void)
{
    int n, k;
    int rq, cq;
    std::cin >> n >> k;    
    std::cin >> rq >> cq;

    std::vector<std::bitset<100*1000>> obstacles;
    obstacles.resize(n+1);
    //for(int i = 0; i <= n; ++i)
    //{
    //    obstacles[i].resize(n+1);    
    //}
    int r, c;
    for(int i = 0; i < k; ++i)
    {
        std::cin >> r >> c;
        //obstacles[r][c] = true;
        obstacles[r].set(c);
    }

	std::atomic<int> numofCanAttack(0);

    // Up
	std::thread tUp([&](){
		for(int i = rq + 1, j = cq; i <= n; ++i)
		{
			if(obstacles[i][j])
			{
				break;
			}
			printf("i: [%d], j: [%d]\n", i, j);
			numofCanAttack++;
		}
	});

    // Down
	std::thread tDown([&](){
		for(int i = rq - 1, j = cq; i >= 1; --i)
		{
			if(obstacles[i][j])
			{
				break;
			}
			printf("i: [%d], j: [%d]\n", i, j);
			numofCanAttack++;
		}
	});

    // Right
	std::thread tRight([&](){
		for(int i = rq, j = cq + 1; j <= n; ++j)
		{
			if(obstacles[i][j])
			{
				break;
			}
			printf("i: [%d], j: [%d]\n", i, j);
			numofCanAttack++;
		}
	});

    // Left
	std::thread tLeft([&](){
		for(int i = rq, j = cq - 1; j >= 1; --j)
		{
			if(obstacles[i][j])
			{
				break;
			}
			printf("i: [%d], j: [%d]\n", i, j);
			numofCanAttack++;
		}
	});

    // Right-Up
	std::thread tRightUp([&](){
		for(int i = rq + 1, j = cq + 1; i <= n && j <= n; ++i, ++j)
		{
			if(obstacles[i][j])
			{
				break;
			}
			printf("i: [%d], j: [%d]\n", i, j);
			numofCanAttack++;
		}
	});

	tUp.join();
	tDown.join();
	tRight.join();
	tLeft.join();

    // Right-Down
	std::thread tRightDown([&](){
		for(int i = rq + 1, j = cq - 1; i <= n && j >= 1; ++i, --j)
		{
			if(obstacles[i][j])
			{
				break;
			}
			printf("i: [%d], j: [%d]\n", i, j);
			numofCanAttack++;
		}
	});

    // Left-Down
	std::thread tLeftDown([&](){
		for(int i = rq - 1, j = cq - 1; i >= 1 && j >= 1; --i, --j)
		{
			if(obstacles[i][j])
			{
				break;
			}
			printf("i: [%d], j: [%d]\n", i, j);
			numofCanAttack++;
		}
	});

    // Left-Up
	std::thread tLeftUp([&](){
		for(int i = rq - 1, j = cq + 1; i >= 1 && j <= n; --i, ++j)
		{
			if(obstacles[i][j])
			{
				break;
			}
			printf("i: [%d], j: [%d]\n", i, j);
			numofCanAttack++;
		}
	});

	tRightUp.join();
	tRightDown.join();
	tLeftDown.join();
	tLeftUp.join();

    std::cout << numofCanAttack << std::endl;

    return 0;
}

