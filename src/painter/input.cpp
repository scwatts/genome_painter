#include "input.h"


namespace input {


HeaderInfo read_header(std::string &input_fp) {
    HeaderInfo header_info;
    FILE *input_fh = fopen(input_fp.c_str(), "rb");

    // Check for magic bits
    char magic_bits[sizeof(DB_MAGIC)];
    fread(magic_bits, sizeof(char), sizeof(DB_MAGIC), input_fh);
    if (strcmp(magic_bits, DB_MAGIC) != 0) {
        fprintf(stderr, "This does not look like a kmer database\n");
        exit(1);
    }

    // Size of header and number of species
    fread(&header_info.header_size, sizeof(unsigned int), 1, input_fh);
    fread(&header_info.species_num, sizeof(unsigned int), 1, input_fh);

    // Header
    for (unsigned int i = 0; i < header_info.species_num; i++) {
        file::SpeciesCount sp;
        char species_name[SPECIES_NAME_FIELD_SIZE];
        fread(species_name, sizeof(char), SPECIES_NAME_FIELD_SIZE, input_fh);

        sp.name = std::string(species_name);
        fread(&sp.count, sizeof(unsigned int), 1, input_fh);
        header_info.species_counts.push_back(sp);
    }

    // Set data offset
    header_info.offset = ftell(input_fh);


    // TODO: place into record read function
    while (true) {
        std::vector<float> probabilities;
        probabilities.reserve(header_info.species_num);
        common::ullong bincode;
        fread(&bincode, sizeof(common::ullong), 1, input_fh);

        for (unsigned int i = 0; i < probabilities.capacity(); i++) {
            float probability = 0;
            fread(&probability, sizeof(float), 1, input_fh);
            probabilities.push_back(probability);
        }

        // TODO: break at eof
    }


    return header_info;
}


} // namespace input
