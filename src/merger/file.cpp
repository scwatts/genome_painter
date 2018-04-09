#include "file.h"


namespace file {


std::vector<CountFile> init_count_file_objects(std::vector<std::string> &filepaths) {
    std::vector<CountFile> fileobjects;

    // Get filesize and place on return vector
    for (auto& fp : filepaths) {
        struct stat sb;
        stat(fp.c_str(), &sb);
        fileobjects.push_back( { fp, sb.st_size, std::ifstream(fp) } );
    }

    // Sort and return
    std::sort(fileobjects.begin(), fileobjects.end(), fileobject_filesize_comp);
    return fileobjects;
}


bool fileobject_filesize_comp(CountFile &a, CountFile &b) {
    return a.filesize > b.filesize;
}


bool is_sorted(CountFile &fileobject) {
    // Seek to start
    if (fileobject.filehandle.tellg() != 0) {
        fileobject.filehandle.seekg(0);
    }

    // Skip header
    std::vector<std::string> line_tokens;
    while (!common::get_line_tokens(fileobject.filehandle, line_tokens)) {
        // Peek next character
        if (fileobject.filehandle.peek() != '#') {
            break;
        }
    }

    // Check that the next 1e5 lines are ordered
    size_t line_number = -1;
    common::bint prev_bincode = 0;
    while (common::get_line_tokens(fileobject.filehandle, line_tokens) == 0) {
        if (line_number++ > 1e5) {
            fileobject.filehandle.seekg(0);
            return true;
        }

        common::bint bincode = std::stoull(line_tokens[0]);
        if (prev_bincode >= bincode) {
            return false;
        }
    }
    // No control zone
    return false;
}


SpeciesCount get_species_counts(CountFile &fileobject) {
    // Species count from file header
    std::vector<std::string> line_tokens;
    common::get_line_tokens(fileobject.filehandle, line_tokens);

    // Remove leading '#' from first token
    std::string line_token = line_tokens[0];
    line_token.erase(0, 1);

    // Species name from filepath
    std::string fn = fileobject.filepath.substr(fileobject.filepath.find_last_of('/') + 1);
    fn = fn.substr(0, fn.find(".txt"));
    fn = fn.substr(0, fn.find(".tsv"));

    return SpeciesCount { fn, static_cast<unsigned int>(std::stoi(line_token)) };
}


} // namespace file
