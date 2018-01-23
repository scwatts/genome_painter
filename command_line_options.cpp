#include "command_line_options.h"


namespace cmdline {


void print_help(FILE *stdst) {
    // Print version
    print_version(stdst);

    // Print Usage
    fprintf(stdst, "\n");
    fprintf(stdst, "Usage:\n");
    fprintf(stdst, "  database_filter --genome_fp <filepath> ... --kmer_db_fp <filepath> --output_dir <filepath>\n");
    fprintf(stdst, "\n");
    fprintf(stdst, "  -g <filepath> ..., --genome_fp <filepath> ...\n");
    fprintf(stdst, "                Genome input filepaths, space separated (FASTA format)\n");
    fprintf(stdst, "  -k <filepath>, --kmer_db_fp <filepath>\n");
    fprintf(stdst, "                Kmer database filepath\n");
    fprintf(stdst, "  -o <filepath>, --output_dir <filepath>\n");
    fprintf(stdst, "                Output painted data\n");
    fprintf(stdst, "Other:\n");
    fprintf(stdst, "  -h        --help\n");
    fprintf(stdst, "                Display this help and exit\n");
    fprintf(stdst, "  -v        --version\n");
    fprintf(stdst, "                Display version information and exit\n");
}


void print_version(FILE *stdst) {
    fprintf(stdst, "Program: DatabaseFilter\n");
    fprintf(stdst, "Version 0.0.1a\n");
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
        c = getopt_long(argc, argv, "hvg:k:o:", long_options, &long_options_index);

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
        if (!is_file(genome_fp)) {
            print_help(stderr);
            fprintf(stderr, "\n%s: error: input file %s is not a regular file or does not exist\n", argv[0], genome_fp.c_str());
            exit(1);
        }
    }
    if (!is_file(options.kmer_db_fp)) {
        print_help(stderr);
        fprintf(stderr, "\n%s: error: input file %s is not a regular file or does not exist\n", argv[0], options.kmer_db_fp.c_str());
        exit(1);
    }
    if (!is_directory(options.output_dir)) {
        print_help(stderr);
        fprintf(stderr, "\n%s: error: output argument %s is not a directory\n", argv[0], options.output_dir.c_str());
        exit(1);
    }

    return options;
}


bool is_file(std::string &filepath) {
    struct stat sb;
    stat(filepath.c_str(), &sb);
    return sb.st_mode & S_IFREG;
}


bool is_directory(std::string &filepath) {
    struct stat sb;
    stat(filepath.c_str(), &sb);
    return sb.st_mode & S_IFDIR;
}


} // namespace cmdline
