#include "output.h"




#include "zlib.h"


namespace output {


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


void write_painted_genome(std::vector<paint::FastaPaint> &fasta_painting, db::HeaderInfo db_header, std::vector<genome::FastaRecord> &fastas, std::string &output_fp) {
    gzFile output_fh = gzopen(output_fp.c_str(), "wb");
    char *buffer = (char *)malloc(CHUNK_SIZE);
    int buffer_size = 0;
    size_t line_size = (2 + db_header.species_counts.size()) +                  /* separators */
                       MAX_FASTA_DESC_LEN +                                     /* contig name */
                       sizeof(size_t) +                                         /* position */
                       (PROB_FIELD_SIZE * db_header.species_counts.size());     /* probabilities */

    // TODO: this is repeatitive, can we simplify?
    // Header. Group count and names
    buffer_size += snprintf(buffer+buffer_size, CHUNK_SIZE-buffer_size, "#%d\n", db_header.species_num);
    for (const auto& species_count : db_header.species_counts) {
        // Using very conservative line size here but must ensure we don't discard bytes
        if ( (buffer_size + line_size) > CHUNK_SIZE) {
            gzwrite(output_fh, buffer, buffer_size);
            buffer_size = 0;
        }
        buffer_size += snprintf(buffer+buffer_size, CHUNK_SIZE-buffer_size, "#%s\n", species_count.name.c_str());
    }
    // FASTA record count and lengths
    buffer_size += snprintf(buffer+buffer_size, CHUNK_SIZE-buffer_size, "#%zd\n", fastas.size());
    for (const auto& fasta : fastas) {
        // Using very conservative line size here but must ensure we don't discard bytes
        if ( (buffer_size + line_size) > CHUNK_SIZE) {
            gzwrite(output_fh, buffer, buffer_size);
            buffer_size = 0;
        }
        buffer_size += snprintf(buffer+buffer_size, CHUNK_SIZE-buffer_size, "#%s\t%zd\n", fasta.name.c_str(), fasta.sequence.size());
    }

    // Data
    for (const auto& fasta_paint : fasta_painting) {
        size_t position = 0;
        for (const auto& paint_bucket : fasta_paint.paint) {
            position++;
            // Write only if we have probabilities
            if (!paint_bucket.probabilities.empty()) {
                // Empty buffer if it's full
                if ( (buffer_size + line_size) > CHUNK_SIZE) {
                    gzwrite(output_fh, buffer, buffer_size);
                    buffer_size = 0;
                }

                // Add to buffer
                buffer_size += snprintf(buffer+buffer_size, 5000, "%s\t%zu", fasta_paint.name.c_str(), position);
                for (auto probability : paint_bucket.probabilities) {
                    buffer_size += snprintf(buffer+buffer_size, CHUNK_SIZE-buffer_size, "\t%f", probability);
                }
                buffer[buffer_size++] = '\n';
            }
        }
        // Flush buffer
        gzwrite(output_fh, buffer, buffer_size);
        buffer_size = 0;
    }

    // Deallocate resources
    free(buffer);
    gzclose(output_fh);
}


} // namespace output
