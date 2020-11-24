#include <stdio.h>
#include <iostream>
#include <chrono>
#include <valarray>
#include <date/date.h>
#include <omp.h>
#include "Algorithm.h"

#ifndef Benchmark_H
#define Benchmark_H

using namespace std;
using namespace chrono;
using namespace date;

class Benchmark {
public: 
    static void bench(int threads, int n, int times, Algorithm *algorithm)
    {
        nanoseconds total_elapsed = nanoseconds{0};
        for (int i = 0; i < times; i++)
        {
            algorithm->UpdateParam(n);
            auto started = system_clock::now();
            algorithm->Run();
            auto elapsed = system_clock::now() - started;
            total_elapsed += elapsed;
        }
        cout << algorithm->GetLabel()                                       // Название алгоритма
            << ":" << threads                                              // Количество потоков
            << ":" << n                                                    // Размер входных данных
            << ":" << (floor<milliseconds>(total_elapsed / times).count()) // Время выполнения
            << endl;
    }
};
#endif