#ifndef __COMMON_H__
#define __COMMON_H__


#include <fstream>
#include <string>
#include <map>
#include <vector>


#include <sys/stat.h>


#include "config.h"


#define MAX_FASTA_DESC_LEN 65536


namespace common {


typedef unsigned long long bint;
// TODO: this still common?
typedef std::map<common::bint,std::vector<unsigned int>> countvecmap;

// Filesystem
bool is_file(std::string &filepath);
bool is_directory(std::string &filepath);
bool is_writeable(std::string &filepath);

// Input
int get_line_tokens(std::ifstream &fh, std::vector<std::string> &line_tokens);


} // namespace common


#endif
