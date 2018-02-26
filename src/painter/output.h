#ifndef __OUTPUT_H__
#define __OUTPUT_H__


#include <string>
#include <vector>


#include "paint.h"
#include "merger/file.h"


namespace output {


std::string construct_output_fp(std::string &genome_fp, std::string &suffix, std::string output_dir);
void write_painted_genome(std::vector<paint::FastaPaint> &fasta_painting, std::vector<file::SpeciesCount> species_names, std::string &output_fp);


} // namespace output


#endif
