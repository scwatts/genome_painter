#include "output.h"


namespace output {


void write_kmer_counts(count::countmap &kmer_counts, unsigned int species_counts, std::string &output_fp) {
    FILE *output_fh = fopen(output_fp.c_str(), "w");

    // Header
    fprintf(output_fh, "#%u\n", species_counts);

    // Data
    for (const auto& items : kmer_counts) {
        fprintf(output_fh, "%llu\t%d\n", items.first, items.second);
    }

    fclose(output_fh);
}


} // namespace output
