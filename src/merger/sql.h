#ifndef __SQL_H__
#define __SQL_H__


#include <string>


#include "sqlite3.h"


#include "file.h"
#include "lib/database.h"


namespace sql {


std::string generate_create_sql(std::vector<file::SpeciesCount> &species_counts);
sqlite3_stmt *generate_insert_stmt(int species_count, sqlite3 *dbp);


} // namespace sql


#endif
