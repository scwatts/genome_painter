#ifndef __PAINT_H__
#define __PAINT_H__


#include <algorithm>
#include <vector>


#include "sqlite3.h"


#include "sql.h"
#include "lib/database.h"
#include "lib/genome.h"
#include "lib/kmer.h"


namespace paint {


struct PaintBucket {
    std::vector<float> probabilities;
    float max_probability;

    void set_probabilities(std::vector<float> &probablities);
};

struct FastaPaint {
    std::string name;
    std::vector<PaintBucket> paint;
};

std::vector<PaintBucket> paint_sequence(genome::FastaRecord &fasta, sqlite3 *dbp);
PaintBucket get_best_probabilities(common::ullong f_bincode, common::ullong r_bincode);
PaintBucket get_best_probabilities(std::vector<float> &f_probabilities, std::vector<float> &r_probabilities);
bool get_probabilities(std::string &sequence, size_t i, kmer::bitshifter bop, kmer::encoder eop, std::vector<float> &probabilities, sqlite3 *dbp);
void compare_paint(std::vector<PaintBucket> &paint, PaintBucket &bucket, size_t i);


} // namespace paint


#endif
