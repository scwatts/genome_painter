#include "count.h"


namespace count {


void collect_kmers(std::string &sequence, std::set<common::ullong> &sample_kmers) {
    // Get first kmer we can encode and add to counts
    common::ullong f_bincode, r_bincode;
    size_t i = 0;
    for ( ; i < sequence.size(); i++) {
        if(! kmer::encode_kmer(sequence, i, f_bincode, kmer::bitshift_forward, kmer::encode_forward)) {
            continue;
        }
        if (! kmer::encode_kmer(sequence, i, r_bincode, kmer::bitshift_reverse, kmer::encode_reverse)) {
            continue;
        }
        sample_kmers.insert(f_bincode);
        sample_kmers.insert(r_bincode);
        break;
    }

    // TODO: increment i manually here?
    i++;

    // Now get kmer binary encoding in a rolling fasion
    for ( ; i < sequence.size(); i++) {
        // Attempt to encode (currently we're unable to encode nucleotides other than atgc)
        if(kmer::rolling_encode_kmer(sequence[i], f_bincode, kmer::bitshift_forward, kmer::encode_forward)) {
            sample_kmers.insert(f_bincode);
        } else {
            i = i + 32;
            continue;
        }
        // Attempt to encode (currently we're unable to encode nucleotides other than atgc)
        if (kmer::rolling_encode_kmer(sequence[i], r_bincode, kmer::bitshift_reverse, kmer::encode_reverse)) {
            sample_kmers.insert(r_bincode);
        } else {
            i = i + 32;
            continue;
        }
    }
}


} // namespace count
