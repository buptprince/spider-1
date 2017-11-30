#include <stdio.h>
#include <stdlib.h>

/*
*   Queue implements by C
*	initQueue()--->return an empty queue
*	push() pop() isEmpty() freeCqueue()
*/

typedef struct Node{
	char * val;
	struct Node* next;
} Node;

typedef struct cqueue{
	Node* head;
	Node* tail;
	int size;
} cqueue;

cqueue* initQueue(){
	cqueue* queue=(cqueue*)malloc(sizeof(cqueue));
	queue->head=(Node*)malloc(sizeof(Node));
	queue->tail=queue->head;
	queue->size=0;
	return queue;
}

void push(cqueue* queue,char* val){
	if(queue==NULL)
		return;
	Node* newNode=(Node*)malloc(sizeof(Node));
	newNode->next=NULL;
	newNode->val=val;
	queue->tail->next=newNode;
	queue->tail=newNode;
	queue->size+=1;
}

char* pop(cqueue* queue){
	if(queue==NULL || queue->size==0)
		return NULL;
	char* retval=queue->head->next->val;
	Node* oldHead=queue->head->next;
	queue->head->next=oldHead->next;
	free(oldHead);
	queue->size--;
	if(queue->size==0)
		queue->tail=queue->head;
	return retval;
}

char* front(cqueue* queue){
	if(queue==NULL || queue->size==0)
		return NULL;
	return queue->head->next->val;
}

int isEmpty(cqueue* queue){
	if(queue==NULL)
		return 1;
	return queue->size==0;
}

void freeCqueue(cqueue* queue){
	if(queue==NULL) return;

	Node*node=queue->head;
	Node*temp=NULL;

	while(node->next){
		temp=node;
		node=node->next;
		free(temp);
	}
	free(node);
}