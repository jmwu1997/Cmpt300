#include "stats.h"
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <time.h>

//type struct
typedef struct stats
{
	int factory_num;
	int num_made;
	int num_eaten;
	double minDelay;
	double avgDelay;
	double maxDelay;
	double totalDelay;
} stats_t;

//global intialial
int totalcount = 0;
sem_t mutex;
stats_t* array;



void stats_init(int num_producers)
{
 	//initialize data storage 
	array = malloc(num_producers*sizeof(stats_t));
	for(int i = 0; i< num_producers; i++) {
		array[i].factory_num = i;
		array[i].num_made = 0;
		array[i].num_eaten = 0;
		array[i].minDelay = TMP_MAX;
		array[i].avgDelay = 0;
		array[i].maxDelay = 0;
		array[i].totalDelay = 0;
	}

	sem_init(&mutex, 0, 1);
	totalcount = num_producers;
}

void stats_cleanup(void){
	//free any dynamically allocated memory
	if(array) {free(array);}
}

void stats_record_produced(int factory_num)
{
	array[factory_num].num_made++;
}

void stats_record_consumed(int factory_num, double delay_in_ms){
	sem_wait(&mutex);

	// min delay
	if(array[factory_num].minDelay > delay_in_ms) {
		array[factory_num].minDelay = delay_in_ms;
	}
	// max delay
	if(array[factory_num].maxDelay < delay_in_ms) {
		array[factory_num].maxDelay = delay_in_ms;
	}
	// #Eaten, total delay, avg delay
	array[factory_num].num_eaten++;
	array[factory_num].totalDelay += delay_in_ms;
	array[factory_num].avgDelay = (array[factory_num].totalDelay)/(array[factory_num].num_eaten);
	
	

	sem_post(&mutex);
}

void stats_display(void){
    // title row
	printf("Statistics:\n%8s%8s%8s%14s%14s%14s\n", "Factory#", "#Made", "#Eaten", "Min Delay[ms]", "Avg Delay[ms]", "Max Delay[ms]");
	// data row
	for(int i=0; i<totalcount; i++) {
		if(array[i].num_made == array[i].num_eaten) {
			printf("%8d%8d%8d%14.5f%14.5f%14.5f\n",
				array[i].factory_num,
				array[i].num_made,
				array[i].num_eaten,
				array[i].minDelay,
				array[i].avgDelay,
				array[i].maxDelay);
		}
		//#Made and #Eaten columns don't match
		else {
			printf("ERROR: Mismatch between number made and eaten.\n");
		}
		
	}
}
