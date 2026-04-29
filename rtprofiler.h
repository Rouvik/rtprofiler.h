/**
 * @file rtprofiler.h
 * @author Rouvik Maji (majirouvik@gmail.com)
 * @brief A simple header only performance measurement library for easy low effort benchmarks
 * @date 2026-04-24
 *
 * @copyright MIT (c) 2026
 *
 * @example basic_bench.c
 * @example fact_bench.c
 * @example fact_stack_bench.c
 * @example heap_bench.c
 * @example file_bench.c
 * @example csv_bench.c
 * @example bsearch_bench.c
 */

#ifndef RTPROFILER_INCLUDE
#define RTPROFILER_INCLUDE

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/** @cond INTERNAL_MACROS */

// conditional include for windows QueryPerformanceCounter, QueryPerformanceFrequency
#if defined(_WIN32) || defined(_WIN64)

#define __BENCH_IN_WINDOWS

#include <profileapi.h>

extern LARGE_INTEGER __bench_freq;
extern int __bench_freq_computed;
#endif

/** @endcond */

/**
 * @page config_macros Configuration Macros
 *
 * <b>RTPROFILER_IMPLEMENTATION</b><br>
 * Must be used only once in a single translation unit to include the main implementation
 *
 * <b>BENCH_OUT_TO_FILE</b><br>
 * Enables file output for BENCH(fname, start_size, end_size, incr)
 *
 * <b>BENCH_OUT_AS_CSV</b><br>
 * Enables benchmark output as CSV, read <i><b>examples/csv_bench.c</b></i> for example
 *
 * <b>BENCH_EXCLUDE_HEADER</b><br>
 * Excludes printing the header in benchmarks
 *
 * <b>BENCH_EXCLUDE_INDEX</b><br>
 * Exclude the index field in benchmark output
 */

/**
 * @brief Helper function to print contents of an int array
 *
 * @param arr The array to print
 * @param length The length of the array
 */
void bPrintArr(int *arr, int length);

/**
 * @brief Generate a random unsorted int array
 *
 * @param arr The array to generate into
 * @param length The length of array
 * @param range The max range of values: [0, range)
 */
void bGenRandArr(int *arr, int length, int range);

/**
 * @brief Generate a random sorted int array growing in steps of growth
 *
 * @param arr The array to generate into
 * @param length The length of the array
 * @param growth The amount of random steps to increase in while generating, [0, growth)
 */
void bGenRandSortedArr(int *arr, int length, int growth);

/**
 * @brief Passthrough function through malloc to collect heap allocation information, behaves same as stdlib malloc
 *
 * @param size The size of memory to allocate in bytes
 * @return void* The pointer to allocated memory
 */
void *bmalloc(size_t size);

/**
 * @brief Returns the size of memory allocated by bmalloc
 * @note This function is NULL safe
 * @warning This function works only with bmalloc, if used with normal malloc and stack pointers, it will result in invalid values
 *
 * @param ptr The pointer to get size from
 * @return size_t The size of the memory allocated in bytes
 */
size_t bGetMallocSize(void *ptr);

/**
 * @brief Passthrough function through free to collect heap allocation information, behaves same as stdlib free
 *
 * @param ptr The pointer to free
 */
void bfree(void *ptr);

/**
 * @brief External variables used to track stack bounds of current measurement, refer to BENCH_STACK_RST and BENCH_STACK_MSR
 */
extern size_t BENCH_STACK_LOW;

/**
 * @brief External variables used to track stack bounds of current measurement, refer to BENCH_STACK_RST and BENCH_STACK_MSR
 */
extern size_t BENCH_STACK_HIGH;

/**
 * @brief External variables used to track heap bounds of current measurement, refer to BENCH_HEAP_RST, bmalloc and bfree
 * TOTAL tracks the entire memory allocated since BENCH_HEAP_RST and doesnot decrease on free
 */
extern size_t BENCH_HEAP_TOTAL;

