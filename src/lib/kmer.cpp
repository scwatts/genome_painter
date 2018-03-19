#include "kmer.h"


namespace kmer {


bool encode_kmer(std::string &sequence, size_t i, common::ullong &kmer_bincode) {
    // Make lexicographical comparison with partially reverse complemented kmer
    // TODO: can we improve efficiency by encoding both strands during lex cmp?
    std::string::iterator iter = sequence.begin() + i;
    std::string::reverse_iterator riter = sequence.rend() - (i + KMER_SIZE);
    std::string kmer_rc;
    kmer_rc.reserve(KMER_SIZE);

    char complement_nucleotide;
    std::string::iterator kmer_iter = iter;
    for (unsigned long j = 0; j < KMER_SIZE; j++, iter++, riter++) {
        // Complement (only accepting ATGC)
        if (complement(*riter, complement_nucleotide)) {
            kmer_rc.push_back(complement_nucleotide);
        } else {
            return false;
        }

        // Lex cmp
        if (*iter > complement_nucleotide) {
            // Finish reverse complementing kmer_rc and set iterator
            // TODO: this is getting a little messy, clean up?
            for (riter++; riter != sequence.rend()-i; riter++) {
                if (complement(*riter, complement_nucleotide)) {
                    kmer_rc.push_back(complement_nucleotide);
                } else {
                    return false;
                }
            }
            kmer_iter = kmer_rc.begin();
            break;
        } else if (*iter < complement_nucleotide) {
            // Keep kmer_iter as default
            break;
        }
    }

    // Encode lexicographically smallest kmer
    common::ullong nucleotide_bincode = 0;
    kmer_bincode = 0;
    for (unsigned int j = 0; j < KMER_SIZE; j++, kmer_iter++) {
        kmer_bincode <<= 2;
        if (encode_nucleotide(*kmer_iter, nucleotide_bincode)) {
            kmer_bincode |= nucleotide_bincode;
        } else {
            return false;
        }
    }
    return true;
}


bool complement(char nucleotide, char &complement_nucleotide) {
    switch(nucleotide) {
        case 'A':
            complement_nucleotide = 'T';
            break;
        case 'T':
            complement_nucleotide = 'A';
            break;
        case 'G':
            complement_nucleotide = 'C';
            break;
        case 'C':
            complement_nucleotide = 'G';
            break;
        default:
            return false;
    }
    return true;
}


bool encode_nucleotide(char nucleotide, common::ullong &nucleotide_bincode) {
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


} // namespace kmer
