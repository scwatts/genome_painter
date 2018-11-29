# Build flags
AM_CXXFLAGS = -std=c++11 -Wall -Wextra -fopenmp -I$(top_srcdir)/src
if STATIC_COMPILE
AM_CXXFLAGS += -static
endif

# File sets
common_fps = lib/common.cpp lib/common.h
genome_fps = lib/genome.cpp lib/genome.h lib/kseq.h
kmer_fps = lib/kmer.cpp lib/kmer.h

count_kmers_fps =                    \
		counter/command_line_options.cpp \
		counter/command_line_options.h   \
		counter/output.cpp               \
		counter/output.h                 \
		counter/count.cpp                \
		counter/count.h                  \
		$(genome_fps)                    \
		$(kmer_fps)                      \
		$(common_fps)

generate_database_fps =             \
		merger/command_line_options.cpp \
		merger/command_line_options.h   \
		merger/merge.cpp                \
		merger/merge.h                  \
		merger/file.cpp                 \
		merger/file.h                   \
		merger/output.cpp               \
		merger/output.h                 \
		merger/probability.cpp          \
		merger/probability.h            \
		$(common_fps)

paint_genome_fps =                   \
		painter/command_line_options.cpp \
		painter/command_line_options.h   \
		painter/paint.cpp                \
		painter/paint.h                  \
		painter/output.cpp               \
		painter/output.h                 \
		lib/database.cpp		             \
		lib/database.h                   \
		$(genome_fps)                    \
		$(kmer_fps)                      \
		$(common_fps)

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
