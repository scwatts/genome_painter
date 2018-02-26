#include "paint.h"


namespace paint {

std::vector<PaintBucket> paint_sequence(genome::FastaRecord &fasta, db::Database &database) {
    // Forward and reverse kmers
    std::vector<PaintBucket> paint(fasta.sequence.size(), { {}, 0 });
    for (size_t i = 0; i < fasta.sequence.size(); i++) {
        // Encode kmer and get probabilities
        bool f_status, r_status;
        std::vector<float> f_probabilities, r_probabilities;
        f_status = get_probabilities(fasta.sequence, i, kmer::bitshift_forward, kmer::encode_forward, f_probabilities, database);
        r_status = get_probabilities(fasta.sequence, i, kmer::bitshift_reverse, kmer::encode_reverse, r_probabilities, database);

        // If forward and reverse kmers are good, select kmer with best probabilities
        PaintBucket kmer_probabilities;
        if (f_status && r_status) {
            kmer_probabilities = get_best_probabilities(f_probabilities, r_probabilities);
        } else if (f_status) {
            kmer_probabilities.set_probabilities(f_probabilities);
        } else if (r_status) {
            kmer_probabilities.set_probabilities(r_probabilities);
        } else {
            continue;
        }

        // Add current kmer_probabilities is they're higher than what is currently present
        compare_paint(paint, kmer_probabilities, i);
    }
    return paint;
}


bool get_probabilities(std::string &sequence, size_t i, kmer::bitshifter bitshift_op,
                        kmer::encoder encode_op, std::vector<float> &probabilities, db::Database &database) {
    // Encode
    common::ullong bincode;
    if (! kmer::encode_kmer(sequence, i, bincode, bitshift_op, encode_op)) {
        return false;
    }

    // Get probabilities
    if (database.cache.find(bincode) != database.cache.end()) {
        // Cache hit
        probabilities = database.cache[bincode];
        return true;
    } else if (database.index.find(bincode) != database.index.end()) {
        // Cache miss; read in data, place into cache
        probabilities = db::read_bincode_probabilities(bincode, database);
        database.cache[bincode] = probabilities;
        return true;
    } else {
        return false;
    }
}


PaintBucket get_best_probabilities(std::vector<float> f_probabilities, std::vector<float> r_probabilities) {
    PaintBucket forward, reverse;
    forward.set_probabilities(f_probabilities);
    reverse.set_probabilities(r_probabilities);

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
