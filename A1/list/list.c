#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>



/*
 * Allocate memory for a node of type struct nodeStruct and initialize
 * it with the value item. Return a pointer to the new node.
 */

struct nodeStruct* List_createNode(int item){
	struct nodeStruct* node = malloc(sizeof(struct nodeStruct));
	node -> item = item;
	node -> next = NULL;
	return node;
}


/*
 * Insert node at the head of the list.
 */
void List_insertHead (struct nodeStruct **headRef, struct nodeStruct *node){
	struct nodeStruct* curr = *headRef;
	//emptylist
	if (curr == NULL) curr = node;
	//else
	node -> next = *headRef;
	*headRef = node;
}

/*
 * Insert node after the tail of the list.
 */
void List_insertTail (struct nodeStruct **headRef, struct nodeStruct *node){
	struct nodeStruct* curr = *headRef;
	//emptylist
	if (curr == NULL) curr = node;
	//else
	while (curr -> next != NULL) {
		curr = curr -> next;
	}
	curr -> next = node;
}
/*
 * Count number of nodes in the list.
 * Return 0 if the list is empty, i.e., head N== NULL
 */
int List_countNodes (struct nodeStruct *head){
	struct nodeStruct* curr = head;
	int i = 0;
	//count as it traverse
	while (curr) {
		curr = curr -> next;
		i++;
	}
	return i;
}
	
/*
 * Return the first node holding the value item, return NULL if none found
 */
struct nodeStruct* List_findNode(struct nodeStruct *head, int item){
	struct nodeStruct* curr = head;
	// if empty;
	if (curr == NULL) return NULL;
	//else
	while (curr != NULL && curr->item !=item){
		curr = curr -> next;
	}
	return curr;
}

/*
 * Delete node from the list and free memory allocated to it.
 * This function assumes that node has been properly set to a valid node 
 * in the list. For example, the client code may have found it by calling 
 * List_findNode(). If the list contains only one node, the head of the list 
 * should be set to NULL.
 */
void List_deleteNode (struct nodeStruct **headRef, struct nodeStruct *node){

	struct nodeStruct *head=*headRef;
	struct nodeStruct *pointer=node;
	
	
	// empty
	if (head == NULL) return;
	if (pointer == NULL) return;
	// node is first elements
	if(head->next != NULL && head == pointer){
	*headRef = pointer->next;
	free(pointer);
	}
	
	// search
	while (head->next!=node && head != NULL){
		head = head->next;
	}
	//link & free
	head->next = pointer->next;
	
	free (pointer);
	






}
/*
 * Sort the list in ascending order based on the item field.
 * Any sorting algorithm is fine.
 */

void List_sort (struct nodeStruct **headRef){
	

	struct nodeStruct *sorted = *headRef;
	// empt or single node return
	if (sorted == NULL) return;
	if (sorted -> next==NULL) return;
	//else
	struct nodeStruct *curr;
	struct nodeStruct *min;

	int len = List_countNodes(*headRef);
	for( int i=0;i<len; i++){
	
		min = sorted;

		//update curr back to head pointer
		curr = sorted->next;
		
		//find min
		while(curr!=NULL){
		//update min if found
			if(curr->item < min->item ){
			       min = curr;
			}
		curr = curr->next;
		}
		
		//swap value of the node
		int temp = min->item;
		min->item = sorted ->item;
		sorted->item = temp;
		sorted=sorted->next;
		}
	
		
	
}


void List_print (struct nodeStruct *headRef) {
	struct nodeStruct *currentNodeRef = headRef;

	while (currentNodeRef != NULL) {
		printf("%d ", currentNodeRef -> item);
		currentNodeRef = currentNodeRef -> next;
	}

	printf("\n");
}







