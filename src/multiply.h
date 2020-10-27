#include <omp.h>
#include <chrono>
#include <valarray>

using namespace std;

valarray<int> multiply(valarray<int> matrix, valarray<int> vector);

valarray<int> multiply_parallel(valarray<int> matrix, valarray<int> vector);

valarray<int> multiply_parallel_dumb(valarray<int> matrix, valarray<int> vector);