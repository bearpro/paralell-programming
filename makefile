all:
	g++ -fopenmp -ggdb -Wall -o bin/program src/*.cpp

standalone:
	mpiCC -ggdb -Wall -O0 -o standalone-algorithms/lab4/bin/program standalone-algorithms/lab4/src/*.cpp
	mpiCC -ggdb -Wall -O0 -o standalone-algorithms/lab5/bin/program standalone-algorithms/lab5/src/*.cpp