#ifndef __COUNT_H__
#define __COUNT_H__


#include <map>
#include <set>
#include <string>


#include "lib/common.h"
#include "lib/kmer.h"


namespace count {


typedef std::map<common::bint,unsigned int> countmap;

void collect_kmers(std::string &sequence, std::set<common::bint> &sample_kmers);


} // namespace count


#endif
