# GenomePainter
Paint genomes with taxonomic probabilities using *k*-mers from curated datasets

![Example plot](http://image.ibb.co/gTrxpx/example_plot.png)

[![Build Status](https://travis-ci.org/scwatts/genome_painter.svg?branch=master)](https://travis-ci.org/scwatts/genome_painter) [![License GPL v3](https://img.shields.io/badge/license-GPL%20v3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0.en.html)


## Table of contents
* [Installation](#installation)
* [Usage](#usage)
* [License](#license)


## Installation
### Quick
Statically linked binaries for generic amd64 linux and ancillary Python scripts can be found under the GitHub releases page. `GenomePainter` can be run directly from these files (after downloading and extraction) without any required dependencies.


### Slow (compiling from source)
If you're feeling brave, you can attempt to compile the source. Autotools has been used for the build system so this process should be somewhat robust. First ensure you have the appropriate tools installed:
```text
C++11 (gcc-4.9.0+, clang-4.9.0+, etc)
OpenMP 4.0+
GNU getopt
GNU automake
GNU autoconf
GNU autoconf-archive
```

Once these dependencies have been provisioned, compiling can be done by issuing:
```bash
./autogen.sh
./configure --prefix=$MY_INSTALL_PREFIX --program-prefix=genomepainter_
make install -j4
```

It's also a good idea to run the included tests:
```bash
make check -j4
```


## Usage
### Data selection
The taxonomic groupings must be defined prior to using this tool. This typically involves acquisition of publicly available assemblies which are then curated. See [fetching genomes](https://github.com/scwatts/fetch_genomes) for a bare bones method of quickly collecting **all** members (specified rank and below) of a taxonomic group. Curation is done manually by examining high-level, low-resolution phylogenetic relationships using mashtree.

### *k*-mer counting
For each taxonomic group, the frequency of *k*-mers observed between assemblies is calculated. Currently only *32*-mers can be used. Here the input data is a set of FASTA files representing genomes assemblies of a single taxa and the output is a single count file for that rank. So for each rank you must run:
```bash
./genomepainter_count_kmers --genome_fps [taxa_A_fasta_1, taxa_A_fasta_2, ...] --output_fp counts/taxa_A.tsv
```
This is straightforward to parallelise if the FASTA files are separated in some way (e.g. on a filesystem or via a predefined list):
```bash
parallel './genomepainter_count_kmers --genome_fps {}/*fasta --output_fp counts/{/}.tsv' ::: assemblies/*
```
Optionally `genomepainter_count_kmers` allows you to utilise additional threads by specifying `--threads int` , but this is only beneficial with thousands of genomes within a single rank.

### Database generation
After *k*-mer counts have been done for all taxonomic groups, the final database can be generated:
```bash
./genomepainter_generate_database --count_fps counts/*tsv --output_fp database/my_database.bin
```
During database generation, the probability that a *k*-mer occurs in a species given a specific *k*-mer sequence is calculated. Extreme differences in sample sizes can cause apparent skewing of these probabilities. To mitigate these effects, you can specify a smooth factor `α` ([see here for details](https://en.wikipedia.org/wiki/Additive_smoothing)) using `--alpha`.
There are naturally many *k*-mers which cannot be used discriminate between taxa. Typically these *k*-mers are unwanted and can be filtered by adjusting the `--threshold` parameter. The value of this parameter relates to the absolute difference between the highest and second highest taxa probabilities for a given *k*-mer. If the difference is less than the `--threshold` value for these two probabilities, then the *k*-mer is excluded from the database.


### Genome painting
To perform genome painting, all that is required is a database and one or more input FASTA assemblies:
```bash
./genomepainter_paint_genome --genome_fp [genome_1, genome_2, ...] --kmer_db_fp database/my_database.bin --output_dir output
```
During painting the *k*-mer with the highest probability for each position is selected. Painting a genome produces a single tab-delimited file for each input genome. The output file contains a set of all taxa probabilities for each position which has a hit in the database. See below for visualising the probabilities.
If there are many genomes to paint, it is strongly recommended to use threads to reduce runtime. This is done by specifying `--threads`.


### Visualising results
GenomePainter is distributed with a Python3 script to generate an interactive plots of results. This script requires the `plot.ly` Python library to be installed prior to use:
```bash
pip3 install --user plotly
```

The script accepts both uncompressed and gzipped painted genomes. To generate the interactive you can run:
```bash
./plot_painting.py --genome_painter_fp output/genome_1_painted.tsv.gz --output_fp plots/genome_1_painted.html
```

Output plots can be viewed using any modern web browers. An example plot can be found at <https://paint.stephen.ac>.


## License
[GNU General Public License v3.0](https://www.gnu.org/licenses/gpl-3.0.en.html)