/**
 * @brief External variables used to track heap bounds of current measurement, refer to BENCH_HEAP_RST, bmalloc and bfree
 * CURRENT tracks the currently allocated memory and needed to be freed
 */
extern size_t BENCH_HEAP_CURRENT;

/** @cond INTERNAL_MACROS */
#ifdef __BENCH_IN_WINDOWS

/**
 * @brief Timing variable for storing time windows platform
 */
#define __BENCH_TIME_VAR LARGE_INTEGER

/**
 * @brief How to read the time output in windows
 */
#define __BENCH_GET_TIME(var) (QueryPerformanceCounter(&var))

#else
/**
 * @brief Timing variable for storing time posix platforms
 */
#define __BENCH_TIME_VAR struct timespec

/**
 * @brief How to read time in posix using clock_gettime, here I'm using monotonic since its the most accurate stopwatch like clock
 */
#define __BENCH_GET_TIME(var) (clock_gettime(CLOCK_MONOTONIC, &var))
#endif

/**
 * @brief Actually computes the difference of time and adds it in the total variable to be averaged later
 *
 * @param total The total time to add to
 * @param st Starting time
 * @param end Ending time
 */
void __bench_get_computed_time(double *total, __BENCH_TIME_VAR *st, __BENCH_TIME_VAR *end);

/** @endcond */

#if defined(__x86_64__)
#define __BENCH_STACK_RST_READ asm volatile("mov %%rsp, %0" : "=r"(BENCH_STACK_HIGH));
#define __BENCH_STACK_MSR_READ asm volatile("mov %%rsp, %0" : "=r"(x));
#elif defined(__i386__)
#define __BENCH_STACK_RST_READ asm volatile("mov %%esp, %0" : "=r"(BENCH_STACK_HIGH));
#define __BENCH_STACK_MSR_READ asm volatile("mov %%esp, %0" : "=r"(x));
#elif defined(__arm__) || defined(__aarch64__)
#define __BENCH_STACK_RST_READ asm volatile("mov %0, sp" : "=r"(BENCH_STACK_HIGH));
#define __BENCH_STACK_MSR_READ asm volatile("mov %0, sp" : "=r"(x));
#else
#error "Unsupported architecture, if this was a mistake try compiling with __x86_64__ or i386 or __arm__ or __aarch64__ defined"
#endif

/**
 * @brief Used to reset the stack bounds to begin with the benchmark
 */
#define BENCH_STACK_RST()                   \
	do                                      \
	{                                       \
		__BENCH_STACK_RST_READ              \
		BENCH_STACK_LOW = BENCH_STACK_HIGH; \
	} while (0);

/**
 * @brief Used to measure and record the lowest bound of the stack
 */
#define BENCH_STACK_MSR()        \
	do                           \
	{                            \
		register size_t x = 0;   \
		__BENCH_STACK_MSR_READ   \
		if (BENCH_STACK_LOW > x) \
			BENCH_STACK_LOW = x; \
	} while (0);

/**
 * @brief Used to reset the heap bounds to begin with the benchmark
 */
#define BENCH_HEAP_RST()        \
	do                          \
	{                           \
		BENCH_HEAP_TOTAL = 0;   \
		BENCH_HEAP_CURRENT = 0; \
	} while (0);

/**
 * @brief Helper condition to check if there is a memory leak in heap, works by checking if BENCH_HEAP_CURRENT != 0 after all frees,
 * non zero indicates that some memory was not freed
 */
#define IS_HEAP_LEAKY (BENCH_HEAP_CURRENT != 0)

/**
 * @brief Measures the time required for a code to run within the block averaged over avg_count iterations,
 * also takes an optional argument to add in a value in naming to prevent duplicates in code and differentiate between multiple
 * measurements within the same bench
 */
