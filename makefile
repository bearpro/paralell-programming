all:
	g++ -fopenmp -ggdb -Wall -o bin/program src/*.cpp

mpi:
	mpiCC -fopenmp -ggdb -Wall -o bin/program src/*.cpp