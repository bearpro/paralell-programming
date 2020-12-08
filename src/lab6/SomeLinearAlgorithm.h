#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include "../Algorithm.h"

#ifndef SOME_LINEAR_ALGORITHM
#define SOME_LINEAR_ALGORITHM

using namespace std;
class SomeLinearAlgorithm : public Algorithm
{
private:
    static const int n = 100;
    static const int m = 100;

public:
    virtual void UpdateParam(int n) {}
    virtual void SetThreads(int n) {}
    virtual string GetLabel() { return "some_linear"; }
   
    virtual void Run()
    {
        double a[n][m];
        double b[n][m];
        int i, j;
        FILE *ff;
        for (i = 0; i < n; i++)
        {
            for (j = 0; j < m; j++)
            {
                a[i][j] = 10 * i + j;
                b[i][j] = 0.;
            }
        }
        for (i = 0; i < n; i++)
        {
            for (j = 0; j < m; j++)
            {
                a[i][j] = sin(0.00001 * a[i][j]);
            }
        }
        for (i = 0; i < n - 4; i++)
        {
            for (j = 1; j < m; j++)
            {
                b[i][j] = a[i + 4][j - 1] * 1.5;
            }
        }
        ff = fopen("result.txt", "w");
        for (i = 0; i < n; i++)
        {
            for (j = 0; j < m; j++)
            {
                fprintf(ff, "%f ", b[i][j]);
            }
            fprintf(ff, "\n");
        }
        fclose(ff);
    }
};
#endif