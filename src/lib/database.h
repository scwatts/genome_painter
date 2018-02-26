#ifndef __DATABASE_H__
#define __DATABASE_H__


#include <cstring>
#include <string>
#include <map>
#include <vector>


#include "lib/common.h"
#include "merger/file.h"


#define DB_MAGIC "DBBR"
#define SPECIES_NAME_FIELD_SIZE 64
#define SPECIES_COUNT_FIELD_SIZE sizeof(unsigned int)
#define BINCODE_FIELD_SIZE sizeof(common::ullong)
#define PROB_FIELD_SIZE sizeof(float)


namespace db {


struct HeaderInfo {
    unsigned int size;
    unsigned int species_num;
    std::vector<file::SpeciesCount> species_counts;
    long int offset;
};

struct Database {
    HeaderInfo header;
    std::string fp;
    std::map<common::ullong, long int> index;
    std::map<common::ullong,std::vector<float>> cache;

    Database(std::string _fp): fp(_fp) {}
};

void read_header(Database &database);
void read_index(Database &database);
std::vector<float> read_bincode_probabilities(common::ullong bincode, Database &database);


} // namespace db

#endif
