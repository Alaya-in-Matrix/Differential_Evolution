CXX   = g++
FLAGS = -std=c++11 -O0
EXE   = de 
SRC   = Evolution.cpp main.cpp
OBJ   = obj/Evolution.o obj/main.o

$(EXE):$(OBJ)
	$(CXX) $^ -o $@ $(FLAGS)

obj/%.o:%.cpp
	mkdir -p obj
	$(CXX) -c $< -o $@ $(FLAGS)

.PHONY: clean

clean:
	rm -rf $(EXE)
	rm -rf $(OBJ)
