#include "database.h"


namespace db {


Database read_database(std::string &filepath) {
    fprintf(stdout, "Reading in database...\n");
    Database database;

    std::ifstream database_fh(filepath);
    std::vector<std::string> line_tokens;

    // Header; count species for probability vector size
    unsigned int species_count = 0;
    while (!common::get_line_tokens(database_fh, line_tokens)) {
        // Remove leading '#'
        line_tokens[0].erase(0, 1);
        database.species_names.push_back(line_tokens[0]);

        // Peek next character
        if (database_fh.peek() != '#') {
            break;
        } else {
            ++species_count;
        }
    }

    // Data
    line_tokens.reserve(species_count);
    while (!common::get_line_tokens(database_fh, line_tokens)) {
        long bincode = std::stoull(line_tokens[0]);

        // TODO: check performance
        for (unsigned int i = 1; i < line_tokens.size(); ++i) {
            database.probability_map[bincode].push_back(std::stof(line_tokens[i]));
        }
    }
    database_fh.close();
    return database;
}


} // namesspace db
