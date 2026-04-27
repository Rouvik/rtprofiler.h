#include <stdio.h>

#define RTPROFILER_IMPLEMENTATION
#include "../rtprofiler.h"

int fact(int n)
{
    BENCH_STACK_MSR();  // measure stack depth inside function to capture usage and store the minimum value
    return n == 1 ? 1 : n * fact(n - 1);
}

int main()
{
    BENCH(10, 100000, n *= 10)
    {
        int volatile out;
        MEASURE_T(100)
        {
            BENCH_STACK_RST();   // get current stack position to begin measuring from
            out = fact(n);
        }

        PRINT_MEASURE();
    }

    return 0;
}

/*
Expected output:
index   n       ticks   stack   heap
1       10      0       480     0
2       100     1       4800    0
3       1000    15      48000   0
4       10000   134     480000  0
5       100000  795     4800000 0
*/