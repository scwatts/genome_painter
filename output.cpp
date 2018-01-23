#include "output.h"


namespace output {


void write_painted_genome(std::vector<paint::FastaPaint> &fasta_painting, std::string &output_fp) {
    FILE *output_fh = fopen(output_fp.c_str(), "w");

    for (const auto& fasta_paint : fasta_painting) {
        size_t position = 0;
        for (const auto& paint_bucket : fasta_paint.paint) {
            position++;

            // Write only if we have probabilities
            if (!paint_bucket.probabilities.empty()) {
                fprintf(output_fh, "%s\t%lu", fasta_paint.name.c_str(), position);
                for (auto probability : paint_bucket.probabilities) {
                    fprintf(output_fh, "\t%f", probability);
                }
                fprintf(output_fh, "\n");
            }
        }
    }

    fclose(output_fh);
}


} // namespace output
