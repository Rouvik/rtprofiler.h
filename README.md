# RTBench - Rouviks Tiny Benchmarking Library
A simple header only performance measurement library for easy low effort benchmarks.

# Usage:
Just include rtbench.h in your main C file with RTBENCH_IMPLEMENTATION defined in a single translation unit (refer examples)
and get going!


# Docs:
You can find documentation for this header in: [https://rouvik.github.io/RTBench/](https://rouvik.github.io/RTBench/)

# Examples:
You can find example snippets in the `examples/` directory.

# Some tests I did:
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