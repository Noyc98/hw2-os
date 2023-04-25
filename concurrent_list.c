#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "concurrent_list.h"

typedef struct node {
	int value;
	struct node* next;
	pthread_mutex_t lockNode;
} node;

typedef struct list {
	struct node* head;
	pthread_mutex_t lockList;
} list;

void print_node(node* node)
{
  // DO NOT DELETE
  if(node)
  {
    printf("%d ", node->value);
  }
}

// ask
list* create_list()
{
	list* newList = (list*)malloc(sizeof(list));

	if (!newList)
	{
		// check malloc
		exit(1);
	}
	else {
		pthread_mutex_init(&(newList->lockList), NULL);
		newList->head = NULL;
		return newList;
	}
}

// ask
void delete_list(list* list)
{
	node* current, * nextNode;

	while (list != NULL)
	{
		node* current = NULL;
		node* nextNode = list->head;
		
		while (nextNode != NULL)
		{

			current = nextNode;
			nextNode = nextNode->next;
			pthread_mutex_destroy(&(current->lockNode));
			free(current);
		}
		pthread_mutex_destroy(&(list->lockList));
		free(list);
  }
}

void insert_value(list* list, int value)
{
	if (list == NULL)
		return;

	node* newNode = (node*)malloc(sizeof(node));
	node* currentNode = list->head;
	node* nextNode = NULL;

	newNode->value = value;
	newNode->next = NULL;
	
	// lock changes in insertions
	pthread_mutex_init(&(newNode->lockNode), NULL);

	// empty list
	if (currentNode == NULL)
	{
		//pthread_mutex_lock(&(list->lockList));
		list->head = newNode;
		//pthread_mutex_unlock(&(list->lockList));
		return;
	}

	pthread_mutex_lock(&(currentNode->lockNode));

	// new head insert
	if (currentNode->value > value)
	{
		newNode->next = currentNode;
		list->head = newNode;
		pthread_mutex_unlock(&(currentNode->lockNode));
		return;
	}

	nextNode = currentNode->next;

	// insert in middle
	// should we lock list to prevent DeleteList???????????????????????
	if (nextNode != NULL)
	{
		pthread_mutex_lock(&(nextNode->lockNode));
	}
	
	while (nextNode != NULL && nextNode->value < value) 
	{
		pthread_mutex_unlock(&(currentNode->lockNode));
		currentNode = nextNode;

		if (nextNode->next != NULL)
		{
			pthread_mutex_lock(&(nextNode->next->lockNode));
		}
		
		nextNode = nextNode->next;
	}

	currentNode->next = newNode;
	newNode->next = nextNode;

	if (nextNode != NULL)
	{
		pthread_mutex_unlock(&(newNode->lockNode));
	}

	pthread_mutex_unlock(&(currentNode->lockNode));

	return;
}

void remove_value(list* list, int value)
{
	node* currentNode;
	node* nextNode;
	
	if (list != NULL)
	{
		currentNode = list->head;
		nextNode = currentNode->next;
		pthread_mutex_lock(&(currentNode->lockNode));
		
		// delete the list head
		if (currentNode->value == value)		
		{
			pthread_mutex_lock(&(nextNode->lockNode));
			list->head = nextNode;
			pthread_mutex_unlock(&(currentNode->lockNode));
			pthread_mutex_unlock(&(nextNode->lockNode));
			free(currentNode);
		}
		// delete other node
		else									
		{
			if (nextNode != NULL)
			{
				pthread_mutex_lock(&(nextNode->lockNode));
			}
			while (nextNode != NULL)
			{
				if (nextNode->next != NULL)
				{
					pthread_mutex_lock(&(nextNode->next->lockNode));
				}
				// find value
				if (nextNode->value == value)		
				{
					// delete value
					currentNode->next = nextNode->next;
					pthread_mutex_unlock(&(nextNode->lockNode));
					pthread_mutex_destroy(&(nextNode->lockNode));
					free(nextNode);
					pthread_mutex_unlock(&(currentNode->lockNode));
					pthread_mutex_unlock(&(nextNode->next->lockNode));
					return;
				}
				else
				{
					pthread_mutex_unlock(&(currentNode->lockNode));
					currentNode = nextNode;
					nextNode = nextNode->next;
				}
			}
			pthread_mutex_unlock(&(currentNode->lockNode));
		}
		
	}
}


void print_list(list* list)
{
	if (list != NULL)
	{
		node* currentNode = list->head;

		while (currentNode != NULL)
		{
			print_node(currentNode);
			currentNode = currentNode->next;
		}
	}
	/*else
	{
		printf("list is empty\n");
	}
	*/
	printf("\n"); // DO NOT DELETE
}

void count_list(list* list, int (*predicate)(int))
{
  int count = 0; // DO NOT DELETE
  node* current = list->head;
  node* temp;

  if (list != NULL)
  {
	  while (current != NULL)
	  {
		  // the function predicate returns 1 if predicate happened
		  count += predicate(current->value);

		  if (current->next != NULL)
		  {
			  current = current->next;
		  }
	  }
  }

  printf("%d items were counted\n", count); // DO NOT DELETE
}
