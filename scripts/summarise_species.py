#!/usr/bin/env python3
"""
This script summarises the species composition of Genome Painter results.

It outputs a tab-delimited table with these columns:
  1) Assembly name
  2) Unclassified %: the percentage of the assembly which was not used for classification
  3) Species 1 name: the best matching species
  4) Species 1 %:    the percentage of the assembly (only counting the positions used for
                     classification) supporting the best matching species
  5) Species 2 name: the second-best matching species
  6) Species 2 %:    the percentage of the assembly (only counting the positions used for
                     classification) supporting the second-best matching species
  etc.

If the second column (Unclassified %) has a particular large value (e.g. more than 50%), that
suggests the query assembly species is not represented in the database. In this case, you probably
shouldn't take the results too seriously.

If the second column (Unclassified %) has a small value but the sixth column (Species 2 %) has a
not-too-small value (e.g. 10% or more), then you may be looking at a contaminated assembly or
cross-species hybrid. Visualising the results would be advised to investigate further.

Example usage:
    summarise_species.py output_dir
    summarise_species.py single_output_file.tsv
"""

import argparse
import gzip
import os
import pathlib
import sys
from multiprocessing import Pool


def get_arguments():
    parser = argparse.ArgumentParser(description='Summarise species from genome painter results',
                                     formatter_class=argparse.ArgumentDefaultsHelpFormatter)

    parser.add_argument('input', type=str,
                        help='A tsv file from paint_genome or a directory of tsv files from '
                             'paint_genome (tsv files can be gzipped)')
    parser.add_argument('--threshold', type=float, required=False, default=0.9,
                        help='Minimum probability threshold - positions with a max probability '
                             'lower than this will be ignored')
    parser.add_argument('--top_num', type=int, required=False, default=4,
                        help='This many of the top species matches will be displayed')
    parser.add_argument('--threads', type=int, required=False, default=8,
                        help='Number of threads for multiprocessing')

    args = parser.parse_args()
    return args


def main():
    args = get_arguments()
    result_files = get_result_files(args.input)
    print_output_header(args.top_num)
    pool = Pool(args.threads)
    for output in pool.imap(summarise_species,
                            ((r, args.threshold, args.top_num) for r in result_files)):
        print(output)


def get_result_files(input_dir_or_file):
    p = pathlib.Path(input_dir_or_file)
    if p.is_file():
        return [str(p)]
    elif p.is_dir():
        tsv_files = [str(x) for x in p.glob('*_painted.tsv')]
        tsv_gz_files = [str(x) for x in p.glob('*_painted.tsv.gz')]
        return sorted(tsv_files + tsv_gz_files)
    else:
        sys.exit('Error: {} is neither a file nor a directory'.format(input_dir_or_file))


def print_output_header(top_num):
    header = ['Assembly name', 'Unclassified %']
    for i in range(top_num):
        header.append('Species {} name'.format(i+1))
        header.append('Species {} %'.format(i+1))
    print('\t'.join(header))


def summarise_species(args):
    result_file, threshold, top_num = args
    assembly_name = os.path.basename(result_file)
    assembly_name = assembly_name.replace('_painted.tsv.gz', '').replace('_painted.tsv', '')
    species_names = get_species_names(result_file)
    species_tallies = {x: 0 for x in species_names}
    contig_lengths = get_contig_lengths(result_file)

    open_func = get_open_function(result_file)
    with open_func(result_file, 'rt') as result:
        for line in result:
            if line.startswith('#'):
                continue
            parts = line.strip().split('\t')
            contig = parts[0]
            position = int(parts[1])
            probabilities = [float(x) for x in parts[2:]]
            assert len(probabilities) == len(species_names)
            max_prob = max(probabilities)
            if max_prob >= threshold:
                best_species = species_names[probabilities.index(max_prob)]
                species_tallies[best_species] += 1

    total = sum(species_tallies.values())
    if total == 0:
        return '\t'.join([assembly_name, 'Error: no assembly positions meet threshold'])
    percentages = [(x, 100.0 * species_tallies[x] / total) for x in species_names]
    percentages = sorted(percentages, reverse=True, key=lambda x: x[1])

    assembly_size = sum(contig_lengths.values())
    uncalled = assembly_size - total
    uncalled_percent = 100.0 * uncalled / assembly_size

    output = [assembly_name, '%.4f' % uncalled_percent]
    for p in percentages[:top_num]:
        output.append(p[0])
        output.append('%.4f' % p[1])
    return '\t'.join(output)


def get_species_names(result_file):
    species_names = []
    species_count = None
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
                if species_count is None:
                    species_count = int(line)
                else:  # we've hit the contig count
                    break
            else:
                species_names.append(line)
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
