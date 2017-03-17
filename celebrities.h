//return values of all functions

typedef struct degree {
	int UID;
	int count;
} Degree;


typedef struct OutputHeapElement {
	int UID;
	int in_degree;
	int out_degree;
	int diff;
} OutputHeapElement;


typedef struct join_manager {
	FILE *inputFP; //stays closed, opens each time we need to reupload some amount of data from disk runs
	int *input_file_numbers;  //we need to know the run id to read from the corresponding run	
	Degree **input_buffers; //array of buffers to buffer part of runs
	int input_buffer_capacity; //how many elements max can each input buffer hold
	int *current_input_file_positions; //current position in each sorted run, can use -1 if the run is complete
	int *file_capacity;	// How many elements in total for each file.
	int *current_input_buffer_positions; //position in current input buffer
	int *total_input_buffer_elements;  //number of actual elements currently in input buffer - can be less than max capacity
	int column;		// The column (uid1/uid2) that the file is sorted by
	char output_file_name [MAX_PATH_LENGTH]; //stores name of the file to which to write the final output
	/****************************************New for join*********************************************************/
	OutputHeapElement *output_heap;		// A min heap that stores the top 10 celebrities.
	int current_output_heap_size;	// Number of celebrities in the heap
	int output_heap_capacity;	// Max number of celebrities that heap can contain
}JoinManager;

int get_degrees(int mem_size, int block_size);
int get_degrees_by_column(char* file_name, int mem_size, int block_size, int column);
int compare_heap (const void *one, const void *two) ;//use for qsort
int get_celebrities (JoinManager * joiner, int buffer_capacity);
int print_output_heap (JoinManager * manager);
void clean_up_join_manager (JoinManager * merger);
int init_join (JoinManager * manager, int buffer_capacity);
int get_number_degrees_in_file(JoinManager * manager, int file_number);
void join_get_file_name(char* input_file_name, int file_number);
int join_refill_buffer (JoinManager * manager, int file_number);
int join_get_next_input_element(JoinManager * manager, int file_number, Degree *result);
int update_output_heap (JoinManager * manager, Degree * input_outdegree, Degree * input_indegree);
int join_insert_into_heap (JoinManager * manager, int uid, int in_degree, int out_degree, int diff);
int remove_top_heap_element (JoinManager * manager);
int join_compare_heap_elements (OutputHeapElement *a, OutputHeapElement *b);
int get_degrees(int mem_size, int block_size);
int get_degrees_by_column(char* file_name, int mem_size, int block_size, int column);