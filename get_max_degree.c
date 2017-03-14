#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/timeb.h>
#include "merge.h"

long get_max_degree(char* file_name, int blocksize, int column_id){
	int block_size = blocksize;
    int records_per_block = block_size / sizeof(Record);
    FILE *fp_read;
    long max_num = 0;
    long current_id = 0;
    long current_num = 0;
    long unique_uids = 0;

    /* allocate buffer for 1 block */
    Record* buffer = (Record *) calloc (records_per_block, sizeof(Record)) ;
    if (!(fp_read = fopen ( file_name , "rb" ))){
    	exit(1);
    }
    
    /*first get the size of opening file*/
    fseek(fp_read, 0, SEEK_END);
    int file_size = ftell(fp_read);
    fseek(fp_read, 0, SEEK_SET); 
    int total_following = file_size / sizeof(Record);
    int unread_records = total_following;
   
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
	    /*compute the query*/
	    int i;
	    if(column_id == 2){
		    for (i = 0; i < length; i++){
		    	if(buffer[i].UID2 == current_id){
		    		current_num += 1;
		    	}
		        else{
		        	unique_uids++;
		        	if (current_num > max_num){
		        		max_num = current_num;
		        	}
		        	/*initialization */
		        	current_num = 1;
		        	current_id = buffer[i].UID2;
		        }
		    }
		    if (current_num > max_num){
		    	max_num = current_num;
		    }
		}

		if(column_id == 1){
			for (i = 0; i < length; i++){
		    	if(buffer[i].UID1 == current_id){
		    		current_num += 1;
		    	}
		        else{
		        	unique_uids++;
		        	if (current_num > max_num){
		        		max_num = current_num;
		        	}
		        	/*initialization */
		        	current_num = 1;
		        	current_id = buffer[i].UID1;
		        }
		    }
		    if (current_num > max_num){
		    	max_num = current_num;
		    }

		}

    }
    printf("max degree: %ld\n", max_num);

    fclose (fp_read);
    free (buffer);

    return max_num;

}