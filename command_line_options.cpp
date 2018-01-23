#include "command_line_options.h"


namespace cmdline {


void print_help() {
    // Print version
    print_version();

    // Print Usage
    fprintf(stderr, "\n");
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "  database_filter --input_fp <filepath> --output_fp <filepath>\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "  -g <filepath>, --genome_fp <filepath>\n");
    fprintf(stderr, "                Genome input filepath (FASTA format)\n");
    fprintf(stderr, "  -k <filepath>, --kmer_db_fp <filepath>\n");
    fprintf(stderr, "                Kmer database filepath\n");
    fprintf(stderr, "  -o <filepath>, --output_fp <filepath>\n");
    fprintf(stderr, "                Output painted data\n");
    fprintf(stderr, "Other:\n");
    fprintf(stderr, "  -h        --help\n");
    fprintf(stderr, "                Display this help and exit\n");
    fprintf(stderr, "  -v        --version\n");
    fprintf(stderr, "                Display version information and exit\n");
}


void print_version() {
    fprintf(stderr, "Program: DatabaseFilter\n");
    fprintf(stderr, "Version 0.0.1a\n");
    fprintf(stderr, "Contact: Stephen Watts (s.watts2@student.unimelb.edu.au)\n");
}


Options get_arguments(int argc, char **argv) {
    // Commandline arguments (for getlongtops)
    Options options;
    struct option long_options[] =
        {
            {"genome_fp", required_argument, NULL, 'g'},
            {"kmer_db_fp", required_argument, NULL, 'k'},
            {"output_Fp", required_argument, NULL, 'o'},
            {"version", no_argument, NULL, 'v'},
            {"help", no_argument, NULL, 'h'},
            {NULL, 0, 0, 0}
        };

    // Parse commandline arguments
    while (1) {
        // Parser variables
        int option_index = 0;
        int c;

        // Parser
        c = getopt_long(argc, argv, "hvg:k:o:", long_options, &option_index);

        // If no more arguments to parse, break
        if (c == -1) {
            break;
        }

        // Process current arguments
        switch(c) {
            case 'g':
                options.genome_fp = optarg;
                break;
            case 'k':
                options.kmer_db_fp = optarg;
                break;
            case 'o':
                options.output_fp = optarg;
                break;
            case 'v':
                print_version();
                exit(0);
            case 'h':
                print_help();
                exit(0);
            default:
                exit(1);
        }
    }

    // Ensure that filepaths are present
    if (options.genome_fp.empty()) {
        print_help();
        fprintf(stderr,"\n%s: error: argument -g/--genome_fp is required\n", argv[0]);
        exit(1);
    }
    if (options.kmer_db_fp.empty()) {
        print_help();
        fprintf(stderr,"\n%s: error: argument -k/--kmer_db_fp is required\n", argv[0]);
        exit(1);
    }
    if (options.output_fp.empty()) {
        print_help();
        fprintf(stderr,"\n%s: error: argument -o/--output_fp is required\n", argv[0]);
        exit(1);
    }

    if (!filepath_exists(options.genome_fp)) {
        print_help();
        fprintf(stderr, "\n%s: error: input file %s does not exist\n", argv[0], options.genome_fp.c_str());
        exit(1);
    }
    if (!filepath_exists(options.kmer_db_fp)) {
        print_help();
        fprintf(stderr, "\n%s: error: input file %s does not exist\n", argv[0], options.kmer_db_fp.c_str());
        exit(1);
    }

    return options;
}


bool filepath_exists(std::string &filepath) {
    std::ifstream file(filepath);
    bool file_status = file.good();
    file.close();
    return file_status;
}


} // namespace cmdline
