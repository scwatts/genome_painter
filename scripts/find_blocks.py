#!/usr/bin/env python3
"""
This script attempts to define which regions (i.e. blocks) of an assembly belong to an alternate
species.

Example usage:
    summarise_species.py genome_painter_output.tsv
"""

import argparse
import collections
import gzip
import numpy as np
import pytest
import sys


def get_arguments():
    parser = argparse.ArgumentParser(description='Summarise species from genome painter results',
                                     formatter_class=argparse.ArgumentDefaultsHelpFormatter)

    parser.add_argument('input', type=str,
                        help='A tsv file from paint_genome (can be gzipped)')

    parser.add_argument('--threshold_1', type=float, default=0.95,
                        help='An alternative species probability must exceed this value to '
                             'start a block')
    parser.add_argument('--threshold_2', type=float, default=0.1,
                        help='Adjacent blocks are merged if the region between them remains '
                             'above this probability')
    parser.add_argument('--bandwidth', type=int, default=1001,
                        help='Smoothing bandwidth window')
    parser.add_argument('--min_block_size', type=int, default=10000,
                        help='Only output blocks of at least this size')

    args = parser.parse_args()
    return args


def main():
    args = get_arguments()

    species_names = get_species_names(args.input)
    num_to_species = {i: name for i, name in enumerate(species_names)}

    labelled_contigs = {}
    contig_lengths = get_contig_lengths(args.input)
    for name, length in contig_lengths.items():
        labelled_contigs[name] = LabelledContig(name, length)

    load_data(args.input, labelled_contigs, species_names)
    for contig in labelled_contigs.values():
        contig.smooth_data(args.bandwidth)

    primary_species = get_primary_species(labelled_contigs)

    print('contig\tstart\tend\tblock_size\tspecies')
    for contig in labelled_contigs.values():
        contig.find_blocks(primary_species, args.threshold_1, args.threshold_2, num_to_species,
                           args.min_block_size)


class LabelledContig(object):

    def __init__(self, name, length):
        self.name = name
        self.length = length
        self.positions = []
        self.probabilities = {}

    def extend_data_to_start_and_end(self):
        """
        This function copies the first/last positions of a contig to the beginning/end of the
        contig, if they are not already there.
        """
        if not self.positions:
            return

        first = self.positions[0]
        first_probabilities = self.probabilities[first]
        if first != 0:
            self.positions.insert(0, 0)
            self.probabilities[0] = first_probabilities

        last = self.positions[-1]
        last_probabilities = self.probabilities[last]
        if last != self.length-1:
            self.positions.append(self.length-1)
            self.probabilities[self.length-1] = last_probabilities

    def smooth_data(self, bandwidth):
        if not self.positions:
            return
        species_count = len(self.probabilities[self.positions[0]])
        smoothed_data = [[] for _ in range(species_count)]
        for i in range(species_count):
            probs = [self.probabilities[p][i] for p in self.positions]
            smoothed_probs = smooth_data(probs, bandwidth)
            assert len(smoothed_probs) == len(self.probabilities)
            for j, p in enumerate(self.positions):
                self.probabilities[p][i] = smoothed_probs[j]

    def get_top_species_per_position(self):
        top_species_per_position = []
        for pos in self.positions:
            probabilities = self.probabilities[pos]
            top_species_per_position.append(probabilities.index(max(probabilities)))
        return top_species_per_position

    def find_blocks(self, primary_species, threshold_1, threshold_2, num_to_species,
                    min_block_size):
        initial_blocks = self.get_initial_blocks(primary_species, threshold_1)
        merged_blocks = self.merge_blocks(initial_blocks, threshold_2)
        for species_num, start_i, end_i in merged_blocks:
            start_pos, end_pos = self.positions[start_i], self.positions[end_i]
            species_name = num_to_species[species_num]
            block_size = end_pos - start_pos + 1
            if block_size >= min_block_size:
                print(f'{self.name}\t{start_pos+1}\t{end_pos+1}\t{block_size}\t{species_name}')

    def get_initial_blocks(self, primary_species, threshold_1):
        """
        This function finds initial blocks which are defined as:
        * regions with a consistent top species
        * where the top species is not the primary species
        * where the probability of the top species exceeds the threshold at some point in the block

        These blocks may be fragmented, because even a single position with a different top species
        will end the block.

        Blocks are returned as a list of 3-tuples containing:
        * the species index
        * the block start (as an index in the positions list)
        * the block end (as an index in the positions list)
        """
        initial_blocks = []
        pos_count = len(self.positions)

        i = 0
        while i < pos_count:
            top_prob, top_species = get_top_prob_and_species(self.probabilities[self.positions[i]])

            if top_species != primary_species and top_prob >= threshold_1:
                block_start, block_end = i, i

                # Extend the block to the left:
                while block_start > 0:
                    _, prev_top_species = \
                        get_top_prob_and_species(self.probabilities[self.positions[block_start-1]])
                    if prev_top_species == top_species:
                        block_start -= 1
                    else:
                        break

                # Extend the block to the right:
                while block_end < len(self.positions)-1:
                    _, next_top_species = \
                        get_top_prob_and_species(self.probabilities[self.positions[block_end+1]])
                    if next_top_species == top_species:
                        block_end += 1
                    else:
                        break

                initial_blocks.append((top_species, block_start, block_end))
                i = block_end

            i += 1

        return initial_blocks


    def merge_blocks(self, initial_blocks, threshold_2):
        """
        This function merges blocks:
        * which have the same species
        * where the region between them consistently stays above the threshold for that species
        """
        merged_blocks = []
        i = 0
        while i < len(initial_blocks):
            species, start, end = initial_blocks[i]
            while i < len(initial_blocks)-1:
                next_species, next_start, next_end = initial_blocks[i+1]
                if species == next_species and self.all_above_threshold(species, end, next_start, threshold_2):
                    end = next_end
                    i += 1
                else:
                    break
            merged_blocks.append((species, start, end))
            i += 1
        return merged_blocks


    def all_above_threshold(self, species, start, end, threshold):
        """
        Checks to see if the probability for a given species stays above the given threshold for
        a range.
        """
        for i in range(start, end+1):
            probabilities = self.probabilities[self.positions[i]]
            if probabilities[species] < threshold:
                return False
        return True


