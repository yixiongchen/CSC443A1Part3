#include "merge.h"
#include "celebrities.h"

int main(int argc, char **argv){
	char *filename = argv[1];
	int mem_size = atoi(argv[2]);
    int block_size = atoi(argv[3]);
	
	// Sort input file by uid1
	sort_uid(filename, mem_size, block_size, 1);
	// Sort input file by uid2
	sort_uid(filename, mem_size, block_size, 2);
	
	get_degrees(mem_size, block_size);
}


int get_celebrities(int mem_size, int block_size){
	int celebrities_id[10];
	int celebrities_count[10];
	
	FILE *fp_outdegree, *fp_indegree;
	
	if (!(fp_outdegree = fopen ( "out_degree.dat" , "rb" ))){
		printf("CANNOT OPEN FILE out_degree.dat\n");
		exit(1);
	}
	
	if (!(fp_indegree = fopen ( "in_degree.dat" , "rb" ))){
		printf("CANNOT OPEN FILE in_degree.dat\n");
		exit(1);
	}
	
	/*first get the size of opening file*/
    fseek(fp_outdegree, 0, SEEK_END);
    int outdegree_file_size = ftell(fp_outdegree);
    fseek(fp_outdegree, 0, SEEK_SET); 
    int total_outdegrees = outdegree_file_size / sizeof(Degree);
    int unread_outdegrees = total_outdegrees;
	
	fseek(fp_indegree, 0, SEEK_END);
    int indegree_file_size = ftell(fp_indegree);
    fseek(fp_indegree, 0, SEEK_SET); 
    int total_indegrees = indegree_file_size / sizeof(Degree);
    int unread_indegrees = total_indegrees;
	
	int blocks_per_buffer = mem_size / 2 / block_size;
	int degrees_per_block = block_size / sizeof(Degree);
	int degrees_per_buffer = degrees_per_block * blocks_per_buffer;
	
	Degree* outdegree_buffer = (Degree *) calloc (degrees_per_buffer, sizeof(Degree)) ;
	Degree* indegree_buffer = (Degree *) calloc (degrees_per_buffer, sizeof(Degree)) ;
	
	int read_degrees_in_outdegree_buffer = 0;
	int read_degrees_in_indegree_buffer = 0;
	
	fread (outdegree_buffer, sizeof(Degree), degrees_per_buffer, fp_read);
	fread (input_buffer, sizeof(Degree), degrees_per_buffer, fp_read);
	
	int left_outdegrees = unread_outdegrees - degrees_per_buffer;
	int outdegree_buffer_size;
	/*left data is greater than or equal to one block size*/
	if(left_outdegrees >= 0){
		unread_outdegrees = unread_outdegrees - degrees_per_buffer;
		outdegree_buffer_size = degrees_per_buffer;
	}
	/*left data is smaller than one block size*/
	else{
		outdegree_buffer_size = unread_outdegrees;
	}
	
	int left_indegrees = unread_indegrees - degrees_per_buffer;
	int indegree_buffer_size;
	/*left data is greater than or equal to one block size*/
	if(left_indegrees >= 0){
		unread_indegrees = unread_indegrees - degrees_per_buffer;
		indegree_buffer_size = degrees_per_buffer;
	}
	/*left data is smaller than one block size*/
	else{
		indegree_buffer_size = unread_indegrees;
	}
	
	while (outdegree_buffer_size - read_degrees_in_indegree_buffer > 0 || indegree_buffer_size - read_degrees_in_indegree_buffer){
		
	}
	
	fclose(fp_outdegree);
	fclose(fp_indegree);
	free(outdegree_buffer);
	free(indegree_buffer);
}


int loadBuffer(Degree *buffer, fp_read, unread_degrees){
	
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