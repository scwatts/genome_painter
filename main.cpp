#include "command_line_options.h"
#include "database.h"
#include "genome.h"
#include "paint.h"


int main(int argc, char *argv[]) {
    // Get command line arguments
    cmdline::Options options = cmdline::get_arguments(argc, argv);

    // Get in FASTA records
    std::vector<genome::FastaRecord> fastas = genome::read_fasta_records(options.genome_fp);

    // Read in database
    db::Database database = db::read_database(options.kmer_db_fp);

    // Paint genome
    for (auto& fasta : fastas) {
        paint::paint_sequence(fasta, database);
    }

    return 0;
}
