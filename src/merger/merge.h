#ifndef __MERGE_H__
#define __MERGE_H__


#include <limits>
#include <string>
#include <unordered_map>
#include <vector>


#include "file.h"
#include "probability.h"
#include "lib/common.h"


namespace merge {


struct Bincodes {
    common::bint max = 0;
    common::bint min = std::numeric_limits<common::bint>::max();
};

void add_partial_reads(std::vector<file::CountFile> &fileobjects, size_t species_count, Bincodes &bincodes, common::countvecmap &kmer_db);
bool passes_threshold(std::vector<float> probabilities, float threshold);


} // namespace merge


#endif
