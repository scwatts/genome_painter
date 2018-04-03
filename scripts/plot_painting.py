#!/usr/bin/env python3
"""
Summarise species composition of Genome Painter results into blocks of n size and generate plot.

Output is an interactive plotly plot which can be viewed in a modern web brower

Example usage:
    ./plot_painting.py --genome_painter_fp genome_painter_results.tsv.gz --output_fp genome_painter_plot.html
"""


import argparse
import pathlib
import gzip


import plotly.plotly
import plotly.graph_objs


SHADING_TEMPLATE = {
        'type': 'rect',
        'xref': 'x',
        'yref': 'y',
        'x0': None,
        'y0': 0,
        'x1': None,
        'y1': 1,
        'fillcolor': '#e2e2e2',
        'opacity': 0.2,
        'line': {
            'width': 0.25,
        }
    }


class LineData():
    '''Single line of data'''

    def __init__(self, contig, position_str, probabilities_str_tokens):
        self.contig = contig
        self.position = int(position_str)
        self.probabilities = [float(p) for p in probabilities_str_tokens]

        self.adjusted_position = None

    def __str__(self):
        return '%s %s' % (self.contig, self.position)


class BlockDataContainer():
    '''Contains BlockData instances sorted by contigs'''

    def __init__(self, block_size):
        self.block_size = block_size
        self.contig_data = dict()


class BlockData():
    '''Stores LineData instances which make up a single block'''

    def __init__(self, upper_bound, block_size):
        self.data = list()
        self.position = upper_bound


class BlockContainer():
    '''Contains summarised blocks'''

    def __init__(self, contig, size):
        self.contig = contig
        self.size = size
        self.blocks = list()

    @property
    def has_data(self):
        try:
            return len(self.blocks[0].probabilities) > 0
        except IndexError:
            return False


class Block():
    '''Stores sumamrised BlockData metrics'''
    def __init__(self, position, probabilities):
        self.position = position
        self.probabilities = probabilities


def get_arguments():
    parser = argparse.ArgumentParser()
    parser.add_argument('--genome_painter_fp', required=True, type=pathlib.Path,
            help='Input painted genome filepath. Can be gzip compressed or uncompressed.')
    parser.add_argument('--block_size', type=int, default=int(1e3),
            help='Size of summary blocks [default: 1000] ')
    parser.add_argument('--output_fp', required=True, type=str,
            help='Filepath for output plot')

    # Ensure that input file exists
    args = parser.parse_args()
    if not args.genome_painter_fp.exists():
        parser.error('Input file %s does not exist' % args.genome_painter_fp)

    return args


def main():
    # Get commandline arguments
    args = get_arguments()

    # Detect input filetype and bin data by blocks within each contig
    file_open_func = get_filehandle_class(args.genome_painter_fp)
    with file_open_func() as fh:
        species, contig_sizes = process_header(fh)
        genome_block_data = collect_block_data(fh, contig_sizes, args.block_size)

    # Summarise binned contig block data and remove contig blocks without data
    genome_blocks = summarise_block_data(genome_block_data, contig_sizes)
    genome_blocks = {cn: cb for cn, cb in genome_blocks.items() if cb.has_data}

    # Adjust positions such that the positions within one contig follow from the previous
    genome_blocks = adjust_positions(genome_blocks)

    # Create traces. Probabilities must be grouped by species
    traces = list()
    block_prob_gen = (b.probabilities for cb in genome_blocks.values() for b in cb.blocks)
    block_positions = [b.position for cb in genome_blocks.values() for b in cb.blocks]
    species_probabilities = zip(*block_prob_gen)
    for species, probability_zip in zip(species, species_probabilities):
        # First point in probabilities is at origin (0,0)
        probs = list(probability_zip)
        trace = plotly.graph_objs.Scatter(x=[0] + block_positions, y=[0] + probs, fill='tozeroy', mode='none', name=species)
        traces.append(trace)

    # Create contig shading shapes
    contig_shapes = generate_contig_shapes(genome_blocks)

    # Generate plot layout
    if len(genome_block_data.contig_data) > 1:
        layout = {'shapes': contig_shapes}
    else:
        layout = plotly.graph_objs.Layout()

    # Create plot
    plotly.offline.plot({'data': traces, 'layout': layout}, show_link=False, auto_open=False, filename=output_fp)


