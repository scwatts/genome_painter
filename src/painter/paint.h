#ifndef __PAINT_H__
#define __PAINT_H__


#include <algorithm>
#include <vector>


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

std::vector<PaintBucket> paint_sequence(genome::FastaRecord &fasta, db::Database &database);
bool get_probabilities(std::string &sequence, size_t i, kmer::bitshifter bitshift_op, kmer::encoder encode_op, std::vector<float> &probabilities, db::Database &database);
PaintBucket get_best_probabilities(std::vector<float> f_probabilities, std::vector<float> r_probabilities);
void compare_paint(std::vector<PaintBucket> &paint, PaintBucket &bucket, size_t i);


} // namespace paint


#endif
