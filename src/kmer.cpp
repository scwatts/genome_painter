#include "kmer.h"


namespace kmer {


KmerPairBin encode_substring_kmer(std::string &sequence, size_t i) {
    KmerPairBin kmer_pair;
    kmer_pair.forward_bincode = encode_kmer_forward(sequence, i);
    kmer_pair.reverse_bincode = encode_kmer_reverse(sequence, i);
    return kmer_pair;
}


// TODO: there seems to be a large amount of code duplication. can we reuse without losing clarity?
common::ullong encode_kmer_forward(std::string &sequence, size_t i) {
    common::ullong kmer_bincode = 0;
    common::ullong nucleotide_bincode = 0;

    for (unsigned long j = i; j < i+32; j++) {
        kmer_bincode <<= 2;
        if (encode_forward_nucleotide(sequence[j], &nucleotide_bincode)) {
            kmer_bincode |= nucleotide_bincode;
        } else {
            // TODO: THIS IS WRONG
            kmer_bincode = -1;
            break;
        }
    }
    return kmer_bincode;
}


common::ullong encode_kmer_reverse(std::string &sequence, size_t i) {
    common::ullong kmer_bincode = 0;
    common::ullong nucleotide_bincode = 0;

    for (unsigned long j = i; j < i+32; j++) {
        kmer_bincode >>= 2;
        if (encode_reverse_nucleotide(sequence[j], &nucleotide_bincode)) {
            kmer_bincode |= nucleotide_bincode;
        } else {
            kmer_bincode = -1;
            break;
        }
    }
    return kmer_bincode;
}


bool encode_forward_nucleotide(char nucleotide, common::ullong *nucleotide_bincode) {
    switch (nucleotide) {
        case 'A':
            *nucleotide_bincode = 0;
            break;
        case 'T':
            *nucleotide_bincode = 1;
            break;
        case 'G':
            *nucleotide_bincode = 2;
            break;
        case 'C':
            *nucleotide_bincode = 3;
            break;
        default:
            return false;
    }
    return true;
}


bool encode_reverse_nucleotide(char nucleotide, common::ullong *nucleotide_bincode) {
    switch (nucleotide) {
        case 'A':
            *nucleotide_bincode = (common::ullong)1 << 62;
            break;
        case 'T':
            *nucleotide_bincode = (common::ullong)0 << 62;
            break;
        case 'G':
            *nucleotide_bincode = (common::ullong)3 << 62;
            break;
        case 'C':
            *nucleotide_bincode = (common::ullong)2 << 62;
            break;
        default:
            return false;
    }
    return true;
}


} // namespace kmer
