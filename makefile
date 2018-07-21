CC=g++
FLAG= -m64 -std=c++11 -fopenmp
test:MainTrivium721.o
	$(CC) $(FLAG) -o test MainTrivium721.o
MainTrivium721.o: MainTrivium721.cpp Trivium.hpp
	$(CC) $(FLAG) -c MainTrivium721.cpp -o MainTrivium721.o

