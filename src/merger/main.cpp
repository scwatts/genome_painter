#include <limits>


#include "sqlite3.h"


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

    // SQLite database connection
    sqlite3 *db;
    if (sqlite3_open(options.output_fp.c_str(), &db) != SQLITE_OK) {
        fprintf(stderr, "Can't open database %s: %s\n", options.output_fp.c_str(), sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }

    // Generate SQL statments
    std::string create_table_sql("CREATE TABLE kmer_probabilities (bincode INTEGER PRIMARY KEY");
    std::string insert_sql("INSERT INTO kmer_probabilities VALUES (?");
    for (auto& species_count : species_counts) {
        char table_str[100]; snprintf(table_str, 100, ", %s REAL", species_count.name.c_str()); create_table_sql.append(table_str);
        insert_sql.append(", ?");
    }
    create_table_sql.append(")"); insert_sql.append(")");

    // Create SQL probability table
    char *errmsg;
    if (sqlite3_exec(db, create_table_sql.c_str(), NULL, NULL, &errmsg) != SQLITE_OK) {
        fprintf(stderr, "Can't create table for %s: %s\n", options.output_fp.c_str(), errmsg);
        sqlite3_close(db);
        exit(1);
    }

    // SQLite insert statement template
    unsigned int buffer_size = 256;
    char zSQL[buffer_size] = "\0";
	char *ztail = 0;
    sqlite3_stmt *stmt;
    sprintf(zSQL, insert_sql.c_str());
    if (sqlite3_prepare_v2(db, zSQL, buffer_size, &stmt, (const char **)&ztail) != SQLITE_OK) {
        fprintf(stderr, "Unable to prepare SQL statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }

    // Write species count header
    // TODO: separate table for species counts
    output::write_species_counts_header(species_counts, db);

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
            output::write_completed_counts(kmer_db, species_counts, bincodes, options.threshold, options.alpha, stmt, db);
            break;
        } else {
            output::write_completed_counts(kmer_db, species_counts, bincodes, options.threshold, options.alpha, stmt, db);
        }
        fprintf(stdout, "\tIncomplete counts %lu\n", kmer_db.size());
    }

    // TODO: close all file objects
    // TODO: do we need to manually index lookup column?

    // Vacuum SQLite database
    if (sqlite3_exec(db, "VACUUM", NULL, NULL, &errmsg) != SQLITE_OK) {
        fprintf(stderr, "Failed to vacuum: %s\n", errmsg);
        sqlite3_close(db);
        exit(1);
    }


    // Close database and destory statement object
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return 0;
}
