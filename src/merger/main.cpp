#include <limits>


#include "command_line_options.h"
#include "file.h"
#include "merge.h"
#include "output.h"
#include "lib/common.h"


int main(int argc, char *argv[]) {
    // Get command line arguments
    cmdline::Options options = cmdline::get_arguments(argc, argv);

    // Get CountFile instances ordered by filesize
    std::vector<file::CountFile> fileobjects = file::init_count_file_objects(options.count_fps);

    // Check that each file is sorted
    for (auto& fileobject: fileobjects) {
        if (!file::is_sorted(fileobject)) {
            fprintf(stderr, "File %s does not appear to be sorted\n", fileobject.filepath.c_str());
            exit(1);
        }
    }

    // Get species counts
    std::vector<file::SpeciesCount> species_counts;
    for (auto& fileobject : fileobjects) {
        species_counts.push_back(file::get_species_counts(fileobject));
    }

    // Write magic bits and species count header, getting inclusive header size
    output::write_magic_bits(options.output_fp);
    long int header_size = output::write_species_counts_header(species_counts, options.output_fp);

    // Set up data structures for clarity
    output::KmerData kmer_data(&species_counts);
    output::Parameters parameters = { options.threshold, options.alpha, &options.output_fp };
    output::Index indices(header_size, (PROB_FIELD_SIZE * species_counts.size()) + BINCODE_FIELD_SIZE);

    // Merge kmer counts, count probabilites and write out
    unsigned int iteration = 0;
    while (true) {
        ++iteration;
        fprintf(stdout, "Iteration %d:\n", iteration);

        // Read in some lines and add them to kmer_db
        merge::add_partial_reads(fileobjects, fileobjects.size(), kmer_data.bincodes, kmer_data.kmer_db);
        fprintf(stdout, "\n\tMax bincodes [%llu-%llu]\n", kmer_data.bincodes.min, kmer_data.bincodes.max);

        // If all files at consumed, force kmer_db to empty by setting bincodes.min
        bool all_consumed = true;
        for (auto& fo : fileobjects) {
            if (!fo.filehandle.eof()) {
                all_consumed = false;
                break;
            }
        }

        if (all_consumed) {
            kmer_data.bincodes.min = std::numeric_limits<common::ullong>::max();
            output::write_completed_counts(kmer_data, parameters, indices);
            output::write_indices(indices, options.output_fp);
            break;
        } else {
            output::write_completed_counts(kmer_data, parameters, indices);
            output::write_indices(indices, options.output_fp);
        }
        fprintf(stdout, "\tIncomplete counts %lu\n", kmer_data.kmer_db.size());
    }

    return 0;
}
