# Compiler flags
AM_CXXFLAGS=-std=c++11 -Wall -Wextra -fopenmp -I$(top_srcdir)/src

# Declaring files
bin_PROGRAMS = count_kmers generate_database paint_genome
count_kmers_SOURCES = counter/main.cpp counter/command_line_options.cpp counter/output.cpp counter/count.cpp lib/common.cpp lib/genome.cpp lib/kmer.cpp
generate_database_SOURCES = merger/main.cpp merger/command_line_options.cpp merger/merge.cpp merger/file.cpp merger/output.cpp merger/probability.cpp merger/sql.cpp lib/common.cpp lib/database.cpp
paint_genome_SOURCES = painter/main.cpp painter/command_line_options.cpp painter/paint.cpp painter/output.cpp lib/common.cpp lib/genome.cpp lib/kmer.cpp lib/database.cpp
