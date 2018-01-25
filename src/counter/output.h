#ifndef __OUTPUT_H__
#define __OUTPUT_H__


#include <string>
#include <vector>


#include "count.h"
#include "../common.h"


namespace output {


void wite_kmer_counts(count::countmap &kmer_counts, unsigned int species_counts, std::string &output_fp);


} // namespace output


#endif
