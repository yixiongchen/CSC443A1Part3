#include "merge.h"

int main (int argc, char **argv) {
	//process and validate command-line arguments

	char *input_file = argv[1];
	int mem_size = atoi(argv[2]);
    int block_size = atoi(argv[3]);
    sort_uid(input_file, mem_size, block_size, 1);
	sort_uid(input_file, mem_size, block_size, 2);
}