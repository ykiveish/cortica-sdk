PLATFORM  := linux
CXX	      := g++
LIB       := ./lib
INC       := ./include
OBJS	  := cortica-test.o
CXX_FLAGS := -g -std=c++0x -Wall
CXX_LIBS  += -L${LIB} -L/usr/lib/x86_64-linux-gnu -lcurl -lcortica-sdk
CXX_INCS  += -I${INC}

all: cortica-test

cortica-test: $(OBJS)
	$(CXX) $(CXX_FLAGS) build/*.o -o cortica-test ${CXX_INCS} $(CXX_LIBS)

cortica-test.o: cortica-test.cpp
	$(CXX) -c $(CXX_FLAGS) cortica-test.cpp -o build/cortica-test.o $(CXX_INCS) $(CXX_LIBS)

clean:
	rm build/*
	rm cortica-test
