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

    KmerData(std::vector<file::SpeciesCount> *_species_counts): species_counts(_species_counts) {}
};

struct Parameters {
    float threshold;
    float alpha;
    std::string *output_fp;
};

void write_magic_bits(std::string &output_fp);
void write_species_counts_header(std::vector<file::SpeciesCount> &species_counts, std::string &output_fp);
void write_completed_counts(KmerData &data, Parameters &parameters);


} // namespace output


#endif
