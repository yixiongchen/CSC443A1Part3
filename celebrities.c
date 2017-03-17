#include "merge.h"
#include "celebrities.h"

int main(int argc, char **argv){
	JoinManager manager;
	
	char *filename = argv[1];
	int mem_size = atoi(argv[2]);
    int block_size = atoi(argv[3]);
	
	// Sort input file by uid1
	sort_uid(filename, mem_size, block_size, 1);
	// Sort input file by uid2
	sort_uid(filename, mem_size, block_size, 2);
	
	get_degrees(mem_size, block_size);
	
	// Start join.
	int blocks_per_buffer = mem_size / 2 / block_size;
	int degrees_per_block = block_size / sizeof(Degree);
	int buffer_capacity = blocks_per_buffer * degrees_per_block;

	//initialize all fields according to the input and the results of Phase I
	return get_celebrities (&manager, buffer_capacity);
}

//manager fields should be already initialized in the caller
int get_celebrities (JoinManager * manager, int buffer_capacity){	
	int  result; //stores SUCCESS/FAILURE returned at the end
	
	Degree next_outdegree, next_indegree; //here next comes from input buffer
	next_outdegree.UID = -1;
	next_outdegree.count = -1;
	next_indegree.UID = -1;
	next_indegree.count = -1;
	
	//1. go in the loop through all input files and fill-in initial buffers
	if (init_join (manager, buffer_capacity)!=SUCCESS){
		printf("INIT_MERGE FAILED\n");
		return FAILURE;
	}
	
	int result_outdegree = join_get_next_input_element (manager, 0, &next_outdegree);
	int result_indegree = join_get_next_input_element (manager, 1, &next_indegree);
	
	if (result_outdegree==FAILURE || result_indegree == FAILURE){
		printf("JOIN_GET_NEXT_INPUT_ELEMENT FAILURE: Fail to get degree from degree file\n");
		return FAILURE;
	}
	
	int prevent = 0;
	
	
	while (result == SUCCESS && prevent < 50) { //Not finish reading both files
		result = update_output_heap (manager, &next_outdegree, &next_indegree);
		if (result == FAILURE){
			printf("UPDATE_OUTPUT_HEAP FAIL\n");
			return FAILURE;
		}
		prevent++;
	}
	
	// Print whatever in the heap;
	print_output_heap(manager);
	
	clean_up_join_manager(manager);
	return SUCCESS;
}


int print_output_heap (JoinManager * manager){
	for (int i = 0; i < manager->current_output_heap_size; i++){
		printf("UID = %d, out_degree = %d, in_degree = %d, diff = %d\n", manager->output_heap[i].UID, manager->output_heap[i].out_degree, manager->output_heap[i].in_degree, manager->output_heap[i].diff);
	}
	
	return SUCCESS;
}


void clean_up_join_manager (JoinManager * merger) {
	free(merger->output_heap);
	free(merger->input_file_numbers);
	for (int i = 0; i < 2; i++) { 
		free(merger->input_buffers[i]);
	}
	free(merger->input_buffers);
	free(merger->current_input_file_positions);
	free(merger->file_capacity);
}


int init_join (JoinManager * manager, int buffer_capacity) {
	
	int num_trunks = 2;
	manager->input_buffer_capacity = buffer_capacity;
	manager->output_heap_capacity = 10;
	manager->current_output_heap_size = 0;
	
	manager->output_heap = (OutputHeapElement *) calloc (manager->output_heap_capacity, sizeof (OutputHeapElement));
	
	manager->input_file_numbers = (int *) calloc (num_trunks, sizeof (int));
	for (int i = 0; i < num_trunks; i++){
		manager->input_file_numbers[i] = i;
	}
	
	manager->input_buffers = (Degree **) calloc (num_trunks, sizeof (Degree *));
	for (int i = 0; i < num_trunks; i++) { 
		manager->input_buffers[i] = (Degree *) calloc (buffer_capacity, sizeof (Degree));
	}
	
	manager->current_input_file_positions = (int *) calloc (num_trunks, sizeof (int));
	for (int i = 0; i < num_trunks; i++){
		manager->current_input_file_positions[i] = 0;
	}
	
	manager->current_input_buffer_positions = (int *) calloc (num_trunks, sizeof (int));
	for (int i = 0; i < num_trunks; i++){
		manager->current_input_buffer_positions[i] = 0;
	}
	
	manager->file_capacity = (int *) calloc (num_trunks, sizeof (int));
	for (int i = 0; i < num_trunks; i++){
		manager->file_capacity[i] = get_number_degrees_in_file(manager, i);
	}
	
	manager->total_input_buffer_elements = calloc (num_trunks, sizeof (int));
	for (int i = 0; i < num_trunks; i++){
		manager->total_input_buffer_elements[i] = 0;
	}
	
	// Fill buffer and update celebrities output heap.
	for (int i = 0; i < num_trunks; i++){
		join_refill_buffer(manager, i);
	}
	
	strcpy(manager->output_file_name, "celebrities.dat");
	
	return SUCCESS;
}


