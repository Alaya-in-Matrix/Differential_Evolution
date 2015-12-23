CXX   = g++
FLAGS = -std=c++11 -O3 -fopenmp -I inc
SRC   = Evolution.cpp \
		weixin.cpp \
		hspice_util.cpp
OBJ   = obj/Evolution.o \
		obj/weixin.o \
		obj/hspice_util.o

.PHONY: all
all:
	make liminghua
	make weixin

liminghua: obj/Evolution.o obj/hspice_util.o obj/liminghua.o
	$(CXX) $^ -o $@ $(FLAGS)

weixin:obj/Evolution.o obj/hspice_util.o obj/weixin.o
	$(CXX) $^ -o $@ $(FLAGS)

obj/%.o:%.cpp
	mkdir -p obj
	$(CXX) -c $< -o $@ $(FLAGS)


.PHONY: clean

clean:
	rm -rf $(EXE)
	rm -rf $(OBJ)
