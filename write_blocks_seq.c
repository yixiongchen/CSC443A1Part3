#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/timeb.h>

#include "merge.h"


Record parseToRecord(char* str) {

    Record r;
    
    r.UID1 = atoi(strtok(str, ","));
    r.UID2 = atoi(strtok(NULL, ","));

    return r; 
}


int main(int argc, char **argv) {
    
    struct timeb t_begin, t_end;
    long time_spent_ms = 0;
    long total_records = 0;
  
    char *file_name = argv[1];
    int block_size = atoi(argv[2]);
    int records_per_block = block_size/sizeof(Record);
    
    char current_line[MAX_CHARS_PER_LINE];
    FILE *fp_read, *fp_write;
    Record * buffer = (Record *) calloc (records_per_block, sizeof (Record));
 

    /* open text file for reading */
    if (!(fp_read= fopen ( file_name , "r" ))) {
		printf ("Could not open file \"%s\" for reading \n", file_name);
		return (-1);
    }
    
    /* open dat file for writing */
    if (!(fp_write = fopen ( "records.dat" , "wb" ))) {
		printf ("Could not open file \"records.dat\" for reading \n");
		return (-1);
    }
    
    int records_in_buffer = 0;
    
    /* reading lines */
    while( fgets (current_line, MAX_CHARS_PER_LINE, fp_read)!=NULL ) {
	current_line [strcspn (current_line, "\r\n")] = '\0'; //remove end-of-line characters
	if (strlen(current_line) > 0){
	    Record r = parseToRecord(current_line);
	    if (records_in_buffer == records_per_block){
			ftime(&t_begin);
			fwrite ( buffer, sizeof(Record), records_in_buffer, fp_write);
			ftime(&t_end);
			records_in_buffer = 0;
			time_spent_ms += (long) (1000 *(t_end.time - t_begin.time)
				+ (t_end.millitm - t_begin.millitm)); 
			}
			ftime(&t_begin);
			buffer[records_in_buffer] = r;
			ftime(&t_end);
			time_spent_ms += (long) (1000 *(t_end.time - t_begin.time)
			+ (t_end.millitm - t_begin.millitm)); 
			records_in_buffer++;
			total_records++;
		}
    }
    
    if (records_in_buffer > 0){
	ftime(&t_begin);
	fwrite ( buffer, sizeof(Record), records_in_buffer, fp_write);
	ftime(&t_end);
	time_spent_ms += (long) (1000 *(t_end.time - t_begin.time)
	    + (t_end.millitm - t_begin.millitm)); 
    }
    
    fclose(fp_write);
    
    free(buffer);
    fclose(fp_read);
 
    /* result in MB per second */
    int MB = 1024 * 1024;
    printf ("block size: %d bytes, rate: %.3f MBPS\n", block_size, ((total_records*sizeof(Record))/(float)time_spent_ms * 1000)/MB);

    return(0);
}