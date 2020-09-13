#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "myalloc.h"
#include "list.h"

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

struct Myalloc {
    enum allocation_algorithm aalgorithm;
    int size;
    void* memory;
    // Some other data members you want, 
    // such as lists to record allocated/free memory
    struct nodeStruct *allocated;
    struct nodeStruct *unallocated;
};

struct Myalloc myalloc;

void initialize_allocator(int _size, enum allocation_algorithm _aalgorithm) {
    
    if (_size < 0){
        printf("Failed to allocate 0");
    }
    assert(_size > 0);
    myalloc.aalgorithm = _aalgorithm;
    myalloc.size = _size;
    myalloc.memory = malloc((size_t)myalloc.size);
    myalloc.allocated = NULL;
    myalloc.unallocated = NULL;
  
    //insert head to a list
    struct nodeStruct *head = List_createNode(_size, myalloc.memory);
    List_insertHead(&myalloc.unallocated,head);
    

}

void destroy_allocator() {
	 pthread_mutex_lock(&m);
    
    if (myalloc.memory!=NULL){
        free(myalloc.memory);
    }
    while (myalloc.allocated) {
        List_deleteNode(&myalloc.allocated, myalloc.allocated);
    }
    while (myalloc.unallocated) {
        List_deleteNode(&myalloc.unallocated, myalloc.unallocated);
    }
     pthread_mutex_unlock(&m);
}

void* first_fit(int size){
	struct nodeStruct *curr = myalloc.unallocated;
    struct nodeStruct *temp = NULL;
        while (curr) {
            if (curr->size >= size) {
                temp = curr;
            }
            curr = curr->next;
        }
    return temp;
}

void* best_fit(int size){
	struct nodeStruct *temp = myalloc.unallocated;
    struct nodeStruct *current = NULL;
    int min = myalloc.size;
  
    while (temp && temp->size >= size) { 
        if (min > temp->size) 
        	min = temp->size;
            current = temp;
        temp = temp->next; 
    } 
    return current;
}

void* worst_fit(int size){
	struct nodeStruct *temp = myalloc.unallocated;
    struct nodeStruct *current = NULL;
    int max = myalloc.size;
  
    while (temp && temp->size >= size) { 
        if (max < temp->size) 
        	max = temp->size;
            current = temp;
        temp = temp->next; 
    } 

    return current;
}


void* allocate(int _size) {
	pthread_mutex_lock(&m);
     //base case.
    if (_size <= 0) {
        return NULL;
    }
    if (myalloc.unallocated==NULL){
    	return NULL;
    }
    // size + header size 8 bytes
    int size = _size +8;
    struct nodeStruct *current;

    if(myalloc.aalgorithm == BEST_FIT){
    	//printf("best");
    	current = best_fit(size);
    }
    if(myalloc.aalgorithm == FIRST_FIT){
    	//printf("first");
    	current = first_fit(size);
    }
    if(myalloc.aalgorithm == WORST_FIT){
    	//printf("worst");
    	current = worst_fit(size);
    }

    void* address = NULL;
    if (current){
        address = current->ptr;
        struct nodeStruct *node = List_createNode(size, address);
        List_insertHead(&myalloc.allocated, node );
        
        current->size=current->size-size;
        current->ptr=current->ptr + size;
        if (current->size == 0) {
            List_deleteNode(&myalloc.unallocated, current);
        }
        
        
        List_sort(&myalloc.allocated);
        List_sort(&myalloc.unallocated);
    }
    return address;
    pthread_mutex_unlock(&m);
}



