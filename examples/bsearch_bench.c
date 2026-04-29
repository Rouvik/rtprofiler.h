#include <stdio.h>

#define RTPROFILER_IMPLEMENTATION

#define BENCH_OUT_TO_FILE
#define BENCH_OUT_AS_CSV
#define BENCH_EXCLUDE_INDEX
#define BENCH_EXCLUDE_HEADER
#include "../rtprofiler.h"

// custom binary search implementation
int rbsearch(int *arr, int length, int target)
{
	int i = 0, j = length - 1, mid;

	BENCH_STACK_MSR();

	while (i <= j)
	{
		mid = (i + j) / 2;
		if (arr[mid] == target)
			return mid;

		if (target < arr[mid])
		{
			j = mid - 1;
		}
		else
		{
			i = mid + 1;
		}
	}

	return -999;
}

int main()
{
    const int max_n = 1000000;
    int arr[max_n];
    bGenRandSortedArr(arr, max_n, 100);

    // the BENCH macro now takes an additional output filename as its first argument
    BENCH("bench.csv", 10, max_n, n += 500)
    {
        int rand_elem = arr[rand() % n];

        int volatile out;
        MEASURE_T(100000)
        {
            BENCH_STACK_RST();
            out = rbsearch(arr, n, rand_elem);
        }

        if (arr[out] != rand_elem)
        {
            printf("FATAL ERROR mismatch: %d %d %d", arr[out], rand_elem, out);
        }

        PRINT_MEASURE();
    }

    return 0;
}

/*
Expected output (out.csv):
10,0.000022,24,0
510,0.000040,24,0
1010,0.000037,24,0
.......
998510,0.000061,24,0
999010,0.000062,24,0
999510,0.000062,24,0
*/