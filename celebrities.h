typedef struct degree {
	int UID;
	int count;
} Degree;

typedef struct join_manager {
	FILE *inputFP; //stays closed, opens each time we need to reupload some amount of data from disk runs, index 0 is outdegree, index 1 is indegree
	int *input_file_names;	// first is "out_degree.dat", second is "in_degree.dat"
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
	int column;
	int heap_capacity;  //corresponds to the total number of runs (input buffers)
	char output_file_name [MAX_PATH_LENGTH]; //stores name of the file to which to write the final output
	char input_prefix [MAX_PATH_LENGTH]; //stores the prefix of a path to each run - to concatenate with run id and to read the file
}JoinManager;

int get_degrees(int mem_size, int block_size);
int get_degrees_by_column(char* file_name, int mem_size, int block_size, int column);