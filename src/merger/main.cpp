#include <limits>


#include "sqlite3.h"


#include "command_line_options.h"
#include "file.h"
#include "merge.h"
#include "sql.h"
#include "output.h"
#include "lib/common.h"
#include "lib/database.h"


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

    // SQLite database connection
    sqlite3 *dbp = db::open_and_configure(options.output_fp.c_str());

    // Create tables and write out species counts
    output::write_species_counts_header(species_counts, dbp);
    std::string create_table_sql = sql::generate_create_sql(species_counts);
    db::execute(dbp, create_table_sql.c_str());

    // Get SQL statement object for inserts
    sqlite3_stmt *stmt = sql::generate_insert_stmt(species_counts.size(), dbp);

    // Merge kmer counts, count probabilites and write out
    common::countvecmap kmer_db;
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
            bincodes.min = std::numeric_limits<common::ullong>::max();
            output::write_completed_counts(kmer_db, species_counts, bincodes, options.threshold, options.alpha, stmt, dbp);
            break;
        } else {
            output::write_completed_counts(kmer_db, species_counts, bincodes, options.threshold, options.alpha, stmt, dbp);
        }
        fprintf(stdout, "\tIncomplete counts %lu\n", kmer_db.size());
    }

    // TODO: close all file objects
    // TODO: do we need to manually index lookup column?

    // Vacuum SQLite database
    db::execute(dbp, "VACUUM");

    // Close database and destory statement object
    sqlite3_finalize(stmt);
    sqlite3_close(dbp);

    return 0;
}
