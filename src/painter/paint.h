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
    float max_probability = 0;

    explicit PaintBucket(std::vector<float> &_probablities) :
        probabilities(std::move(_probablities)),
        max_probability(*std::max_element(probabilities.begin(), probabilities.end())) { }

    explicit PaintBucket(size_t _probability_size) { probabilities.reserve(_probability_size); }
};

struct FastaPaint {
    std::string name;
    std::vector<PaintBucket> paint;
};

std::vector<PaintBucket> paint_sequence(genome::FastaRecord &fasta, db::Database &database);
bool get_probabilities(std::string &sequence, size_t i, std::vector<float> &probabilities, db::Database &database);
void compare_paint(std::vector<PaintBucket> &paint, PaintBucket &bucket, size_t i);


} // namespace paint


#endif
