#ifndef __GENOME_H__
#define __GENOME_H__


#include <string>
#include <vector>


#include <zlib.h>


#include "kseq.h"


namespace genome {


// TODO: combine name and comment to create full FASTA description
struct FastaRecord {
    std::string name;
    std::string sequence;
};

std::vector<FastaRecord> read_fasta_records(std::string &input_fp);


} // namespace genome


#endif
