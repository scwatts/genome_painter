#include "catch.hpp"


#include "../src/lib/kmer.h"


TEST_CASE( "kmer_encoding: Complement of ATGC is TACG (pass)") {
    char complement_nucleotide;

    kmer::complement('A', complement_nucleotide);
    REQUIRE(complement_nucleotide =='T');

    kmer::complement('T', complement_nucleotide);
    REQUIRE(complement_nucleotide =='A');

    kmer::complement('G', complement_nucleotide);
    REQUIRE(complement_nucleotide =='C');

    kmer::complement('C', complement_nucleotide);
    REQUIRE(complement_nucleotide =='G');
}


TEST_CASE( "kmer_encoding: Integer representation of ATGC is 0, 1, 2, 3 (pass)") {
    common::bint nucleotide_bincode;

    kmer::encode_nucleotide('A', nucleotide_bincode);
    REQUIRE(nucleotide_bincode == 0);

    kmer::encode_nucleotide('T', nucleotide_bincode);
    REQUIRE(nucleotide_bincode == 1);

    kmer::encode_nucleotide('G', nucleotide_bincode);
    REQUIRE(nucleotide_bincode == 2);

    kmer::encode_nucleotide('C', nucleotide_bincode);
    REQUIRE(nucleotide_bincode == 3);
}


TEST_CASE( "kmer_encoding: ATGCATGCATGCATGCATGCATGCATGCATGC encoded is 1953184666628070171 (pass)") {
    std::string sequence("ATGCATGCATGCATGCATGCATGCATGCATGC");
    common::bint kmer_bincode;
    kmer::encode_kmer(sequence, 0, kmer_bincode);
    REQUIRE(kmer_bincode == 1953184666628070171);
}


TEST_CASE( "kmer_encoding: AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA encoded is 0 (pass)") {
    std::string sequence("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
    common::bint kmer_bincode;
    kmer::encode_kmer(sequence, 0, kmer_bincode);
    REQUIRE(kmer_bincode == 0);
}


TEST_CASE( "kmer_encoding: AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT encoded is 1 (pass)") {
    std::string sequence("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT");
    common::bint kmer_bincode;
    kmer::encode_kmer(sequence, 0, kmer_bincode);
    REQUIRE(kmer_bincode == 1);
}


TEST_CASE( "kmer_encoding: GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG encoded is 18446744073709551615 (pass)") {
    std::string sequence("GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG");
    common::bint kmer_bincode;
    kmer::encode_kmer(sequence, 0, kmer_bincode);
    REQUIRE(kmer_bincode == 18446744073709551615);
}


TEST_CASE( "kmer_encoding: GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGT encoded is 4611686018427387903 (pass)") {
    std::string sequence("GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGT");
    common::bint kmer_bincode;
    kmer::encode_kmer(sequence, 0, kmer_bincode);
    REQUIRE(kmer_bincode == 4611686018427387903);
}
