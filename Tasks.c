#define _CRT_SECURE_NO_WARNINGS
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define LINE_SIZE 256
#define TABLE_SIZE 26

struct Task {
	unsigned int taskId;
	char* taskDate;
	char* ownerName;
	float taskCompletion;
};
typedef struct Task Task;

struct Node {
	Task* data;
	struct Node* next;
};
typedef struct Node Node;

struct HashTable {
	Node** buckets;
	int size;
};
typedef struct HashTable HT;

Task* createTask(const unsigned int, const char*, const char*, const float);
void printTask(const Task* task);
void deleteTask(Task** task);

Node* createNode(Task*);
void addNode(Node**, Task*);
void printList(const Node*);
void deleteList(Node**);

int fhash(const char, int);
void addHT(HT** ht, Task* task);
void printHT(const HT* ht);
void deleteHT(HT** ht);

int countTasksForOwner(const HT*, const char*);
int countTasksWithCompletionLevelAbove(const HT*, const float);
void updateOwnerForTask(const HT*, const unsigned int, const char*);
Node* createListSameDate(const HT*, const char*);

int main() {
	/*1.Create a HashTable with at least 7 entries/tasks. The data for each entry is read from 
	a text file. The search key used by the HashTable is the attribute owner’s name. (1 p)*/
	FILE* fp = fopen("tasks.txt", "r");
	if (!fp) return -1;

	HT* ht = NULL;

	char buffer[LINE_SIZE];
	char* token;
	char delimiters[] = ",\n";

	unsigned int taskId;
	char taskDate[LINE_SIZE];
	char ownerName[LINE_SIZE];
	float taskCompletion;

	while (fgets(buffer, LINE_SIZE, fp)) 
	{
		token = strtok(buffer, delimiters);
		taskId = atoi(token);

		token = strtok(NULL, delimiters);
		strcpy(taskDate, token);

		token = strtok(NULL, delimiters);
		strcpy(ownerName, token);

		token = strtok(NULL, delimiters);
		taskCompletion = atof(token);

		Task* task = createTask(taskId, taskDate, ownerName, taskCompletion);
		addHT(&ht, task);
	}

	fclose(fp);

	/*2.Print out to the console all the elements in the HashTable. (0.5 p)*/
	printHT(ht);

	/*3.Write the function that determines the number of tasks for a given owner 
	specified as a parameter. The function is called in the main() and the result 
	is displayed on the console. (1 p)*/
	char name[] = "John Doe";
	int noTasks = countTasksForOwner(ht, name);
	printf("\nTotal number of tasks for %s: %d\n", name, noTasks);

	/*4.Write the function for counting the number of tasks with the completion level 
	above a certain value specified as a parameter. The function is called in the main() 
	and the result is displayed on the console. (1 p) */
	float completion = 0.50f;
	int noTasksAbove = countTasksWithCompletionLevelAbove(ht, completion);
	printf("\nTotal number of tasks with completion level above %.2f: %d\n", completion, noTasksAbove);

	/*5.Write the function that changes the owner of a given task, specified by task id. 
	The function is called in the main() and the result is validated at the console by 
	displaying all the elements. (1 p)*/
	unsigned int id = 1;
	char newName[] = "Popescu Ion";
	updateOwnerForTask(ht, id, newName);
	printf("\nAfter updating the owner name for task %d to %s:\n", id, newName);
	printHT(ht);

	/*6.Write the function that returns a list with all the tasks that were assigned 
	on a given date, specified as a parameter. The list doesn’t share HEAP memory space 
	with the elements found in the HashTable. The function is called in the main() and 
	the result (elements stored in the list) is displayed on the console. (1.5 p)*/
	char date[] = "2024-06-02";
	Node* list = createListSameDate(ht, date);
	printf("\nList with all tasks assigned on %s:\n", date);
	printList(list);

	return 0;
}

Task* createTask(const unsigned int taskId, const char* taskDate, const char* ownerName, const float taskCompletion)
{
	Task* t = (Task*)malloc(sizeof(Task));

	if (t)
	{
		t->taskId = taskId;
		t->taskCompletion = taskCompletion;
		t->taskDate = (char*)malloc(strlen(taskDate) + 1);
		if (t->taskDate)
			strcpy(t->taskDate, taskDate);
		t->ownerName = (char*)malloc(strlen(ownerName) + 1);
		if (t->ownerName)
			strcpy(t->ownerName, ownerName);
	}

	return t;
}

void printTask(const Task* t) 
{
	printf("\tTask id: %d, Date: %s, Owner: %s, Completion: %.2f/100.00\n",
		t->taskId, t->taskDate, t->ownerName, t->taskCompletion * 100);
}

