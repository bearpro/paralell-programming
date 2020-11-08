#include "multiply.h"

valarray<int> multiply(valarray<int> matrix, valarray<int> vector)
{
    auto n = vector.size();
    auto result = valarray<int>(n);
    for (size_t i = 0; i < matrix.size(); i++)
    {
        auto j = i % n;
        int matrix_item = matrix[i];
        int vector_item = vector[j];
        result[i / n] += vector_item * matrix_item;
    }
    return result;
}

valarray<int> multiply_parallel(valarray<int> matrix, valarray<int> vector)
{
    auto n = vector.size();
    auto result = valarray<int>(n);
    #pragma omp parallel for shared(result, n, matrix, vector)
    for (size_t i = 0; i < matrix.size(); i++)
    {
        auto j = i % n;
        int matrix_item = matrix[i];
        int vector_item = vector[j];
        int *value = &result[i / n];
        auto increment = vector_item * matrix_item;
        #pragma omp atomic
        *value += increment;
    }
    return result;
}

valarray<int> multiply_parallel_dumb(valarray<int> matrix, valarray<int> vector)
{
    // Количество потоков, которые будут вычислять.
    int thread_count = 0;
    if (omp_get_num_threads() <= (int)matrix.size())
        thread_count = omp_get_num_threads();
    else
        thread_count = matrix.size();

    // Количество элементов, обсчитываемых каждым потоком.
    int counted_by_single_thread = matrix.size() / thread_count;

    // Результаты вычислений отдельных потоков.
    auto partial_results = valarray<valarray<int>>(thread_count);

    auto n = vector.size();

    #pragma omp parallel for shared(thread_count, counted_by_single_thread, partial_results, n)
    for (int thread_index = 0; thread_index < thread_count; thread_index++)
    {
        auto thread_result = valarray<int>(n);
        auto first_index = thread_index * counted_by_single_thread;
        auto last_index = (thread_index + 1) * counted_by_single_thread;
        if (thread_index == thread_count - 1)
            last_index += matrix.size() % thread_count;
        for (int i = first_index; i < last_index; i++)
        {
            auto j = i % n;
            int matrix_item = matrix[i];
            int vector_item = vector[j];
            thread_result[i / n] += vector_item * matrix_item;
        }
        partial_results[thread_index] = thread_result;
    }

    auto result = valarray<int>(n);
    result = partial_results.sum();
    return result;
}
