#include <iostream>
#include "../Algorithm.h"
#include <valarray>

using namespace std;

#ifndef FLOYD_WARSHALL_H
#define FLOYD_WARSHALL_H

class FloydWarshallBase : public Algorithm
{
protected:
    int n = 0;
    valarray<double> *matrix = nullptr;

public:
    virtual void UpdateParam(int n)
    {
        if (matrix != nullptr)
            delete matrix;
        matrix = new valarray<double>(n * n);
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                if (i == j)
                {
                    (*matrix)[i * n + j] = (double) 0;
                }
                else
                {
                    auto r = rand();
                    double f = -300.0 + (double) r / RAND_MAX * 600.0;
                    (*matrix)[i * n + j] = f;
                }
            }
        }
        n = n;
    }
};

#endif