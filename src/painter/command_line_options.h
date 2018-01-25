#ifndef __COMMAND_LINE_OPTIONS_H_
#define __COMMAND_LINE_OPTIONS_H_


#include <cstdio>
#include <fstream>
#include <string>
#include <vector>


#include <getopt.h>


#include "../common.h"
#include "../version.h"


namespace cmdline {


struct Options {
    std::vector<std::string> genome_fps;
    std::string kmer_db_fp;
    std::string output_dir;
};

void print_help(FILE *stdst);
void print_version(FILE *stdst);
Options get_arguments(int argc, char **argv);


} // namespace cmdline


#endif
