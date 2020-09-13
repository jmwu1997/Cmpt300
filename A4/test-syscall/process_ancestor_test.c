#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include "process_ancestors.h"

#define _PROCESS_ANCESTOR 342// define syscall number

int main(int argc, char *argv[])
{
        //inital 
	struct process_info *process = malloc(sizeof(struct process_info)*4);

        //struct process_info *process = malloc(sizeof(struct process_info)*10);
   
	long process_size = 4; //there are only 4 process running on it 
	long val;
        printf("\nDiving to kernel level again for process_stats\n\n");
	//call syscall
	int ret = syscall(_PROCESS_ANCESTOR, process, process_size, &val); 
        
	//print infos
	for(int i = 0; i < process_size; i++){
			printf("process = %s\n", process[i].name);
			printf("pid = %li\n", process[i].pid);
			printf("uid = %li\n", process[i].uid);
                        printf("state = %li\n \n", process[i].state);
			printf("voluntary context switch = %li\n", process[i].nvcsw);
			printf("involuntary context switch = %li\n", process[i].nivcsw);
			printf("num of children = %li\n", process[i].num_children);
			printf("num of silbings = %li\n", process[i].num_siblings);
			
			
	
	}
	printf("Rising to user level w/ process return = %d\n\n", ret);
	
	return 0;
	}