int get_number_degrees_in_file(JoinManager * manager, int file_number){
	char filename[MAX_PATH_LENGTH];
	join_get_file_name(filename, file_number);
	FILE *fp_read;
	fp_read = fopen(filename, "r");
	if (!fp_read){
		printf ("GET_NUMBER_DEGREES_IN_FILE ERROR: Could not open file \"%s\" for reading \n", filename);
		return FAILURE;
	}
	
	fseek(fp_read, 0, SEEK_END);
    int file_size = ftell(fp_read);
	int total_degrees = file_size / sizeof(Degree);
    fseek(fp_read, 0, SEEK_SET); 
	
	fclose(fp_read);
	return total_degrees;
}


void join_get_file_name(char* input_file_name, int file_number){
	if (file_number == 0){
		strcpy(input_file_name, "out_degree.dat");
	} else {
		strcpy(input_file_name, "in_degree.dat");
	}
}


int join_refill_buffer (JoinManager * manager, int file_number) {
	
	if (manager->current_input_file_positions[file_number] == -1){
		return EMPTY;
	}
	
	char input_file_name[MAX_PATH_LENGTH];
	join_get_file_name(input_file_name, file_number);
	
	manager->inputFP = fopen(input_file_name, "r");
	if (!manager->inputFP){
		printf ("JOIN_REFILL_BUFFER ERROR: Could not open file \"%s\" for reading \n", input_file_name);
		return FAILURE;
	}
	
	fseek(manager->inputFP, sizeof(Degree) * manager->current_input_file_positions[file_number], SEEK_SET);
	int degrees_left = manager->file_capacity[file_number] - manager->current_input_file_positions[file_number];

	int degrees_to_read;
	if (degrees_left > manager->input_buffer_capacity){
		degrees_to_read = manager->input_buffer_capacity;
		manager->current_input_file_positions[file_number] += degrees_to_read;
	}
	else{
		degrees_to_read = degrees_left;
		manager->current_input_file_positions[file_number] = -1;
	}
	int read_result = fread(manager->input_buffers[file_number], sizeof(Degree), degrees_to_read, manager->inputFP);
	if (read_result <= 0){
		printf ("JOIN_REFILL_BUFFER ERROR: Could not read file \"%s\".\n", input_file_name);
		return FAILURE;
	}
	
	manager->total_input_buffer_elements[file_number] = degrees_to_read;
	manager->current_input_buffer_positions[file_number] = 0;
	
	fclose(manager->inputFP);
	
	return SUCCESS;
}


int join_get_next_input_element(JoinManager * manager, int file_number, Degree *result) {
	
	if (manager->current_input_buffer_positions[file_number] == manager->total_input_buffer_elements[file_number]){
		if (manager->total_input_buffer_elements[file_number] < manager->input_buffer_capacity){
			result->UID = -1;
			return EMPTY;
		}
		else{	//REFILL
			int function_result = join_refill_buffer(manager, file_number);
			if (function_result != SUCCESS){
				return function_result;
			}
		}
	}
	
	*result = manager->input_buffers[file_number][manager->current_input_buffer_positions[file_number]];
	manager->current_input_buffer_positions[file_number]++;
	
	return SUCCESS;
}


