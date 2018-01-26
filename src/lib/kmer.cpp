#include "kmer.h"


namespace kmer {


bool encode_kmer(std::string sequence, size_t i, common::ullong &kmer_bincode, bitshifter bitshift_op, encoder encode_op) {
    common::ullong nucleotide_bincode = 0;

    kmer_bincode = 0;
    for (unsigned long j = i; j < i+32; j++) {
        bitshift_op(kmer_bincode);
        if (encode_op(sequence[j], nucleotide_bincode)) {
            kmer_bincode |= nucleotide_bincode;
        } else {
            return false;
        }
    }
    return true;
}


void bitshift_forward(common::ullong &kmer_bincode) {
     kmer_bincode <<= 2;
}


void bitshift_reverse(common::ullong &kmer_bincode) {
     kmer_bincode >>= 2;
}


bool encode_forward(char nucleotide, common::ullong &nucleotide_bincode) {
    switch (nucleotide) {
        case 'A':
            nucleotide_bincode = 0;
            break;
        case 'T':
            nucleotide_bincode = 1;
            break;
        case 'G':
            nucleotide_bincode = 2;
            break;
        case 'C':
            nucleotide_bincode = 3;
            break;
        default:
            return false;
    }
    return true;
}


bool encode_reverse(char nucleotide, common::ullong &nucleotide_bincode) {
    switch (nucleotide) {
        case 'A':
            nucleotide_bincode = (common::ullong)1 << 62;
            break;
        case 'T':
            nucleotide_bincode = (common::ullong)0 << 62;
            break;
        case 'G':
            nucleotide_bincode = (common::ullong)3 << 62;
            break;
        case 'C':
            nucleotide_bincode = (common::ullong)2 << 62;
            break;
        default:
            return false;
    }
    return true;
}


} // namespace kmer
