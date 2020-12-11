#include <stdio.h>
#include <stdlib.h>
#include "../Algorithm.h"
#include <iostream>
#include <stdexcept>
#include <fstream>

#ifndef FLOYD_WARSHALL_MPI
#define FLOYD_WARSHALL_MPU

using namespace std;

class FloydWarshallMpi : public Algorithm
{
private:
    int n = 0;
    int threads = 0;
    string executable;
    const string filename = "matrix";

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

    void pure()
    {
        ifstream file(filename);
        if (file.good())
        {
            file.close();
            remove(filename.c_str());
        }
    }

    void random_matrix(int n)
    {
        pure();
        ofstream file(filename);
        if (file.good())
        {
            file << n << endl;
            for (int i = 0; i < n; i++)
            {
                for (int j = 0; j < n; j++)
                {
                    int value = rand() % 100;
                    file << value << " ";
                }
                file << endl;
            }
        }
    }

public:
    FloydWarshallMpi(string executable)
    {
        this->executable = executable;
    }
    virtual string GetLabel() { return "floydwarshall_mpi"; }

    virtual void SetThreads(int n)
    {
        threads = n;
    }

    virtual void UpdateParam(int n)
    {
        random_matrix(n);
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