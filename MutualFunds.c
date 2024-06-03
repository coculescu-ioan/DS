#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_SIZE 256

typedef struct {
	char* mutualFundCode;
	char* mutualFundGroup;
	int riskLevel;
	double netAssetValue;
	float returnOfInvestment;
} MutualFund;

typedef struct Node {
	MutualFund* data;
	struct Node* prev;
	struct Node* next;
} Node;

MutualFund* createMutualFund(const char*, const char*, const int, const double, const float);
void printMutualFund(const MutualFund*);
void freeMutualFund(MutualFund*);

Node* createNode(MutualFund*);
void addNode(Node**, Node*);

void printList(const Node*);
int getListSize(const Node*);
void freeList(Node*);

int countMutualFundsGreaterThanRiskLevel(const Node*, const int);
void displayCapitalGainOrLoss(const Node*);
MutualFund* findFirstMutualFundGreaterThanNAV(const Node*, const double);

MutualFund** createArray(const Node*, const int, const float, const char*);


int main() {
	/*1. Create a doubly linked list by inserting elements based on a selected criterion.
	The list must have at least 7 entries that are read from a file. (1 p)*/
	FILE* fp = fopen("mutual_funds.txt", "r");
	if (!fp) return -1;

	Node* head = NULL;

	char buffer[LINE_SIZE];
	char* token;
	char delimiters[] = ",\n";

	char mutualFundCode[LINE_SIZE];
	char mutualFundGroup[LINE_SIZE];
	int riskLevel;
	double netAssetValue;
	float returnOfInvestment;

	while (fgets(buffer, LINE_SIZE, fp)) 
	{
		token = strtok(buffer, delimiters);
		strcpy(mutualFundCode, token);

		token = strtok(NULL, delimiters);
		strcpy(mutualFundGroup, token);

		token = strtok(NULL, delimiters);
		riskLevel = atoi(token);

		token = strtok(NULL, delimiters);
		netAssetValue = atof(token);

		token = strtok(NULL, delimiters);
		returnOfInvestment = atof(token);

		MutualFund* mutualFund = createMutualFund(mutualFundCode, mutualFundGroup, riskLevel, netAssetValue, returnOfInvestment);

		Node* node = createNode(mutualFund);

		addNode(&head, node);
	}

	fclose(fp);

	/*2. Write the function for printing all the mutual funds 
	in the structure for both directions. (0.5 p)*/
	printList(head);

	/*3.Write the function that counts how many mutual funds have 
	the risk level greater than a given value, sent as a parameter. (1 p)*/
	int riskLevelThreshold = 5;
	int noFunds = countMutualFundsGreaterThanRiskLevel(head, riskLevelThreshold);
	printf("\nThe number of funds that have a risk level greater than %d is: %d\n", riskLevelThreshold, noFunds);

	/*4.Write the function that determines the capital gain/loss for all the mutual funds.
	The function displays on the console the mutual fund code and the capital gain/loss by
	applying the positive/negative return of investments to the net asset value attribute. (1 p)*/
	printf("\nCapital gain/loss for mutual fund:\n");
	displayCapitalGainOrLoss(head);

	/*5.Write the function that searches in the doubly linked list for the first mutual fund 
	that has a net asset value greater than a threshold specified as a parameter. The mutual fund 
	is returned in main() and the values are displayed. (1 p)*/
	double NAVThreshold = 2500;
	MutualFund* firstFund = findFirstMutualFundGreaterThanNAV(head, NAVThreshold);
	
	if (firstFund) 
	{
		printf("\nFirst mutual fund with net asset value above %.2f:\n", NAVThreshold);
		printMutualFund(firstFund);
	}
	else {
		printf("\nNo mutual fund with net asset value above %.2f found!\n", NAVThreshold);
	}

	/*6.Write the function for creating an array with all the elements that have the nominal return
	of investment higher than a limit specified as a parameter and are part of a specified mutual
	fund group. The array doesn’t share HEAP memory space with the elements found in the doubly
	linked list. The function is called in the main() and the result (elements stored in the array)
	is displayed on the console. (1.5 p)*/

	int size = getListSize(head);

	float ROIThreshold = 1;
	char group[] = "D";

	MutualFund** array = createArray(head, size, ROIThreshold, group);
	freeList(head);

	printf("\nPrinting mutual funds above %.2f return level:\n", ROIThreshold);
	for (int i = 0; i < size && array[i]; i++) 
		printMutualFund(array[i]);


	for (int i = 0; i < size && array[i]; i++)
		freeMutualFund(array[i]);

	return 0;
}

