#include <stdio.h>
#include <omp.h>
#include <chrono>
#include <valarray>

#include "Algorithm.h"
#include "Benchmark.h"
#include "lab1/MultiplyLinear.h"
#include "lab1/MultiplyParallel.h"

using namespace std;
using namespace chrono;

int main()
{
    #ifdef _OPENMP
    printf("[*]Compiled with OpenMP\n");
    printf("[*]Maximum number of threads: %d\n", omp_get_max_threads());
    #endif
    valarray<int> test_amount = {500, 600, 700, 800, 900, 1000, 5000, 10000};
    valarray<int> thread_count = {2, 4, 8, 16, 32};
    auto test_repeat = 50;
    auto p = new MultiplyParallel();
    auto l = new MultiplyLinear();
    omp_set_dynamic(0);
    for (size_t ti = 0; ti < thread_count.size(); ti++)
    {
        omp_set_num_threads(thread_count[ti]);
        for (size_t i = 0; i < test_amount.size(); i++)
        {
            int n = test_amount[i];
            Benchmark::bench(thread_count[ti], n, test_repeat, p);
        }
    }
    for (size_t i = 0; i < test_amount.size(); i++)
    {
        int n = test_amount[i];
        Benchmark::bench(1, n, test_repeat, l);
    }
    
}
