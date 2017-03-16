#ifndef MERGE_H
#define MERGE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PATH_LENGTH 1024
#define MAX_CHARS_PER_LINE 1024

//return values of all functions
#define SUCCESS 0
#define FAILURE 1
#define EMPTY 2

typedef struct record {
	int UID1;
	int UID2;
} Record;

typedef struct HeapElement {
	int UID1;
	int UID2;
	int run_id;
} HeapElement;

//Record-keeping struct, to pass around to all small functions
//has to be initialized before merge starts
typedef struct merge_manager {
	HeapElement *heap;  //keeps 1 from each buffer in top-down order - smallest on top (according to compare function)	
	FILE *inputFP; //stays closed, opens each time we need to reupload some amount of data from disk runs
	int *input_file_numbers;  //we need to know the run id to read from the corresponding run	
	FILE *outputFP; //flushes output from output buffer 
	Record *output_buffer; //buffer to store output elements until they are flushed to disk
	int current_output_buffer_position;  //where to add element in the output buffer
	int output_buffer_capacity; //how many elements max can it hold
	Record **input_buffers; //array of buffers to buffer part of runs
	int input_buffer_capacity; //how many elements max can each input buffer hold
	int *current_input_file_positions; //current position in each sorted run, can use -1 if the run is complete
	int *file_capacity;	// How many elements in total for each file.
	int *current_input_buffer_positions; //position in current input buffer
	int *total_input_buffer_elements;  //number of actual elements currently in input buffer - can be less than max capacity
	int current_heap_size;
	int heap_capacity;  //corresponds to the total number of runs (input buffers)
	char output_file_name [MAX_PATH_LENGTH]; //stores name of the file to which to write the final output
	char input_prefix [MAX_PATH_LENGTH]; //stores the prefix of a path to each run - to concatenate with run id and to read the file
}MergeManager;


// MAIN
int sort_uid2(char *input_file, int mem_size, int block_size);
int sort_uid1(char *input_file, int mem_size, int block_size);

// DISK_SORT
int compare_uid1 (const void *a, const void *b);
int compare_uid2 (const void *a, const void *b);
void uid1_sort(Record * buffer, int total_records);
void uid2_sort(Record * buffer, int total_records);
void print_records(Record * buffer, int total_records);

// SORTED_RUN
int phase1(char* input_file, int mem_size, int block_size, char* output_filename, int uid);

// HELPER
int get_number_records_in_file(MergeManager * manager, int file_number);
void get_file_name(char* input_file_name, MergeManager * manager, int file_number);
void print_buffers(MergeManager * manager);
void print_heap(MergeManager * manager);
long get_max_degree(char* file_name, int block_size, int column_id);

//1. main loop
int merge_runs_uid1 (MergeManager * manager, int num_trunks, char *input_prefix, int buffer_capacity); 
int merge_runs_uid2 (MergeManager * manager, int num_trunks, char *input_prefix, int buffer_capacity); 

//2. creates and fills initial buffers, initializes heap taking 1 top element from each buffer 
int init_merge_uid1 (MergeManager * manager, int num_trunks, char *input_prefix, int buffer_capacity); 
int init_merge_uid2 (MergeManager * manager, int num_trunks, char *input_prefix, int buffer_capacity); 
//3. flushes output buffer to disk when full
int flush_output_buffer (MergeManager * manager); 

//4. returns top heap element, rearranges heap nodes
int get_top_heap_element_uid1 (MergeManager * manager, HeapElement * result);
int get_top_heap_element_uid2 (MergeManager * manager, HeapElement * result);
//5. inserts new element into heap, rearranges nodes to keep smallest on top
int insert_into_heap_uid1 (MergeManager * manager, int run_id, Record *input);
int insert_into_heap_uid2 (MergeManager * manager, int run_id, Record *input);

//6. reads next element from an input buffer to transfer it to the heap. Uploads records from disk if all elements are processed
int get_next_input_element(MergeManager * manager, int file_number, Record *result); 

//7. refills input buffer from the corresponding run
int refill_buffer (MergeManager * manager, int file_number);

//8. Frees all dynamically allocated memory
void clean_up (MergeManager * merger);

//9. Application-specific comparison function
int compare_heap_elements_uid1 (HeapElement *a, HeapElement *b);
int compare_heap_elements_uid2 (HeapElement *a, HeapElement *b);

// query computation
void merge_sort(char* file_uid1, char* file_uid2);
int compare_two_uids(Record buffer1, Record buffer2);


#endif