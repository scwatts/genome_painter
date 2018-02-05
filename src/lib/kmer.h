#ifndef __KMER_H__
#define __KMER_H__


#include <cmath>
#include <string>


#include "common.h"


namespace kmer {


typedef void (*bitshifter)(common::ullong &kmer_bincode);
typedef bool (*encoder)(char nucleotide, common::ullong &nucleotide_bincode);

bool encode_kmer(std::string &sequence, size_t i, common::ullong &kmer_bincode, bitshifter bitshift_op, encoder encode_op);
bool rolling_encode_kmer(char nucleotide, common::ullong &kmer_bincode, bitshifter bitshift_op, encoder encode_op);
void bitshift_forward(common::ullong &kmer_bincode);
void bitshift_reverse(common::ullong &kmer_bincode);
bool encode_forward(char nucleotide, common::ullong &nucleotide_bincode);
bool encode_reverse(char nucleotide, common::ullong &nucleotide_bincode);


} // namespace kmer


#endif
