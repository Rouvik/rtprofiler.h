#include <stdio.h>

#define RTPROFILER_IMPLEMENTATION

// Add the BENCH_OUT_TO_FILE definition to change macros to their file output variant
#define BENCH_OUT_TO_FILE
#define BENCH_OUT_AS_CSV    // outputs in CSV format
#define BENCH_EXCLUDE_INDEX // exclude the index field, you can now paste the output directly in desmos to compare other fields against n
#include "../rtprofiler.h"

void bsort(int *arr, int n)
{
    int i, j, t;
    BENCH_STACK_MSR();

    for (i = 0; i < n - 1; i++)
    {
        for (j = 0; j < n - i - 1; j++)
        {
            if (arr[j] < arr[j + 1])
            {
                t = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = t;
            }
        }
    }
}

int main()
{
    // the BENCH macro now takes an additional output filename as its first argument
    BENCH("out.csv", 10, 10000, n *= 10)
    {
        int arr[n];
        bGenRandSortedArr(arr, n, 100);

        MEASURE_T(100)
        {
            BENCH_STACK_RST();
            bsort(arr, n);
        }

        PRINT_MEASURE();
    }

    return 0;
}

/*
Expected output (out.csv):
n,ticks,stack,heap
10,1,24,0
100,30,24,0
1000,1032,24,0
10000,92555,24,0
*/