#include "paint.h"


namespace paint {


std::vector<PaintBucket> paint_sequence(genome::FastaRecord &fasta, db::Database &database) {
    std::vector<PaintBucket> paint(fasta.sequence.size(), { {}, 0 });

    // Forward and reverse kmers
    for (size_t i = 0; i < fasta.sequence.size(); i++) {
        // Attempt to encode (currently we're unable to encode nucleotides other than atgc)
        common::ullong f_bincode, r_bincode;
        bool f_status, r_status;
        f_status = kmer::encode_kmer(fasta.sequence, i, f_bincode, kmer::bitshift_forward, kmer::encode_forward);
        r_status = kmer::encode_kmer(fasta.sequence, i, r_bincode, kmer::bitshift_reverse, kmer::encode_reverse);

        // Check for presence in database
        if ( !(f_status && database.probability_map.find(f_bincode) != database.probability_map.end()) ) { f_status = false; }
        if ( !(r_status && database.probability_map.find(r_bincode) != database.probability_map.end()) ) { r_status = false; }

        // If forward and reverse kmers are good, select kmer with best probabilities
        PaintBucket kmer_probabilities;
        if (f_status && r_status) {
            kmer_probabilities = get_best_probabilities(f_bincode, r_bincode, database);
        } else if (f_status) {
            kmer_probabilities.set_probabilities(database.probability_map[f_bincode]);
        } else if (r_status) {
            kmer_probabilities.set_probabilities(database.probability_map[r_bincode]);
        } else {
            continue;
        }

        // Add current kmer_probabilities is they're higher than what is currently present
        compare_paint(paint, kmer_probabilities, i);
    }
    return paint;
}


PaintBucket get_best_probabilities(common::ullong f_bincode, common::ullong r_bincode, db::Database &database) {
    PaintBucket forward, reverse;
    forward.set_probabilities(database.probability_map[f_bincode]);
    reverse.set_probabilities(database.probability_map[r_bincode]);

    if (forward.max_probability > reverse.max_probability) {
        return forward;
    } else {
        return reverse;
    }
}


void compare_paint(std::vector<PaintBucket> &paint, PaintBucket &bucket, size_t i) {
    size_t end_idx;
    if (i+32 > paint.size()) {
        end_idx = paint.size();
    } else {
        end_idx = i + 32;
    }

    // TODO: to save memory we could use a reference and one of each bucket on the stack
    for ( ; i < end_idx; i++) {
        if (paint[i].max_probability < bucket.max_probability) {
            paint[i] = bucket;
        }
    }
}

void PaintBucket::set_probabilities(std::vector<float> &probabilities) {
    PaintBucket::probabilities = probabilities;
    PaintBucket::max_probability = *std::max_element(probabilities.begin(), probabilities.end());
}


} // namespace paint
