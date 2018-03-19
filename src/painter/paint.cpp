#include "paint.h"


namespace paint {

std::vector<PaintBucket> paint_sequence(genome::FastaRecord &fasta, db::Database &database) {
    std::vector<PaintBucket> paint(fasta.sequence.size(), PaintBucket(database.header.species_num));
    for (size_t i = 0; i < fasta.sequence.size(); i++) {
        std::vector<float> probabilities;
        if(get_probabilities(fasta.sequence, i, probabilities, database)) {
            // Add current kmer_probabilities if they're higher than what is currently present
            PaintBucket kmer_probabilities(probabilities);
            compare_paint(paint, kmer_probabilities, i);
        }
    }
    return paint;
}


bool get_probabilities(std::string &sequence, size_t i, std::vector<float> &probabilities, db::Database &database) {
    // Encode
    common::ullong bincode;
    if (! kmer::encode_kmer(sequence, i, bincode)) {
        return false;
    }

    // Get probabilities
    if (database.probabilities.find(bincode) != database.probabilities.end()) {
        probabilities = database.probabilities[bincode];
        return true;
    } else {
        return false;
    }
}


void compare_paint(std::vector<PaintBucket> &paint, PaintBucket &bucket, size_t i) {
    size_t end_idx;
    if (i+KMER_SIZE > paint.size()) {
        end_idx = paint.size();
    } else {
        end_idx = i + KMER_SIZE;
    }

    // TODO: to save memory we could use a reference and one of each bucket on the stack
    for ( ; i < end_idx; i++) {
        if (paint[i].max_probability < bucket.max_probability) {
            paint[i] = bucket;
        }
    }
}


} // namespace paint
