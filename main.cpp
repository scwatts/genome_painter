#include "command_line_options.h"
#include "database.h"
#include "genome.h"
#include "paint.h"


// TODO: think about moving header
struct FastaPaint { std::string name; std::vector<paint::PaintBucket> paint; };


int main(int argc, char *argv[]) {
    // Get command line arguments
    cmdline::Options options = cmdline::get_arguments(argc, argv);

    // Get in FASTA records
    std::vector<genome::FastaRecord> fastas = genome::read_fasta_records(options.genome_fp);

    // Read in database
    db::Database database = db::read_database(options.kmer_db_fp);

    // Paint genome
    std::vector<FastaPaint> fasta_painting;
    for (auto& fasta : fastas) {
        fasta_painting.push_back(FastaPaint { fasta.name, paint::paint_sequence(fasta, database) });
    }

    return 0;
}
