#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/timeb.h>
#include "merge.h"


int success = 1;
int move_R = 2;
int move_S = 3;


/*
 use merge sort to compute query
 200MB, 98MB(49 blocks input buffer) , 4MB (2 blocks  output buffer)
*/
void merge_sort(char* file_uid1, char* file_uid2, int mem_size, int block_size){
	FILE *fp_read_uid1, *fp_read_uid2, *fp_write;
	int position_1, position_2;
	int capacity_output_buffer;
	int count;
   
    //each input buffer contain 49 blocks (98MB)
	int num_of_blocks =  (mem_size / block_size / 2) - 1;
	int num_records_chunk = num_of_blocks * block_size / sizeof(Record);
    
    // allocate input buffers for sort_uid1 and sort_uid2
	Record * buffer_uid1 = (Record *) calloc (num_records_chunk, sizeof (Record));
	Record * buffer_uid2 = (Record *) calloc (num_records_chunk, sizeof (Record));
   
    // allocate ouput buffer 4MB (2 blocks)
    int num_block_output = (mem_size / block_size)-(2*num_of_blocks);

    int output_num_records = num_block_output * block_size / sizeof (Record);
    Record * output = (Record *) calloc (output_num_records, sizeof (Record));
    //ouput buffer is empty for initialization
    capacity_output_buffer = 0;

  

    if (!(fp_read_uid1= fopen (file_uid1 , "r" ))) {
		printf ("Could not open file \"%s\" for reading \n", file_uid1);
		exit(1);
    }

    if (!(fp_read_uid2= fopen (file_uid2 , "r" ))) {
		printf ("Could not open file \"%s\" for reading \n", file_uid2);
		exit(1);
    }

    if (!(fp_write = fopen ("true_friends.dat" , "wb" ))) {  
    	printf ("Could not open file \"true_friends\" for writing \n");
        exit(1);
    }


    //initialization: first read R and S into main memory
    int result_1 = fread(buffer_uid1, sizeof(Record), num_records_chunk, fp_read_uid1);
    int result_2 = fread(buffer_uid2, sizeof(Record), num_records_chunk, fp_read_uid2);
    
    //keep track of position in R and S in main memory
    position_1 = 0;
    position_2 = 0; 
    count = 0;

    //iteration
    while(1){
    	//refill input buffer for R 
    	if(position_1== result_1){
    		result_1 = fread(buffer_uid1, sizeof(Record), num_records_chunk, fp_read_uid1);
    		if(result_1 > 0){
    			position_1 = 0;
    		}
    		else{
    			fwrite(output, sizeof(Record), capacity_output_buffer, fp_write);
    			break;
    		}
    	}
    	//refill input buffer for S
    	if(position_2 == result_2){
    		result_2 = fread(buffer_uid2, sizeof(Record), num_records_chunk, fp_read_uid2);
    		if(result_2 > 0){
    			position_2 = 0;

    		}
    		else{
    			fwrite(output, sizeof(Record), capacity_output_buffer, fp_write);
    			break;
    		}
    	}

        // when R.uid1 = S.uid2
    	if(buffer_uid1[position_1].UID1 == buffer_uid2[position_2].UID2){

	    	int query_result = compare_two_uids(buffer_uid1[position_1], buffer_uid2[position_2]);
	    
	    	//when R.uid2 = S.uid1
	    	if(query_result == success){
             
	    		if(buffer_uid1[position_1].UID1 < buffer_uid1[position_1].UID2){
	    		    // if output buffer is full, then write output buffer into disk
		    		if(capacity_output_buffer == output_num_records ){
		    			fwrite(output, sizeof(Record), capacity_output_buffer, fp_write);
		    			capacity_output_buffer = 0;
		    		}
	
		    		//save the tuple (R.uid1, R.uid2) into output buffer
		    		count += 1;
                    //printf("buffer_uid1: %d %d\n", buffer_uid1[position_1].UID1, buffer_uid1[position_1].UID2);
		    		memcpy(&output[capacity_output_buffer], &buffer_uid1[position_1], sizeof(Record));
		    		capacity_output_buffer++;
		    	}
		    	//increment on both pointer
		    	position_1 += 1;
	    		position_2 += 1;

	    	}
	    	//when R.uid2 > S.uid1
	    	else if(query_result == move_S){
	    		position_2++;
	    	}
	    	//when R.uid2 < S.uid1
	    	else{
	    		position_1++;
	    	} 	 	
    	}

        //when R.uid1 < S.uid2, increment on R' pointer
    	else if(buffer_uid1[position_1].UID1 < buffer_uid2[position_2].UID2){
    		position_1 += 1;
    	}

    	//when R.uid1 > S.uid2, increment on S' pointer
    	else{
    		position_2 += 1;
    	}

    }
    
    // free buffer
    free(buffer_uid1);
    free(buffer_uid2);
    free(output);
    fclose(fp_read_uid1);
    fclose(fp_read_uid1);
    fclose(fp_write);  
    printf("total number is:%d\n", count);	

}


/*
  When R.uid1 = S.uid2, Compare R.uid2 and S.uid1
*/
int compare_two_uids(Record buffer1, Record buffer2){

	if(buffer1.UID2 == buffer2.UID1){
		return success;
	}

	else if(buffer1.UID2 > buffer2.UID1){
		return move_S;
	}

	else{
		return move_R;
	}

}


int main (int argc, char **argv) {

	struct timeb t_begin, t_end;
    long time_spent_ms = 0;

    ftime(&t_begin);
	//process and validate command-line arguments
	char* sorted_uid1 = "records_sorted_uid1.dat";
    char* sroted_uid2 = "records_sorted_uid2.dat";
	char *input_file = argv[1];
    
    int MB = 1024*1024;
	int mem_size=atoi(argv[2]) * MB;
    int block_size= atoi(argv[3]) * MB;

    printf("begin sort uid2\n");
    sort_uid(input_file, mem_size, block_size, 2);
    printf("begin sort uid1\n");
    sort_uid(input_file, mem_size, block_size, 1);

    merge_sort(sorted_uid1, sroted_uid2, mem_size, block_size);
    ftime(&t_end);

    time_spent_ms = (long) (1000 *(t_end.time - t_begin.time)
       + (t_end.millitm - t_begin.millitm))/1000; 
    printf("Total time is:%lu\n", time_spent_ms);

}