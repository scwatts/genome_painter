#include "omp.h"


#include "command_line_options.h"
#include "output.h"
#include "paint.h"
#include "lib/common.h"
#include "lib/database.h"
#include "lib/genome.h"


int main(int argc, char *argv[]) {
    // Get command line arguments
    cmdline::Options options = cmdline::get_arguments(argc, argv);

    // Set number of OpenMP threads
    omp_set_num_threads(options.threads);

    // Read in database
    fprintf(stdout, "Loading database...\n");
    db::Database database(options.kmer_db_fp);
    db::read_header(database);
    db::read_database(database);

    // Paint genomes
    fprintf(stdout, "Painting genomes...\n");
    #pragma omp parallel for schedule(static, 1)
    for (size_t i = 0; i < options.genome_fps.size(); ++i) {
        // Get in FASTA records
        #pragma omp critical
        fprintf(stdout, "Loading %s\n", options.genome_fps[i].c_str());
        std::vector<genome::FastaRecord> fastas = genome::read_fasta_records(options.genome_fps[i]);

        std::vector<paint::FastaPaint> fasta_painting;
        #pragma omp critical
        fprintf(stdout, "Painting %s\n", options.genome_fps[i].c_str());
        for (auto& fasta : fastas) {
            // Check that the FASTA name is not excessively large. Simplifies zlib buffering
            // TODO: move this before database loading so that errors are raised early
            if (fasta.name.size() > MAX_FASTA_DESC_LEN) {
                fprintf(stderr, "Input file has %s an excessively large ", options.genome_fps[i].c_str());
                fprintf(stderr, "FASTA description (over %d characters), please truncate and try again\n", MAX_FASTA_DESC_LEN);
                // TODO: confirm how exits are handled by openmp
                exit(1);
            }
            // Skip sequences less than size of kmer
            if (fasta.sequence.size() < KMER_SIZE) {
                continue;
            }

            fasta_painting.push_back(paint::FastaPaint { fasta.name, paint::paint_sequence(fasta, database) });
        }

        // Write painted genome
        #pragma omp critical
        fprintf(stdout, "Writing results for %s\n", options.genome_fps[i].c_str());
        std::string output_suffix = "_painted.tsv.gz";
        std::string output_fp = output::construct_output_fp(options.genome_fps[i], output_suffix, options.output_dir);
        output::write_painted_genome(fasta_painting, database.header.species_counts, output_fp);
    }

    return 0;
}
