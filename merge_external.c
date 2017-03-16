#include "merge.h"

//manager fields should be already initialized in the caller
int merge_runs (MergeManager * merger, int num_trunks, char *input_prefix, int buffer_capacity, int column){	
	int  result; //stores SUCCESS/FAILURE returned at the end	
	
	//1. go in the loop through all input files and fill-in initial buffers
	if (init_merge (merger, num_trunks, input_prefix, buffer_capacity, column)!=SUCCESS){
		printf("INIT_MERGE FAILED\n");
		return FAILURE;
	}
	while (merger->current_heap_size > 0) { //heap is not empty
		HeapElement smallest;
		Record next; //here next comes from input buffer
		
		if(get_top_heap_element (merger, &smallest)!=SUCCESS)
			return FAILURE;

		result = get_next_input_element (merger, smallest.run_id, &next);
		
		if (result==FAILURE)
			return FAILURE;

		if(result==SUCCESS) {//next element exists, may also return EMPTY
			if(insert_into_heap (merger, smallest.run_id, &next)!=SUCCESS)
				return FAILURE;
		}


		merger->output_buffer [merger->current_output_buffer_position].UID1=smallest.UID1;
		merger->output_buffer [merger->current_output_buffer_position].UID2=smallest.UID2;
		
		merger->current_output_buffer_position++;

        //staying on the last slot of the output buffer - next will cause overflow
		if(merger->current_output_buffer_position == merger-> output_buffer_capacity ) {
			if(flush_output_buffer(merger)!=SUCCESS) {
				return FAILURE;			
				merger->current_output_buffer_position=0;
			}	
		}
	}

	
	//flush what remains in output buffer
	if(merger->current_output_buffer_position > 0) {
		if(flush_output_buffer(merger)!=SUCCESS)
			return FAILURE;
	}
	
	clean_up(merger);
	return SUCCESS;	
}


void print_heap(MergeManager * manager){
	printf("heap: ");
	for (int i = 0; i < manager->current_heap_size; i++){
		printf("[%d](%d, %d) ", manager->heap[i].run_id, manager->heap[i].UID1, manager->heap[i].UID2);
	}
	printf("\n");
}


void print_buffers(MergeManager * manager){
	for (int i = 0; i < manager->heap_capacity; i++){
		printf("input %d: ", i);
		for (int j = 0; j < manager->total_input_buffer_elements[i]; j++){
			if (j == manager->current_input_buffer_positions[i]){
				printf("->");
			}
			printf("(%d, %d) ", manager->input_buffers[i][j].UID1, manager->input_buffers[i][j].UID2);
		}
		printf("\n");
	}
	printf("output: ");
	for (int i = 0; i < manager->output_buffer_capacity; i++){
		if (i == manager->current_output_buffer_position){
			printf("->");
		}
		printf("(%d, %d) ", manager->output_buffer[i].UID1, manager->output_buffer[i].UID2);
	}
	printf("\n------------------------------\n");
}


int get_top_heap_element (MergeManager * merger, HeapElement * result){
	HeapElement item;
	int child, parent;

	if(merger->current_heap_size == 0){
		printf( "UNEXPECTED ERROR: popping top element from an empty heap\n");
		return FAILURE;
	}

	*result=merger->heap[0];  //to be returned

	//now we need to reorganize heap - keep the smallest on top
	item = merger->heap [--merger->current_heap_size]; // to be reinserted 

	parent =0;
	while ((child = (2 * parent) + 1) < merger->current_heap_size) {
		// if there are two children, compare them 
		if (child + 1 < merger->current_heap_size && 
				(compare_heap_elements(&(merger->heap[child]),&(merger->heap[child + 1]), merger->column)>0)) 
			++child;
		
		// compare item with the larger 
		if (compare_heap_elements(&item, &(merger->heap[child]), merger->column)>0) {
			merger->heap[parent] = merger->heap[child];
			parent = child;
		} 
		else 
			break;
	}
	merger->heap[parent] = item;
	
	return SUCCESS;
}