MutualFund* createMutualFund(const char* mutualFundCode, const char* mutualFundGroup, const int riskLevel, const double netAssetValue, const float returnOfInvestment)
{
	MutualFund* mutualFund = (MutualFund*)malloc(sizeof(MutualFund));

	if (mutualFund) 
	{
		mutualFund->mutualFundCode = (char*)malloc(strlen(mutualFundCode) + 1);
		if(mutualFund->mutualFundCode)
			strcpy(mutualFund->mutualFundCode, mutualFundCode);

		mutualFund->mutualFundGroup = (char*)malloc(strlen(mutualFundGroup) + 1);
		if (mutualFund->mutualFundGroup)
			strcpy(mutualFund->mutualFundGroup, mutualFundGroup);

		mutualFund->riskLevel = riskLevel;
		mutualFund->netAssetValue = netAssetValue;
		mutualFund->returnOfInvestment = returnOfInvestment;
	}

	return mutualFund;
}

void printMutualFund(const MutualFund* mutualFund) 
{
	printf("Mutual fund code: %s, group: %s, risk level: %d, net asset value: %.2f, return of investment: %.2f\n", mutualFund->mutualFundCode, mutualFund->mutualFundGroup, mutualFund->riskLevel, mutualFund->netAssetValue, mutualFund->returnOfInvestment);
}

void freeMutualFund(MutualFund* mutualFund) 
{
	free(mutualFund->mutualFundCode);
	free(mutualFund->mutualFundGroup);
	free(mutualFund);
}

Node* createNode(MutualFund* mutualFund) 
{
	Node* node = (Node*)malloc(sizeof(Node));

	if (node) 
	{
		node->data = mutualFund;
		node->prev = NULL;
		node->next = NULL;
	}
	return node;
}

void addNode(Node** head, Node* node) 
{
	if (*head == NULL) 
		*head = node;
	else {
		node->next = *head;
		(*head)->prev = node;

		*head = node;
	}
}

void printList(const Node* head) 
{
	if (!head) return;

	printf("\nForward traversal:\n");
	while (head->next)
	{
		printMutualFund(head->data);
		head = head->next;
	}

	printMutualFund(head->data);

	printf("\nBackward traversal:\n");
	while (head)
	{
		printMutualFund(head->data);
		head = head->prev;
	}
}

int getListSize(const Node* head)
{
	int count = 0;
	while (head)
	{
		count++;
		head = head->next;
	}
	return count;
}

void freeList(Node* head) 
{
	while (head) 
	{
		Node* temp = head;
		head = head->next;
		freeMutualFund(temp->data);
		free(temp);
	}
}

int countMutualFundsGreaterThanRiskLevel(const Node* head, const int riskLevelThreshold) 
{
	int count = 0;
	while (head)
	{
		if (head->data->riskLevel > riskLevelThreshold)
			count++;
		head = head->next;
	}
	return count;
}

void displayCapitalGainOrLoss(const Node* head) 
{
	while (head)
	{
		double result = head->data->netAssetValue * head->data->returnOfInvestment;
		printf("code: %s, value: %.2f\n", head->data->mutualFundCode, result);
		head = head->next;
	}
}

MutualFund* findFirstMutualFundGreaterThanNAV(const Node* head, const double NAVThreshold) 
{
	MutualFund* fund = NULL;
	while (head)
	{
		if (head->data->netAssetValue > NAVThreshold)
		{
			fund = head->data;
			break;
		}
		head = head->next;
	}
	return fund;
}

MutualFund** createArray(const Node* head, const int size, const float ROIThreshold, const char* group) 
{
	MutualFund** array = (MutualFund**)malloc(size * sizeof(MutualFund*));
	if (array)
	{
		// Initialize the allocated memory
		memset(array, 0, size * sizeof(MutualFund*));

		int index = 0;
		while (head)
		{
			if (head->data->returnOfInvestment > ROIThreshold && strcmp(head->data->mutualFundGroup, group) == 0) {
				array[index++] = createMutualFund(head->data->mutualFundCode, head->data->mutualFundGroup, head->data->riskLevel, head->data->netAssetValue, head->data->returnOfInvestment);
			}
			head = head->next;
		}
	}
	return array;
}
