# C++ compiler and flags
CXX=g++
CXXFLAGS=-std=c++11 -Wall -Wextra -Wpedantic -Wnonnull

# Libraries and includes
LDLIBS=-lz
# LDFLAGS=
INC=-I./src/

# Files
COUNTER_SOURCES_EXEC=main.cpp command_line_options.cpp output.cpp count.cpp
COUNTER_SOURCES_LIB=common.cpp genome.cpp kmer.cpp
COUNTER_SOURCES=$(addprefix ./src/counter/,$(COUNTER_SOURCES_EXEC)) $(addprefix ./src/lib/,$(COUNTER_SOURCES_LIB))
COUNTER_OBJECTS=$(COUNTER_SOURCES:.cpp=.o)

MERGER_SOURCES_EXEC=main.cpp command_line_options.cpp merge.cpp file.cpp probability.cpp
MERGER_SOURCES_LIB=common.cpp
MERGER_SOURCES=$(addprefix ./src/merger/,$(MERGER_SOURCES_EXEC)) $(addprefix ./src/lib/,$(MERGER_SOURCES_LIB))
MERGER_OBJECTS=$(MERGER_SOURCES:.cpp=.o)

PAINTER_SOURCES_EXEC=main.cpp command_line_options.cpp paint.cpp output.cpp
PAINTER_SOURCES_LIB=common.cpp database.cpp genome.cpp kmer.cpp
PAINTER_SOURCES=$(addprefix ./src/painter/,$(PAINTER_SOURCES_EXEC)) $(addprefix ./src/lib/,$(PAINTER_SOURCES_LIB))
PAINTER_OBJECTS=$(PAINTER_SOURCES:.cpp=.o)


EXECUTABLES=bin/count_kmers bin/merge_database bin/genome_painter

# Rules
debug: CXXFLAGS+=-g -O0
debug: all

release: CXXFLAGS+=-DNDEBUG -s -O2
release: all

all: directory $(EXECUTABLES)

.PHONY: directory
directory:
	mkdir -p bin

bin/count_kmers: $(COUNTER_OBJECTS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(LDLIBS) -o $@ $^

bin/merge_database: $(MERGER_OBJECTS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(LDLIBS) -o $@ $^

bin/genome_painter: $(PAINTER_OBJECTS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(LDLIBS) -o $@ $^

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INC) $< -o $@

.PHONY: clean
clean:
	rm $(COUNTER_OBJECTS) $(MERGER_OBJECTS) $(PAINTER_OBJECTS) $(EXECUTABLES)
