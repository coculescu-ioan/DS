#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define LINE_SIZE 256
#define INITIAL_HEAP_SIZE 10

typedef struct {
    char* fileIdentifier;
    unsigned int fileSize;
    bool readOnly;
    char createdDate[11]; // including space for null terminator
    char* fileOwner;
} File;

typedef struct BSTNode {
    File* data;
    struct BSTNode* left;
    struct BSTNode* right;
} BSTNode;

typedef struct {
    File** files; // Array of pointers to files
    int size;     // Current number of elements in the heap
    int capacity; // Max capacity of the heap array
} MaxHeap;

File* createFile(const char*, unsigned int, bool, const char*, const char*);
void printFile(File*);
void freeFile(File*);

BSTNode* createNode(File*);
BSTNode* insertBST(BSTNode*, File*);
void printBST(BSTNode*);
void freeBST(BSTNode*);

void printFilesWithDateGreater(BSTNode*, const char*);
int countReadOnlyFiles(BSTNode*);
float averageFileSize(BSTNode*, int*);
void updateFileSizeByIdentifier(BSTNode*, const char*);

MaxHeap* createHeap(int capacity);
void insertHeap(MaxHeap*, File*);
void printHeap(MaxHeap*);
void freeHeap(MaxHeap*);

void heapifyDown(MaxHeap*, int);
void heapifyUp(MaxHeap*, int);
void extractFilesToHeap(BSTNode*, const char*, MaxHeap*);

int main()
{
    /*1. Create a Binary Search Tree to store at least 7 entries for which data is taken from a text file.
    Insertion should be implemented recursively, and the search key should be the file identifier.*/
    BSTNode* root = NULL;

    FILE* fp = fopen("files.txt", "r");
    if (!fp) return -1;

    char buffer[LINE_SIZE];
    char* token;
    char delimiters[] = ",\n";

    char fileIdentifier[LINE_SIZE];
    unsigned int fileSize;
    bool readOnly;
    char createdDate[11];
    char fileOwner[LINE_SIZE];

    while (fgets(buffer, LINE_SIZE, fp))
    {
        token = strtok(buffer, delimiters);
        strcpy(fileIdentifier, token);

        token = strtok(NULL, delimiters);
        fileSize = atoi(token);

        token = strtok(NULL, delimiters);
        readOnly = (strcmp(token, "True") == 0);

        token = strtok(NULL, delimiters);
        strncpy(createdDate, token, 10);
        createdDate[10] = '\0'; 

        token = strtok(NULL, delimiters);
        strcpy(fileOwner, token);

        File* newFile = createFile(fileIdentifier, fileSize, readOnly, createdDate, fileOwner);
        root = insertBST(root, newFile);
    }

    fclose(fp);

    printf("In-Order Traversal of BST:\n");
    printBST(root);

    /*2. Write the function that prints only the files that have the created date greater than 
    a given value specified as a parameter.*/
    printf("\nFiles with created date greater than 2024/01/01:\n");
    printFilesWithDateGreater(root, "2024/01/01");

    /*3. Write the function that counts the number of files that have read only access. 
    The function should return the value in the main function and results should be displayed at the console.*/
    int readOnlyCount = countReadOnlyFiles(root);
    printf("\nCount of Read-Only files: %d\n",readOnlyCount);

    /*4. Write the function that determines the average size for all the files in the structure. 
    The value returned should be listed on the console in the main function.*/
    int totalFiles = 0;
    float avgSize = averageFileSize(root, &totalFiles);
    printf("\nAverage file size: %.2f\n", avgSize);

    /*5. Write the function that searches for a specific file identifier given as a parameter. 
    The file size should be increased by 100% the current size for all the files that it processes 
    until it finds the given key. If no key is found, then all the files should be updated.*/
    char id[] = "fileD";
    printf("\nUpdating file sizes until identifier %s is found:\n", id);
    updateFileSizeByIdentifier(root, id);
    printBST(root);

    /*6. Write the function for saving the files of a specific owner, given as a parameter, in a Heap structure 
    in which the priority key is represented by the file size. The new data structure doesn’t share data with 
    the Binary Search Tree and all the nodes should be displayed (array manipulation) in the main function after 
    complete creation of the structure.*/
    const char* ownerToExtract = "john";
    MaxHeap* heap = createHeap(INITIAL_HEAP_SIZE);
    extractFilesToHeap(root, ownerToExtract, heap);

    freeBST(root);

    printf("\nSaving files owned by %s:\n", ownerToExtract);
    printHeap(heap);

    freeHeap(heap);

    return 0;
}

File* createFile(const char* id, unsigned int size, bool read_only, const char* date, const char* owner)
{
    File* newFile = (File*)malloc(sizeof(File));
    if (newFile)
    {
        newFile->fileIdentifier = (char*)malloc(strlen(id) + 1);
        if (newFile->fileIdentifier)
            strcpy(newFile->fileIdentifier, id);

        newFile->fileSize = size;
        newFile->readOnly = read_only;

        strncpy(newFile->createdDate, date, 10);
        newFile->createdDate[10] = '\0'; // ensure null terminator

        newFile->fileOwner = (char*)malloc(strlen(owner) + 1);
        if (newFile->fileOwner)
            strcpy(newFile->fileOwner, owner);
    }
    return newFile;
}