int update_output_heap (JoinManager * manager, Degree * input_outdegree, Degree * input_indegree){
	int next_result0, next_result1;
	int uid, in_degree, out_degree, diff;
	int result;
	if (input_outdegree->UID == -1 && input_indegree->UID == -1){
		return EMPTY;
	}
	else if (input_outdegree->UID == -1){
		uid = input_indegree->UID;
		in_degree = input_indegree->count;
		out_degree = 0;
		next_result1 = join_get_next_input_element (manager, 1, input_indegree);
		if (next_result1 == FAILURE){
			return next_result1;
		}
	}
	else if (input_indegree->UID == -1){
		uid = input_outdegree->UID;
		in_degree = 0;
		out_degree = input_outdegree->count;
		next_result0 = join_get_next_input_element (manager, 0, input_outdegree);
		if (next_result0 == FAILURE){
			return next_result0;
		}
	}
	else{
		if (input_indegree->UID == input_outdegree->UID){
			uid = input_indegree->UID;
			in_degree = input_indegree->count;
			out_degree = input_outdegree->count;
			next_result1 = join_get_next_input_element (manager, 1, input_indegree);
			next_result0 = join_get_next_input_element (manager, 0, input_outdegree);
			if (next_result0 == FAILURE){
				return next_result0;
			}
			if (next_result1 == FAILURE){
				return next_result1;
			}
		}
		else if (input_outdegree->UID < input_indegree->UID){
			uid = input_outdegree->UID;
			in_degree = 0;
			out_degree = input_outdegree->count;
			next_result0 = join_get_next_input_element (manager, 0, input_outdegree);
			if (next_result0 == FAILURE){
				return next_result0;
			}
		}
		else{
			uid = input_indegree->UID;
			in_degree = input_indegree->count;
			out_degree = 0;
			next_result1 = join_get_next_input_element (manager, 1, input_indegree);
			if (next_result1 == FAILURE){
				return next_result1;
			}
		}
	}

	diff = in_degree - out_degree;
	
	if (manager->current_output_heap_size < manager->output_heap_capacity) {
		// Insert when heap is not full
		result = join_insert_into_heap (manager, uid, in_degree, out_degree, diff);
		if (result != SUCCESS){
			return FAILURE;
		}
	} else {
		// If heap is full, insert only when min in heap is smaller than current diff.
		if (manager->output_heap[0].diff < diff){
			// Remove head of min heap.
			if (remove_top_heap_element(manager) != SUCCESS){
				return FAILURE;
			}
			// Add new element to min heap.
			result = join_insert_into_heap (manager, uid, in_degree, out_degree, diff);
			if (result != SUCCESS){
				return FAILURE;
			}
		}
	}
	
	return SUCCESS;
}


int join_insert_into_heap (JoinManager * manager, int uid, int in_degree, int out_degree, int diff){
	OutputHeapElement new_heap_element;
	int child, parent;
	
	new_heap_element.UID = uid;
	new_heap_element.in_degree = in_degree;
	new_heap_element.out_degree = out_degree;
	new_heap_element.diff = diff;
	
	if (manager->current_output_heap_size == manager->output_heap_capacity) {
		printf( "Unexpected ERROR: heap is full\n");
		return FAILURE;
	}
  	
	child = manager->current_output_heap_size++; /* the next available slot in the heap */
	
	while (child > 0) {
		parent = (child - 1) / 2;
		if (join_compare_heap_elements(&(manager->output_heap[parent]),&new_heap_element)>0) {
			manager->output_heap[child] = manager->output_heap[parent];
			child = parent;
		} 
		else 
			break;
	}
	
	manager->output_heap[child]= new_heap_element;

	return SUCCESS;
}


int remove_top_heap_element (JoinManager * manager){
	OutputHeapElement item;
	int child, parent;

	if(manager->current_output_heap_size == 0){
		printf( "UNEXPECTED ERROR: popping top element from an empty heap\n");
		return FAILURE;
	}

	//now we need to reorganize heap - keep the smallest on top
	item = manager->output_heap [--manager->current_output_heap_size]; // to be reinserted 

	parent =0;
	while ((child = (2 * parent) + 1) < manager->current_output_heap_size) {
		// if there are two children, compare them 
		if (child + 1 < manager->current_output_heap_size && 
				(join_compare_heap_elements(&(manager->output_heap[child]),&(manager->output_heap[child + 1]))>0)) 
			++child;
		
		// compare item with the larger 
		if (join_compare_heap_elements(&item, &(manager->output_heap[child]))>0) {
			manager->output_heap[parent] = manager->output_heap[child];
			parent = child;
		} 
		else 
			break;
	}
	manager->output_heap[parent] = item;
	
	return SUCCESS;
}


int join_compare_heap_elements (OutputHeapElement *a, OutputHeapElement *b) {
	if (a->diff > b->diff){
		return 1;
	}
	else if (a->diff == b->diff){
		return 0;
	}
	else{
		return -1;
	}
}


