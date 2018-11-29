# Build flags
AM_CXXFLAGS = -std=c++11 -Wall -Wextra -fopenmp -I$(top_srcdir)/src

TESTS = $(check_PROGRAMS)
check_PROGRAMS = run_tests
run_tests_SOURCES = run_tests.cpp test_kmer_encoding.cpp catch.hpp
run_tests_LDADD = ../src/libcount_kmers.a ../src/libgenerate_database.a ../src/libpaint_genome.a
