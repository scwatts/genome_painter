#include "sql.h"


namespace sql {


std::string generate_create_sql(std::vector<file::SpeciesCount> &species_counts) {
    std::string create_table_sql("CREATE TABLE kmer_probabilities (bincode INTEGER PRIMARY KEY");
    for (auto& species_count : species_counts) {
        char table_str[100];
        snprintf(table_str, 100, ", %s REAL", species_count.name.c_str());
        create_table_sql.append(table_str);
    }
    create_table_sql.append(")");
    return create_table_sql;
}


sqlite3_stmt *generate_insert_stmt(int species_count, sqlite3 *dbp) {
    std::string insert_sql("INSERT INTO kmer_probabilities VALUES (?");
    for (int i = 0; i < species_count; i++) { insert_sql.append(", ?"); }
    insert_sql.append(")");
    return db::prepare_statement(insert_sql.c_str(), dbp);
}


std::string index_sql("CREATE UNIQUE INDEX bincode_index ON kmer_probabilities (bincode)");


} // namespace sql
