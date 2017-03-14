#include "merge.h"

int main (int argc, char **argv) {
	//process and validate command-line arguments

	char *input_file = argv[1];
	int mem_size = atoi(argv[2]);
    int block_size = atoi(argv[3]);
    return sort_uid2(input_file, mem_size, block_size);
}