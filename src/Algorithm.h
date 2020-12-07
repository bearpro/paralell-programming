#include <iostream>

#ifndef Algorithm_h
#define Algorithm_h

using namespace std;

class Algorithm
{
public:
    virtual void Run() = 0;
    virtual void UpdateParam(int n) = 0;
    virtual void SetThreads(int n) { }
    virtual string GetLabel() = 0;
    virtual ~Algorithm(){};
};
#endif