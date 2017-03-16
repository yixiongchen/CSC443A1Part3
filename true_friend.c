#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/timeb.h>
#include "merge.h"


int success = 1 ;
int unsuccess = -1 ;


/*
 use merge sort to compute query
 200MB, 98MB(49 blocks input buffer) , 4MB (2 blocks  output buffer)
*/
void merge_sort(char* file_uid1, char* file_uid2){
	FILE *fp_read_uid1, *fp_read_uid2, *fp_write;
	int S_pointer;
	int current_uid1, current_uid2;
	int position_1, position_2;
    int MB = 1024*1024;
	int block_size =2*MB;
	int mem_size = 200*MB;
	int capacity_output_buffer;
	int	restart_point_uid2;
	int count;
   
    //each input buffer contain 49 blocks (98MB)
	int num_of_blocks =  (mem_size / block_size / 2) - 1;
	int num_records_chunk = num_of_blocks * block_size / sizeof(Record);
    
    // allocate input buffers for sort_uid1 and sort_uid2
	Record * buffer_uid1 = (Record *) calloc (num_records_chunk, sizeof (Record));
	Record * buffer_uid2 = (Record *) calloc (num_records_chunk, sizeof (Record));

    // allocate ouput buffer
    int output_num_records = 1*block_size / sizeof (Record);
    Record * output = (Record *) calloc (output_num_records, sizeof (Record));
    //ouput buffer is empty for initialization
    capacity_output_buffer = 0;

    printf("Each input buffer size:  %d\n",num_records_chunk);
    printf("Output buffer size:  %d\n", output_num_records);

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


    //initialization: first read relations into main memory
    int result_1 = fread(buffer_uid1, sizeof(Record), num_records_chunk, fp_read_uid1);
    int result_2 = fread(buffer_uid2, sizeof(Record), num_records_chunk, fp_read_uid2);
    S_pointer = 0;
    position_1 = 0;
    position_2 = 0;
    current_uid1 = buffer_uid1[0].UID1;
    current_uid2 = buffer_uid2[0].UID2;
    restart_point_uid2 = S_pointer;
    count = 0;

    //iteration
    while(1){
    	//keep track of position in R and S in main memory
    	if(position_1== result_1){
    		result_1 = fread(buffer_uid1, sizeof(Record), num_records_chunk, fp_read_uid1);
    		if(result_1 > 0){
    			printf("new load for R\n");
    			position_1 = 0;
    		}
    		else{
    			fwrite(output, sizeof(Record), capacity_output_buffer, fp_write);
    			break;
    		}
    	}
    	if(position_2 == result_2){
    		result_2 = fread(buffer_uid2, sizeof(Record), num_records_chunk, fp_read_uid2);
    		if(result_2 > 0){
    		    printf("new load for S\n");
    			position_2 = 0;

    		}
    		else{
    			fwrite(output, sizeof(Record), capacity_output_buffer, fp_write);
    			break;
    		}
    	}
        //printf("current_uid1 is: %d\n", current_uid1);
        //printf("current_uid2 is: %d\n", current_uid2);

        //printf("R: uid1=%d uid2=%d\n",buffer_uid1[position_1].UID1, buffer_uid1[position_1].UID2);
        //printf("S: uid1=%d,uid2=%d\n",buffer_uid2[position_2].UID1, buffer_uid2[position_2].UID2);
        // when first attribute of tuple are equal
    	if(current_uid1 == current_uid2){

	    	int query_result = compare_two_uids(buffer_uid1[position_1], buffer_uid2[position_2]);
	    	
	    	// when their second attribute are equal
	    	if(query_result == success){

                //remove duplicate
	    		if(buffer_uid1[position_1].UID1 > buffer_uid1[position_1].UID2){
	    			position_1 += 1;
	    			current_uid1 = buffer_uid1[position_1].UID1;
	    		} 
	    		//get the tuple	
                else{
		            //printf("Success");
		            // if output buffer is full, then write output buffer into disk
		    		if(capacity_output_buffer == output_num_records ){
		    			print("push outbuffer to disk\n");
		    			fwrite(output, sizeof(Record), capacity_output_buffer, fp_write);
		    			capacity_output_buffer = 0;
		    		}
		    		//printf("Output tuple uid1=%d uid2=%d\n", buffer_uid1[position_1].UID1, buffer_uid1[position_1].UID2);
		    		//store the tuple in output buffer
		    		count += 1;
		    		memcpy(&output[capacity_output_buffer], &buffer_uid1[position_1], sizeof(Record));
		    		//printf("Output buffer has: uid1=%d uid2=%d\n",output[capacity_output_buffer].UID1,output[capacity_output_buffer].UID2);
		    		capacity_output_buffer++;

		    		position_1 += 1;
		    		// if next tuple in R has same first attribute with preivious tuple
		    		if(buffer_uid1[position_1].UID1 == current_uid1){
		    			//reset S's position to read
		    			position_2 = 0;
		    			S_pointer = restart_point_uid2;
		    			//set read buffer back to restart_point_uid2
		    			//printf("restart S");
		    			fseek(fp_read_uid2, restart_point_uid2 * sizeof(Record), SEEK_SET);
		    			//read new buffer from S
		    			result_2 = fread(buffer_uid2, sizeof(Record), num_records_chunk, fp_read_uid2);
		    			current_uid1 = buffer_uid1[position_1].UID1;
		    			current_uid2 = buffer_uid2[position_2].UID2; 
		    		}
		    		else{	
		    			position_2 += 1;
		    		    S_pointer += 1;
		    			current_uid1 = buffer_uid1[position_1].UID1;
		    			current_uid2 = buffer_uid2[position_2].UID2;
		    		}

		    	
		    	}

	    	}
	    	//when their second attribute are not equal
	    	if(query_result == unsuccess){
	    		//printf("unsuccess\n");
	    		position_2++;
	    		S_pointer += 1;
	    		// when S's loop done
	    		if(current_uid2 != buffer_uid2[position_2].UID2){
	    			position_1 += 1;
	    			//if next R's tuple has same first attributes as the preivous tuple
	    			if(buffer_uid1[position_1].UID1 == current_uid1){
		    			//set read buffer back to restart_point_uid2
		    			position_2 = 0;
		    			S_pointer =restart_point_uid2 ;
		    			//printf("restart point is:%d\n", restart_point_uid2);
		    			fseek(fp_read_uid2, restart_point_uid2 * sizeof(Record), SEEK_SET);
		    			//read new buffer from S
		    			result_2 = fread(buffer_uid2, sizeof(Record), num_records_chunk, fp_read_uid2);
		    			current_uid1 = buffer_uid1[position_1].UID1;
		    			current_uid2 = buffer_uid2[position_2].UID2;	
		    		}
		    		else{
		    			current_uid2 = buffer_uid2[position_2].UID2;
		    			current_uid1 = buffer_uid1[position_1].UID1;
		    		}
	    			
	    		}
	    		// keep looping on S
	    		else{
	    			current_uid2 = buffer_uid2[position_2].UID2;	
	    		}	
	    	}
	    	//printf("\n");    	 	
    	}

        // when R's first attribute less than S' first attribute
    	else if(current_uid1 < current_uid2){
    		//printf("R increment\n");
    		position_1 += 1;
    		current_uid1 = buffer_uid1[position_1].UID1;
    		//printf("\n");
    	}

    	//when S's first attribute greater than S' first attribute
    	else{
    		//printf("S increment\n");
    		position_2 += 1;
    		S_pointer += 1;
    		if(current_uid2 != buffer_uid2[position_2].UID2){
    			restart_point_uid2 = S_pointer;
	    	}
    		current_uid2 = buffer_uid2[position_2].UID2;
    		//printf("\n");
    	}

    }
    free(buffer_uid1);
    free(buffer_uid2);
    free(output);
    fclose(fp_read_uid1);
    fclose(fp_read_uid1);
    fclose(fp_write);  
    printf("total number is:%d\n", count);	

}


/*
  When their first attribute is equal, compare their second attribute 
*/
int compare_two_uids(Record buffer1, Record buffer2){

	if(buffer1.UID2 == buffer2.UID1){
			return success;
	}

	else{
		return unsuccess;
	}

}


int main (int argc, char **argv) {
	//process and validate command-line arguments
	char* file1 = "records_sorted_uid1.dat";
    char* file2 = "records_sorted_uid2.dat";
	char *input_file = argv[1];
	int mem_size = 1024*1024*200;
    int block_size = 1024*1024*2;
    printf("begin sort uid2\n");
    sort_uid2(input_file, mem_size, block_size);
    printf("begin sort uid1\n");
    sort_uid1(input_file, mem_size, block_size);

 

    merge_sort(file1, file2);

}