def smooth_data(data, bandwidth):
    data = np.array(data)
    smoothed_data = []
    bandwidth_step = (bandwidth - 1) // 2
    for i, _ in enumerate(data):
        start = max(i - bandwidth_step, 0)
        end = min(i + bandwidth_step + 1, len(data))
        smoothed_data.append(np.mean(data[start:end]))
    return smoothed_data


def get_top_prob_and_species(probabilities):
    top_prob = max(probabilities)
    top_species = probabilities.index(top_prob)
    return top_prob, top_species


def load_data(result_file, labelled_contigs, species_names):
    column_count = len(species_names) + 2
    with get_open_function(result_file)(result_file, 'rt') as result:
        for line in result:
            if line.startswith('#'):
                continue
            parts = line.strip().split('\t')
            if len(parts) != column_count:
                continue
            name = parts[0]
            pos = int(parts[1]) - 1
            contig = labelled_contigs[name]
            contig.positions.append(pos)
            contig.probabilities[pos] = [float(n) for n in parts[2:]]
    for contig in labelled_contigs.values():
        contig.extend_data_to_start_and_end()


def get_primary_species(labelled_contigs):
    counts = collections.defaultdict(int)
    for contig in labelled_contigs.values():
        for species in contig.get_top_species_per_position():
            counts[species] += 1
    return max(counts, key=counts.get)


def get_species_names(result_file):
    species_names = []
    species_count = None
    with get_open_function(result_file)(result_file, 'rt') as result:
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
    with get_open_function(result_file)(result_file, 'rt') as result:
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


# Unit tests for Pytest
# =====================

def test_smooth_data():
    data = [0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0]
    assert smooth_data(data, 1) ==  pytest.approx([0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0])
    assert smooth_data(data, 3) ==  pytest.approx([0.0, 0.0, 1/3, 1/3, 1/3, 0.0, 0.0])
    assert smooth_data(data, 5) ==  pytest.approx([0.0, 0.25, 0.2, 0.2, 0.2, 0.25, 0.0])
