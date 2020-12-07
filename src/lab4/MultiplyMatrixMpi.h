#include <stdio.h>
#include <stdlib.h>
#include "../Algorithm.h"
#include <iostream>
#include <stdexcept>

#ifndef MUL_MATRIX_MPI
#define MUL_MATRIX_MPU

using namespace std;

class MultiplyMatrixMpi : public Algorithm
{
private:
    int n = 0;
    int threads = 0;
    string executable;

    string GetStdoutFromCommand(string cmd)
    {
        string data;
        FILE *stream;
        const int max_buffer = 256;
        char buffer[max_buffer];
        cmd.append(" 2>&1");

        stream = popen(cmd.c_str(), "r");
        if (stream)
        {
            while (!feof(stream))
                if (fgets(buffer, max_buffer, stream) != NULL)
                    data.append(buffer);
            pclose(stream);
        }
        return data;
    }

    string GetCmd()
    {
        stringstream stream;
        stream << "mpirun -n " << threads << " --oversubscribe " << executable << " " << n;
        auto cmd = stream.str();
        return cmd;
    }

public:
    MultiplyMatrixMpi(string executable)
    {
        this->executable = executable;
    }
    virtual string GetLabel() { return "matrix_mul_mpi"; }
    
    virtual void SetThreads(int n)
    {
        threads = n;
    }

    virtual void UpdateParam(int n)
    {
        this->n = n;
    }

    virtual void Run()
    {
        auto output = GetStdoutFromCommand(GetCmd());
        if (output != "done\n")
        {
            cout << "Command '" << GetCmd() << "' not return 'done'. Actual output:" << endl
                 << output 
                 << endl;
            throw exception();
        }
    }
};
#endif