#ifndef __OUTPUT_H__
#define __OUTPUT_H__


#include <string>
#include <vector>


#include "zlib.h"


#include "paint.h"
#include "merger/file.h"


#define CHUNK_SIZE 1024 * 25
#define BUFFER_ADD(buffer, buffer_size, output_fh, format, ...)                                                                 \
    do {                                                                                                                        \
        size_t BUFFER_ADD_line_size = snprintf(NULL, 0, format, __VA_ARGS__);                                                 \
        if ( (buffer_size + BUFFER_ADD_line_size) > CHUNK_SIZE) { gzwrite(output_fh, buffer, buffer_size); buffer_size = 0; } \
        buffer_size += snprintf(buffer+buffer_size, CHUNK_SIZE-buffer_size, format, __VA_ARGS__);                               \
    } while(0)


namespace output {


std::string construct_output_fp(std::string &genome_fp, std::string &suffix, std::string output_dir);
void write_painted_genome(std::vector<paint::FastaPaint> &fasta_painting, db::HeaderInfo db_header, std::vector<genome::FastaRecord> &fastas, std::string &output_fp);


} // namespace output


#endif
