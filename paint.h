#ifndef __PAINT_H__
#define __PAINT_H__


#include <vector>


namespace paint {


struct PaintBucket {
    std::vector<float> probabilities;
    float max_probability;
};

std::vector<PaintBucket> paint_sequence(genome::FastaRecord &fasta, db::Database &database);


} // namespace paint


#endif
