#include "paint.h"


namespace paint {


std::vector<PaintBucket> paint_sequence(genome::FastaRecord &fasta, sqlite3 *dbp) {
    std::vector<PaintBucket> paint(fasta.sequence.size(), { {}, 0 });

    // Forward and reverse kmers
    for (size_t i = 0; i < fasta.sequence.size(); i++) {
        // Get kmer probabilities
        bool f_status, r_status;
        std::vector<float> f_probabilities, r_probabilities;
        f_status = get_probabilities(fasta.sequence, i, kmer::bitshift_forward, kmer::encode_forward, f_probabilities, dbp);
        r_status = get_probabilities(fasta.sequence, i, kmer::bitshift_reverse, kmer::encode_reverse, r_probabilities, dbp);

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


bool get_probabilities(std::string &sequence, size_t i, kmer::bitshifter bop, kmer::encoder eop, std::vector<float> &probabilities, sqlite3 *dbp) {
        common::ullong bincode;
        bool status;

        // Attempt to encode (currently we're unable to encode nucleotides other than atgc)
        if (! kmer::encode_kmer(sequence, i, bincode, bop, eop)) {
            return false;
        }

        // Check for presence in database
        char *errmsg;
        std::string select_sql = sql::generate_select_statement(bincode);
        if (sqlite3_exec(dbp, select_sql.c_str(), db::vector_fill_probabilities, &probabilities, &errmsg) != SQLITE_OK) {
            fprintf(stderr, "Error retrieving from database: %s\n", sqlite3_errmsg(dbp));
            sqlite3_close(dbp);
            exit(1);
        }

        return ! probabilities.empty();
}


PaintBucket get_best_probabilities(std::vector<float> &f_probabilities, std::vector<float> &r_probabilities) {
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
