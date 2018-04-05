#!/usr/bin/env python3
"""
This script summarises the species composition of Genome Painter results, per contig.

It outputs a tab-delimited table with these columns:
  1) Contig name
  2) Contig length
  3) Species 1: the best matching species (across all contigs)
  4) Species 2: the second-best matching species
  etc.

Example usage:
    summarise_contigs.py genome_painter_results.tsv
"""

import argparse
import gzip
import os
import sys


def get_arguments():
    parser = argparse.ArgumentParser(description='Summarise contigs from genome painter results',
                                     formatter_class=argparse.ArgumentDefaultsHelpFormatter)

    parser.add_argument('input', type=str,
                        help='A tsv file from paint_genome (can be gzipped)')
    parser.add_argument('--threshold', type=float, required=False, default=0.9,
                        help='Minimum probability threshold - positions with a max probability '
                             'lower than this will be ignored')
    parser.add_argument('--top_num', type=int, required=False, default=4,
                        help='This many of the top species matches will be displayed')

    args = parser.parse_args()
    return args


def main():
    args = get_arguments()
    summarise_contigs(args.input, args.threshold, args.top_num)


def summarise_contigs(result_file, threshold, top_num):
    species_names = get_species_names(result_file)
    species_tallies = {x: 0 for x in species_names}
    contig_tallies = {}
    contig_names = []
    contig_lengths = get_contig_lengths(result_file)

    open_func = get_open_function(result_file)
    with open_func(result_file, 'rt') as result:
        for line in result:
            if line.startswith('#'):
                continue
            parts = line.strip().split('\t')
            contig = parts[0]
            if contig not in contig_tallies:
                contig_names.append(contig)
                contig_tallies[contig] = {x: 0 for x in species_names}
            position = int(parts[1])
            probabilities = [float(x) for x in parts[2:]]
            assert len(probabilities) == len(species_names)
            max_prob = max(probabilities)
            if max_prob >= threshold:
                best_species = species_names[probabilities.index(max_prob)]
                species_tallies[best_species] += 1
                contig_tallies[contig][best_species] += 1

    total = sum(species_tallies.values())
    if total == 0:
        sys.exit('Error: no assembly positions meet threshold')
    percentages = [(x, 100.0 * species_tallies[x] / total) for x in species_names]
    percentages = sorted(percentages, reverse=True, key=lambda x: x[1])

    top_species = [p[0] for p in percentages[:top_num]]
    print_header(top_species)

    for contig in contig_names:
        contig_total = sum(contig_tallies[contig].values())
        if contig_total == 0:
            continue
        output = [contig, str(contig_lengths[contig])]
        for s in top_species:
            contig_percentage = 100.0 * contig_tallies[contig][s] / contig_total
            output.append('%.4f' % contig_percentage)
        print('\t'.join(output))


def get_species_names(result_file):
    species_names = []
    species_count = None
    open_func = get_open_function(result_file)
    with open_func(result_file, 'rt') as result:
        for line in result:
            if line.startswith('#'):
                line = line.strip()[1:]
                try:
                    _ = int(line)
                    integer_line = True
                except ValueError:
                    integer_line = False
                if integer_line:
                    if species_count is None:
                        species_count = int(line)
                    else:  # we've hit the contig count
                        break
                else:
                    species_names.append(line)
            else:
                break
    assert len(species_names) == species_count
    return species_names


def get_contig_lengths(result_file):
    contig_lengths = {}
    integer_line_count = 0
    contig_count = None
    open_func = get_open_function(result_file)
    with open_func(result_file, 'rt') as result:
        for line in result:
            if not line.startswith('#'):
                break
            line = line.strip()[1:]
            try:
                _ = int(line)
                integer_line = True
            except ValueError:
                integer_line = False
            if integer_line:
                integer_line_count += 1
                if integer_line_count == 2:
                    contig_count = int(line)
            elif integer_line_count == 2:
                contig_name, contig_length = line.split('\t')
                contig_lengths[contig_name] = int(contig_length)
    return contig_lengths


def print_header(top_species):
    header = ['Contig', 'Length'] + top_species
    print('\t'.join(header))


def get_compression_type(filename):
    """
    Attempts to guess the compression (if any) on a file using the first few bytes.
    http://stackoverflow.com/questions/13044562
    """
    magic_dict = {'gz': (b'\x1f', b'\x8b', b'\x08'),
                  'bz2': (b'\x42', b'\x5a', b'\x68'),
                  'zip': (b'\x50', b'\x4b', b'\x03', b'\x04')}
    max_len = max(len(x) for x in magic_dict)
    with open(filename, 'rb') as unknown_file:
        file_start = unknown_file.read(max_len)
    compression_type = 'plain'
    for file_type, magic_bytes in magic_dict.items():
        if file_start.startswith(magic_bytes):
            compression_type = file_type
    if compression_type == 'bz2':
        sys.exit('Error: cannot use bzip2 format - use gzip instead')
    if compression_type == 'zip':
        sys.exit('Error: cannot use zip format - use gzip instead')
    return compression_type


def get_open_function(filename):
    """
    Returns either open or gzip.open, as appropriate for the file.
    """
    if get_compression_type(filename) == 'gz':
        return gzip.open
    else:  # plain text
        return open


if __name__ == '__main__':
    main()