def get_filehandle_class(filepath):
    '''Checks if file is gzipped and returns appropriate file object handler'''
    with filepath.open('rb') as fh:
        if fh.read(2) == b'\037\213':
            return lambda: gzip.open(filepath, 'rt')
        else:
            return lambda: filepath.open('r')


def process_header(fh):
    '''Collect header information'''
    species = list()
    contig_sizes = dict()
    species_total = int(fh.readline().lstrip('#'))
    for i in range(species_total):
        species.append(fh.readline().rstrip().lstrip('#'))
    contig_total = int(fh.readline().lstrip('#'))
    for i in range(contig_total):
        contig_name, size_str = fh.readline().rstrip().lstrip('#').split()
        contig_sizes[contig_name] = int(size_str)

    return species, contig_sizes


def collect_block_data(fh, contig_sizes, block_size):
    '''Sort data into blocks'''
    line_token_gen = (line.rstrip().split() for line in fh)
    line_data_gen = (LineData(name, pos, probs) for name, pos, *probs in line_token_gen)

    # Sort data by contig names
    contig_data = {contig_name: list() for contig_name in contig_sizes}
    for line_data in line_data_gen:
        contig_data[line_data.contig].append(line_data)

    # Sort contigs by size
    contigs_size_order = sorted(contig_sizes.items(), key=lambda k: k[1], reverse=True)

    # TODO: add range specifier. perform contig range selection and then line data selection

    # Process contigs in order by descending size
    genome_block_data = BlockDataContainer(block_size)
    for contig_name, contig_size in contigs_size_order:
        contig_block_data = list()
        block_bounds = list(range(block_size, contig_size, block_size)) + [contig_size]
        block_bounds_iter = iter(block_bounds)
        upper_bound = next(block_bounds_iter)
        block_data = BlockData(upper_bound, block_size)

        # Place each contig data line into a block
        for line_data in contig_data[contig_name]:
            if line_data.position > upper_bound:
                upper_bound = next(block_bounds_iter)
                # Only append if the block has data
                if block_data.data:
                    contig_block_data.append(block_data)
                block_data = BlockData(upper_bound, block_size)
            block_data.data.append(line_data)

        # Append remaining block data
        contig_block_data.append(block_data)

        # Record contig data in BlockData instance
        genome_block_data.contig_data[contig_name] = contig_block_data
    return genome_block_data


def summarise_block_data(genome_block_data, contig_sizes):
    '''Summarises block data'''
    genome_blocks = dict()
    for contig_name, contig_block_data in genome_block_data.contig_data.items():
        contig_blocks = BlockContainer(contig_name, contig_sizes[contig_name])
        for block_data in contig_block_data:
            # To zip the probabilities together and arrange by species, we must create a generator
            # that pulls this data directly out of LineData class via the BlcokData class
            raw_data_gen = (line_data.probabilities for line_data in block_data.data)
            block_probabilities = [sum(d) / len(d) for d in zip(*raw_data_gen)]
            block = Block(block_data.position, block_probabilities)
            contig_blocks.blocks.append(block)
        genome_blocks[contig_name] = contig_blocks
    return genome_blocks


def adjust_positions(genome_blocks):
    '''Applies offset to each block as if contigs were concatenated'''
    position_offset = 0
    for contig_name, contig_blocks in genome_blocks.items():
        for block in contig_blocks.blocks:
            block.position += position_offset
        position_offset += contig_blocks.size
    return genome_blocks


def generate_contig_shapes(genome_blocks):
    '''Create contig shading data for plot'''
    shapes = list()
    running_bound = 0
    for i, contig_blocks in enumerate(genome_blocks.values()):
        if not contig_blocks.blocks:
            continue
        if (i % 2) == 0:
            shape = SHADING_TEMPLATE.copy()
            shape['x0'] = running_bound
            running_bound += contig_blocks.size
            shape['x1'] = running_bound
            shapes.append(shape)
        else:
            running_bound += contig_blocks.size
    return shapes


if __name__ == '__main__':
    main()
