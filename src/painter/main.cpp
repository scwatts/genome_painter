#include "command_line_options.h"
#include "output.h"
#include "paint.h"
#include "../common.h"
#include "../database.h"
#include "../genome.h"


int main(int argc, char *argv[]) {
    // Get command line arguments
    cmdline::Options options = cmdline::get_arguments(argc, argv);

    // Read in database
    fprintf(stdout, "Reading in database...\n");
    fflush(stdout);
    db::Database database = db::read_database(options.kmer_db_fp);

    // Paint genomes
    fprintf(stdout, "Painting genomes...\n");
    for (auto& genome_fp : options.genome_fps) {
        // Get in FASTA records
        fprintf(stdout, "Loading %s\n", genome_fp.c_str());
        std::vector<genome::FastaRecord> fastas = genome::read_fasta_records(genome_fp);

        std::vector<paint::FastaPaint> fasta_painting;
        fprintf(stdout, "Painting contigs:\n");
        fflush(stdout);
        for (auto& fasta : fastas) {
            // Skip sequences less than size of kmer
            if (fasta.sequence.size() < 32) {
                continue;
            }

            fprintf(stdout, "\t%s...", fasta.name.c_str());
            fasta_painting.push_back(paint::FastaPaint { fasta.name, paint::paint_sequence(fasta, database) });
            fprintf(stdout, " done\n");
        }

        // Write painted genome
        fprintf(stdout, "Writing results for %s...", genome_fp.c_str());
        std::string output_suffix = "_painted.tsv";
        std::string output_fp = common::construct_output_fp(genome_fp, output_suffix, options.output_dir);
        output::write_painted_genome(fasta_painting, database.species_names, output_fp);
        fprintf(stdout, " done\n");
        fflush(stdout);
    }

    return 0;
}
