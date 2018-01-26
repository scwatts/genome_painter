#include "command_line_options.h"
#include "count.h"
#include "output.h"
#include "../genome.h"


int main(int argc, char *argv[]) {
    // Get command line arguments
    cmdline::Options options = cmdline::get_arguments(argc, argv);

    // Count the frequency of all kmers across genomes
    count::countmap kmer_counts;
    for (auto& genome_fp : options.genome_fps) {
        std::vector<genome::FastaRecord> fastas = genome::read_fasta_records(genome_fp);

        // Collect kmers in the genome
        std::set<common::ullong> sample_kmers;
        for (auto& fasta : fastas) {
            count::collect_kmers(fasta.sequence, sample_kmers);
        }

        // Increment counts for those kmers found
        for (auto& kmer : sample_kmers) {
            if (kmer_counts.find(kmer) == kmer_counts.end()) {
                kmer_counts[kmer] = 1;
                continue;
            }
            kmer_counts.at(kmer) += 1;
        }
    }

    // Write out counts
    output::write_kmer_counts(kmer_counts, options.genome_fps.size(), options.output_fp);

    return 0;
}
