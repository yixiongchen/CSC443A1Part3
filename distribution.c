#include "merge.h"

int main(int argc, char **argv){
	
	FILE *fp_read, *fp_write;
	
	char *file_name = argv[1];
    int block_size = atoi(argv[2]);
	int column_id = atoi(argv[3]);
	
	
	if (column_id == 2){
		int mem_size = 200 * 1024 * 1024;
		if (sort_uid2(file_name, mem_size, block_size) > 0){
			printf("ERROR IN SORTING BY UID2\n");
			exit(1);
		}
		file_name = "records_sorted.dat";
	}
	
	long max_degree = get_max_degree(file_name, block_size, column_id);
	
	long counts[max_degree];
	
	// Init counts;
	for (int i = 0; i < max_degree; i++){
		counts[i] = 0;
	}
	
	int records_per_block = block_size/sizeof(Record);
	
	/* allocate buffer for 1 block */
    Record* buffer = (Record *) calloc (records_per_block, sizeof(Record)) ;
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
	
	/*compute the query*/
	int current_id = 0;
	int current_num = 0;
   
    /* read records into buffer */
    while (fread (buffer, sizeof(Record), records_per_block, fp_read) > 0){
		/*how many records left to read after reading one block*/
		int left_records = unread_records - records_per_block;
		int length;
		/*left data is greater than or equal to one block size*/
		if(left_records >= 0){
			unread_records = unread_records - records_per_block;
			length = records_per_block;
		}
		/*left data is smaller than one block size*/
		else{
			length = unread_records;
		}
		
		int i;
		for (i = 0; i < length; i++){
			if (column_id == 1){
				if(buffer[i].UID1 == current_id){
					current_num += 1;
				}
				else{
					counts[current_num-1] += 1;
					
					/*initialization */
					current_num = 1;
					current_id = buffer[i].UID1;
				}
			}
			else{
				if(buffer[i].UID2 == current_id){
					current_num += 1;
				}
				else{
					counts[current_num-1] += 1;
					
					/*initialization */
					current_num = 1;
					current_id = buffer[i].UID2;
				}
			}
		}
	}
	
	counts[current_num-1] += 1;
	
	fclose(fp_read);
	free (buffer);
	
	// Write array counts to a txt file.
	char *output_file_name;
	if (column_id == 1){
		output_file_name = "out_degree.txt";
	}
	else{
		output_file_name = "in_degree.txt";
	}
	
	if (!(fp_write = fopen ( output_file_name , "w" ))){
		printf("CANNOT OPEN FILE %s\n", output_file_name);
		exit(1);
	}
	
	for (long i = 0; i < max_degree; i++){
		fprintf(fp_write, "%ld %ld\n", i+1, counts[i]);
	}
	
	fclose(fp_write);
	
	return 0;
}