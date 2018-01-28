#include "output.h"


namespace output {


std::string construct_output_fp(std::string &genome_fp, std::string &suffix, std::string output_dir) {
    // Placing a trailing slash on output filepath if it doesn't already exist
    if (output_dir.back() != '/') {
        output_dir.push_back('/');
    }

    // Get base filename from path
    std::string fn = genome_fp.substr(genome_fp.find_last_of('/') + 1);

    // Attempt to drop some file extensions
    fn = fn.substr(0, fn.find(".gz"));
    fn = fn.substr(0, fn.find(".fasta"));
    fn = fn.substr(0, fn.find(".fna"));

    // ...giving up now
    fn.append(suffix);
    return output_dir.append(fn);
}


void write_painted_genome(std::vector<paint::FastaPaint> &fasta_painting, std::vector<std::string> species_names, std::string &output_fp) {
    FILE *output_fh = fopen(output_fp.c_str(), "w");

    // Header
    for (const auto& species_name : species_names) {
        fprintf(output_fh, "#%s\n", species_name.c_str());
    }

    // Data
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