#define MEASURE_T(avg_count, ...)                                                                                                                \
	double measure_time_##__VA_ARGS__ = 0;                                                                                                       \
	__BENCH_TIME_VAR __measure_timespec_st_##__VA_ARGS__, __measure_timespec_end_##__VA_ARGS__;                                                  \
	for (int _avg_out_once = 1, _i = 0; _avg_out_once; measure_time_##__VA_ARGS__ = measure_time_##__VA_ARGS__ / (avg_count), _avg_out_once = 0) \
		for (; _i < (avg_count); _i++)                                                                                                           \
			for (int _once = (__BENCH_GET_TIME(__measure_timespec_st_##__VA_ARGS__), 1); _once; __BENCH_GET_TIME(__measure_timespec_end_##__VA_ARGS__), __bench_get_computed_time(&measure_time_##__VA_ARGS__, &__measure_timespec_st_##__VA_ARGS__, &__measure_timespec_end_##__VA_ARGS__), _once = 0)

/**
 * @brief Helper macro used to get the time variable generated from a measurement
 */
#define T_VAL(...) (measure_time_##__VA_ARGS__)

#ifdef BENCH_OUT_AS_CSV
#ifdef BENCH_EXCLUDE_INDEX
#define BENCH_FILE_HEADER "n,time(ms),stack,heap\n"
#define BENCH_FILE_OUTPUT_FMT "%d,%lf,%zu,%zu\n"
#else
#define BENCH_FILE_HEADER "index,n,time(ms),stack,heap\n"
#define BENCH_FILE_OUTPUT_FMT "%d,%d,%lf,%zu,%zu\n"
#endif
#else
/** @cond INTERNAL_MACROS */
#ifdef BENCH_EXCLUDE_INDEX
#define BENCH_FILE_HEADER "n\t\ttime(ms)\t\tstack\t\theap\n"
#define BENCH_FILE_OUTPUT_FMT "%d\t\t%lf\t\t%zu\t\t%zu\n"
#else
#define BENCH_FILE_HEADER "index\t\tn\t\ttime(ms)\t\tstack\t\theap\n"
#define BENCH_FILE_OUTPUT_FMT "%d\t\t%d\t\t%lf\t\t%zu\t\t%zu\n"
#endif
/** @endcond */
#endif

#ifdef BENCH_EXCLUDE_HEADER
#undef BENCH_FILE_HEADER
#define BENCH_FILE_HEADER ""
#endif

#ifdef BENCH_OUT_TO_FILE

/**
 * @brief The main loop to run bench marks over a set of samples, the samples are generated using the system provided parameters such as:
 * n = sample size, i = sample index <br>
 * Samples generation can be tweaked using parameters such as start_size, end_size, incr <br>
 * @note This version also takes an optional string fname to write the generated output to
 *
 */
#define BENCH(fname, start_size, end_size, incr)                                                       \
	for (FILE *fp = fopen(fname, "w"); fp && (fputs(BENCH_FILE_HEADER, fp), 1); fclose(fp), fp = NULL) \
		for (int i = 1, n = (start_size); n <= (end_size); incr, i++)

#ifdef BENCH_EXCLUDE_INDEX
#define PRINT_MEASURE(...) \
	fprintf(fp, BENCH_FILE_OUTPUT_FMT, n, measure_time_##__VA_ARGS__, BENCH_STACK_HIGH - BENCH_STACK_LOW, BENCH_HEAP_TOTAL);
#else
/**
 * @brief Prints the measurements to the file provided in BENCH fname
 * @note Use the BENCH_EXCLUDE_INDEX flag to exclude the index in output
 */
#define PRINT_MEASURE(...) \
	fprintf(fp, BENCH_FILE_OUTPUT_FMT, i, n, measure_time_##__VA_ARGS__, BENCH_STACK_HIGH - BENCH_STACK_LOW, BENCH_HEAP_TOTAL);
#endif
#else

/**
 * @brief The main loop to run bench marks over a set of samples, the samples are generated using the system provided parameters such as:
 * n = sample size, i = sample index <br>
 * Samples generation can be tweaked using parameters such as start_size, end_size, incr
 *
 * @note The name BENCH simply stands for TESTBENCH, we simply make a testbench and run our code within some MEASUREMENTS and attach probes to it (BENCH_HEAP_RST, BENCH_HEAP_MSR) to get measurements.
 *
 * There is also a BENCH version with file output selected using the BENCH_OUT_TO_FILE definition as BENCH(fname, start_size, end_size, incr)
 * where fname is the filename to output to
 *
 */
#define BENCH(start_size, end_size, incr) \
	printf(BENCH_FILE_HEADER);            \
	for (int i = 1, n = (start_size); n <= (end_size); incr, i++)

#ifdef BENCH_EXCLUDE_INDEX
#define PRINT_MEASURE(...) \
	printf(BENCH_FILE_OUTPUT_FMT, n, measure_time_##__VA_ARGS__, BENCH_STACK_HIGH - BENCH_STACK_LOW, BENCH_HEAP_TOTAL);
#else
/**
 * @brief Prints the measurements to stdout or a file as per BENCH_OUT_TO_FILE definition
 * @note Use the BENCH_EXCLUDE_INDEX flag to exclude the index in output
 */
#define PRINT_MEASURE(...) \
	printf(BENCH_FILE_OUTPUT_FMT, i, n, measure_time_##__VA_ARGS__, BENCH_STACK_HIGH - BENCH_STACK_LOW, BENCH_HEAP_TOTAL);
#endif
#endif

// -------------------------------- IMPLEMENTATION --------------------------------

#ifdef RTPROFILER_IMPLEMENTATION

void bPrintArr(int *arr, int length)
{
	for (int i = 0; i < length; i++)
	{
		printf("%d ", arr[i]);
	}
}
void bGenRandArr(int *arr, int length, int range)
{
	srand(time(NULL));
	for (int i = 0; i < length; i++)
	{
		arr[i] = rand() % range;
	}
}

void bGenRandSortedArr(int *arr, int length, int growth)
{
	srand(time(NULL));

	int s = 0;
	for (int i = 0; i < length; i++)
	{
		s += rand() % growth;
		arr[i] = s;
	}
}

void *bmalloc(size_t size)
{
	size_t *ptr = malloc(size + sizeof(size_t));
	if (!ptr)
	{
		return NULL;
	}

	*ptr = size;
	BENCH_HEAP_TOTAL += size;
	BENCH_HEAP_CURRENT += size;
	return ptr + 1;
}

size_t bGetMallocSize(void *ptr)
{
	if (!ptr)
	{
		return 0;
	}

	size_t *size = (size_t *)ptr - 1;
	return *size;
}

void bfree(void *ptr)
{
	if (!ptr)
	{
		return;
	}

	size_t *alloc_size = (size_t *)ptr - 1;
	BENCH_HEAP_CURRENT -= *alloc_size;
	free(alloc_size);
}

#ifdef __BENCH_IN_WINDOWS
void __bench_get_computed_time(double *total, __BENCH_TIME_VAR *st, __BENCH_TIME_VAR *end)
{
	if (!__bench_freq_computed) // avoiding redundant queries
	{
		__bench_freq_computed = 1;
		QueryPerformanceFrequency(&__bench_freq);
	}

	*total += ((double)(end->QuadPart - st->QuadPart) * 1e3) / __bench_freq.QuadPart;
}

LARGE_INTEGER __bench_freq;
int __bench_freq_computed = 0;

#else

void __bench_get_computed_time(double *total, __BENCH_TIME_VAR *st, __BENCH_TIME_VAR *end)
{
	*total += ((double)(end->tv_sec - st->tv_sec) * 1e3) + ((double)(end->tv_nsec - st->tv_nsec) * 1e-6);
}

#endif

size_t BENCH_STACK_LOW, BENCH_STACK_HIGH;
size_t BENCH_HEAP_TOTAL, BENCH_HEAP_CURRENT;

#endif

#endif // RTPROFILER_INCLUDE