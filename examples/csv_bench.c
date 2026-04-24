#include <stdio.h>

#define RTBENCH_IMPLEMENTATION

// Add the BENCH_OUT_TO_FILE definition to change macros to their file output variant
#define BENCH_OUT_TO_FILE
#define BENCH_OUT_AS_CSV    // outputs in CSV format
#define BENCH_EXCLUDE_INDEX // exclude the index field, you can now paste the output directly in desmos to compare other fields against n
#include "../rtbench.h"

int fact(int n)
{
    BENCH_STACK_MSR();
    return n == 1 ? 1 : n * fact(n - 1);
}

int main()
{
    // the BENCH macro now takes an additional output filename as its first argument
    BENCH("out.csv", 10, 10000, n += 50)
    {
        int volatile out;
        MEASURE_T(100)
        {
            BENCH_STACK_RST();
            out = fact(n);
        }

        PRINT_MEASURE();
    }

    return 0;
}

/*
Expected output (out.txt):
n,ticks,stack,heap
10,0,480,0
60,1,2880,0
110,1,5280,0
160,2,7680,0
210,3,10080,0
260,3,12480,0
310,4,14880,0
.............
9660,45,463680,0
9710,46,466080,0
9760,45,468480,0
9810,46,470880,0
9860,46,473280,0
9910,48,475680,0
9960,47,478080,0
*/