#include "bbuff.h"
#include "stats.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <assert.h>
#include <unistd.h>
#include <stdbool.h>
#include <semaphore.h>


_Bool stop_thread = false;


typedef struct candy
{
	int factory_number;
	double time_stamp_in_ms;
} candy_t;

double current_time_in_ms(void)
{
	struct timespec now;
	clock_gettime(CLOCK_REALTIME, &now);
	return now.tv_sec * 1000.0 + now.tv_nsec/1000000.0;
}


void* factory_thread(void* argu){

	int factory_num = *((int *)argu);
	while(stop_thread==false){
		int time = rand()%4;
		printf("Factory %d ships candy & waits %ds \n", factory_num, time);
		candy_t* candy = malloc(sizeof(candy_t));
		candy -> factory_number = factory_num;
		candy -> time_stamp_in_ms = current_time_in_ms();
		bbuff_blocking_insert(candy);
		stats_record_produced(factory_num);
		sleep(time);
	}
		printf("Candy-factory %d done\n", factory_num);
		pthread_exit(NULL);
}

void* kid_thread(void* kid_list){
    int time;
    while(1){
        //extract a candy item
        candy_t* candy = (candy_t *) bbuff_blocking_extract();
        //printf("    eat %d, wait %ds\n", (intptr_t) kid_list, time);
        if(candy != NULL){
            stats_record_consumed(candy->factory_number, current_time_in_ms()- candy->time_stamp_in_ms);
        }
        //free candy
        free(candy);
        //sleep for 0 or 1 sec
        time = rand() % 2;
        sleep(time);

    }
    pthread_exit(NULL);
}

void launch_threads(pthread_t* thread, int val,void* (*thread_function)(void*)){
	int list[val];
	pthread_t temp;
	for (int i =0; i < val; i++){
		list[i]=i;
		if((pthread_create(&temp, NULL, thread_function, &list[i]))!=0){
			printf("Fail to create thread.");
			exit(-1);
		}
		thread[i]=temp;
	}


}

int main(int argc, char* argv[]){
	srand(time(NULL));
	int factories = atoi(argv[1]);
	int kids = atoi(argv[2]);
	int seconds= atoi(argv[3]);
// 1. Extract arguments
	for(int i=1; i<4; i++) {
		if(atoi(argv[i])<=0||argc != 4) {
		printf("Invalid input!\n");
		exit(-1);
		}
	}

// 2. Initialize modules
	bbuff_init();
	stats_init(factories);

// 3. Launch candy-factory threads
	pthread_t factories_thread[factories];
	launch_threads(factories_thread,factories,factory_thread);
	
// 4. Launch kid threads
	pthread_t kids_thread[kids];;
	launch_threads(kids_thread,kids,kid_thread);
 
// 5. Wait for requested time
	for(int i=1; i<=seconds; i++) {
		sleep(1);
		printf("Time: %ds\n", i);
	}
	
// 6. Stop candy-factory threads
	stop_thread = true;
	for(int i=0; i<factories; i++) {
		pthread_join(factories_thread[i], NULL);
	}

// 7. Wait until no more candy
	while(!bbuff_is_empty()) {
		printf("Waiting for all candy to be consumed.\n");
		sleep(1);
	}
	
// 8. Stop kid threads
	for(int i=0; i<kids; i++) {
		pthread_cancel(kids_thread[i]);
		pthread_join(kids_thread[i], NULL);
	}

//9. Display table
	stats_display();
//10. Cleanup any allocated memory
	stats_cleanup();
	free(factories_thread);
	free(kids_thread);
}
