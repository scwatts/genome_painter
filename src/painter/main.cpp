#include "sqlite3.h"


#include "command_line_options.h"
#include "output.h"
#include "paint.h"
#include "lib/common.h"
#include "lib/genome.h"
#include "lib/database.h"


int main(int argc, char *argv[]) {
    // Get command line arguments
    cmdline::Options options = cmdline::get_arguments(argc, argv);

    // Open database connection
    sqlite3 *dbp = db::open_and_configure(options.kmer_db_fp.c_str());

    // Get species names
    char *errmsg;
    std::vector<std::string> species_names;
    if (sqlite3_exec(dbp, sql::species_select.c_str(), db::vector_fill_species, &species_names, &errmsg) != SQLITE_OK) {
        fprintf(stderr, "Error retrieving from database: %s\n", sqlite3_errmsg(dbp));
        sqlite3_close(dbp);
        exit(1);
    }

    // Paint genomes
    fprintf(stdout, "Painting genomes...\n");
    for (size_t i = 0; i < options.genome_fps.size(); ++i) {
        // Get in FASTA records
        fprintf(stdout, "Loading %s\n", options.genome_fps[i].c_str());
        std::vector<genome::FastaRecord> fastas = genome::read_fasta_records(options.genome_fps[i]);

        std::vector<paint::FastaPaint> fasta_painting;
        fprintf(stdout, "Painting %s\n", options.genome_fps[i].c_str());
        for (auto& fasta : fastas) {
            // Skip sequences less than size of kmer
            if (fasta.sequence.size() < 32) {
                continue;
            }

            fasta_painting.push_back(paint::FastaPaint { fasta.name, paint::paint_sequence(fasta, dbp) });
        }

        // Write painted genome
        fprintf(stdout, "Writing results %s\n", options.genome_fps[i].c_str());
        std::string output_suffix = "_painted.tsv";
        std::string output_fp = output::construct_output_fp(options.genome_fps[i], output_suffix, options.output_dir);
        output::write_painted_genome(fasta_painting, species_names, output_fp);
    }

    sqlite3_close(dbp);
    return 0;
}
