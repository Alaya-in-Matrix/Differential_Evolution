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

spice:read_spice.cpp Evolution.cpp
	g++ read_spice.cpp Evolution.cpp -o $@ -std=c++11

.PHONY: clean

clean:
	rm -rf $(EXE)
	rm -rf $(OBJ)
