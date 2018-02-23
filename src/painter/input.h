#ifndef __INPUT_H__
#define __INPUT_H__


#include <cstring>
#include <string>
#include <vector>


#include "merger/file.h"
#include "lib/database.h"


namespace input {


struct HeaderInfo {
    unsigned int header_size;
    unsigned int species_num;
    std::vector<file::SpeciesCount> species_counts;
    long int offset;
};

HeaderInfo read_header(std::string &input_fp);


} // namespace input

#endif
