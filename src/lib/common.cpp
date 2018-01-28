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


} // namespace common
