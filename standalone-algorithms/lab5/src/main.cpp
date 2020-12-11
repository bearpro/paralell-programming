#include <iostream>
#include <algorithm>
#include <fstream>
#include <mpi.h>

#define INF 101
#define HOST_RANK 0

using namespace std;

int main(int argc, char **argv)
{
    srand(time(NULL));
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int numberOfVert;
    MPI_Request request;
    MPI_Status status;

    if (rank == HOST_RANK)
    {
        numberOfVert = 5;
        int *matrix;
        ifstream file("matrix");
        file >> numberOfVert;
        matrix = (int *)malloc(sizeof(int) * (numberOfVert * numberOfVert));
        for (int i = 0; i < numberOfVert; i++)
        {
            for (int j = 0; j < numberOfVert; j++)
            {
                file >> matrix[i * numberOfVert + j];
            }
        }

        file.close();
        MPI_Bcast(&numberOfVert, 1, MPI_INT, HOST_RANK, MPI_COMM_WORLD);
        int workSize = size - 1;
        int *rowCounts = (int *)malloc(sizeof(int) * workSize);
        for (int i = 0; i < workSize - 1; i++)
        {
            rowCounts[i] = numberOfVert / workSize;
        }
        if (numberOfVert % workSize == 0)
        {
            rowCounts[workSize - 1] = numberOfVert / workSize;
        }
        else
        {
            rowCounts[workSize - 1] = numberOfVert - ((numberOfVert / workSize) * (workSize - 1));
        }

        for (int i = 1; i < size; i++)
        {
            MPI_Send(&rowCounts[i - 1], 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }

        for (int k = 0; k < numberOfVert; k++)
        {
            //Раздаем k-ую строку всем процессам(кроме рута)
            for (int p = 1; p < size; p++)
            {
                MPI_Isend(&matrix[k * numberOfVert], numberOfVert, MPI_INT, p, 0, MPI_COMM_WORLD, &request);
            }

            //Отдать каждому процессу его строки матрицы
            int num = 0; //Номер строки
            for (int i = 0; i < workSize; i++)
            { //Бежим по массиву с кол-вом строк
                for (int j = 0; j < rowCounts[i]; j++)
                { //Отдаем нужное количество строк
                    MPI_Isend(&matrix[num * numberOfVert], numberOfVert, MPI_INT, i + 1, 0, MPI_COMM_WORLD, &request);
                    num++;
                }
            }

            //Получаем от процессов строки, измененные на этой итерации
            num = 0; //Номер строки
            for (int i = 0; i < workSize; i++)
            { //Бежим по массиву с кол-вом строк
                for (int j = 0; j < rowCounts[i]; j++)
                { //Принимаем нужное количество строк
                    MPI_Recv(&matrix[num * numberOfVert], numberOfVert, MPI_INT, i + 1, 0, MPI_COMM_WORLD, &status);
                    num++;
                }
            }
        }
        free(matrix);
        cout << "done" << endl;
    }
    else
    {
        MPI_Bcast(&numberOfVert, 1, MPI_INT, HOST_RANK, MPI_COMM_WORLD);
        int rowCount;
        MPI_Recv(&rowCount, 1, MPI_INT, HOST_RANK, 0, MPI_COMM_WORLD, &status);
        int *kRow = (int *)malloc(sizeof(int) * numberOfVert);
        int *rows = (int *)malloc(sizeof(int) * (rowCount * numberOfVert));
        for (int k = 0; k < numberOfVert; k++)
        {
            MPI_Recv(kRow, numberOfVert, MPI_INT, HOST_RANK, 0, MPI_COMM_WORLD, &status);
            for (int i = 0; i < rowCount; i++)
            {
                MPI_Recv(&rows[i * numberOfVert], numberOfVert, MPI_INT, HOST_RANK, 0, MPI_COMM_WORLD, &status);
            }
            for (int i = 0; i < rowCount; i++)
            {
                for (int j = 0; j < numberOfVert; j++)
                {
                    rows[i * numberOfVert + j] = min(
                        rows[i * numberOfVert + j],
                        rows[i * numberOfVert + k] + kRow[j]);
                }
            }
            for (int i = 0; i < rowCount; i++)
            {
                MPI_Send(&rows[i * numberOfVert], numberOfVert, MPI_INT, HOST_RANK, 0, MPI_COMM_WORLD);
            }
        }
        free(kRow);
        free(rows);
    }

    MPI_Finalize();
    return 0;
}