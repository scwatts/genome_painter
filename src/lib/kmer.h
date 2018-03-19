#ifndef __KMER_H__
#define __KMER_H__


#include <cmath>
#include <string>


#include "common.h"


#define KMER_SIZE 32


namespace kmer {


bool complement(char nucleotide, char &complement_nucleotide);
bool encode_kmer(std::string &sequence, size_t i, common::ullong &kmer_bincode);
bool encode_nucleotide(char nucleotide, common::ullong &nucleotide_bincode);


} // namespace kmer


#endif
