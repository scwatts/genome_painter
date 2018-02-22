#include "output.h"


namespace output {


void write_species_counts_header(std::vector<file::SpeciesCount> &species_counts, sqlite3 *db) {
    // TODO: clean this up
    char *errmsg;
    std::string create_table_sql("CREATE TABLE species_counts (species TEXT, count INTEGER)");
    if (sqlite3_exec(db, create_table_sql.c_str(), NULL, NULL, &errmsg) != SQLITE_OK) {
        fprintf(stderr, "Can't create table species count table: %s\n", errmsg);
        sqlite3_close(db);
        exit(1);
    }

    for (auto& species_count : species_counts) {
        // TODO: don't use append...
        std::string insert_sql("INSERT INTO species_counts VALUES (\"");
        insert_sql.append(species_count.name);
        insert_sql.append("\", ");
        insert_sql.append(std::to_string(species_count.count));
        insert_sql.append(")");

        if (sqlite3_exec(db, insert_sql.c_str(), NULL, NULL, &errmsg) != SQLITE_OK) {
            fprintf(stderr, "Could not execute statement: %s\n", errmsg);
            sqlite3_close(db);
            exit(1);
        }
    }
}


void write_completed_counts(common::countvecmap &kmer_db, std::vector<file::SpeciesCount> &species_counts, merge::Bincodes &bincodes, float threshold, float alpha, sqlite3_stmt *stmt, sqlite3 *db) {
    // Process current data; we should now have all data up to min_bincode
    // We must account for kmers which have not yet been read in for all species

    // Begin SQL transaction
    char *errmsg = 0;
    if (SQLITE_OK != sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &errmsg)) {
        fprintf(stderr, "Could not execute SQL: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }

    auto it = kmer_db.begin();
    while (it != kmer_db.end()) {
        if (it->first > bincodes.min) {
            ++it;
            continue;
        }

        // Calculate probabilty and exclude on threshold
        std::vector<float> probabilities = prob::calculate_probabilities(it->second, species_counts, alpha);

        if (merge::passes_threshold(probabilities, threshold)) {
            // Add bincode to SQL insert statement
            if (SQLITE_OK != sqlite3_bind_int64(stmt, 1, it->first)) {
                fprintf(stderr, "Error binding value in insert: %s\n", sqlite3_errmsg(db));
                sqlite3_close(db);
                exit(1);
            }

            // Format SQL insert with probabilities; base-1 indexing
            int i = 2;
            for (auto & p : probabilities) {
                if (SQLITE_OK != sqlite3_bind_double(stmt, i++, p)) {
                    fprintf(stderr, "Error binding value in insert: %s\n", sqlite3_errmsg(db));
                    sqlite3_close(db);
                    exit(1);
                }
            }

            // Runs insert statement and then reset the statement object
            if (SQLITE_DONE != sqlite3_step(stmt)) {
                fprintf(stderr, "Error evaluating statement: %s\n", sqlite3_errmsg(db));
                sqlite3_close(db);
                exit(1);
            }
            sqlite3_reset(stmt);
        }
        // Remove from map once done
        it = kmer_db.erase(it);
    }
    // Complete the transaction
    sqlite3_exec(db, "END TRANSACTION", NULL, NULL, &errmsg);
}


} // namespace output
