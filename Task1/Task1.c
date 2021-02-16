#include <stdio.h>
#include <stdbool.h>
#include <string.h>

int main(){
	FILE *fp;
	char* filename = "Task1.txt";
	int count,c;
	fp = fopen(filename, "r");
	if(fp == NULL){
		printf("Could not open file %s",filename);	
	}
	do{
		
	}
	printf("test\n");
	return 0;
}
