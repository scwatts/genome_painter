#ifndef __COUNT_H__
#define __COUNT_H__


#include <set>
#include <string>


#include "../common.h"
#include "../kmer.h"


namespace count {


typedef std::unordered_map<common::ullong,unsigned int> countmap;

void collect_kmers(std::string &sequence, std::set<common::ullong> &sample_kmers);


} // namespace count


#endif
