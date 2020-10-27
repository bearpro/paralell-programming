#include "benchmark.h"

valarray<int> random_matrix(int n)
{
    auto matrix = valarray<int>(n * n);
    for (int i = 0; i < n * n; i++)
    {
        matrix[i] = rand();
    }
    return matrix;
}

valarray<int> random_vector(int n)
{
    auto vector = valarray<int>(n);
    for (int i = 0; i < n; i++)
    {
        vector[i] = rand();
    }
    return vector;
}

void bench(string label, int threads, int n, int times, valarray<int> function(valarray<int>, valarray<int>))
{
    nanoseconds total_elapsed = nanoseconds {0};
    for (int i = 0; i < times; i++)
    {
        auto matrix = random_matrix(n);
        auto vector = random_vector(n);
        auto started = system_clock::now();
        function(matrix, vector);
        auto elapsed = system_clock::now() - started;
        total_elapsed += elapsed;
    }
    cout << label
         << ":" << threads
         << ":" << n
         << ":" << floor<milliseconds>(total_elapsed).count()
         << endl;
    // cout << "Algorithm: "
    //      << label
    //      << "|N: " << n
    //      << "|Total time: " << floor<milliseconds>(total_elapsed)
    //      << endl;
}