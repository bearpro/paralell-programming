all:
	g++ -fopenmp -ggdb -Wall -O0 -o bin/program src/*.cpp

standalone:
	mpiCC -ggdb -Wall -O0 -o standalone-algorithms/lab4/bin/program standalone-algorithms/lab4/src/*.cpp