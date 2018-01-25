#ifndef __MERGE_H__
#define __MERGE_H__


#include <limits>
#include <string>
#include <unordered_map>
#include <vector>


#include "file.h"
#include "probability.h"
#include "../common.h"


namespace merge {


struct Bincodes {
    common::ullong max = 0;
    common::ullong min = std::numeric_limits<common::ullong>::max();
};

void add_partial_reads(std::vector<file::CountFile> &fileobjects, size_t species_count, Bincodes &bincodes, common::countvecmap &kmer_db);
void write_completed_counts(common::countvecmap &kmer_db, std::vector<unsigned int> &species_counts, Bincodes &bincodes, float threshold, float alpha, std::string &output_fp);
bool passes_threshold(std::vector<float> probabilities, float threshold);


} // namespace merge


#endif
