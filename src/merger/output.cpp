#include "output.h"


namespace output {


void write_species_counts_header(std::vector<file::SpeciesCount> &species_counts, std::string &output_fp) {
    FILE *output_fh = fopen(output_fp.c_str(), "w");
    for (auto& species_count : species_counts) {
        fprintf(output_fh, "#%s\t%d\n", species_count.name.c_str(), species_count.count);
    }
    fclose(output_fh);
}


void write_completed_counts(common::countvecmap &kmer_db, std::vector<file::SpeciesCount> &species_counts, merge::Bincodes &bincodes, float threshold, float alpha, std::string &output_fp) {
    // Process current data; we should now have all data up to min_bincode
    // We must account for kmers which have not yet been read in for all species
    FILE *output_fh = fopen(output_fp.c_str(), "a");
    auto it = kmer_db.begin();
    while (it != kmer_db.end()) {
        if (it->first > bincodes.min) {
            ++it;
            continue;
        }

        // Calculate probabilty and exclude on threshold
        std::vector<float> probabilities = prob::calculate_probabilities(it->second, species_counts, alpha);

        if (merge::passes_threshold(probabilities, threshold)) {
            fprintf(output_fh, "%llu", it->first);
            for (auto & k : it->second) {
                fprintf(output_fh, "\t%d", k);
            }
            for (auto & p : probabilities) {
                fprintf(output_fh, "\t%f", p);
            }
            fprintf(output_fh, "\n");
        }

        // Remove from map once done
        it = kmer_db.erase(it);

    }
    fclose(output_fh);
}


} // namespace output
