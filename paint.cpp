#include "genome.h"
#include "database.h"
#include "kmer.h"
#include "paint.h"


namespace paint {


std::vector<PaintBucket> paint_sequence(genome::FastaRecord &fasta, db::Database &database) {
    std::vector<PaintBucket> paint(fasta.sequence.size(), { {}, 0 });

    // Forward and reverse kmers
    // TODO: use input iterator and place in kmer.cpp
    for (size_t i = 0; i < fasta.sequence.size(); i++) {
        // Look up kmers and get kmer direction which has the most probable species
        kmer::KmerPairBin kmer_pair = kmer::encode_substring_kmer(fasta.sequence, i);
        PaintBucket kmer_probabilities;
        if (get_most_probable_kmer(kmer_pair, database, &kmer_probabilities)) {
            // See if the best one is better than the current; add or skip
            compare_paint(paint, kmer_probabilities, i);
        }
    }

    return paint;
}


bool get_most_probable_kmer(kmer::KmerPairBin kmer_pair, db::Database &database, PaintBucket *probabilities) {
    // Get probabilities
    PaintBucket forward, reverse;
    if (database.probability_map.find(kmer_pair.forward_bincode) != database.probability_map.end()) {
        forward.set_probabilities(database.probability_map[kmer_pair.forward_bincode]);
    }
    if (database.probability_map.find(kmer_pair.reverse_bincode) != database.probability_map.end()) {
        reverse.set_probabilities(database.probability_map[kmer_pair.reverse_bincode]);
    }

    // Ensure that at least one look up was successful
    if (! (forward.probabilities.empty() && reverse.probabilities.empty())) {
        // Set most probable
        if (forward.max_probability >= reverse.max_probability) {
            *probabilities = forward;
            return true;
        } else if (reverse.max_probability > forward.max_probability) {
            *probabilities = reverse;
            return true;
        }
    } else {
        return false;
    }

    // No control zone
    return false;
}


void compare_paint(std::vector<PaintBucket> &paint, PaintBucket &bucket, size_t i) {
    size_t end_idx;
    if (i+32 > paint.size()) {
        end_idx = paint.size();
    } else {
        end_idx = i + 32;
    }

    for ( ; i < end_idx; i++) {
        if (paint[i].max_probability < bucket.max_probability) {
            paint[i] = std::move(bucket);
        }
    }
}

void PaintBucket::set_probabilities(std::vector<float> &probabilities) {
    PaintBucket::probabilities = probabilities;
    PaintBucket::max_probability = *std::max_element(probabilities.begin(), probabilities.end());
}


} // namespace paint
