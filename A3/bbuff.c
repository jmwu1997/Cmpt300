#include "bbuff.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <semaphore.h>


//Initialize variable
sem_t mutex;
sem_t full;
sem_t empty;
void* bbuff[BUFFER_SIZE];
int slot_taken;
int slot_free=BUFFER_SIZE;
//Initialize buffer
void bbuff_init(void)
{
	//intialized full empty & mutex
	sem_init(&full, 0, 0);
	sem_init(&empty, 0, BUFFER_SIZE);
	sem_init(&mutex, 0, 1);

}

// Insert into buffer
void bbuff_blocking_insert(void* item)
{

	
	sem_wait(&empty);
	sem_wait(&mutex);

	//insertion
	bbuff[BUFFER_SIZE-slot_free] = item;
	// printf("slot_taken IS %d\n",slot_taken);
	slot_taken++;
	slot_free--;

	sem_post(&mutex);
	sem_post(&full);

	return;
}

void* bbuff_blocking_extract(void)
{
	
	sem_wait(&full);
	sem_wait(&mutex);
	
	//extraction
	slot_taken--;
	slot_free++;
	// printf("slot_taken is %d\n",slot_taken);
	void* item = bbuff[slot_taken];
    
	bbuff[slot_taken]=NULL;
	sem_post(&mutex);
	sem_post(&empty);

	return item;
}

_Bool bbuff_is_empty(void)
{
	//return when slots are empty; all extractions completed	
	return (slot_taken == 0 && slot_free == BUFFER_SIZE);
}