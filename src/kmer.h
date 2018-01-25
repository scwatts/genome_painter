#ifndef __KMER_H__
#define __KMER_H__


#include <cmath>
#include <string>


#include "common.h"


namespace kmer {


struct KmerPairBin {
    common::ullong forward_bincode;
    common::ullong reverse_bincode;
};

KmerPairBin encode_substring_kmer(std::string &sequence, size_t i);
common::ullong encode_kmer_forward(std::string &sequence, size_t i);
common::ullong encode_kmer_reverse(std::string &sequence, size_t i);
bool encode_forward_nucleotide(char nucleotide, common::ullong *nucleotide_bincode);
bool encode_reverse_nucleotide(char nucleotide, common::ullong *nucleotide_bincode);


} // namespace kmer


#endif
