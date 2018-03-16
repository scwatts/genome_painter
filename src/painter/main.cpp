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
            // Skip sequences less than size of kmer
            if (fasta.sequence.size() < 32) {
                continue;
            }

            fasta_painting.push_back(paint::FastaPaint { fasta.name, paint::paint_sequence(fasta, database) });
        }

        // Write painted genome
        #pragma omp critical
        fprintf(stdout, "Writing results for %s\n", options.genome_fps[i].c_str());
        std::string output_suffix = "_painted.tsv";
        std::string output_fp = output::construct_output_fp(options.genome_fps[i], output_suffix, options.output_dir);
        output::write_painted_genome(fasta_painting, database.header.species_counts, output_fp);
    }

    return 0;
}
