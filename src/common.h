#ifndef __COMMON_H__
#define __COMMON_H__


#include <fstream>
#include <string>
#include <vector>


#include <sys/stat.h>


namespace common {


// Filesystem
bool is_file(std::string &filepath);
bool is_directory(std::string &filepath);

// Input
int get_line_tokens(std::ifstream &fh, std::vector<std::string> &line_tokens);

// Output
std::string construct_output_fp(std::string &genome_fp, std::string &suffix, std::string output_dir);


} // namespace common


#endif
