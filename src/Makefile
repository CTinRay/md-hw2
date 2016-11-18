CXXFLAGS = -std=c++11 -Wall -O3 -mtune=core2 -Ofast -flto -funroll-loops -fopenmp 
# CXXFLAGS = -std=c++11 -Wall
DBGFLAGS = -g3

OBJS = factor-graph.o factor-functions.o gibbs-sampler.o construct2.o 

.PHONY: clean

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(DBGFLAGS) $<

main: $(OBJS) main.cpp
	$(CXX) $(CXXFLAGS) $(DBGFLAGS) $(OBJS)  main.cpp -o $@

pred: $(OBJS) pred.cpp
	$(CXX) $(CXXFLAGS) $(DBGFLAGS) $(OBJS)  pred.cpp -o $@

clean:
	rm $(OBJS)
