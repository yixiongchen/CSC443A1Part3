#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/timeb.h>
#include "merge.h"

/**
* Compares two records a and b 
* with respect to the value of the integer field f.
* Returns an integer which indicates relative order: 
* positive: record a > record b
* negative: record a < record b
* zero: equal records
*/
int compare_uid2 (const void *a, const void *b) {
	Record one = *(Record*)a;
	Record two = *(Record*)b;
	int a_f = one.UID2;
	int b_f = two.UID2;
	return (a_f - b_f);
}


int compare_uid1 (const void *a, const void *b) {
	Record one = *(Record*)a;
	Record two = *(Record*)b;
	int a_f = one.UID1;
	int b_f = two.UID1;
	return (a_f - b_f);
}


void uid1_sort(Record * buffer, int total_records){
	/**
	* Arguments:
	* 1 - an array to sort
	* 2 - size of an array
	* 3 - size of each array element
	* 4 - function to compare two elements of the array
	*/
	
	qsort(buffer, total_records, sizeof(Record), compare_uid1);
}




void uid2_sort(Record * buffer, int total_records){
	/**
	* Arguments:
	* 1 - an array to sort
	* 2 - size of an array
	* 3 - size of each array element
	* 4 - function to compare two elements of the array
	*/
	
	qsort(buffer, total_records, sizeof(Record), compare_uid2);
}


// Print all the records in order in the buffer.
void print_records(Record * buffer, int total_records){
	for (int i = 0; i < total_records; i++){
		printf("uid2 = %d, uid1 = %d\n", buffer[i].UID2, buffer[i].UID1);
	}
}