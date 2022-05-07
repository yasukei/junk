#include <stdio.h>
#include <random>

int main()
{
	std::random_device seed_gen;
	std::mt19937 engine(seed_gen());

	constexpr std::size_t bits = std::numeric_limits<double>::digits;

	int accumlator = 10;
	int num = 100;
	for(int i = 0; i < num; ++i)
	{
		double result = std::generate_canonical<double, bits>(engine);
		if(result < 0.5)
		{
			accumlator -= 1;
		}
		else
		{
			accumlator += 1;
		}

		for(int j = 0; j < accumlator; ++j)
		{
			printf(" ");
		}
		printf("|\n");
		//printf("%lf\n", accumlator);
	}

	return 0;
}

