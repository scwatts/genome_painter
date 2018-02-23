#ifndef __SQL_H__
#define __SQL_H__


#include <string>


#include "sqlite3.h"


#include "lib/common.h"


namespace sql {


extern std::string species_select;

std::string generate_select_statement(common::ullong bincode);


} // namespace sql


#endif
