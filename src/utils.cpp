#include "utils.hpp"

int weight(int n)
{
	int w = 0;
	while (n)
	{
		w += n & 1;
		n >>= 1;
	}
	return w;
}
