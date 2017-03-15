#include "merge.h"

int sort_uid1(char *input_file, int mem_size, int block_size){
	MergeManager manager;
	char *input_prefix = "sorted_list";

    int num_trunks = phase1(input_file, mem_size, block_size, input_prefix, 1);
	printf("start test with memory size: %dMB  block size Byte: %d\n", mem_size/1024/1024, block_size);
	int blocks_per_buffer = mem_size/(num_trunks+1) / block_size;
	if (blocks_per_buffer < 1){
		printf("Main memory size too small for merging.\n");
		exit(1);
	}
	int buffer_capacity = blocks_per_buffer * block_size / sizeof(Record);

	//initialize all fields according to the input and the results of Phase I
	return merge_runs_uid1(&manager, num_trunks, input_prefix, buffer_capacity);

}


int sort_uid2(char *input_file, int mem_size, int block_size){
	MergeManager manager;
	char *input_prefix = "sorted_list";

    int num_trunks = phase1(input_file, mem_size, block_size, input_prefix, 2);
	printf("start test with memory size: %dMB  block size Byte: %d\n", mem_size/1024/1024, block_size);
	int blocks_per_buffer = mem_size/(num_trunks+1) / block_size;
	if (blocks_per_buffer < 1){
		printf("Main memory size too small for merging.\n");
		exit(1);
	}
	int buffer_capacity = blocks_per_buffer * block_size / sizeof(Record);

	//initialize all fields according to the input and the results of Phase I
	return merge_runs_uid2 (&manager, num_trunks, input_prefix, buffer_capacity);

}
