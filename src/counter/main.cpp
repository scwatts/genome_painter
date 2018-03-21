#include "omp.h"


#include "command_line_options.h"
#include "count.h"
#include "output.h"
#include "lib/genome.h"


int main(int argc, char *argv[]) {
    // Get command line arguments
    cmdline::Options options = cmdline::get_arguments(argc, argv);

    // Set number of OpenMP threads
    omp_set_num_threads(options.threads);

    // Count the frequency of all kmers across genomes
    // TODO: implement a mapreduce pattern here
    count::countmap kmer_counts;
    #pragma omp parallel for schedule(static, 1)
    for (size_t i = 0; i < options.genome_fps.size(); i++) {
        std::vector<genome::FastaRecord> fastas = genome::read_fasta_records(options.genome_fps[i]);

        // Collect kmers in the genome
        std::set<common::ullong> sample_kmers;
        for (auto& fasta : fastas) {
            if (fasta.sequence.size() < KMER_SIZE) {
                continue;
            }
            count::collect_kmers(fasta.sequence, sample_kmers);
        }

        // Increment counts for those kmers found
        #pragma omp critical
        {
        for (auto& kmer : sample_kmers) {
            if (kmer_counts.find(kmer) == kmer_counts.end()) {
                kmer_counts[kmer] = 1;
                continue;
            }
            kmer_counts.at(kmer) += 1;
        }
        } // critical pragma
    }

    // Write out counts
    output::write_kmer_counts(kmer_counts, options.genome_fps.size(), options.output_fp);

    return 0;
}
