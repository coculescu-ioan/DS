#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_SIZE 256
#define TABLE_SIZE 10

struct Room {
	unsigned short floor;
	unsigned short number;
	char* type;
	double price;
};
typedef struct Room Room;

struct SLLNode {
	Room* data;
	struct SLLNode* next;
};
typedef struct SLLNode Node;

struct HashTable {
	Node** buckets;
	int size;
};
typedef struct HashTable HT;

struct DLLNode {
	Room* data;
	struct DLLNode* prev;
	struct DLLNode* next;
};
typedef struct DLLNode DNode;

Room* createRoom(const unsigned short, const unsigned short, const char*, const double);
void printRoom(const Room*);

Node* createNode(const Room*);
void putNode(HT*, Node*, int);

int fhash(unsigned short, int);

void putHT(HT*, Node*);
void printHT(const HT);

void printDLL(const DNode*);

int countRoomsOnFloor(const HT*, const unsigned short);
DNode* createListRoomsWithPriceAbove(const HT*, const double);
void deleteRoomsOfType(DNode**, const char*);

int main() 
{
	FILE* fp = fopen("hotel_rooms.txt", "r");
	if (!fp) return -1;

	HT ht = { NULL, 0 };

	char buffer[LINE_SIZE];
	char* token;
	char delimiters[] = ",\n";

	unsigned short floor;
	unsigned short number;
	char type[LINE_SIZE];
	double price;

	while (fgets(buffer, LINE_SIZE, fp)) 
	{
		token = strtok(buffer, delimiters);
		floor = atoi(token);

		token = strtok(NULL, delimiters);
		number = atoi(token);

		token = strtok(NULL, delimiters);
		strcpy(type, token);

		token = strtok(NULL, delimiters);
		price = atof(token);

		Room* room = createRoom(floor, number, type, price);
		Node* node = createNode(room);
		putHT(&ht, node);
	}
	fclose(fp);

	printHT(ht);

	/*Write the function that determines the number of rooms for a given floor passed by parameter.*/
	unsigned short floorNo = 3;
	int noRooms = countRoomsOnFloor(&ht, floorNo);
	printf("\nThe number of rooms on floor %d is: %d\n", floorNo, noRooms);

	/*Write the function that creates a doubly linked list with all the rooms that have a price 
	higher than a given value, passed as a parameter for the function. The new structure does not
	share heap memory with the hash table.*/
	double priceThreshold = 100.0f;
	DNode* list = createListRoomsWithPriceAbove(&ht, priceThreshold);
	printf("\nThe list of rooms that have the price higher than %.2f:\n", priceThreshold);
	printDLL(list);

	/*Write the function that deletes all the entries from the doubly linked list 
	for a given room type, passed as a parameter for the function.*/
	char roomType[] = "Suite";
	deleteRoomsOfType(&list, roomType);
	printf("\nThe list after deleting all rooms with type %s:\n", roomType);
	printDLL(list);
	return 0;
}

Room* createRoom(const unsigned short floor, const unsigned short number, const char* type, const double price)
{
	Room* room = (Room*)malloc(sizeof(Room));
	if (room) 
	{
		room->floor = floor;
		room->number = number;

		room->type = (char*)malloc(strlen(type) + 1);
		if(room->type)
			strcpy(room->type, type);

		room->price = price;
	}
	return room;
}

void printRoom(const Room* room) 
{
	printf("\tRoom floor: %d, number: %d, type: %s, price: %.2f\n", room->floor, room->number, room->type, room->price);
}

Node* createNode(Room* room)
{
	Node* node = (Node*)malloc(sizeof(Node));
	if (node)
	{
		node->next = NULL;
		node->data = room;
	}
	return node;
}

void putNode(HT* ht, Node* node, int index)
{
	if (ht->buckets[index] == NULL)
		ht->buckets[index] = node;
	else
	{
		node->next = ht->buckets[index];
		ht->buckets[index] = node;
	}
}

int fhash(unsigned short key, int size)
{
	return key % size;
}

void putHT(HT* ht, Node* node)
{
	if (ht->buckets == NULL)
	{
		ht->size = TABLE_SIZE;
		ht->buckets = (Node**)malloc(ht->size * sizeof(Node*));
		if(ht->buckets)
			memset(ht->buckets, NULL, ht->size * sizeof(Node*));
	}

	int index = fhash(node->data->floor, ht->size);
	putNode(ht, node, index);
}

void printHT(HT ht)
{
	printf("HASH TABLE START:\n");
	for (int i = 0; i < ht.size; i++)
	{
		printf("Bucket %d:\n", i);

		Node* bucket = ht.buckets[i];

		while (bucket)
		{
			printRoom(bucket->data);
			bucket = bucket->next;
		}
	}
	printf("HASH TABLE END.\n");
}

void printDLL(const DNode* list)
{
	while (list)
	{
		printRoom(list->data);
		list = list->next;
	}
}

int countRoomsOnFloor(const HT* ht, const unsigned short floor)
{
	int counter = 0;
	int index = fhash(floor, ht->size);

	Node* bucket = ht->buckets[index];
	while (bucket)
	{
		if (bucket->data->floor == floor) counter++;
		bucket = bucket->next;
	}

	return counter;
}

DNode* createListRoomsWithPriceAbove(HT* ht, double price)
{
	DNode* list = NULL;
	for (int i = 0; i < ht->size; i++)
	{
		Node* bucket = ht->buckets[i];
		while (bucket)
		{
			if (bucket->data->price > price)
			{
				DNode* node = (DNode*)malloc(sizeof(DNode));
				if (node)
				{
					node->data = bucket->data;
					node->next = node->prev = NULL;
				}

				if (list)
				{
					node->next = list;
					list->prev = node;
					list = node;
				}
				else
					list = node;
			}
			bucket = bucket->next;
		}
	}
	return list;
}

void deleteRoomsOfType(DNode** list, const char* type)
{
	if (!list || !(*list)) return;  // Check if the list is empty

	DNode* current = *list;

	// Handle the head nodes that need to be deleted
	while (current && strcmp(current->data->type, type) == 0) {
		DNode* temp = current;
		current = current->next;
		if (current) current->prev = NULL;

		free(temp->data->type);  
		free(temp);  
	}
	*list = current;  // Update the head of the list

	// Handle the rest of the list
	while (current) {
		if (strcmp(current->data->type, type) == 0) {
			DNode* temp = current;
			if (current->prev) current->prev->next = current->next;
			if (current->next) current->next->prev = current->prev;
			current = current->next;

			free(temp->data->type);  
			free(temp);  
		}
		else
			current = current->next;
	}
}
