#include <algorithm>


#include "command_line_options.h"
#include "file.h"
#include "merge.h"


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
    std::vector<unsigned int> species_counts;
    for (auto& fileobject : fileobjects) {
        species_counts.push_back(file::get_species_counts(fileobject));
    }

    // Merge kmer counts, count probabilites and write out
    merge::countmap kmer_db;
    merge::Bincodes bincodes;
    unsigned int iteration = 0;
    while (true) {
        ++iteration;
        fprintf(stdout, "Iteration %d:\n", iteration);

        // Read in some lines and add them to kmer_db
        merge::add_partial_reads(fileobjects, fileobjects.size(), bincodes, kmer_db);
        fprintf(stdout, "\n\tMax bincodes [%llu-%llu]\n", bincodes.min, bincodes.max);

        // If all files at consumed, force kmer_db to empty by setting bincodes.min
        bool all_consumed = true;
        for (auto& fo : fileobjects) {
            if (!fo.filehandle.eof()) {
                all_consumed = false;
                break;
            }
        }

        if (all_consumed) {
            bincodes.min = std::numeric_limits<merge::ullong>::max();
            merge::write_completed_counts(kmer_db, species_counts, bincodes, options.threshold, options.alpha, options.output_fp);
            break;
        } else {
            merge::write_completed_counts(kmer_db, species_counts, bincodes, options.threshold, options.alpha, options.output_fp);
        }
        fprintf(stdout, "\tIncomplete counts %lu\n", kmer_db.size());
    }

    return 0;
}
