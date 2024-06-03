#define _CRT_SECURE_NO_WARNINGS
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define LINE_SIZE 256
#define TABLE_SIZE 26

typedef struct {
	unsigned int taskId;
	char* taskDate;
	char* ownerName;
	float taskCompletion;
} Task;

typedef struct Node {
	Task* data;
	struct Node* next;
} Node;

typedef struct {
	Node** buckets;
	int size;
} HashTable;

Task* createTask(const unsigned int, const char*, const char*, const float);
void printTask(const Task* task);
void freeTask(Task* task);

Node* createNode(Task*);
void addNode(Node**, Task*);
void printList(const Node*);
void freeList(Node**);

int hash(const char, int);
void addToHashTable(HashTable** HashTable, Task* task);
void printHashTable(const HashTable* HashTable);
void freeHashTable(HashTable* HashTable);

int countTasksForOwner(const HashTable*, const char*);
int countTasksWithCompletionAbove(const HashTable*, const float);
void updateOwnerForTask(HashTable*, const unsigned int, const char*);
Node* createTaskListOnDate(const HashTable*, const char*);

int main() {
	FILE* fp = fopen("tasks.txt", "r");
	if (!fp) return -1;

	HashTable* ht = NULL;

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
		addToHashTable(&ht, task);
	}

	fclose(fp);

	printHashTable(ht);

	char name[] = "John Doe";
	int noTasks = countTasksForOwner(ht, name);
	printf("\nTotal number of tasks for %s: %d\n", name, noTasks);

	float completion = 0.50f;
	int noTasksAbove = countTasksWithCompletionAbove(ht, completion);
	printf("\nTotal number of tasks with completion level above %.2f: %d\n", completion, noTasksAbove);

	unsigned int id = 1;
	char newName[] = "Popescu Ion";
	updateOwnerForTask(ht, id, newName);
	printf("\nAfter updating the owner name for task %d to %s:\n", id, newName);
	printHashTable(ht);

	char date[] = "2024-06-02";
	Node* list = createTaskListOnDate(ht, date);
	freeHashTable(ht);

	printf("\nList with all tasks assigned on %s:\n", date);
	printList(list);
	freeList(&list);

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

void freeTask(Task* t)
{
	if (t) {
		free(t->taskDate);
		free(t->ownerName);
		free(t);
	}
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

void freeList(Node** head)
{
	while (*head)
	{
		Node* temp = *head;
		*head = (*head)->next;

		freeTask(temp->data);
		free(temp);
	}
	*head = NULL;
}

int hash(const char key, int size)
{
	return key % size;
}

void addToHashTable(HashTable** ht, Task* t)
{
	if (*ht == NULL)
	{
		*ht = (HashTable*)malloc(sizeof(HashTable));
		if (*ht)
		{
			(*ht)->size = TABLE_SIZE;
			(*ht)->buckets = (Node**)malloc((*ht)->size * sizeof(Node*));
			if ((*ht)->buckets)
				memset((*ht)->buckets, 0, (*ht)->size * sizeof(Node*));
		}
	}
	else {
		int index = hash(t->ownerName[0], (*ht)->size);
		addNode(&(*ht)->buckets[index], t);
	}
}

void printHashTable(const HashTable* ht)
{
	printf("Hash Table START.\n");

	for (int i = 0; i < ht->size; i++)
	{
		if (ht->buckets[i])
		{
			printf("Bucket %d:\n", i);
			printList(ht->buckets[i]);
		}
	}

	printf("Hash Table END.\n");
}

void freeHashTable(HashTable* ht)
{
	if (ht)
	{
		for (int i = 0; i < ht->size; i++)
			freeList(&ht->buckets[i]);

		free(ht->buckets);
		free(ht);
	}
}

int countTasksForOwner(const HashTable* ht, const char* name)
{
	int count = 0;
	int index = hash(name[0], ht->size);

	Node* bucket = ht->buckets[index];
	while (bucket)
	{
		if (strcmp(bucket->data->ownerName, name) == 0) count++;
		bucket = bucket->next;
	}

	return count;
}

int countTasksWithCompletionAbove(const HashTable* ht, const float completion)
{
	int count = 0;
	for (int i = 0; i < ht->size; i++)
	{
		Node* bucket = ht->buckets[i];
		while (bucket)
		{
			if (bucket->data->taskCompletion > completion) count++;
			bucket = bucket->next;
		}
	}
	return count;
}

void updateOwnerForTask(HashTable* ht, const unsigned int id, const char* newName)
{
	for (int i = 0; i < ht->size; i++)
	{
		Node* bucket = ht->buckets[i];
		Node* prev = NULL;

		while (bucket)
		{
			if (bucket->data->taskId == id)
			{
				free(bucket->data->ownerName);
				bucket->data->ownerName = (char*)malloc(strlen(newName) + 1);
				if (bucket->data->ownerName)
					strcpy(bucket->data->ownerName, newName);

				if (prev == NULL) {
					ht->buckets[i] = bucket->next;
				}
				else {
					prev->next = bucket->next;
				}

				Task* updatedTask = bucket->data;
				free(bucket);

				addToHashTable(&ht, updatedTask);
				return;
			}
			prev = bucket;
			bucket = bucket->next;
		}
	}
	printf("\nTask id %d is invalid!", id);
}

Node* createTaskListOnDate(const HashTable* ht, const char* date)
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
