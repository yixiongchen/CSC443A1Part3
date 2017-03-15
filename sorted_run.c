#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/timeb.h>
#include "merge.h"




int phase1(char* input_file, int mem_size, int block_size, char* output_prefix, int uid){

	FILE *fp_read, *fp_write;
	int total_records;
    int left_records;
    int num_records_chunk;


    // the number of records in one buffer read
    int num_of_blocks =  mem_size / block_size / 2;
	if (num_of_blocks < 1){
		printf("Memory size is too small.\n");
		exit(1);
	}
    num_records_chunk = num_of_blocks * block_size / sizeof(Record);
    

    // allocate a buffer in main
    Record * buffer = (Record *) calloc (num_records_chunk, sizeof (Record));

	if (!(fp_read= fopen (input_file , "r" ))) {
		printf ("Could not open file \"%s\" for reading \n", input_file);
		exit(1);
    }

    /*first get the size of opening file*/
    fseek(fp_read, 0, SEEK_END);
    int file_size = ftell(fp_read);
	total_records = file_size / sizeof(Record);

    fseek(fp_read, 0, SEEK_SET); 
  
    
    left_records = total_records;
    int load_records;
    int chunck_num = 0;
    while(left_records > 0){
        // read records into main memory and sort them
        load_records=fread(buffer, sizeof(Record), num_records_chunk, fp_read);
  
		if (load_records > 0){
			 //sort records in main memory
			if(uid == 1){
				uid1_sort(buffer, load_records);

			}
    	    if(uid == 2){
    	    	uid2_sort(buffer, load_records);
    	    }
    	    //write sorted list into disk
    	    
    	    char string[MAX_PATH_LENGTH];
			char file_number_str[chunck_num/10+2];
			
			strcpy(string, output_prefix);
			sprintf(file_number_str,"%d",chunck_num);
			
			strcat(string, file_number_str);
			strcat(string, ".dat");
    	    
    	    if (!(fp_write = fopen ( string , "wb" ))) {  
    	    	printf ("Could not open file \"sorted_list\" for writing \n");
    	    	exit(1);
    	    }
    	    fwrite (buffer, sizeof(Record), load_records, fp_write);
    	    fclose(fp_write);
			
			//decrement on the left_records
			left_records -= load_records;
			chunck_num ++;
		}
		else{
			printf ("Could not read file \"%s\".\n", input_file);
			exit(1);
		}
	}

	free(buffer);
	fclose(fp_read);

	return chunck_num;
}






