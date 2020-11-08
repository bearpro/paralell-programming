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
