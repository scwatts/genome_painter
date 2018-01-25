#include "count.h"


namespace count {


void collect_kmers(std::string &sequence, std::set<common::ullong> &sample_kmers) {
    // Forward and reverse kmers
    // TODO: use input iterator and place in kmer.cpp
    for (size_t i = 0; i < sequence.size(); i++) {
        // Look up kmers and get kmer direction which has the most probable species
        kmer::KmerPairBin kmer_pair = kmer::encode_substring_kmer(sequence, i);

        // Add kmers to set if successful
        if (kmer_pair.forward_bincode >= 0) {
            sample_kmers.insert(kmer_pair.forward_bincode);
        }
        if (kmer_pair.reverse_bincode >= 0) {
            sample_kmers.insert(kmer_pair.reverse_bincode);
        }
    }
}


} // namespace count
