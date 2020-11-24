#include <omp.h>
#include <chrono>
#include <valarray>

#include "../Algorithm.h"

using namespace std;

#ifndef MULTIPLY_H
#define MULTIPLY_H

class Multiply : public Algorithm
{
protected:
    valarray<int> matrix;
    valarray<int> vector;
public:
    virtual void UpdateParam(int n)
    {
        matrix = valarray<int>(n * n);
        for (int i = 0; i < n * n; i++)
        {
            matrix[i] = rand();
        }
        vector = valarray<int>(n);
        for (int i = 0; i < n; i++)
        {
            vector[i] = rand();
        }
    }
};

#endif