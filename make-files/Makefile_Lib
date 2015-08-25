MKDIR     := mkdir -p 
PLATFORM  := linux
CXX	      := g++
BUILD     := ../build
LIB       := ../lib
INC       := ../include
OBJS	  := Cortica.o CorticaProviderCloud.o CorticaProviderFactory.o jsoncpp.o
CXX_FLAGS := -shared -g -std=c++0x -Wall -fPIC
CXX_LIBS  += -L${LIB} -lcurl
CXX_INCS  += -I${INC}

all: Cortica

Cortica: $(OBJS)
	$(MKDIR) $(LIB)
	$(CXX) $(CXX_FLAGS) ${BUILD}/*.o -o ${LIB}/libcortica-sdk.so ${CXX_INCS} $(CXX_LIBS)
	ldconfig -n $(shell pwd)/lib
	cp $(LIB)/libcortica-sdk.so /usr/lib

CorticaProviderCloud.o: CorticaProviderCloud.cpp
	$(MKDIR) $(BUILD)
	$(CXX) -c $(CXX_FLAGS) CorticaProviderCloud.cpp -o ${BUILD}/CorticaProviderCloud.o $(CXX_INCS) $(CXX_LIBS)

CorticaProviderFactory.o: CorticaProviderFactory.cpp
	$(MKDIR) $(BUILD)
	$(CXX) -c $(CXX_FLAGS) CorticaProviderFactory.cpp -o ${BUILD}/CorticaProviderFactory.o $(CXX_INCS) $(CXX_LIBS)

jsoncpp.o: jsoncpp.cpp
	$(MKDIR) $(BUILD)
	$(CXX) -c $(CXX_FLAGS) jsoncpp.cpp -o ${BUILD}/jsoncpp.o $(CXX_INCS) $(CXX_LIBS)

Cortica.o: Cortica.cpp
	$(MKDIR) $(BUILD)
	$(CXX) -c $(CXX_FLAGS) Cortica.cpp -o ${BUILD}/Cortica.o $(CXX_INCS) $(CXX_LIBS)

clean:
	rm -r ${BUILD}/*
	rm -r $(LIB)/*
