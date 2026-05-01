# RTProfiler - Rouviks Tiny Profiler Library
A simple header only performance measurement library for easy low effort benchmarks.

# Usage:
Just include rtprofiler.h in your main C file with RTBENCH_IMPLEMENTATION defined in a single translation unit (refer examples)
and get going!

# How it works?
The timing is done using native Windows `QueryPerformanceCounter` and POSIX `clock_gettime` to read time in nano seconds over a loop block and compute the difference to get timing.

For stack however it is an **approximation** based on the stack pointer `rsp, esp, sp` on respective platforms **ASSUMING STACK GROWS DOWNWARD**. But is good enough to determing growth rate in memory and plot for analysis.

The macro `BENCH_STACK_MSR` is where stack pointer in a function is picked on each call and compared with existing measurement to keep the max difference hence tracking the max stack depth during analysis. Hence this function must be called in the deepest function call in a recursive implementation, if branching exists then called multiple times to track the deepest stack depth during analysis and get the closest accurate result to real stack depth.

For heap, `bmalloc, brealloc, bcalloc, bfree` act as hooks to the C `malloc, realloc, calloc, free` heap allocation functions and track the allocated memory accurately.

The macros `BENCH_STACK_RST, BENCH_HEAP_RST` simply reset the internal structures to start tracking.

# Explanation:
The profiler works by creating a testbench using the BENCH(...) macro [see examples/basic_bench.c](./examples/basic_bench.c) and then run our code to test within
measurements (MEASURE_T) or attach probes to the code (BENCH_HEAP_RST, BENCH_HEAP_MSR, BENCH_STACK_RST, BENCH_STACK_MSR) to get the performance readings off of
the code in question.
I would highly recommend going through all the [examples/](./examples/) to better understand the profiler, its quite simple!

# Docs:
You can find documentation for this header in: [https://rouvik.github.io/rtprofiler.h/](https://rouvik.github.io/rtprofiler.h/)

# Examples:
You can find example snippets in the `examples/` directory.

# Some tests I did:
## Binary Search [./examples/bsearch_bench.c](./examples/bsearch_bench.c):

### Time:
![bsearch_time.png](./images/bsearch_time.png)

You can clearly observe the logarithmic trend in binary search.

**`Capturing this level of detail is possible since rtprofiler.h uses nanosecond precision timing in its implementation!`**

## Factorial recursive implementation:
```c
int fact(int n)
{
    return n == 1 ? 1 : n * fact(n - 1);
}
```
### Time:
![fact_time.png](./images/fact_time.png)

### Stack:
![fact_stack.png](./images/fact_stack.png)

## Bubble Sort:
```c
void bsort(int *arr, int n)
{
    int i, j, t;
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
```
### Time:
![bsort_time.png](./images/bsort_time.png)

> Values plotted in desmos