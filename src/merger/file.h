#ifndef __FILE_H__
#define __FILE_H__


#include <algorithm>
#include <fstream>
#include <string>
#include <vector>


#include <sys/stat.h>


#include "lib/common.h"


namespace file {


struct CountFile {
    std::string filepath;
    off_t filesize;
    std::ifstream filehandle;
};

struct SpeciesCount {
    std::string name;
    unsigned int count;

};

std::vector<CountFile> init_count_file_objects(std::vector<std::string> &filepaths);
void sort_filepaths_by_filesize(std::vector<std::string> &filepaths);
bool fileobject_filesize_comp(CountFile &a, CountFile &b);
bool is_sorted(CountFile &fileobject);
SpeciesCount get_species_counts(CountFile &fileobject);


} // namespace file



#endif
