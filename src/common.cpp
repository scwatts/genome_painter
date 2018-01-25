#include "common.h"


namespace common {


// Filesystem
bool is_file(std::string &filepath) {
    struct stat sb;
    stat(filepath.c_str(), &sb);
    return sb.st_mode & S_IFREG;
}


bool is_directory(std::string &filepath) {
    struct stat sb;
    stat(filepath.c_str(), &sb);
    return sb.st_mode & S_IFDIR;
}


// Input
int get_line_tokens(std::ifstream &fh, std::vector<std::string> &line_tokens) {
    char c;
    std::string token;

    line_tokens.clear();
    while (fh.get(c)) {
        if (c == '\t') {
            line_tokens.push_back(token);
            token.clear();
            continue;
        }
        if (c == '\n') {
            line_tokens.push_back(token);
            return 0;
        }
        token.push_back(c);
    }
    return 1;
}


// Output
std::string construct_output_fp(std::string &genome_fp, std::string &suffix, std::string output_dir) {
    // Placing a trailing slash on output filepath if it doesn't already exist
    if (output_dir.back() != '/') {
        output_dir.push_back('/');
    }

    // Get base filename from path
    std::string fn = genome_fp.substr(genome_fp.find_last_of('/') + 1);

    // Attempt to drop some file extensions
    fn = fn.substr(0, fn.find(".gz"));
    fn = fn.substr(0, fn.find(".fasta"));
    fn = fn.substr(0, fn.find(".fna"));

    // ...giving up now
    fn.append(suffix);
    return output_dir.append(fn);
}


} // namespace common
