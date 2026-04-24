#include <stdio.h>

#define RTBENCH_IMPLEMENTATION
#include "../rtbench.h"

int fact(int n)
{
    return n == 1 ? 1 : n * fact(n - 1);
}

int main()
{
    BENCH(10, 100000, n *= 10)
    {
        int volatile out;
        MEASURE_T(100)  // measure the code within the block for 100 iterations and average the time
        {
            out = fact(n);
        }

        PRINT_MEASURE();
    }

    return 0;
}

/*
Expected output:
index   n       ticks   stack   heap
1       10      0       0       0
2       100     1       0       0
3       1000    12      0       0
4       10000   104     0       0
5       100000  617     0       0
*/