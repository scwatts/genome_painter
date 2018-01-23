#include "genome.h"
#include "database.h"
#include "kmer.h"
#include "paint.h"


namespace paint {


std::vector<PaintBucket> paint_sequence(genome::FastaRecord &fasta, db::Database &database) {
    std::vector<PaintBucket> paint(fasta.sequence.size(), { {}, 0 });

    // Forward and reverse kmers
    // TODO: use input iterator and place in kmer.cpp
    for (size_t i = 0; i < fasta.sequence.size(); i++) {
        kmer::KmerPairBin kmer_pair = kmer::encode_substring_kmer(fasta.sequence, i);
    }

    return paint;
}


} // namespace paint
