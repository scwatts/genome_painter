#ifndef __PAINT_H__
#define __PAINT_H__


#include <algorithm>
#include <vector>


#include "../genome.h"
#include "../database.h"
#include "../kmer.h"


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
PaintBucket get_best_probabilities(common::ullong f_bincode, common::ullong r_bincode, db::Database &database);
void compare_paint(std::vector<PaintBucket> &paint, PaintBucket &bucket, size_t i);


} // namespace paint


#endif
