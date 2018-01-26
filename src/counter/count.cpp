#include "count.h"


namespace count {


void collect_kmers(std::string &sequence, std::set<common::ullong> &sample_kmers) {
    for (size_t i = 0; i < sequence.size(); i++) {
        // Attempt to encode (currently we're unable to encode nucleotides other than atgc)
        common::ullong f_bincode, r_bincode;
        if(kmer::encode_kmer(sequence, i, f_bincode, kmer::bitshift_forward, kmer::encode_forward)) {
            sample_kmers.insert(f_bincode);
        }
        if (kmer::encode_kmer(sequence, i, r_bincode, kmer::bitshift_reverse, kmer::encode_reverse)) {
            sample_kmers.insert(r_bincode);
        }
    }
}


} // namespace count
