#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_SIZE 256

typedef struct 
{
    unsigned int invoiceNumber;
    char* issueDate;
    char* beneficiary;
    float amount;
    unsigned int dueDays;
} Invoice;

typedef struct DoublyLinkedListNode 
{
    Invoice* data;
    struct DoublyLinkedListNode* prev;
    struct DoublyLinkedListNode* next;
} Node;

Invoice* createInvoice(unsigned int, const char*, const char*, float, unsigned int);
void printInvoice(Invoice*);
void freeInvoice(Invoice*);

Node* createNode(Invoice*);
void addNode(Node**, Node*);

void printList(Node*);
void freeList(Node*);
int getListSize(Node*);

float getTotalValueForBeneficiary(Node*, const char*);
void modifyAmountForInvoiceNumber(Node*, const unsigned int, const float);
void deleteInvoicesDueDaysLessThan(Node**, const unsigned int);
Invoice** getInvoicesIssuedAfterDate(Node*, const char*, int);

int main()
{
    /*1. Define a structure Invoice that contains: invoice number (unsigned int), issue date (char*), beneficiary (char*),
    amount to be paid (float), and number of days until due (unsigned int). Create a doubly linked list with at least 5
    invoices whose data is read from a text file. The structure is created by repeated calls to an insertion function, 
    and the successful creation is verified by traversing the list in both directions. (1 p)*/

    FILE* fp = fopen("invoices.txt", "r");
    if (!fp) return -1;

    Node* head = NULL;

    char buffer[LINE_SIZE];
    char* token;
    char delimiters[] = ",\n";

    unsigned int invoiceNumber;
    char issueDate[LINE_SIZE];
    char beneficiary[LINE_SIZE];
    float amount;
    unsigned int dueDays;

    while (fgets(buffer, LINE_SIZE, fp))
    {
        token = strtok(buffer, delimiters);
        invoiceNumber = atoi(token);

        token = strtok(NULL, delimiters);
        strcpy(issueDate, token);

        token = strtok(NULL, delimiters);
        strcpy(beneficiary, token);

        token = strtok(NULL, delimiters);
        amount = atof(token);

        token = strtok(NULL, delimiters);
        dueDays = atoi(token);

        Invoice* invoice = createInvoice(invoiceNumber, issueDate, beneficiary, amount, dueDays);
        Node* node = createNode(invoice);
        addNode(&head, node);
    }

    fclose(fp);

    printList(head);

    /*2. Implement a function that determines the total value of invoices issued 
    to a specific beneficiary, specified as an input parameter to the function. (1 p)*/
    char inputBeneficiary[] = "John Doe";
    float totalValue = getTotalValueForBeneficiary(head, inputBeneficiary);
    printf("\nThe total value of invoices issued to %s is: %.2f\n", inputBeneficiary, totalValue);

    /*3. Implement a function that modifies the payment amount of an invoice specified by 
    the invoice number in the function parameter list. The new payment amount is also specified 
    in the function parameter list. (1 p)*/
    unsigned int inputInvoiceNumber = 2;
    float newAmount = 100;
    modifyAmountForInvoiceNumber(head, inputInvoiceNumber, newAmount);
    printList(head);

    /*4. Implement a function that deletes invoices from the doubly linked list for which 
    the number of days until due is less than a threshold specified as a parameter. (1.5 p)*/
    unsigned int dueDaysThreshold = 30;
    deleteInvoicesDueDaysLessThan(&head, dueDaysThreshold);
    printList(head);

    /*5. Implement a function that returns an array of invoices issued after a certain calendar date 
    specified as an input parameter to the function. The values are taken from the doubly linked list 
    created above, and no heap memory is shared between the two structures. (1.5 p)*/
    char date[] = "2023-05-03";
    int size = getListSize(head);

    Invoice** invoices = getInvoicesIssuedAfterDate(head, date, size);

    freeList(head);

    printf("\nThe invoices issued after %s:\n", date);
    for (int i = 0; i < size && invoices[i]; i++)
        printInvoice(invoices[i]);

    for (int i = 0; i < size && invoices[i]; i++)
        freeInvoice(invoices[i]);
    free(invoices);

    return 0;
}

