#include <stdio.h>
#include <iostream>
#include <chrono>
#include <valarray>
#include <date/date.h>
#include <omp.h>

using namespace std;
using namespace chrono;
using namespace date;

void bench(string label, int threads, int n, int times, valarray<int> function(valarray<int>, valarray<int>));