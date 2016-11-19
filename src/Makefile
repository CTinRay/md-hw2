CXXFLAGS = -std=c++11 -Wall -O3 -march=native -Ofast -flto -funroll-loops -pthread
# CXXFLAGS = -std=c++11 -Wall -lpthread
DBGFLAGS = -g3
LDLIBS+=-lboost_program_options
OBJS = factor-graph.o factor-functions.o gibbs-sampler.o construct.o gradient.o target-functions.o 

.PHONY: clean

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(DBGFLAGS) $<

main: $(OBJS) main.cpp
	$(CXX) $(CXXFLAGS) $(DBGFLAGS) $(OBJS) $(LDLIBS) main.cpp -o $@

clean:
	rm $(OBJS)
