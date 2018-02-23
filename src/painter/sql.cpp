#include "sql.h"


namespace sql {


std::string species_select("SELECT species FROM species_counts");

std::string generate_select_statement(common::ullong bincode) {
    std::string select_sql("SELECT * FROM kmer_probabilities WHERE bincode ==");
    select_sql.append(std::to_string(bincode));
    return select_sql;
}


} // namespace sql
