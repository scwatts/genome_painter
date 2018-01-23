#ifndef __OUTPUT_H__
#define __OUTPUT_H__


#include <string>
#include <vector>


#include "paint.h"


namespace output {


void write_painted_genome(std::vector<paint::FastaPaint> &fasta_painting, std::string &output_fp);


} // namespace output


#endif
