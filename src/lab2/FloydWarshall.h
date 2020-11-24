#include <iostream>
#include "../Algorithm.h"
#include <valarray>

using namespace std;

#ifndef FLOYD_WARSHALL_H
#define FLOYD_WARSHALL_H

class FloydWarshall : public Algorithm
{
protected:
    valarray<double> matrix;
    int n;

public:
    virtual string GetLabel() { return "FloydWarshall"; }

    virtual void UpdateParam(int n)
    {
        matrix = valarray<double>(n * n);
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                if (i == j)
                {
                    matrix[i * n + j] = 0;
                }
                else
                {
                    matrix[i * n + j] = rand() * 0.01;
                }
            }
        }
        this->n = n;
    }

    virtual void Run()
    {
        for (int k = 0; k < n; k++)
        {
            for (int i = 0; i < n; i++)
            {
                auto v = matrix[i * n + k];
                for (int j = 0; j < n; ++j)
                {
                    auto val = v + matrix[k * n + j];
                    if (matrix[i * n + j] > val)
                    {
                        matrix[i * n + j] = val;
                    }
                }
            }
        }
    }
    
    valarray<double> Result()
    {
        return matrix;
    }
};

#endif