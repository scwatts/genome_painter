#ifndef __PAINT_H__
#define __PAINT_H__


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
bool get_most_probable_kmer(kmer::KmerPairBin kmer_pair, db::Database &database, PaintBucket *probabilities);
void compare_paint(std::vector<PaintBucket> &paint, PaintBucket &bucket, size_t i);


} // namespace paint


#endif