void deleteTask(Task** t) 
{
	free((*t)->taskDate);
	free((*t)->ownerName);
	*t = NULL;
}

Node* createNode(Task* t) 
{
	Node* n = (Node*)malloc(sizeof(Node));
	if (n)
	{
		n->data = t;
		n->next = NULL;
	}
	return n;
}

void addNode(Node** head, Task* t)
{
	Node* n = createNode(t);

	if (*head == NULL) 
		*head = n;
	else {
		// Add at the beginning of the list, O(1)
		n->next = *head;
		*head = n;
	}
}

void printList(const Node* head)
{
	while (head)
	{
		printTask(head->data);
		head = head->next;
	}
}

void deleteList(Node** head)
{
	while (*head) 
	{
		Node* temp = *head;
		*head = (*head)->next;

		deleteTask(&temp->data);
		free(temp);
	}
	*head = NULL;
}

int fhash(const char key, int size) 
{
	// Hashing function logic: first letter of the word (A to Z) % 26
	return key % size;
}

void addHT(HT** ht, Task* t)
{
	// Initialize the hash table (on first run)
	if (*ht == NULL) 
	{
		*ht = (HT*)malloc(sizeof(HT));
		if (*ht)
		{
			(*ht)->size = TABLE_SIZE;
			(*ht)->buckets = (Node**)malloc((*ht)->size * sizeof(Node*));
			if ((*ht)->buckets)
				memset((*ht)->buckets, 0, (*ht)->size * sizeof(Node*));
		}
	}
	
	if (*ht) 
	{
		// Get the hash 
		int index = fhash(t->ownerName[0], (*ht)->size);

		// Add the element
		if ((*ht)->buckets)
			addNode(&(*ht)->buckets[index], t);
	}
}

void printHT(const HT* ht) 
{
	printf("Hash Table START.\n");

	for (int i = 0; i < ht->size; i++) 
	{
		if (ht->buckets[i] != NULL) 
		{
			printf("Bucket %d:\n", i);
			printList(ht->buckets[i]);
		}
	}

	printf("Hash Table END.\n");
}

void deleteHT(HT** ht) 
{
	for (int i = 0; i < (*ht)->size; i++)
		deleteList(&(*ht)->buckets[i]);

	free((*ht)->buckets);

	free(*ht);
	*ht = NULL;
}

int countTasksForOwner(const HT* ht, const char* name) 
{
	int counter = 0;
	int index = fhash(name[0], ht->size);

	Node* bucket = ht->buckets[index];
	while (bucket) 
	{
		if (strcmp(bucket->data->ownerName, name) == 0) counter++;
		bucket = bucket->next;
	}

	return counter;
}

int countTasksWithCompletionLevelAbove(const HT* ht, const float completion) 
{
	int counter = 0;

	for (int i = 0; i < ht->size; i++) 
	{
		Node* bucket = ht->buckets[i];
		while (bucket) 
		{
			if (bucket->data->taskCompletion > completion) counter++;
			bucket = bucket->next;
		}
	}

	return counter;
}

void updateOwnerForTask(HT* ht, const unsigned int id, const char* newName) 
{
	Task* t = NULL;
	Node* prev = NULL;

	for (int i = 0; i < ht->size && t == NULL; i++) 
	{
		Node* bucket = ht->buckets[i];
		while (bucket && t == NULL) 
		{
			if (bucket->data->taskId == id) 
			{
				// Change the name
				free(bucket->data->ownerName);
				bucket->data->ownerName = (char*)malloc(strlen(newName) + 1);
				if (bucket->data->ownerName) {
					strcpy(bucket->data->ownerName, newName);
				}
				t = bucket->data;

				// Remove the node from the current bucket
				if (prev == NULL) {
					ht->buckets[i] = bucket->next;
				}
				else {
					prev->next = bucket->next;
				}
				free(bucket);
				break;
			}
			prev = bucket;
			bucket = bucket->next;
		}
	}

	// If found, re-add the task to the hash table
	if (t) 
		addHT(&ht, t);
	else 
		printf("\nTask id %d is invalid!", id);
}

Node* createListSameDate(const HT* ht, const char* date) 
{
	Node* list = NULL;

	for (int i = 0; i < ht->size; i++) 
	{
		Node* bucket = ht->buckets[i];

		while (bucket) 
		{
			if (strcmp(bucket->data->taskDate, date) == 0) 
			{
				Task* t = createTask(bucket->data->taskId, bucket->data->taskDate, bucket->data->ownerName, bucket->data->taskCompletion);
				addNode(&list, t);
			}
			bucket = bucket->next;
		}
	}

	return list;
}
