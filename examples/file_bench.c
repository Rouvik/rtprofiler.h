#include <stdio.h>

#define RTBENCH_IMPLEMENTATION

// Add the BENCH_OUT_TO_FILE definition to change macros to their file output variant
#define BENCH_OUT_TO_FILE
#include "../rtbench.h"

void someHeapFunction(int n)
{
    int *arr = bmalloc(n * sizeof(int));
    
    for (int i = 0; i < n; i++)
    {
        arr[i] = i;
    }
 
    printf("Size: %ld\n", bGetMallocSize(arr)); // using the bGetMallocSize to read malloced info from heap storage

    bfree(arr);
}

int main()
{
    // the BENCH macro now takes an additional output filename as its first argument
    BENCH("out.txt", 10, 1000, n *= 10)
    {
        MEASURE_T(1)
        {
            BENCH_HEAP_RST();
            someHeapFunction(n);
        }

        PRINT_MEASURE();
    }

    return 0;
}

/*
Expected output (stdout):
Size: 40
Size: 400
Size: 4000

Expected output (out.txt):
index	n	    ticks	stack	heap
1	    10	    35	    0	    40
2	    100	    7	    0	    400
3	    1000	19	    0	    4000
*/