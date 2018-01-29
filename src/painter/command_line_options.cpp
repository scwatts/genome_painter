#include "command_line_options.h"


namespace cmdline {


void print_help(FILE *stdst) {
    // Print version
    print_version(stdst);

    // Print Usage
    fprintf(stdst, "\n");
    fprintf(stdst, "Usage:\n");
    fprintf(stdst, "  genome_painter --genome_fp <filepath> ... --kmer_db_fp <filepath> --output_dir <directorypath>\n");
    fprintf(stdst, "\n");
    fprintf(stdst, "  -g <filepath> ..., --genome_fps <filepath> ...\n");
    fprintf(stdst, "                Genome input filepaths, space separated (FASTA format)\n");
    fprintf(stdst, "  -k <filepath>, --kmer_db_fp <filepath>\n");
    fprintf(stdst, "                Kmer database filepath\n");
    fprintf(stdst, "  -o <directorypath>, --output_dir <directorypath>\n");
    fprintf(stdst, "                Output painted data\n");
    fprintf(stdst, "Options:\n");
    fprintf(stdst, "  -t <integer>, --threads <integer>\n");
    fprintf(stdst, "                Number of threads to use [Default: 1]\n");
    fprintf(stdst, "Other:\n");
    fprintf(stdst, "  -h        --help\n");
    fprintf(stdst, "                Display this help and exit\n");
    fprintf(stdst, "  -v        --version\n");
    fprintf(stdst, "                Display version information and exit\n");
}


void print_version(FILE *stdst) {
    fprintf(stdst, "Program: GenomePainter\n");
    fprintf(stdst, "Version %s\n", VERSION.c_str());
    fprintf(stdst, "Contact: Stephen Watts (s.watts2@student.unimelb.edu.au)\n");
}


Options get_arguments(int argc, char **argv) {
    // Commandline arguments (for getlongtops)
    Options options;
    struct option long_options[] =
        {
            {"genome_fp", required_argument, NULL, 'g'},
            {"kmer_db_fp", required_argument, NULL, 'k'},
            {"output_dir", required_argument, NULL, 'o'},
            {"threads", required_argument, NULL, 't'},
            {"version", no_argument, NULL, 'v'},
            {"help", no_argument, NULL, 'h'},
            {NULL, 0, 0, 0}
        };

    // Parse commandline arguments
    while (1) {
        // Parser variables
        int manual_index = 0;
        int long_options_index = 0;
        int c;

        // Parser
        c = getopt_long(argc, argv, "hvg:k:o:t:", long_options, &long_options_index);

        // If no more arguments to parse, break
        if (c == -1) {
            break;
        }

        // Process current arguments
        switch(c) {
            case 'g':
                manual_index = optind - 1;
                while(manual_index < argc){
                    // Collect all genome_fps; must not start with '-'
                    if (*argv[manual_index] != '-') {
                        options.genome_fps.push_back(argv[manual_index]);
                    } else {
                        // Update optind to new position
                        optind = manual_index - 1;
                        break;
                    }
                    manual_index++;
                }
                break;
            case 'k':
                options.kmer_db_fp = optarg;
                break;
            case 'o':
                options.output_dir= optarg;
                break;
            case 't':
                options.threads = static_cast<unsigned int>(std::stoi(optarg));
                break;
            case 'v':
                print_version(stdout);
                exit(0);
            case 'h':
                print_help(stdout);
                exit(0);
            default:
                exit(1);
        }
    }

    // Ensure that filepaths are present
    if (options.genome_fps.empty()) {
        print_help(stderr);
        fprintf(stderr,"\n%s: error: argument -g/--genome_fps is required\n", argv[0]);
        exit(1);
    }
    if (options.kmer_db_fp.empty()) {
        print_help(stderr);
        fprintf(stderr,"\n%s: error: argument -k/--kmer_db_fp is required\n", argv[0]);
        exit(1);
    }
    if (options.output_dir.empty()) {
        print_help(stderr);
        fprintf(stderr,"\n%s: error: argument -o/--output_dir is required\n", argv[0]);
        exit(1);
    }

    for (auto& genome_fp : options.genome_fps) {
        if (!common::is_file(genome_fp)) {
            print_help(stderr);
            fprintf(stderr, "\n%s: error: input file %s is not a regular file or does not exist\n", argv[0], genome_fp.c_str());
            exit(1);
        }
    }
    if (!common::is_file(options.kmer_db_fp)) {
        print_help(stderr);
        fprintf(stderr, "\n%s: error: input file %s is not a regular file or does not exist\n", argv[0], options.kmer_db_fp.c_str());
        exit(1);
    }
    if (!common::is_directory(options.output_dir)) {
        print_help(stderr);
        fprintf(stderr, "\n%s: error: output argument %s is not a directory\n", argv[0], options.output_dir.c_str());
        exit(1);
    }

    // Check if have a reasonable number of threads
    if (options.threads < 1) {
        print_help(stderr);
        fprintf(stderr,"\n%s: error: must specify at least 1 thread\n", argv[0]);
        exit(1);
    }

    // Check we don't attempt to use more threads than we have
    unsigned int available_threads = std::thread::hardware_concurrency();
    if (available_threads > 1 && options.threads > available_threads) {
        print_help(stderr);
        fprintf(stderr, "\n%s: error: only %d threads are available\n", argv[0], available_threads);
        exit(1);
    } else if (options.threads > 64) {
        print_help(stderr);
        fprintf(stderr, "\n%s: error: current hardcode limit of 64 threads\n", argv[0]);
        exit(1);
    }

    return options;
}


} // namespace cmdline
