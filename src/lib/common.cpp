#include "common.h"


namespace common {


// Filesystem
bool is_file(std::string &filepath) {
    struct stat sb;
    if (stat(filepath.c_str(), &sb) == 0) {
        return (sb.st_mode & S_IFMT) == S_IFREG;
    } else {
        return false;
    }
}


bool is_directory(std::string &filepath) {
    struct stat sb;
    if (stat(filepath.c_str(), &sb) == 0) {
        return (sb.st_mode & S_IFMT) == S_IFDIR;
    } else {
        return false;
    }
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


} // namespace common
