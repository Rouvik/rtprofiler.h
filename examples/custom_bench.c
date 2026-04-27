#include <stdio.h>

#define RTBENCH_IMPLEMENTATION
#define BENCH_IGNORE_HEADER // ignore printing the benchmark header
#include "../rtprofiler.h"

int fact(int n)
{
    BENCH_STACK_MSR();
    return n == 1 ? 1 : n * fact(n - 1);
}

int main(int argc, const char *argv[])
{
    printf("n\ttime\tstack\n");
    BENCH(10, 10000, n *= 10)
    {
        int volatile out;

        MEASURE_T(1, mymeasure)
        {
            BENCH_STACK_RST();
            out = fact(n);
        }

        printf("%d\t%ld\t%ld\n", n, T_VAL(mymeasure), BENCH_STACK_HIGH - BENCH_STACK_LOW); // custom printf statement
    }
    return 0;
}