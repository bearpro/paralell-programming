#include <mpi.h>
#include <stdio.h>

#ifndef MPI_ACTOR
#define MPI_ACTOR

using namespace std;
using namespace MPI;

class MpiActor
{
protected:
    const bool debug = false;
    const int result_collection = 1000;

public:
    ~MpiActor();
    static string get_computer_id()
    {
        char *processor_name = new char[MPI_MAX_PROCESSOR_NAME];
        int name_len;
        MPI::Get_processor_name(processor_name, name_len);

        int world_rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

        string result;
        result
            .append(processor_name, name_len)
            .append("::")
            .append(to_string(world_rank));
        return result;
    }
    static int mpi_rank()
    {
        int world_rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
        return world_rank;
    }
    static int world_size()
    {
        int world_size;
        MPI_Comm_size(MPI_COMM_WORLD, &world_size);
        return world_size;
    }
    static bool is_host()
    {
        return mpi_rank() == 0;
    }
    virtual void run(int n) = 0;
    int* concat_data(const int *matrix, const int *vector, int n)
    {
        auto all_data = new int[(n * n) * n];
        for (int i = 0; i < n * n; i++)
        {
            all_data[i] = matrix[i];
        }
        for (int i = 0; i < n; i++)
        {
            all_data[(n * n) + i] = vector[i];
        }
        return all_data;
    }
    tuple<int*, int*> deconcat_data(const int *all_data, int n)
    {
        int *matrix = new int[n * n];
        int *vector = new int[n];
        for (int i = 0; i < n * n; i++)
        {
            matrix[i] = all_data[i];
        }
        for (int i = 0; i < n; i++)
        {
            vector[i] = all_data[(n * n) + i];
        }
        return make_tuple(matrix, vector);
    }
    tuple<int, int> responsible_slice(int n, int rank, int world_size)
    {
        n = n * n;
        int amount = n / world_size;
        int overage = n % world_size;
        if (debug) cout << get_computer_id() << " - rank=" << rank << endl;
        if (rank == 0)
        {
            return make_tuple(0, amount + overage);
        }
        else
        {
            return make_tuple(overage + amount * rank, overage + amount * (rank + 1));
        }
    }
    int* calculate(const int* matrix, const int* vector, int n, int rank)
    {
        if (debug) cout << get_computer_id() << " - started calculations" << endl;
        int *result = new int[n];
        for (int i = 0; i < n; i++) result[i] = 0;
        if (debug) cout << get_computer_id() << " - initialized collection" << endl;
        int start, end;
        tie(start, end) = responsible_slice(n, rank, world_size());
        if (debug) cout << get_computer_id() << " - slice: start=" << start << " end=" << end << endl;
        for (int i = start; i < end; i++)
        {
            auto j = i % n;
            int matrix_item = matrix[i];
            int vector_item = vector[j];
            result[i / n] += vector_item * matrix_item;
        }
        if (debug) cout << get_computer_id() << " - completed calculations" << endl;
        return result;
    }
};

class MpiHostActor : public MpiActor
{
private:
    int * random_matrix(int n)
    {
        auto matrix = new int[n * n];
        for (int i = 0; i < n * n; i++)
        {
            matrix[i] = rand() % 1000;
        }
        return matrix;
    }

    int * random_vector(int n)
    {
        auto matrix = new int[n];
        for (int i = 0; i < n; i++)
        {
            matrix[i] = rand() % 1000;
        }
        return matrix;
    }

    void broadcast(int* matrix, int* vector, int n)
    {
        int* data = concat_data(matrix, vector, n);
        MPI_Bcast(data, (n * n) + n, MPI_INT, 0, MPI_COMM_WORLD);
    }

    int** collect_results(int n)
    {
        int **results = new int*[world_size()];
        for (int i = 1; i < world_size(); i++)
        {
            int *result = new int[n];
            MPI_Recv(result, n, MPI_INT, i, result_collection, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            results[i] = result;
        }
        if (debug) cout << "Results collected from " << world_size() << " actors." << endl;
        return results;
    }

    void combine_results(int* local_result, int** partial_results, int n)
    {
        for (int result_index = 1; result_index < world_size(); result_index++)
        {
            for (int i = 0; i < n; i++)
            {
                local_result[i] += partial_results[result_index][i];
            }
        }
        if (debug)
            cout << get_computer_id() << " - " << local_result[0] << " " << local_result[1] << " " << local_result[2] << "..." << endl;
    }

public:
    void run(int n)
    {
        if (debug) cout << get_computer_id() << " - Actor started" << endl;
        int *matrix = random_matrix(n);
        int *vector = random_vector(n);
        broadcast(matrix, vector, n);
        int *local_result = calculate(matrix, vector, n, mpi_rank());
        MPI_Barrier(MPI_COMM_WORLD);
        int **results = collect_results(n);
        combine_results(local_result, results, n);
        cout << "done" << endl;
    }
};

class MpiSlaveActor : public MpiActor
{
private:
    tuple<int*, int*> receive_broadcast(int n)
    {
        int *all_data = new int[(n * n) + n];
        MPI_Bcast(all_data, (n * n) + n, MPI_INT, 0, MPI_COMM_WORLD);
        return deconcat_data(all_data, n);
    }

public:
    void run(int n)
    {
        if (debug) cout << get_computer_id() << " - Actor started" << endl;
        int *matrix;
        int *vector;
        tie(matrix, vector) = receive_broadcast(n);
        if (debug) cout << get_computer_id() << " - Actor received data" << endl;
        int *local_result = calculate(matrix, vector, n, mpi_rank());
        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Send(local_result, n, MPI_INT, 0, result_collection, MPI_COMM_WORLD);
    }
};
#endif