#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/timeb.h>
#include "merge.h"

/*
 use merge sort to compute query
 200MB, 98MB(49 blocks input buffer) , 4MB (2 blocks  output buffer)
*/
int success = 10 ;
int insert_uid2 = 2;
int insert_uid1 = 1;



void merge_sort(char* file_uid1, char* file_uid2){
	FILE *fp_uid1, *fp_uid1, *fp_write;
	int uid1_pointer, ui2_pointer;
    int MB = 1024*1024;
	int block_size =2*MB;
	int mem_size = 200*MB;
	int capacity_ouput_buffer;
   

    //generate a compare buffer for a pair of edge
    Record *compare_uid = (Record *) calloc (2, sizeof (Record));
    
    //each input buffer contain 49 blocks (98MB)
	int num_of_blocks =  (mem_size / block_size / 2) - 1;
	int num_records_chunk = num_of_blocks * block_size / sizeof(Record);
    
    // allocate input buffers for sort_uid1 and sort_uid2
	Record * buffer_uid1 = (Record *) calloc (num_records_chunk, sizeof (Record));
	Record * buffer_uid2 = (Record *) calloc (num_records_chunk, sizeof (Record));

    
    // allocate ouput buffer
    output_num_records = 1*block_size / sizeof (Record);
    Record * output = (Record *) calloc (output_num_records, sizeof (Record));
    //ouput buffer is empty for initialization
    capacity_ouput_buffer = 0;

    if (!(fp_read_uid1= fopen (file_uid1 , "r" ))) {
		printf ("Could not open file \"%s\" for reading \n", input_file);
		exit(1);
    }

    if (!(fp_read_uid2= fopen (file_uid2 , "r" ))) {
		printf ("Could not open file \"%s\" for reading \n", input_file);
		exit(1);
    }

    if (!(fp_write = fopen ("true_friends.dat" , "wb" ))) {  
    	printf ("Could not open file \"true_friends\" for writing \n");
        exit(1);
    }

    int current_compare_uid1, current_compare_uid2;
    uid1_pointer = 0;
    ui2_pointer  =0 ;
    while（result_uid1=fread(buffer_uid1, sizeof(Record), num_records_chunk, fp_read_uid1) &&
    	result_uid1=fread(buffer_uid1, sizeof(Record), num_records_chunk, fp_read_uid1){

    		//initilization for imtermidiate buffer
    		if(uid1_pointer == 0){
    			memcpy(compare_uid[0], buffer_uid1[0], sizeof(Record))
    		}
    		if(uid2_pointer == 0){
    			memcpy(compare_uid[1], buffer_uid1[0], sizeof(Record))
    		}
    		
    		while(1){

    			if(uid2_pointer)

    			next_move = compare_uid(buffer_uid1);

    			if(next_move == sucess){

    				//refill compare buffer
    				uid2_pointer++;
    				uid1_pointer++ 
    				memcpy(compare_uid[0], buffer_uid1[uid1_pointer], sizeof(Record));
    				memcpy(compare_uid[1], buffer_uid2[uid2_pointer], sizeof(Record));

    				//save buffer_uid1[0] in output buffer
    				if(capacity_ouput_buffer == output_num_records){
    					//write them to disk
    					fwrite (output, sizeof(Record), output_num_records, fp_write);
    					//fclose(fp_write);
    					capacity_ouput_buffer = 0;
    				}
    				memcpy(output[capacity_ouput_buffer], buffer_uid1[0], sizeof(Record));
    				capacity_ouput_buffer ++;
    			}
    			
    			if(next_move = insert_uid2){
    				uid2_pointer++;
    				memcpy(compare_uid[1], buffer_uid2[uid2_pointer], sizeof(Record));
    			}
    			if(next_move = insert_uid1){
    				uid1_pointer++;
    				memcpy(compare_uid[0], buffer_uid1[uid1_pointer], sizeof(Record));
    			}


    		}


    	}
}


int compare_two_uids(Record *buffer){

	if(buffer[0]->UID1 == buffer[1]->UID2){
		if(buffer[0]->UID2 > buffer[1]->UID1){
			return Sucess;
		}
		if(buffer[0]->UID2 > buffer[1]->UID1){
			//insert new edge into buffer[1] 
			return insert_uid2
		}
		if(buffer[0]->UID2 < buffer[1]->UID1){
			//insert new edge into buffer[0] 
			return insert_uid1;
		}
	}
	if(buffer[0]->UID1 > buffer[1]->UID2){
		//insert new edge into buffer[1] 
		return insert_uid2;
	}

	
	//insert new edge into buffer[0] 
	return insert_uid1;

	}
}


int main (int argc, char **argv) {
	//process and validate command-line arguments

	char *input_file = argv[1];
	int mem_size = 1024*1024*200;
    int block_size = 1024*1024*2;
    printf("begin sort uid2\n");
    sort_uid2(input_file, mem_size, block_size);
    printf("begin sort uid1\n");
    sort_uid1(input_file, mem_size, block_size);
}