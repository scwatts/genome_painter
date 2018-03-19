#include "count.h"


namespace count {


void collect_kmers(std::string &sequence, std::set<common::ullong> &sample_kmers) {
    for (size_t i = 0; i < sequence.size()-KMER_SIZE; i++) {
        common::ullong bincode;
        if (kmer::encode_kmer(sequence, i, bincode)) {
            sample_kmers.insert(bincode);
        }
    }
}


} // namespace count
