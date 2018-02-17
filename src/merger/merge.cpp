#include "merge.h"


namespace merge {


const unsigned int MAX_DB_SIZE = 2e6;
const unsigned int MAX_LINES = 1e7;


void add_partial_reads(std::vector<file::CountFile> &fileobjects, size_t species_count, Bincodes &bincodes, common::countvecmap &kmer_db) {
    // Limit memory usage
    unsigned int lines_per_species = MAX_LINES / fileobjects.size();
    bool sync_bincodes = kmer_db.size() > MAX_DB_SIZE;

    // Reset bincount.min
    bincodes.min = std::numeric_limits<common::ullong>::max();

    for (size_t i = 0; i < fileobjects.size(); ++i) {
        size_t lines_read = 0;
        common::ullong bincode = 0;
        std::vector<std::string> line_tokens;
        while (!common::get_line_tokens(fileobjects[i].filehandle, line_tokens)) {
            ++lines_read;
            bincode = std::stoull(line_tokens[0]);
            unsigned int count = std::stoi(line_tokens[1]);

            if (kmer_db.find(bincode) == kmer_db.end()) {
                kmer_db[bincode] = std::vector<unsigned int> (species_count, 0);
            }
            kmer_db.at(bincode)[i] = count;

            // Read up to the previously max bincode and only break if we've read sufficient lines
            // or if we're trying to sync bincodes in the database
            if ( (bincode >= bincodes.max) && (sync_bincodes || lines_read >= lines_per_species) ) {
                bincodes.max = bincode;
                break;
            }
        }
        fprintf(stdout, "\t%s: %lu lines\n", fileobjects[i].filepath.c_str(), lines_read);

        // Record the minimum bincode among files (only if we have read in lines)
        if (bincode < bincodes.min && lines_read) {
            bincodes.min = bincode;
        }
    }
}


bool passes_threshold(std::vector<float> probabilities, float threshold) {
    // TODO: this there a more efficient way to do this?
    float fh = 0;
    float sh = 0;

    // Fill values for save cycles
    if (probabilities[0] > probabilities[1]) {
        fh = probabilities[0];
        sh = probabilities[1];
    } else {
        fh = probabilities[1];
        sh = probabilities[0];
    }
    for (unsigned int i = 2; i < probabilities.size(); ++i) {
        if (probabilities[i] > sh) {
            if (probabilities[i] > fh) {
                sh = fh;
                fh = probabilities[i];
            } else {
                sh = probabilities[i];
            }
        }
    }
    return (fh - sh) > threshold;
}


} // namespace merge
