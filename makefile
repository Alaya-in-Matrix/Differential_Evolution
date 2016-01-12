CXX   = g++
FLAGS = -std=c++11 -O3 -fopenmp -I inc
SRC   = Evolution.cpp \
		weixin.cpp \
		hspice_util.cpp \
		love.cpp
OBJ   = obj/Evolution.o \
		obj/weixin.o \
		obj/pengbo.o \
		obj/hspice_util.o \
		obj/love.o \
		obj/OptInfo.o

.PHONY: all
all:
	make pengbo
	make weixin

pengbo: obj/Evolution.o obj/hspice_util.o obj/pengbo.o
	$(CXX) $^ -o $@ $(FLAGS)

weixin:obj/Evolution.o obj/hspice_util.o obj/weixin.o
	$(CXX) $^ -o $@ $(FLAGS)

experiment:obj/Evolution.o obj/hspice_util.o obj/love.o obj/OptInfo.o
	$(CXX) $^ -o $@ $(FLAGS)


obj/%.o:%.cpp
	mkdir -p obj
	$(CXX) -c $< -o $@ $(FLAGS)


.PHONY: clean

clean:
	rm -rf $(EXE)
	rm -rf $(OBJ)
