#include "genome.h"


// Macro to define file type and read method for kseq.h functions
KSEQ_INIT(gzFile, gzread)


namespace genome {


std::vector<FastaRecord> read_fasta_records(std::string &input_fp) {
    std::vector<FastaRecord> fastas;
    gzFile fp;
    kseq_t *seq;

    // Get file handles
    fp = gzopen(input_fp.c_str(), "r");
    seq = kseq_init(fp);

    while (kseq_read(seq) >= 0) {
        std::string sequence;
        for (size_t i = 0; i < seq->seq.l; i++) {
            sequence.push_back(std::toupper(seq->seq.s[i]));
        }
        fastas.push_back(FastaRecord { seq->name.s, sequence });
    }

    kseq_destroy(seq);
    gzclose(fp);

    return fastas;
}


} // namespace genome
