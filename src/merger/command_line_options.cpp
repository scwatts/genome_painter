#include "command_line_options.h"


namespace cmdline {


void print_help(FILE *stdst) {
    // Print version
    print_version(stdst);

    // Print Usage
    fprintf(stdst, "\n");
    fprintf(stdst, "Usage:\n");
    fprintf(stdst, "  generate_database --count_fps <filepath> ... --threshold <float> --output_fp <filepath>\n");
    fprintf(stdst, "\n");
    fprintf(stdst, "  -c <filepath> ..., --count_fps <filepath> ...\n");
    fprintf(stdst, "                Count input filepaths, space separated\n");
    fprintf(stdst, "  -t <float>, --threshold <float>\n");
    fprintf(stdst, "                Threshold to filter kmers (absolute difference between two highest probabilities) [Default: 0.20]\n");
    fprintf(stdst, "  -a <float>, --alpha <float>\n");
    fprintf(stdst, "                Alpha for shrinkage factor [Default: 1.0]\n");
    fprintf(stdst, "  -o <filepath>, --output_fp <filepath>\n");
    fprintf(stdst, "                Output painted data\n");
    fprintf(stdst, "Other:\n");
    fprintf(stdst, "  -h        --help\n");
    fprintf(stdst, "                Display this help and exit\n");
    fprintf(stdst, "  -v        --version\n");
    fprintf(stdst, "                Display version information and exit\n");
}


void print_version(FILE *stdst) {
    fprintf(stdst, "Program: GenerateDatabase\n");
    fprintf(stdst, "Version %s\n", VERSION.c_str());
    fprintf(stdst, "Contact: Stephen Watts (s.watts2@student.unimelb.edu.au)\n");
}


Options get_arguments(int argc, char **argv) {
    // Commandline arguments (for getlongtops)
    Options options;
    struct option long_options[] =
        {
            {"count_fps", required_argument, NULL, 'c'},
            {"threshold", required_argument, NULL, 'r'},
            {"alpha", required_argument, NULL, 'a'},
            {"output_fp", required_argument, NULL, 'o'},
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
        c = getopt_long(argc, argv, "hvc:t:a:o:", long_options, &long_options_index);

        // If no more arguments to parse, break
        if (c == -1) {
            break;
        }

        // Process current arguments
        switch(c) {
            case 'c':
                manual_index = optind - 1;
                while(manual_index < argc){
                    // Collect all genome_fps; must not start with '-'
                    if (*argv[manual_index] != '-') {
                        options.count_fps.push_back(argv[manual_index]);
                    } else {
                        // Update optind to new position
                        optind = manual_index - 1;
                        break;
                    }
                    manual_index++;
                }
                break;
            case 'a':
                options.alpha = std::stof(optarg);
                break;
            case 't':
                options.threshold = std::stof(optarg);
                break;
            case 'o':
                options.output_fp = optarg;
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
    if (options.count_fps.empty()) {
        print_help(stderr);
        fprintf(stderr,"\n%s: error: argument -c/--count_fps is required\n", argv[0]);
        exit(1);
    }
    if (options.output_fp.empty()) {
        print_help(stderr);
        fprintf(stderr,"\n%s: error: argument -o/--output_fp is required\n", argv[0]);
        exit(1);
    }

    for (auto& count_fp : options.count_fps) {
        if (!common::is_file(count_fp)) {
            print_help(stderr);
            fprintf(stderr, "\n%s: error: input file %s is not a regular file or does not exist\n", argv[0], count_fp.c_str());
            exit(1);
        }
    }
    if (!common::is_file(options.output_fp)) {
        print_help(stderr);
        fprintf(stderr, "\n%s: error: output argument %s is not a directory\n", argv[0], options.output_fp.c_str());
        exit(1);
    }

    // Check threshold is between 0.0 and 1.0
    if (options.threshold < 0.0 || options.threshold > 1.0) {
        print_help(stderr);
        fprintf(stderr, "\n%s: error: argument -t/--threshold must be between 0.0 and 1.0\n", argv[0]);
        exit(1);
    }

    // Check alpha is greater than 0.0
    if (options.threshold < 0.0) {
        print_help(stderr);
        fprintf(stderr, "\n%s: error: argument -a/--alpha must be greater than 0.0\n", argv[0]);
        exit(1);
    }

    return options;
}


} // namespace cmdline
