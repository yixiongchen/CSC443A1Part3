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
int compareUID2 (const void *a, const void *b) {
	Record one = *(Record*)a;
	Record two = *(Record*)b;
	int a_f = one.UID2;
	int b_f = two.UID2;
	return (a_f - b_f);
}


int compareUID1 (const void *a, const void *b) {
	Record one = *(Record*)a;
	Record two = *(Record*)b;
	int a_f = one.UID1;
	int b_f = two.UID1;
	return (a_f - b_f);
}


void sort(Record * buffer, int total_records, int column){
	/**
	* Arguments:
	* 1 - an array to sort
	* 2 - size of an array
	* 3 - size of each array element
	* 4 - function to compare two elements of the array
	*/
	
	if (column == 1){
		qsort(buffer, total_records, sizeof(Record), compareUID1);
	}
	else{
		qsort(buffer, total_records, sizeof(Record), compareUID2);
	}
}


// Print all the records in order in the buffer.
void print_records(Record * buffer, int total_records){
	for (int i = 0; i < total_records; i++){
		printf("uid2 = %d, uid1 = %d\n", buffer[i].UID2, buffer[i].UID1);
	}
}