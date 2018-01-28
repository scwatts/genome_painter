#include "probability.h"


namespace prob {


std::vector<float> calculate_probabilities(std::vector<unsigned int> &kmer_counts, std::vector<file::SpeciesCount> &species_counts, float alpha) {
    std::vector<float> probabilities;
    probabilities.reserve(kmer_counts.size());
    float probability_sum = 0;
    for (size_t i = 0; i < kmer_counts.size(); ++i) {
        float probability = (kmer_counts[i] + alpha) / (species_counts[i].count + species_counts.size() * alpha);
        probability_sum += probability;
        probabilities.push_back(probability);
    }

    std::vector<float> normalised_probabilties;
    normalised_probabilties.reserve(kmer_counts.size());
    for (auto& p : probabilities) {
        normalised_probabilties.push_back(p / probability_sum);
    }

    return normalised_probabilties;
}


} // namespace prob