int insert_into_heap (MergeManager * merger, int run_id, Record *input){

	HeapElement new_heap_element;
	int child, parent;
	
	new_heap_element.UID1 = input->UID1;
	new_heap_element.UID2 = input->UID2;
	new_heap_element.run_id = run_id;
	
	if (merger->current_heap_size == merger->heap_capacity) {
		printf( "Unexpected ERROR: heap is full\n");
		return FAILURE;
	}
  	
	child = merger->current_heap_size++; /* the next available slot in the heap */
	
	while (child > 0) {
		parent = (child - 1) / 2;
		if (compare_heap_elements(&(merger->heap[parent]),&new_heap_element, merger->column)>0) {
			merger->heap[child] = merger->heap[parent];
			child = parent;
		} 
		else 
			break;
	}
	
	merger->heap[child]= new_heap_element;

	return SUCCESS;
}


/*
** TO IMPLEMENT
*/

int init_merge (MergeManager * manager, int num_trunks, char *input_prefix, int buffer_capacity, int column) {
	
	strcpy(manager->input_prefix, input_prefix);
	manager->column = column;
	manager->input_buffer_capacity = buffer_capacity;
	manager->output_buffer_capacity = buffer_capacity;
	manager->heap_capacity = num_trunks;
	manager->current_heap_size = 0;
	
	manager->heap = (HeapElement *) calloc (num_trunks, sizeof (HeapElement));
	
	manager->input_file_numbers = (int *) calloc (num_trunks, sizeof (int));
	for (int i = 0; i < num_trunks; i++){
		manager->input_file_numbers[i] = i;
	}
	
	manager->output_buffer = (Record *) calloc (buffer_capacity, sizeof (Record));
	
	manager->current_output_buffer_position = 0;
	
	manager->input_buffers = (Record **) calloc (num_trunks, sizeof (Record *));
	for (int i = 0; i < num_trunks; i++) { 
		manager->input_buffers[i] = (Record *) calloc (buffer_capacity, sizeof (Record));
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
		manager->file_capacity[i] = get_number_records_in_file(manager, i);
	}
	
	manager->total_input_buffer_elements = calloc (num_trunks, sizeof (int));
	for (int i = 0; i < num_trunks; i++){
		manager->total_input_buffer_elements[i] = 0;
	}
	
	// Fill buffer and insert_into_heap
	for (int i = 0; i < num_trunks; i++){
		refill_buffer(manager, i);
		
		Record next; //here next comes from input buffer

		int result = get_next_input_element (manager, i, &next);
		
		if (result==FAILURE){
			printf("GET_NEXT_INPUT_ELEMENT FAILURE: Fail to get record from run %d\n", i);
			return FAILURE;
		}
		if(result==SUCCESS) {//next element exists, may also return EMPTY
			if(insert_into_heap (manager, i, &next)!=SUCCESS)
				return FAILURE;
		}
	}
	
	if (column == 1){
		strcpy(manager->output_file_name, "records_sorted_uid1.dat");
	}
	else{
		strcpy(manager->output_file_name, "records_sorted_uid2.dat");
	}
	
	return SUCCESS;
}


int get_number_records_in_file(MergeManager * manager, int file_number){
	char filename[MAX_PATH_LENGTH];
	get_file_name(filename, manager, file_number);
	FILE *fp_read;
	fp_read = fopen(filename, "r");
	if (!fp_read){
		printf ("GET_NUMBER_RECORDS_IN_FILE ERROR: Could not open file \"%s\" for reading \n", filename);
		return FAILURE;
	}
	
	fseek(fp_read, 0, SEEK_END);
    int file_size = ftell(fp_read);
	int total_records = file_size / sizeof(Record);
    fseek(fp_read, 0, SEEK_SET); 
	
	fclose(fp_read);
	return total_records;
}


void get_file_name(char* input_file_name, MergeManager * manager, int file_number){
	char file_number_str[file_number/10+2];
	
	strcpy(input_file_name, manager->input_prefix);
	sprintf(file_number_str,"%d",file_number);
	
	strcat(input_file_name, file_number_str);
	strcat(input_file_name, ".dat");
}

