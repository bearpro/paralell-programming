
#include <stdio.h>
#include <stdlib.h>
// #include <mpi.h>
#include "../Algorithm.h"
#include <iostream>
using namespace std;

class MultiplyMatrixMpi : public Algorithm
{
private:
    double *pMatrix;
    double *pVector;
    double *pResult;
    int Size;
    double *pProcRows;
    double *pProcResult;
    int RowNum;
    double Start, Finish, Duration;
    int ProcNum;
    int ProcRank;
    int argc;
    char **argv;

    void
    RandomDataInitialization(double *pMatrix, double *pVector, int Size)
    {
        int i, j;
        srand(unsigned(clock()));
        for (i = 0; i < Size; i++)
        {
            pVector[i] = rand() / double(1000);
            for (j = 0; j < Size; j++)
                pMatrix[i * Size + j] = rand() / double(1000);
        }
    }

    void ProcessInitialization(double *&pMatrix, double *&pVector,
                               double *&pResult, double *&pProcRows,
                               double *&pProcResult, int &Size, int &RowNum)
    {
        int RestRows;
        int i;

        MPI_Bcast(&Size, 1, MPI_INT, 0, MPI_COMM_WORLD);

        RestRows = Size;
        for (i = 0; i < ProcRank; i++)
            RestRows = RestRows - RestRows / (ProcNum - i);
        RowNum = RestRows / (ProcNum - ProcRank);

        pVector = new double[Size];
        pResult = new double[Size];
        pProcRows = new double[RowNum * Size];
        pProcResult = new double[RowNum];

        if (ProcRank == 0)
        {
            pMatrix = new double[Size * Size];
            RandomDataInitialization(pMatrix, pVector, Size);
        }
    }

    void DataDistribution(double *pMatrix, double *pProcRows, double *pVector, int Size, int RowNum)
    {
        int *pSendNum;
        int *pSendInd;
        int RestRows = Size;

        MPI_Bcast(pVector, Size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        pSendInd = new int[ProcNum];
        pSendNum = new int[ProcNum];

        RowNum = (Size / ProcNum);
        pSendNum[0] = RowNum * Size;
        pSendInd[0] = 0;
        for (int i = 1; i < ProcNum; i++)
        {
            RestRows -= RowNum;
            RowNum = RestRows / (ProcNum - i);
            pSendNum[i] = RowNum * Size;
            pSendInd[i] = pSendInd[i - 1] + pSendNum[i - 1];
        }
        MPI_Scatterv(pMatrix, pSendNum, pSendInd, MPI_DOUBLE, pProcRows,
                     pSendNum[ProcRank], MPI_DOUBLE, 0, MPI_COMM_WORLD);

        delete[] pSendNum;
        delete[] pSendInd;
    }

    void ParallelResultCalculation(double *pProcRows, double *pVector, double *pProcResult, int Size, int RowNum)
    {
        int i, j;
        for (i = 0; i < RowNum; i++)
        {
            pProcResult[i] = 0;
            for (j = 0; j < Size; j++)
                pProcResult[i] += pProcRows[i * Size + j] * pVector[j];
        }
    }

    void ResultReplication(double *pProcResult, double *pResult, int Size, int RowNum)
    {
        int *pReceiveNum;
        int *pReceiveInd;
        int RestRows = Size;
        int i;
        pReceiveNum = new int[ProcNum];
        pReceiveInd = new int[ProcNum];
        pReceiveInd[0] = 0;
        pReceiveNum[0] = Size / ProcNum;
        for (i = 1; i < ProcNum; i++)
        {
            RestRows -= pReceiveNum[i - 1];
            pReceiveNum[i] = RestRows / (ProcNum - i);
            pReceiveInd[i] = pReceiveInd[i - 1] + pReceiveNum[i - 1];
        }
        MPI_Allgatherv(pProcResult, pReceiveNum[ProcRank],
                       MPI_DOUBLE, pResult, pReceiveNum, pReceiveInd,
                       MPI_DOUBLE, MPI_COMM_WORLD);

        delete[] pReceiveNum;
        delete[] pReceiveInd;
    }

    void ProcessTermination(double *pMatrix, double *pVector, double *pResult, double *pProcRows, double *pProcResult)
    {
        if (ProcRank == 0)
            delete[] pMatrix;
        delete[] pVector;
        delete[] pResult;
        delete[] pProcRows;
        delete[] pProcResult;
    }

protected:
public:
    MultiplyMatrixMpi(int argc, char **argv)
    {
        this->argc = argc;
        this->argv = argv;
    }
    virtual string GetLabel() { return "matrix_mul_mpi"; }
    virtual void UpdateParam(int n)
    {
        MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
        MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
        ProcessInitialization(pMatrix, pVector, pResult, pProcRows, pProcResult, Size, RowNum);
    }
    virtual void Run()
    {
        DataDistribution(pMatrix, pProcRows, pVector, Size, RowNum);
        ParallelResultCalculation(pProcRows, pVector, pProcResult, Size, RowNum);
        ResultReplication(pProcResult, pResult, Size, RowNum);
        ProcessTermination(pMatrix, pVector, pResult, pProcRows, pProcResult);
    }
};