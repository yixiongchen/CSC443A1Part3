#include "merge.h"


int sort_uid(char *input_file, int mem_size, int block_size, int column){
	MergeManager manager;
	char *input_prefix;
	
	if (column == 1){
		input_prefix = "sorted_list_uid1_";
	}
	else{
		input_prefix = "sorted_list_uid2_";
	}
	
	/*******************************************************************/
	// Check memory size
	// Get file size
	FILE *fp_read;
	if (!(fp_read= fopen (input_file , "r" ))) {
		printf ("Could not open file \"%s\" for reading \n", input_file);
		exit(1);
    }
    fseek(fp_read, 0, SEEK_END);
    int file_size = ftell(fp_read);
	int total_records = file_size / sizeof(Record);
    fseek(fp_read, 0, SEEK_SET);
	fclose(fp_read);
	
	int num_blocks_in_ram =  mem_size / block_size / 2;
	if (num_blocks_in_ram < 1){
		printf("SORT FAIL: Memory size is too small for splitting.\n");
		exit(1);
	}
    int num_records_in_ram = num_blocks_in_ram * block_size / sizeof(Record);
	int num_runs = total_records / num_records_in_ram + 1;
	
	if (mem_size < (num_runs+1) * block_size){
		printf("SORT FAIL: Memory size is too small for merging.\n");
		exit(1);
	}
	
	/*******************************************************************/
	// Start Sorting
    int num_trunks = phase1(input_file, mem_size, block_size, input_prefix, column);
	printf("start test with memory size: %dMB  block size Byte: %d\n", mem_size/1024/1024, block_size);
	int blocks_per_buffer = mem_size/(num_trunks+1) / block_size;
	int buffer_capacity = blocks_per_buffer * block_size / sizeof(Record);

	//initialize all fields according to the input and the results of Phase I
	return merge_runs (&manager, num_trunks, input_prefix, buffer_capacity, column);
}