int flush_output_buffer (MergeManager * manager) {
	manager->outputFP = fopen ( manager->output_file_name , "a" );
	if (!manager->outputFP){
		printf ("Could not open file \"%s\" for writing \n", manager->output_file_name);
		return FAILURE;
	}
	fwrite ( manager->output_buffer, sizeof(Record), manager->current_output_buffer_position, manager->outputFP);
	fclose(manager->outputFP);
	
	manager->current_output_buffer_position = 0;
	return SUCCESS;
}

int get_next_input_element(MergeManager * manager, int file_number, Record *result) {
	
	if (manager->current_input_buffer_positions[file_number] == manager->total_input_buffer_elements[file_number]){
		if (manager->total_input_buffer_elements[file_number] < manager->input_buffer_capacity){
			return EMPTY;
		}
		else{	//REFILL
			int function_result = refill_buffer(manager, file_number);
			if (function_result != SUCCESS){
				return function_result;
			}
		}
	}
	
	*result = manager->input_buffers[file_number][manager->current_input_buffer_positions[file_number]];
	manager->current_input_buffer_positions[file_number]++;
	
	return SUCCESS;
}

int refill_buffer (MergeManager * manager, int file_number) {
	
	if (manager->current_input_file_positions[file_number] == -1){
		return EMPTY;
	}
	
	char input_file_name[MAX_PATH_LENGTH];
	get_file_name(input_file_name, manager, file_number);
	
	manager->inputFP = fopen(input_file_name, "r");
	if (!manager->inputFP){
		printf ("REFILL_BUFFER ERROR: Could not open file \"%s\" for reading \n", input_file_name);
		return FAILURE;
	}
	
	fseek(manager->inputFP, sizeof(Record) * manager->current_input_file_positions[file_number], SEEK_SET);
	int records_left = manager->file_capacity[file_number] - manager->current_input_file_positions[file_number];

	int records_to_read;
	if (records_left > manager->input_buffer_capacity){
		records_to_read = manager->input_buffer_capacity;
		manager->current_input_file_positions[file_number] += records_to_read;
	}
	else{
		records_to_read = records_left;
		manager->current_input_file_positions[file_number] = -1;
	}
	int read_result = fread(manager->input_buffers[file_number], sizeof(Record), records_to_read, manager->inputFP);
	if (read_result <= 0){
		printf ("REFILL_BUFFER ERROR: Could not read file \"%s\".\n", input_file_name);
		printf("Records to read = %d, records left = %d, input_buffer_capacity = %d\n", records_to_read, records_left, manager->input_buffer_capacity);
		return FAILURE;
	}
	
	manager->total_input_buffer_elements[file_number] = records_to_read;
	manager->current_input_buffer_positions[file_number] = 0;
	
	fclose(manager->inputFP);
	
	return SUCCESS;
}

void clean_up (MergeManager * merger) {
	free(merger->heap);
	free(merger->input_file_numbers);
	free(merger->output_buffer);
	for (int i = 0; i < merger->heap_capacity; i++) { 
		free(merger->input_buffers[i]);
	}
	free(merger->input_buffers);
	free(merger->current_input_file_positions);
	free(merger->file_capacity);
}

int compare_heap_elements (HeapElement *a, HeapElement *b, int column) {
	if (column == 1){
		if (a->UID1 > b->UID1){
			return 1;
		}
		else if (a->UID1 == b->UID1){
			if (a->UID2 > b->UID2){
				return 1;
			}
			else if (a->UID2 == b->UID2){
				return 0;
			}
			else{
				return -1;
			}
		}
		else{
			return -1;
		}
	}
	else{
		if (a->UID2 > b->UID2){
			return 1;
		}
		else if (a->UID2 == b->UID2){
			if (a->UID1 > b->UID1){
				return 1;
			}
			else if (a->UID1 == b->UID1){
				return 0;
			}
			else{
				return -1;
			}
		}
		else{
			return -1;
		}
	}
}