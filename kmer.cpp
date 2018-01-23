#include "kmer.h"


namespace kmer {


KmerPairBin encode_substring_kmer(std::string &sequence, size_t i) {
    KmerPairBin kmer_pair;
    kmer_pair.forward_bincode = encode_kmer_forward(sequence, i);
    kmer_pair.reverse_bincode = encode_kmer_reverse(sequence, i);
    return kmer_pair;
}


// TODO: there seems to be a large amount of code duplication. can we reuse without losing clarity?
unsigned long long encode_kmer_forward(std::string &sequence, size_t i) {
    unsigned long long kmer_bincode = 0;
    unsigned long long nucleotide_bincode = 0;

    for (unsigned long j = i; j < i+32; j++) {
        kmer_bincode <<= 2;
        if (encode_forward_nucleotide(sequence[j], &nucleotide_bincode)) {
            kmer_bincode |= nucleotide_bincode;
        } else {
            kmer_bincode = -1;
            break;
        }
    }
    return kmer_bincode;
}


unsigned long long encode_kmer_reverse(std::string &sequence, size_t i) {
    unsigned long long kmer_bincode = 0;
    unsigned long long nucleotide_bincode = 0;

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


bool encode_forward_nucleotide(char &nucleotide, unsigned long long *nucleotide_bincode) {
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


bool encode_reverse_nucleotide(char &nucleotide, unsigned long long *nucleotide_bincode) {
    switch (nucleotide) {
        case 'A':
            *nucleotide_bincode = (unsigned long long)1 << 62;
            break;
        case 'T':
            *nucleotide_bincode = (unsigned long long)0 << 62;
            break;
        case 'G':
            *nucleotide_bincode = (unsigned long long)3 << 62;
            break;
        case 'C':
            *nucleotide_bincode = (unsigned long long)2 << 62;
            break;
        default:
            return false;
    }
    return true;
}


} // namespace kmer
