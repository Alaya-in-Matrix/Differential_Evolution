CXX   = g++
FLAGS = -std=c++11 -O3 -fopenmp
EXE   = de-spice 
SRC   = Evolution.cpp \
		read_spice.cpp \
		hspice_util.cpp
OBJ   = obj/Evolution.o \
		obj/read_spice.o \
		obj/hspice_util.o

$(EXE):$(OBJ)
	$(CXX) $^ -o $@ $(FLAGS)

obj/%.o:%.cpp
	mkdir -p obj
	$(CXX) -c $< -o $@ $(FLAGS)


.PHONY: clean

clean:
	rm -rf $(EXE)
	rm -rf $(OBJ)
