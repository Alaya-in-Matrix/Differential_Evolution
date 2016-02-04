CXX=g++
love:test.o DifferentialEvolution.o
	$(CXX) -o $@ $^ -std=c++11 -fopenmp

%.o:%.cpp
	$(CXX) -c $^ -o $@ -std=c++11 -fopenmp
