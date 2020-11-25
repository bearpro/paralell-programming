#include <iostream>
#include "FloydWarshallBase.h"
#include <valarray>

using namespace std;

#ifndef FLOYD_WARSHALL_LINEAR_H
#define FLOYD_WARSHALL_LINEAR_H

class FloydWarshallLinear : public FloydWarshallBase
{
public:
    virtual string GetLabel() { return "floydwarshall_linear"; }

    virtual void Run()
    {
        for (int k = 0; k < n; k++)
        {
            for (int i = 0; i < n; i++)
            {
                auto v = (*matrix)[i * n + k];
                for (int j = 0; j < n; ++j)
                {
                    auto val = v + (*matrix)[k * n + j];
                    if ((*matrix)[i * n + j] > val)
                    {
                        (*matrix)[i * n + j] = val;
                    }
                }
            }
        }
    }
};

#endif