#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NO_LISTS 10
#define LINE_SIZE 256

typedef struct {
    char* data;
    char* numeMedic;
    char* specialitate;
    char* diagnostic;
    int pret;
} Consultatie;

typedef struct Node {
    Consultatie* info;
    struct Node* urm;
} Node;

Consultatie* createConsultatie(const char*, const char*, const char*, const char*, int);
void printConsultatie(Consultatie* consultatie);
void freeConsultatie(Consultatie*);

Node* createNode(Consultatie*);
void appendNode(Node**, Consultatie*, int (*hashFunc)(Consultatie*));

int hashFunction(Consultatie*);

void printLists(Node* lists[], int size);
void freeLists(Node* lists[], int size);

int calculateTotalValue(Node**, int, const char*);
void modifyPrice(Node**, int, const char*, int);
void deleteConsultationsByDiagnosis(Node**, int, const char*);
Consultatie** getConsultationsAboveThreshold(Node**, int, int);

int main() 
{
    FILE* fp = fopen("consultatii.txt", "r");
    if (!fp) return -1;

    /*1. Create a list of lists with at least 5 consultations whose dates are taken from a text file.
    The creation of structures is achieved by repeated calls to an insertion function, and the field
    by which the grouping is performed is the specialty. (1 point)*/

    Node* lists[NO_LISTS];

    for (int i = 0; i < NO_LISTS; i++)
        lists[i] = NULL;

    char buffer[LINE_SIZE];
    char* token;
    char delimiters[] = ",\n";

    char data[LINE_SIZE];
    char numeMedic[LINE_SIZE];
    char specialitate[LINE_SIZE];
    char diagnostic[LINE_SIZE];
    int pret;

    while (fgets(buffer, LINE_SIZE, fp))
    {
        token = strtok(buffer, delimiters);
        strcpy(data, token);

        token = strtok(NULL, delimiters);
        strcpy(numeMedic, token);

        token = strtok(NULL, delimiters);
        strcpy(specialitate, token);

        token = strtok(NULL, delimiters);
        strcpy(diagnostic, token);

        token = strtok(NULL, delimiters);
        pret = atoi(token);

        Consultatie* consultatie = createConsultatie(data, numeMedic, specialitate, diagnostic, pret);
        appendNode(lists, consultatie, hashFunction);
    }
    fclose(fp);

    printLists(lists, NO_LISTS);

    /*2. Implement the function that determines the total value of consultations performed for a
    specific specialty. The specialty is specified as an input parameter. (1 point)*/
    char specialty[] = "Cardiology";
    int totalValue = calculateTotalValue(lists, NO_LISTS, specialty);
    printf("\nTotal value of consultations for %s: %d\n", specialty, totalValue);

    /*3. Implement the function that modifies the price of a certain consultation (by date).
    The new price is also specified as an input parameter. (1 point)*/
    char date[] = "2024-06-07";
    int newPrice = 180;
    modifyPrice(lists, NO_LISTS, date, newPrice);
    //printLists(lists, NO_LISTS);

    /*4. Implement the function that deletes consultations whose diagnosis is specified 
    as an input parameter. (1.5 points)*/
    char diagnosis[] = "Fracture";
    deleteConsultationsByDiagnosis(lists, NO_LISTS, diagnosis);
    printLists(lists, NO_LISTS);

    /*5. Implement the function that returns a vector of consultations whose price exceeds a certain
    threshold specified as an input parameter. The data is taken from the list of lists created above,
    and there are no shared heap memory areas between the two structures. (1.5 points)*/
    int priceThreshold = 100;
    Consultatie** vector = getConsultationsAboveThreshold(lists, NO_LISTS, priceThreshold);

    printf("\nConsultations with the price above %d:\n", priceThreshold);
    for (int i = 0; vector[i] != NULL; i++) 
        printConsultatie(vector[i]);

    freeLists(lists, NO_LISTS);

    return 0;
}

Consultatie* createConsultatie(const char* data, const char* numeMedic, const char* specialitate, const char* diagnostic, int pret) 
{
    Consultatie* newConsultatie = (Consultatie*)malloc(sizeof(Consultatie));

    if (newConsultatie) 
    {
        newConsultatie->data = (char*)malloc(strlen(data) + 1);
        if (newConsultatie->data) 
            strcpy(newConsultatie->data, data);

        newConsultatie->numeMedic = (char*)malloc(strlen(numeMedic) + 1);
        if (newConsultatie->numeMedic) 
            strcpy(newConsultatie->numeMedic, numeMedic);

        newConsultatie->specialitate = (char*)malloc(strlen(specialitate) + 1);
        if (newConsultatie->specialitate) {
            strcpy(newConsultatie->specialitate, specialitate);
        }

        newConsultatie->diagnostic = (char*)malloc(strlen(diagnostic) + 1);
        if (newConsultatie->diagnostic) 
            strcpy(newConsultatie->diagnostic, diagnostic);

        newConsultatie->pret = pret;
    }

    return newConsultatie;
}

