#ifndef __MERGE_H__
#define __MERGE_H__


#include <limits>
#include <string>
#include <unordered_map>
#include <vector>


namespace merge {


typedef std::unordered_map<unsigned long long,std::vector<unsigned int>> countmap;
typedef unsigned long long ullong;

struct Bincodes {
    ullong max = 0;
    ullong min = std::numeric_limits<ullong>::max();
};

void add_partial_reads(std::vector<file::CountFile> &fileobjects, size_t species_count, Bincodes &bincodes, countmap &kmer_db);
void write_completed_counts(countmap &kmer_db, std::vector<unsigned int> &species_counts, Bincodes &bincodes, float threshold, float alpha, std::string &output_fp);
bool passes_threshold(std::vector<float> probabilities, float threshold);


} // namespace merge


#endif
