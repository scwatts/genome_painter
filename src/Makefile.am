# Build flags
AM_CXXFLAGS = -std=c++11 -Wall -Wextra -fopenmp -I$(top_srcdir)/src
if STATIC_COMPILE
AM_CXXFLAGS += -static
endif

# File sets
count_kmers_fps = counter/command_line_options.cpp counter/output.cpp counter/count.cpp lib/common.cpp lib/genome.cpp lib/kmer.cpp
generate_database_fps = merger/command_line_options.cpp merger/merge.cpp merger/file.cpp merger/output.cpp merger/probability.cpp lib/common.cpp
paint_genome_fps = painter/command_line_options.cpp painter/paint.cpp painter/output.cpp lib/common.cpp lib/genome.cpp lib/kmer.cpp lib/database.cpp

# Static libraries
lib_LIBRARIES = libcount_kmers.a libgenerate_database.a libpaint_genome.a
libcount_kmers_a_SOURCES = $(count_kmers_fps)
libgenerate_database_a_SOURCES = $(generate_database_fps)
libpaint_genome_a_SOURCES = $(paint_genome_fps)

# ELFs
bin_PROGRAMS = count_kmers generate_database paint_genome

count_kmers_SOURCES = counter/main.cpp
count_kmers_LDADD = libcount_kmers.a

generate_database_SOURCES = merger/main.cpp
generate_database_LDADD = libgenerate_database.a

paint_genome_SOURCES = painter/main.cpp
paint_genome_LDADD = libpaint_genome.a
