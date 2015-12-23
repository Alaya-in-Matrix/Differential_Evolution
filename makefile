CXX   = g++
FLAGS = -std=c++11 -O3 -fopenmp
SRC   = Evolution.cpp \
		weixin.cpp \
		hspice_util.cpp
OBJ   = obj/Evolution.o \
		obj/weixin.o \
		obj/hspice_util.o

weixin:obj/Evolution.o obj/hspice_util.o obj/weixin.o
	$(CXX) $^ -o $@ $(FLAGS)

obj/%.o:%.cpp
	mkdir -p obj
	$(CXX) -c $< -o $@ $(FLAGS)


.PHONY: clean

clean:
	rm -rf $(EXE)
	rm -rf $(OBJ)
