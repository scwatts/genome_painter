#include "command_line_options.h"
#include "database.h"
#include "genome.h"
#include "output.h"
#include "paint.h"


int main(int argc, char *argv[]) {
    // Get command line arguments
    cmdline::Options options = cmdline::get_arguments(argc, argv);

    // Get in FASTA records
    std::vector<genome::FastaRecord> fastas = genome::read_fasta_records(options.genome_fp);

    // Read in database
    db::Database database = db::read_database(options.kmer_db_fp);

    // Paint genome
    std::vector<paint::FastaPaint> fasta_painting;
    for (auto& fasta : fastas) {
        fasta_painting.push_back(paint::FastaPaint { fasta.name, paint::paint_sequence(fasta, database) });
    }

    // Write painted genome
    output::write_painted_genome(fasta_painting, options.output_fp);

    return 0;
}
