#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "merge.h"
#include <sys/timeb.h>


/*
  read the entire binary file into a large in-memory buffer, 
  and perform the same query
 */
void read_ram_seq(char *filename){
    FILE *fp_read;
    int most_follow_id = 0;
    int max_num = 0;
    int current_id = 0;
    int current_num = 0;
    int unique_ids = 0;

    struct timeb t_begin, t_end;
    long time_spent_ms;

    /* Open an existing binary file for reading a record at a time. */
    if ((fp_read = fopen (filename, "rb, type=record" ) ) == NULL )
    {
        printf ( "Cannot open file\n" );
        exit (1);
    }
    
    /* determine the file size */
    fseek(fp_read, 0, SEEK_END);
    long file_size = ftell(fp_read);
    fseek(fp_read, 0, SEEK_SET); 
    long total_records = file_size / sizeof(Record);

    // proceed with allocating memory and reading the file
    Record* buffer = (Record *) calloc (total_records, sizeof(Record)) ;

    // Read records from the file to the buffer.
    long result = fread (buffer, sizeof(Record), total_records, fp_read);
    if (result == total_records){
	/*compute the query*/
	int i;
	ftime(&t_begin);
	for(i=0; i < total_records; i++){
		printf("UID1 = %d, UID2 = %d\n", buffer[i].UID1, buffer[i].UID2);
	    if(buffer[i].UID1 == current_id){
		current_num += 1;
		current_id = buffer[i].UID1;
	    }
	    else{
		if(current_num > max_num){
		    most_follow_id = current_id;
		    max_num = current_num;
		}
		/*initialization */
		current_num = 1;
		current_id = buffer[i].UID1;
		unique_ids++;
	    }
	}
	if(current_num > max_num){
	    most_follow_id = current_id;
	    max_num = current_num;
	}
	ftime(&t_end);

    }
    else{
    printf ("Fread Error");
    return;
    }
    
    free (buffer);
    fclose (fp_read);

    /* time elapsed in milliseconds */
    time_spent_ms = (long) (1000 *(t_end.time - t_begin.time)
       + (t_end.millitm - t_begin.millitm)); 
    
    long MB = 1024 * 1024;
    /* result in MB per second */
    printf ("Data rate: %.3f MBPS\n", ((total_records*sizeof(Record))/(float)time_spent_ms * 1000)/MB);

    float average = (float) total_records / unique_ids;
    printf("The userid has most follows is %d with Maximum number is %d\n", most_follow_id, max_num);
    printf("Avergae number is %f\n", average );
}



int main(int argc, char **argv) {
    char *filename = argv[1];
    read_ram_seq(filename);
    
    return 0;
}