int get_degrees(int mem_size, int block_size){
	int i = get_degrees_by_column("records_sorted_uid1.dat", mem_size, block_size, 1);
	int j = get_degrees_by_column("records_sorted_uid2.dat", mem_size, block_size, 2);
	if (i == 0 && j == 0){
		return 0;
	}
	else{
		return -1;
	}
}


int get_degrees_by_column(char* file_name, int mem_size, int block_size, int column){
	FILE *fp_read, *fp_write;
	
	int records_per_block = block_size/sizeof(Record);
	int degrees_per_block = block_size/sizeof(Degree);
	
	int blocks_per_buffer = mem_size / block_size;
	int input_buffer_records_capacity = (blocks_per_buffer - 1) * records_per_block;
	
	/* allocate input_buffer for blocks_per_buffer-1 block */
    Record* input_buffer = (Record *) calloc (records_per_block * (blocks_per_buffer-1), sizeof(Record)) ;
	
	// Allocate output buffer for 1 block.
	Degree* output_buffer = (Degree *) calloc (degrees_per_block, sizeof(Degree)) ;
	
	int degrees_in_output_buffer = 0;
	
    if (!(fp_read = fopen ( file_name , "rb" ))){
		printf("CANNOT OPEN FILE %s\n", file_name);
		exit(1);
	}
	
	/*first get the size of opening file*/
    fseek(fp_read, 0, SEEK_END);
    int file_size = ftell(fp_read);
    fseek(fp_read, 0, SEEK_SET); 
    int total_following = file_size / sizeof(Record);
    int unread_records = total_following;
	
	// Open output file.
	char *output_file_name;
	if (column == 1){
		output_file_name = "out_degree.dat";
	}
	else{
		output_file_name = "in_degree.dat";
	}
	
	if (!(fp_write = fopen ( output_file_name , "wb" ))){
		printf("CANNOT OPEN FILE %s\n", output_file_name);
		exit(1);
	}
	
	/*compute the query*/
	int current_id = 0;
	int current_num = -1;
   
    /* read records into input_buffer */
    while (fread (input_buffer, sizeof(Record), input_buffer_records_capacity, fp_read) > 0){
		/*how many records left to read after reading one block*/
		int left_records = unread_records - input_buffer_records_capacity;
		int length;
		/*left data is greater than or equal to one block size*/
		if(left_records >= 0){
			unread_records = unread_records - input_buffer_records_capacity;
			length = input_buffer_records_capacity;
		}
		/*left data is smaller than one block size*/
		else{
			length = unread_records;
		}
		
		int i;
		for (i = 0; i < length; i++){
			if (column == 1){
				if (current_num == -1){
					/*initialization */
					current_num = 1;
					current_id = input_buffer[i].UID1;
				}
				else if(input_buffer[i].UID1 == current_id){
					current_num += 1;
				}
				else{
					output_buffer[degrees_in_output_buffer].UID = current_id;
					output_buffer[degrees_in_output_buffer].count = current_num;
					
					degrees_in_output_buffer++;
					
					if (degrees_in_output_buffer >= degrees_per_block){
						fwrite ( output_buffer, sizeof(Degree), degrees_in_output_buffer, fp_write);
						degrees_in_output_buffer = 0;
					}
					
					/*initialization */
					current_num = 1;
					current_id = input_buffer[i].UID1;
				}
			}
			else{
				if (current_num == -1){
					/*initialization */
					current_num = 1;
					current_id = input_buffer[i].UID2;
				}
				else if(input_buffer[i].UID2 == current_id){
					current_num += 1;
				}
				else{
					output_buffer[degrees_in_output_buffer].UID = current_id;
					output_buffer[degrees_in_output_buffer].count = current_num;
					
					degrees_in_output_buffer++;
					
					if (degrees_in_output_buffer >= degrees_per_block){
						fwrite ( output_buffer, sizeof(Degree), degrees_in_output_buffer, fp_write);
						degrees_in_output_buffer = 0;
					}
					
					/*initialization */
					current_num = 1;
					current_id = input_buffer[i].UID2;
				}
			}
		}
	}
	
	output_buffer[degrees_in_output_buffer].UID = current_id;
	output_buffer[degrees_in_output_buffer].count = current_num;
	degrees_in_output_buffer++;
	fwrite ( output_buffer, sizeof(Degree), degrees_in_output_buffer, fp_write);

	fclose(fp_read);
	free (input_buffer);
	free (output_buffer);
	fclose(fp_write);
	
	return 0;
}