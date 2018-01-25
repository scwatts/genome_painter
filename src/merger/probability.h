#ifndef __PROBABILITY_H__
#define __PROBABILITY_H__


#include <string>
#include <vector>


namespace prob {


std::vector<float> calculate_probabilities(std::vector<unsigned int> &kmer_counts, std::vector<unsigned int> &species_count, float alpha);


} // namespace prob


#endif
