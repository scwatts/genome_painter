#ifndef __KMER_H__
#define __KMER_H__


#include <string>


namespace kmer {


struct KmerPairBin {
    unsigned long long forward_bincode;
    unsigned long long reverse_bincode;
};

KmerPairBin encode_substring_kmer(std::string &sequence, size_t i);
unsigned long long encode_kmer_forward(std::string &sequence, size_t i);
unsigned long long encode_kmer_reverse(std::string &sequence, size_t i);
bool encode_forward_nucleotide(char &nucleotide, unsigned long long *nucleotide_bincode);
bool encode_reverse_nucleotide(char &nucleotide, unsigned long long *nucleotide_bincode);


} // namespace kmer


#endif
