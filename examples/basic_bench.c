#include <stdio.h>

#define RTBENCH_IMPLEMENTATION
#include "../rtbench.h"

int main()
{
    // generate a benchmark loop with samples of size [10, 100, 1000]
    BENCH(10, 1000, n *= 10)
    {
        MEASURE_T(1);
        PRINT_MEASURE();
    }

    return 0;
}

/*
Expected output:
index   n       ticks   stack   heap
1       10      2       0       0
2       100     2       0       0
3       1000    1       0       0
*/