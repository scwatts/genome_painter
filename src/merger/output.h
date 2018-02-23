#ifndef __OUTPUT_H__
#define __OUTPUT_H__


#include <string>
#include <vector>


#include "file.h"
#include "merge.h"
#include "lib/common.h"
#include "lib/database.h"


namespace output {


struct KmerData {
    common::countvecmap kmer_db;
    std::vector<file::SpeciesCount> *species_counts;
    merge::Bincodes bincodes;
};

struct Parameters {
    float threshold;
    float alpha;
    std::string *output_fp;
};

struct IndexEntry {
    common::ullong bincode;
    long int position;
};

struct Index {
    long int last_position = 0;
    long int record_size;
    std::vector<IndexEntry> entries;
};

void write_magic_bits(std::string &output_fp);
long int write_species_counts_header(std::vector<file::SpeciesCount> &species_counts, std::string &output_fp);
void write_completed_counts(KmerData &data, Parameters &parameters, Index &indices);
void write_indices(Index &indices, std::string output_fp);


} // namespace output


#endif