void deallocate(void* _ptr) {
     pthread_mutex_lock(&m);
    if (_ptr == NULL){
    	printf("can not free a NULL pointer!\n");
        return;
    }
    assert(_ptr != NULL);
    struct nodeStruct *next = NULL;
    struct nodeStruct *temp = List_findNode(myalloc.allocated, _ptr);
    
    if (temp == NULL){
    	printf("pointer not found!\n");
    	return;
    }
    assert(temp);
      
    //delete the node from allocated and put it in unallocated list  
    struct nodeStruct *node=List_createNode(temp->size, temp->ptr);
    List_insertHead(&myalloc.unallocated, node);
    List_deleteNode(&myalloc.allocated, temp);
    List_sort(&myalloc.unallocated);
    List_sort(&myalloc.allocated);
    struct nodeStruct *curr = myalloc.unallocated;
        while (curr!=NULL && curr->next!=NULL) {
            next = curr->next;
            if (curr->next->ptr - curr->ptr == curr->size) {
                curr->size = curr->size + curr->next->size;
                List_deleteNode(&myalloc.unallocated, curr->next); 
            }
            curr = next;
            List_sort(&myalloc.unallocated);
            List_sort(&myalloc.allocated);
        }
    pthread_mutex_unlock(&m);
}


int compact_allocation(void** _before, void** _after) {
	 pthread_mutex_lock(&m);
    // compact allocated memory
    // update _before, _after and count
    int count = 0;
    int size=0;
    void *memory = myalloc.memory;
    struct nodeStruct *allo = myalloc.allocated;
    struct nodeStruct *unallo = myalloc.unallocated;

    List_sort(&myalloc.allocated);
    List_sort(&myalloc.unallocated);

    while (allo !=NULL) {
        
        
        for(int i=0;i<allo->size;i++) {
            *((char *)memory + i) = *((char *)allo->ptr + i);
        }
        _before[count] = allo->ptr;
        _after[count] = memory;
        count++;
        allo->ptr = memory;
        memory = (char *)memory + allo->size;

        allo = allo->next;
    }
    
   	
   	while (unallo) {
        	size = size + unallo->size;
        	unallo = unallo->next;
    }


    myalloc.unallocated->size = size;
    myalloc.unallocated->ptr = memory;
    while (myalloc.unallocated->next) {
        List_deleteNode(&myalloc.unallocated, myalloc.unallocated->next);
    }
    

    return count;
    pthread_mutex_unlock(&m);
}

int available_memory() {
	 pthread_mutex_lock(&m);
    int available_memory_size = 0;
    //loop the unallocated linked list and increment the unallocated size.
    struct nodeStruct *temp = myalloc.unallocated;
    while (temp) {
        available_memory_size =available_memory_size+ temp->size;
        temp = temp->next;
    }
    return available_memory_size;
     pthread_mutex_unlock(&m);
}

void print_statistics() {
	pthread_mutex_lock(&m);
    int allocated_size = 0;
    int allocated_chunk = 0;
    int free_size = 0;
    int free_chunk = 0;
    int smallest_free_chunk_size = myalloc.size;
    int largest_free_chunk_size = 0;
  
    struct nodeStruct *temp = myalloc.unallocated;
    while (temp!=NULL) {
        free_size += temp->size;
       
        if(largest_free_chunk_size < temp->size)
            largest_free_chunk_size = temp->size;

        if(smallest_free_chunk_size > temp->size)
            smallest_free_chunk_size = temp->size;

        if(largest_free_chunk_size ==0){
        	smallest_free_chunk_size = 0;
        }
        free_chunk ++;
        temp = temp->next;
    }
    struct nodeStruct *tmp= myalloc.allocated;
    while (tmp!=NULL) {
 
        allocated_size += tmp->size;
        tmp = tmp->next;
        allocated_chunk ++;

    }
    
    printf("Allocated size = %d\n", allocated_size);
    printf("Allocated chunk = %d\n", allocated_chunk);
    printf("Free size = %d\n", free_size);
    printf("Free chunks = %d\n", free_chunk);
    printf("Largest free chunk size = %d\n", largest_free_chunk_size);
    printf("Smallest free chunk size = %d\n", smallest_free_chunk_size);
    pthread_mutex_unlock(&m);
}



