#include <stdio.h>
#include <omp.h>
#include <chrono>
#include <valarray>

#include "benchmark.h"
#include "multiply.h"

using namespace std;
using namespace chrono;


void print(valarray<int> matrix, int w, int h)
{
    for (size_t i = 0; i < matrix.size(); i++)
    {
        if (i % w == 0)
            printf("\n");
        printf("%d ", matrix[i]);
    }
    printf("\n^^^^^^^^^^\n");
}

void test()
{
    valarray<int> m = {2, 4, 0, -2, 1, 3, -1, 0, 1};
    valarray<int> v = {1, 2, -1};
    print(m, 3, 3);
    print(multiply(m, v), 1, 3);
    print(multiply_parallel(m, v), 1, 3);
    print(multiply_parallel_dumb(m, v), 1, 3);
}

int main()
{
    #ifdef _OPENMP
    printf("[*]Compiled with OpenMP\n");
    printf("[*]Maximum number of threads: %d\n", omp_get_max_threads());
    #endif
    // test();
    // valarray<int> test_amount = {3, 9, 30, 100, 1000, 10000, 100000};
    valarray<int> test_amount = {500, 600, 700, 800, 900, 1000};
    valarray<int> thread_count = {2, 4, 8, 16, 32};
    auto test_repeat = 8;
    omp_set_dynamic(0);
    for (size_t ti = 0; ti < thread_count.size(); ti++)
    {
        omp_set_num_threads(thread_count[ti]);
        for (size_t i = 0; i < test_amount.size(); i++)
        {
            int n = test_amount[i];
            bench("single", thread_count[ti], n, test_repeat, multiply);
            bench("parallel", thread_count[ti], n, test_repeat, multiply_parallel);
            //bench("mpd", thread_count[ti], n, test_repeat, multiply_parallel_dumb);
        }
    }
}
