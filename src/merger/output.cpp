#include "output.h"


namespace output {


void write_magic_bits(std::string &output_fp) {
    FILE *output_fh = fopen(output_fp.c_str(), "wb");
    fwrite(DB_MAGIC, sizeof(DB_MAGIC), 1, output_fh);
    fclose(output_fh);
}


long int write_species_counts_header(std::vector<file::SpeciesCount> &species_counts, std::string &output_fp) {
    FILE *output_fh = fopen(output_fp.c_str(), "ab");
    // Size of header
    unsigned int header_size = (SPECIES_NAME_FIELD_SIZE + SPECIES_COUNT_FIELD_SIZE) * species_counts.size();
    fwrite(&header_size, sizeof(unsigned int), 1, output_fh);

    // Number of species
    unsigned int species_count = static_cast<unsigned int>(species_counts.size());
    fwrite(&species_count, sizeof(unsigned int), 1, output_fh);

    // Header data
    for (auto& species_count : species_counts) {
        species_count.name.resize(SPECIES_NAME_FIELD_SIZE);
        fwrite(species_count.name.c_str(), SPECIES_NAME_FIELD_SIZE, 1, output_fh);
        fwrite(&species_count.count, SPECIES_COUNT_FIELD_SIZE, 1, output_fh);
    }
    long int position = ftell(output_fh);
    fclose(output_fh);
    return position;
}


void write_completed_counts(KmerData &data, Parameters &parameters, Index &indices) {
    // Process current data; we should now have all data up to min_bincode
    // We must account for kmers which have not yet been read in for all species
    FILE *output_fh = fopen(parameters.output_fp->c_str(), "ab");
    auto it = data.kmer_db.begin();
    while (it != data.kmer_db.end()) {
        if (it->first > data.bincodes.min) {
            ++it;
            continue;
        }

        // Calculate probabilty and exclude on threshold
        std::vector<float> probabilities = prob::calculate_probabilities(it->second, *data.species_counts, parameters.alpha);

        if (merge::passes_threshold(probabilities, parameters.threshold)) {
            // Write probabilities
            fwrite(&it->first, BINCODE_FIELD_SIZE, 1, output_fh);
            for (auto & p : probabilities) {
                fwrite(&p, PROB_FIELD_SIZE, 1, output_fh);
            }

            // Calculate index if at checkpoint
            if ((indices.i % INDEX_DIV) == 0) {
                indices.i++;
                IndexEntry index_entry = { it->first, indices.last_position };
                indices.entries.push_back(index_entry);
                indices.last_position += indices.record_size;
            }
        }

        // Remove from map once done
        it = data.kmer_db.erase(it);

    }
    fclose(output_fh);
}


void write_indices(Index &indices, std::string output_fp) {
    output_fp.append(".idx");
    FILE *output_fh = fopen(output_fp.c_str(), "ab");
    for (auto& index : indices.entries) {
        fwrite(&index.bincode, sizeof(common::ullong), 1, output_fh);
        fwrite(&index.position, sizeof(long int), 1, output_fh);
    }
    fclose(output_fh);
}

} // namespace output
