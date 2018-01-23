#ifndef __COMMAND_LINE_OPTIONS_H_
#define __COMMAND_LINE_OPTIONS_H_


#include <cstdio>
#include <fstream>
#include <string>


#include <getopt.h>


namespace cmdline {


struct Options {
    std::string genome_fp;
    std::string kmer_db_fp;
    std::string output_fp;
};

void print_help();
void print_version();
Options get_arguments(int argc, char **argv);
bool filepath_exists(std::string &filepath);


} // namespace cmdline


#endif
