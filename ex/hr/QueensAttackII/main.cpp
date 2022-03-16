#include <bits/stdc++.h>

/*
	- multi-threading
	- convert values in S into indirect index

 */

#include <bits/stdc++.h>

#define printf

int main(void)
{
    int n, k;
    int rq, cq;
    std::cin >> n >> k;    
    std::cin >> rq >> cq;

    std::vector<std::vector<bool>> obstacles;
    obstacles.resize(n+1);
    for(int i = 0; i <= n; ++i)
    {
        obstacles[i].resize(n+1);    
    }
    int r, c;
    for(int i = 0; i < k; ++i)
    {
        std::cin >> r >> c;
        obstacles[r][c] = true;
    }

    int numofCanAttack = 0;
    // Up
    for(int i = rq + 1, j = cq; i <= n; ++i)
    {
        if(obstacles[i][j])
        {
            break;
        }
        printf("i: [%d], j: [%d]\n", i, j);
        numofCanAttack++;
    }
    // Down
    for(int i = rq - 1, j = cq; i >= 1; --i)
    {
        if(obstacles[i][j])
        {
            break;
        }
        printf("i: [%d], j: [%d]\n", i, j);
        numofCanAttack++;
    }
    // Right
    for(int i = rq, j = cq + 1; j <= n; ++j)
    {
        if(obstacles[i][j])
        {
            break;
        }
        printf("i: [%d], j: [%d]\n", i, j);
        numofCanAttack++;
    }
    // Left
    for(int i = rq, j = cq - 1; j >= 1; --j)
    {
        if(obstacles[i][j])
        {
            break;
        }
        printf("i: [%d], j: [%d]\n", i, j);
        numofCanAttack++;
    }

    // Right-Up
    for(int i = rq + 1, j = cq + 1; i <= n && j <= n; ++i, ++j)
    {
        if(obstacles[i][j])
        {
            break;
        }
        printf("i: [%d], j: [%d]\n", i, j);
        numofCanAttack++;
    }
    // Right-Down
    for(int i = rq + 1, j = cq - 1; i <= n && j >= 1; ++i, --j)
    {
        if(obstacles[i][j])
        {
            break;
        }
        printf("i: [%d], j: [%d]\n", i, j);
        numofCanAttack++;
    }
    // Left-Down
    for(int i = rq - 1, j = cq - 1; i >= 1 && j >= 1; --i, --j)
    {
        if(obstacles[i][j])
        {
            break;
        }
        printf("i: [%d], j: [%d]\n", i, j);
        numofCanAttack++;
    }
    // Left-Up
    for(int i = rq - 1, j = cq + 1; i >= 1 && j <= n; --i, ++j)
    {
        if(obstacles[i][j])
        {
            break;
        }
        printf("i: [%d], j: [%d]\n", i, j);
        numofCanAttack++;
    }

    std::cout << numofCanAttack << std::endl;

    return 0;
}

