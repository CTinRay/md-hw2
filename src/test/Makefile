CXXFLAGS = -std=c++11 -Wall -O3 -march=native -Ofast -flto -funroll-loops -lpthread
# CXXFLAGS = -std=c++11 -Wall
DBGFLAGS = -g3

OBJS = factor-graph.o factor-functions.o gibbs-sampler.o construct.o target-functions.o

.PHONY: clean

%.o: ../%.cpp
	$(CXX) -c $(CXXFLAGS) $(DBGFLAGS) $<

test: $(OBJS) test.cpp
	$(CXX) $(CXXFLAGS) $(DBGFLAGS) $(OBJS) test.cpp -o $@

clean:
	rm $(OBJS)
