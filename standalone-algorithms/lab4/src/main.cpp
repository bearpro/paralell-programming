#include <mpi.h>
#include <stdio.h>
#include <stdio.h>
#include "MpiActor.h"
using namespace std;

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        cout << "No argument 'value amount' specified." << endl;
    }
    int n = atoi(argv[1]);

    MPI::Init();
    MpiActor* actor;
    if (MpiActor::is_host())
    {
        actor = new MpiHostActor();
    }
    else
    {
        actor = new MpiSlaveActor();
    }
    actor->run(n);
    MPI_Finalize();
}
