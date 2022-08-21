CXX := g++
CXXFLAGS := -std=c++17 -O3 -Wall -Wextra -fopenmp
LIB := -lm -fopenmp
SRC_DIRS:= Parser\
					Data\
					FMSolver\
					GlobalTimer
EXEC := ../bin/hw2


SRCS := main.cpp $(wildcard $(SRC_DIRS:=/*.cpp))
OBJS := $(SRCS:.cpp=.o)
DEPS := $(OBJS:.o=.d)

all: $(EXEC)

$(EXEC): $(OBJS)
		$(CXX) $(LIB) -o $@ $^


%.o: %.cpp
		$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

clean:
		rm -rf $(EXEC) $(OBJS) $(DEPS)


.PHONY: all clean
-include $(DEPS)