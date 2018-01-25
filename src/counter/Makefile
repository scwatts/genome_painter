# C++ compiler and flags
CXX=g++
CXXFLAGS=-std=c++11 -Wall -Wextra -Wpedantic -Wnonnull

# Libraries and includes
LDLIBS=-lz
# LDFLAGS=
# INC=

# Files
SOURCES=main.cpp command_line_options.cpp output.cpp count.cpp ../common.cpp ../genome.cpp ../kmer.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=count_kmers

# Rules
debug: CXXFLAGS+=-g -O0
debug: all

release: CXXFLAGS+=-DNDEBUG -s -O2
release: all

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(LDLIBS) -o $(EXECUTABLE) $^

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INC) $< -o $@

.PHONY: clean
clean:
	rm $(OBJECTS) $(EXECUTABLE)