void printFile(File* file)
{
    if (file)
    {
        printf("\tFile ID: %s, Size: %u, Read Only: %s, Date: %s, Owner: %s\n",
            file->fileIdentifier,
            file->fileSize,
            file->readOnly ? "True" : "False",
            file->createdDate,
            file->fileOwner);
    }
}

void freeFile(File* file)
{
    if (file)
    {
        free(file->fileIdentifier);
        free(file->fileOwner);
        free(file);
    }
}

BSTNode* createNode(File* file)
{
    BSTNode* newNode = (BSTNode*)malloc(sizeof(BSTNode));
    if (newNode)
    {
        newNode->data = file;
        newNode->left = newNode->right = NULL;
    }
    return newNode;
}

BSTNode* insertBST(BSTNode* root, File* file)
{
    if (root == NULL)
        return createNode(file);

    int cmp = strcmp(file->fileIdentifier, root->data->fileIdentifier);

    if (cmp < 0)
        root->left = insertBST(root->left, file);
    else if (cmp > 0)
        root->right = insertBST(root->right, file);

    return root;
}

void printBST(BSTNode* root)
{
    if (root)
    {
        // Inorder traversal (L/Root/R)
        printBST(root->left);
        printFile(root->data);
        printBST(root->right);
    }
}

void freeBST(BSTNode* root)
{
    if (root)
    {
        freeBST(root->left);
        freeBST(root->right);
        freeFile(root->data);
        free(root);
    }
}

void printFilesWithDateGreater(BSTNode* root, const char* date)
{
    if (root)
    {
        printFilesWithDateGreater(root->left, date);
        if (strcmp(root->data->createdDate, date) > 0)
            printFile(root->data);
        printFilesWithDateGreater(root->right, date);
    }
}

int countReadOnlyFiles(BSTNode* root)
{
    if (!root) return 0;

    int count = (root->data->readOnly ? 1 : 0);
    count += countReadOnlyFiles(root->left);
    count += countReadOnlyFiles(root->right);

    return count;
}

float averageFileSize(BSTNode* root, int* count)
{
    if (!root) return 0;

    float totalSize = root->data->fileSize;
    (*count)++;

    totalSize += averageFileSize(root->left, count);
    totalSize += averageFileSize(root->right, count);

    return totalSize / *count;
}

void updateFileSizeByIdentifier(BSTNode* root, const char* identifier)
{
    if (!root) return;

    // Update the file size by 100%
    root->data->fileSize *= 2;

    // Stop if the identifier matches
    if (strcmp(root->data->fileIdentifier, identifier) == 0) return;

    updateFileSizeByIdentifier(root->left, identifier);
    updateFileSizeByIdentifier(root->right, identifier);
}

MaxHeap* createHeap(int capacity)
{
    MaxHeap* heap = (MaxHeap*)malloc(sizeof(MaxHeap));
    if (heap)
    {
        heap->files = (File**)malloc(sizeof(File*) * capacity);
        heap->size = 0;
        heap->capacity = capacity;
    }
    return heap;
}

void insertHeap(MaxHeap* heap, File* file)
{
    if (heap->size >= heap->capacity)
    {
        // Resize the heap's underlying array if it's full
        heap->capacity *= 2;
        heap->files = (File**)realloc(heap->files, heap->capacity * sizeof(File*));
    }

    File* newFile = createFile(file->fileIdentifier, file->fileSize, file->readOnly, file->createdDate, file->fileOwner);
    heap->files[heap->size] = newFile;
    heap->size++;

    heapifyUp(heap, heap->size - 1);
}

void heapifyUp(MaxHeap* heap, int index)
{
    while (index > 0)
    {
        int parentIndex = (index - 1) / 2;
        if (heap->files[index]->fileSize <= heap->files[parentIndex]->fileSize)
        {
            break; // Heap property is satisfied
        }
        // Swap with the parent
        File* temp = heap->files[index];
        heap->files[index] = heap->files[parentIndex];
        heap->files[parentIndex] = temp;

        index = parentIndex;
    }
}

void heapifyDown(MaxHeap* heap, int index)
{
    while (index < heap->size / 2)
    {
        int leftChild = 2 * index + 1;
        int rightChild = 2 * index + 2;
        int largerChild = leftChild;

        if (rightChild < heap->size && heap->files[rightChild]->fileSize > heap->files[leftChild]->fileSize)
        {
            largerChild = rightChild;
        }

        if (heap->files[index]->fileSize >= heap->files[largerChild]->fileSize)
        {
            break; // Heap property is satisfied
        }

        // Swap with the larger child
        File* temp = heap->files[index];
        heap->files[index] = heap->files[largerChild];
        heap->files[largerChild] = temp;

        index = largerChild;
    }
}

void printHeap(MaxHeap* heap)
{
    printf("Heap contents (size: %d):\n", heap->size);
    for (int i = 0; i < heap->size; i++)
    {
        printf("%d: ", i);
        printFile(heap->files[i]);
    }
}

void freeHeap(MaxHeap* heap)
{
    if (heap)
    {
        free(heap->files);
        free(heap);
    }
}

void extractFilesToHeap(BSTNode* root, const char* owner, MaxHeap* heap)
{
    if (root)
    {
        // Traverse the left subtree
        extractFilesToHeap(root->left, owner, heap);

        // Check if the current node's file belongs to the specified owner
        if (strcmp(root->data->fileOwner, owner) == 0)
        {
            insertHeap(heap, root->data);
        }

        // Traverse the right subtree
        extractFilesToHeap(root->right, owner, heap);
    }
}

