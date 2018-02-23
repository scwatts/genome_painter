#include "output.h"


namespace output {


void write_species_counts_header(std::vector<file::SpeciesCount> &species_counts, sqlite3 *dbp) {
    // Create table
    db::start_transaction(dbp);
    db::execute(dbp, "CREATE TABLE species_counts (species TEXT, count INTEGER)");

    // Add data
    for (auto& species_count : species_counts) {
        // TODO: don't use append...
        std::string insert_sql("INSERT INTO species_counts VALUES (\"");
        insert_sql.append(species_count.name);
        insert_sql.append("\", ");
        insert_sql.append(std::to_string(species_count.count));
        insert_sql.append(")");
        db::execute(dbp, insert_sql.c_str());
    }
    db::end_transaction(dbp);
}


void write_completed_counts(common::countvecmap &kmer_db, std::vector<file::SpeciesCount> &species_counts, merge::Bincodes &bincodes, float threshold, float alpha, sqlite3_stmt *stmt, sqlite3 *dbp) {
    // Process current data; we should now have all data up to min_bincode
    // We must account for kmers which have not yet been read in for all species
    db::start_transaction(dbp);
    auto it = kmer_db.begin();
    while (it != kmer_db.end()) {
        if (it->first > bincodes.min) {
            ++it;
            continue;
        }

        // Calculate probabilty and exclude on threshold
        std::vector<float> probabilities = prob::calculate_probabilities(it->second, species_counts, alpha);

        if (merge::passes_threshold(probabilities, threshold)) {
            // Add bincode to SQL insert statement and format SQL with probabilities
            db::bind_int64(stmt, 1, it->first, dbp);
            int i = 2;
            for (auto & p : probabilities) {
                db::bind_double(stmt, i++, p, dbp);
            }

            // Runs insert statement and then reset the statement object
            db::step(stmt, dbp); sqlite3_reset(stmt);
        }
        // Remove from map once done
        it = kmer_db.erase(it);
    }
    // Complete the transaction
    db::end_transaction(dbp);
}


} // namespace output