void printConsultatie(Consultatie* consultatie)
{
    printf("\tData: %s, Medic: %s, Specialitate: %s, Diagnostic: %s, Pret: %d\n",
        consultatie->data, consultatie->numeMedic, consultatie->specialitate,
        consultatie->diagnostic, consultatie->pret);
}

void freeConsultatie(Consultatie* consultatie) 
{
    free(consultatie->data);
    free(consultatie->numeMedic);
    free(consultatie->specialitate);
    free(consultatie->diagnostic);
    free(consultatie);
}

Node* createNode(Consultatie* consultatie) 
{
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode) 
    {
        newNode->info = consultatie;
        newNode->urm = NULL;
    }
    return newNode;
}

void appendNode(Node** lists, Consultatie* consultatie, int (*hashFunc)(Consultatie*)) 
{
    int index = hashFunc(consultatie);
    Node* newNode = createNode(consultatie);
    if (lists[index] == NULL) {
        lists[index] = newNode;
    }
    else {
        Node* temp = lists[index];
        while (temp->urm != NULL) 
        {
            temp = temp->urm;
        }
        temp->urm = newNode;
    }
}

int hashFunction(Consultatie* consultatie) {
    // Simple hash function based on the first character of the speciality
    return (consultatie->specialitate[0] - 'A') % NO_LISTS;
}

void printLists(Node* lists[], int size) 
{
    printf("START:\n");
    for (int i = 0; i < size; i++) 
    {
        Node* temp = lists[i];
        printf("List %d:\n", i);

        while (temp != NULL) 
        {
            Consultatie* consultatie = temp->info;
            printConsultatie(consultatie);
            temp = temp->urm;
        }
        
    }
    printf("END.\n");
}

void freeLists(Node* lists[], int size) 
{
    for (int i = 0; i < size; i++) 
    {
        Node* temp = lists[i];
        while (temp != NULL) 
        {
            Node* next = temp->urm;
            freeConsultatie(temp->info);
            free(temp);
            temp = next;
        }
    }
}

int calculateTotalValue(Node** lists, int size, const char* specialty) 
{
    int totalValue = 0;

    for (int i = 0; i < size; i++) 
    {
        Node* temp = lists[i];
        while (temp != NULL) 
        {
            Consultatie* consultatie = temp->info;
            if (strcmp(consultatie->specialitate, specialty) == 0) 
            {
                totalValue += consultatie->pret;
            }
            temp = temp->urm;
        }
    }
    return totalValue;
}

void modifyPrice(Node** lists, int size, const char* date, int newPrice) 
{
    for (int i = 0; i < size; i++) 
    {
        Node* temp = lists[i];
        while (temp != NULL) 
        {
            Consultatie* consultatie = temp->info;
            if (strcmp(consultatie->data, date) == 0) 
            {
                consultatie->pret = newPrice;
                printf("\nPrice for consultation on %s has been modified to %d\n", date, newPrice);
                return; // Assuming only one consultation per date
            }
            temp = temp->urm;
        }
    }
    printf("\nNo consultation found for the date %s!\n", date);
}

void deleteConsultationsByDiagnosis(Node** lists, int size, const char* diagnosis) 
{
    for (int i = 0; i < size; i++) 
    {
        Node** current = &lists[i];
        while (*current != NULL) 
        {
            Consultatie* consultatie = (*current)->info;
            if (strcmp(consultatie->diagnostic, diagnosis) == 0) 
            {
                Node* temp = *current;
                *current = temp->urm;
                freeConsultatie(temp->info);
                free(temp);
            }
            else {
                current = &((*current)->urm);
            }
        }
    }
}

Consultatie** getConsultationsAboveThreshold(Node** lists, int size, int threshold) 
{
    Consultatie** vector = (Consultatie**)malloc(sizeof(Consultatie*));
    int index = 0;

    for (int i = 0; i < size; i++) 
    {
        Node* temp = lists[i];
        while (temp != NULL) 
        {
            Consultatie* consultatie = temp->info;
            if (consultatie->pret > threshold) 
            {
                vector[index] = consultatie;
                index++;
                // Increase the size of the vector
                vector = (Consultatie**)realloc(vector, (index + 1) * sizeof(Consultatie*));
            }
            temp = temp->urm;
        }
    }

    // Set the last element of the vector to NULL to mark the end
    vector[index] = NULL;
    return vector;
}