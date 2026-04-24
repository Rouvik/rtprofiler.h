#include <stdio.h>

#define RTBENCH_IMPLEMENTATION
#include "../rtbench.h"

void someHeapFunction(int n)
{
    int *arr = bmalloc(n * sizeof(int));
    
    for (int i = 0; i < n; i++)
    {
        arr[i] = i;
    }
    
    bfree(arr);
}

int main()
{
    BENCH(10, 1000, n *= 10)
    {
        MEASURE_T(100)
        {
            BENCH_HEAP_RST();
            someHeapFunction(n);
        }

        PRINT_MEASURE();
    }

    return 0;
}

/*
Expected output:
index   n       ticks   stack   heap
1       10      0       0       40
2       100     1       0       400
3       1000    5       0       4000
*/