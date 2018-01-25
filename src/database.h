#ifndef __DATABASE_H__
#define __DATABASE_H__


#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>


namespace db {


typedef std::unordered_map<unsigned long long,std::vector<float>> probmap;

struct Database {
    std::vector<std::string> species_names;
    probmap probability_map;
};

db::Database read_database(std::string &filepath);


} // namesspace db


#endif
