#include <omp.h>
#include <chrono>
#include <valarray>

#include "Multiply.h"

using namespace std;

class MultiplyLinear : public Multiply
{
public:
    virtual string GetLabel() { return "MultiplyLinear"; }
    virtual void Run()
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
    }
};