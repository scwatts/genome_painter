#include "lib/database.h"


namespace db {


void read_header(Database &database) {
    FILE *db_fh= fopen(database.fp.c_str(), "rb");

    // Check for magic bits
    char magic_bits[sizeof(DB_MAGIC)];
    fread(magic_bits, sizeof(char), sizeof(DB_MAGIC), db_fh);
    if (strcmp(magic_bits, DB_MAGIC) != 0) {
        fprintf(stderr, "This does not look like a kmer database\n");
        exit(1);
    }

    // Size of header and number of species
    fread(&database.header.size, sizeof(unsigned int), 1, db_fh);
    fread(&database.header.species_num, sizeof(unsigned int), 1, db_fh);

    // Header
    for (unsigned int i = 0; i < database.header.species_num; i++) {
        file::SpeciesCount sp;
        char species_name[SPECIES_NAME_FIELD_SIZE];
        fread(species_name, sizeof(char), SPECIES_NAME_FIELD_SIZE, db_fh);

        sp.name = std::string(species_name);
        fread(&sp.count, sizeof(unsigned int), 1, db_fh);
        database.header.species_counts.push_back(sp);
    }

    // Set data offset
    database.header.offset = ftell(db_fh);
    fclose(db_fh);
}


void read_database(Database &database) {
    // Open file and seek to main data block
    FILE *db_fh= fopen(database.fp.c_str(), "rb");
    fseek(db_fh, database.header.offset, SEEK_SET);

    // Read in data
    while(!feof(db_fh)) {
        common::ullong bincode;
        fread(&bincode, sizeof(common::ullong), 1, db_fh);

        std::vector<float> probabilities;
        probabilities.reserve(database.header.species_num);
        for (unsigned int i = 0; i < probabilities.capacity(); i++) {
            float probability = 0;
            fread(&probability, sizeof(float), 1, db_fh);
            probabilities.push_back(probability);
        }
        database.probabilities[bincode] = std::move(probabilities);
    }
    fclose(db_fh);
}


} // namespace db