Invoice* createInvoice(unsigned int invoiceNumber, const char* issueDate, const char* beneficiary, float amount, unsigned int dueDays)
{
    Invoice* invoice = (Invoice*)malloc(sizeof(Invoice));
    if (invoice) 
    {
        invoice->invoiceNumber = invoiceNumber;

        invoice->issueDate = (char*)malloc(strlen(issueDate) + 1);
        if (invoice->issueDate)
            strcpy(invoice->issueDate, issueDate);

        invoice->beneficiary = (char*)malloc(strlen(beneficiary) + 1);
        if (invoice->beneficiary)
            strcpy(invoice->beneficiary, beneficiary);

        invoice->amount = amount;
        invoice->dueDays = dueDays;
    }
    return invoice;
}

void printInvoice(Invoice* invoice)
{
    printf("\tInvoice number: %d, issued on: %s, beneficiary: %s, payment amount: %.2f, due in: %d days\n", invoice->invoiceNumber, invoice->issueDate, invoice->beneficiary, invoice->amount, invoice->dueDays);
}

void freeInvoice(Invoice* invoice)
{
    free(invoice->issueDate);
    free(invoice->beneficiary);
    free(invoice);
}

Node* createNode(Invoice* invoice)
{
    Node* node = (Node*)malloc(sizeof(Node));
    if (node) 
    {
        node->data = invoice;
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
        // Add at the beginning of the list, O(1)
        node->next = *head;
        (*head)->prev = node;

        *head = node;
    }
}

void printList(Node* head)
{
    if (head == NULL) return;

    printf("\nTraversing List FORWARDS:\n");
    while (head->next)
    {
        printInvoice(head->data);
        head = head->next;
    }
    Node* tail = head;
    printInvoice(tail->data);

    printf("\nTraversing List BACKWARDS:\n");
    while (tail)
    {
        printInvoice(tail->data);
        tail = tail->prev;
    }
}

void freeList(Node* head)
{
    while (head)
    {
        Node* temp = head;
        head = head->next;
        freeInvoice(temp->data);
        free(temp);
    }
}

int getListSize(Node* head) 
{
    int size = 0;
    while (head)
    {
        size++;
        head = head->next;
    }
    return size;
}

float getTotalValueForBeneficiary(Node* head, const char* beneficiary)
{
    float result = 0;
    while (head)
    {
        if (strcmp(head->data->beneficiary, beneficiary) == 0)
        {
            result += head->data->amount;
        }
        head = head->next;
    }
    return result;
}

void modifyAmountForInvoiceNumber(Node* head, const unsigned int invoiceNumber, const float newAmount)
{
    while (head)
    {
        if (head->data->invoiceNumber == invoiceNumber)
        {
            printf("\nSuccesfully updated payment amount for invoice number %d from %.2f to %.2f!\n", invoiceNumber, head->data->amount, newAmount);
            head->data->amount = newAmount;
            return; // Assuming invoice number is the UID
        }
        head = head->next;
    }
    printf("\nInvoice number %d not found!\n", invoiceNumber);
}

void deleteInvoicesDueDaysLessThan(Node** head, const unsigned int dueDays)
{
    int count = 0;

    Node* current = *head;

    while (current && current->data->dueDays < dueDays) 
    {
        Node* temp = current;
        current = current->next;
        if (current) current->prev = NULL;

        freeInvoice(temp->data);
        free(temp);

        count++;
    }

    *head = current;

    while (current)
    {
        if (current->data->dueDays < dueDays) 
        {
            Node* temp = current;
            if (current->prev) current->prev->next = current->next;
            if (current->next) current->next->prev = current->prev;
            current = current->next;

            freeInvoice(temp->data);
            free(temp);

            count++;
        }
        else
            current = current->next;
    }
    printf("\n%d invoices having the number of days until due less than %d were deleted!\n", count, dueDays);

}

Invoice** getInvoicesIssuedAfterDate(Node* head, const char* issuedDate, int size)
{
    Invoice** invoices = (Invoice**)malloc(size * sizeof(Invoice*));
    if (invoices)
    {
        // Memory initialization
        memset(invoices, 0, size * sizeof(Invoice*));

        int index = 0;
        while (head)
        {
            // Compare dates: strcmp returns a positive value if the first non-matching character 
            // has a higher value in _Str1 (current invoice date) than in _Str2 (threshold date).
            if (strcmp(head->data->issueDate, issuedDate) > 0) 
            {
                invoices[index++] = createInvoice(head->data->invoiceNumber, head->data->issueDate, head->data->beneficiary, head->data->amount, head->data->dueDays);
            }
            head = head->next;
        }
    }
    return invoices;
}
