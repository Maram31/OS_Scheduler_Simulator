#include "headers.h"
#define MAXCHAR 300

struct process              //struct to hold the information of each process
{
    int arrivaltime;
    int priority;
    int runningtime;
    int id;
};

struct node                 //struct to gold the information of each node and a pointer to the next and previous nodes in the linkedlist
{
    struct process data;
    struct node * next;
    struct node * previous;
};

struct linkedlist           //likedlist struct
{
	struct node * head;
	struct node * tail;
    int size;
};

void addNodeToLikedlistEnd(struct linkedlist* list, struct process processToAdd)
{
    struct node * nodeToAdd = (struct node *) malloc(sizeof(struct node));  //create new node and assign the process to it.
    nodeToAdd->data = processToAdd;
    nodeToAdd->next = NULL;

    if(list->head == NULL)
    {
        list->head = nodeToAdd;
        list->tail = nodeToAdd;
    }
    else
    {
        list->tail->next = nodeToAdd;
        nodeToAdd->previous = list->tail;
        list->tail = list->tail->next;
    }

    list->size ++;
}

void clearResources(int);
void readFromFileAndFillList(struct linkedlist* list);
void writeInFile();
void chooseAlgorithm(short* algorithmNumber);

int main(int argc, char * argv[])
{
    pid_t pid;
    signal(SIGINT, clearResources);
    struct linkedlist processes = {NULL, NULL, 0};

    // 1. Read the input files.
    readFromFileAndFillList(&processes);

    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    short algorithmNumber;
    chooseAlgorithm(&algorithmNumber);

    // 3. Initiate and create the scheduler and clock processes.
    pid=fork();
    if(pid == 0){
        printf("\nClock Initialization Succes\n"); 
        return execl("./clk.out", "./clk.out", (char*) NULL);
    }
    else if(pid == -1)
    {
        printf("\nClock Initialization Error\n");
        exit(-1); 
    }
    else
    {
        pid=fork();
        if(pid == 0){
            
            printf("\nScheduler Initialization Succes\n");
            if(algorithmNumber == 1) return execl("./scheduler.out", "./scheduler.out", "1",(char*) NULL);    
            else if(algorithmNumber == 2) return execl("./scheduler.out", "./scheduler.out", "2",(char*) NULL);   
            else if(algorithmNumber == 3) return execl("./scheduler.out", "./scheduler.out", "3",(char*) NULL);
            exit(-1);      
        }
        else if(pid == -1)
        {
            printf("\nScheduler Initialization Error\n");
            exit(-1); 
        }
    }
    // 4. Use this function after creating the clock process to initialize clock
    initClk();
    // To get time use this
    int x = getClk();
    printf("current time is %d\n", x);
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    // 6. Send the information to the scheduler at the appropriate time.
    while (processes.head != NULL)
    {
        x = getClk();
        printf("current time is %d\n", x);
        sleep(5);
    }

    // 7. Clear clock resources
    destroyClk(true);
}

void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
}

bool startsWith(const char *a, const char *b)
{
   if(strncmp(a, b, strlen(b)) == 0) return 1;
   return 0;
}

void chooseAlgorithm(short* algorithmNumber)
{
    printf("Please enter the number of the required scheduling algorithm: \n");
    printf("1- Non-preemptive Highest Priority First (HPF) \n");
    printf("2- Shortest Remaining time Next (SRTN) \n");
    printf("3- Round Robin (RR) \n");

    short algoNum = -1;
    printf("Algorithm Number: ");
    scanf("%hd", &algoNum);
    while(algoNum!=1 && algoNum!=2 && algoNum!=3)
    {
        printf("Please enter a valid algorithm number(1 or 2 or 3): ");
        scanf("%hd", &algoNum);
    }

    *algorithmNumber = algoNum;

    switch (algoNum)
    {
    case 1:
        printf("\nChosen Algorithm: Non-preemptive Highest Priority First (HPF) \n");
        break;
    case 2:
        printf("\nChosen Algorithm: Shortest Remaining time Next (SRTN) \n");
        break;
    case 3:
        printf("\nChosen Algorithm: Round Robin (RR) \n");
        break;
    default:
        break;
    }
    
}

void loadProcess(char str[], struct linkedlist* list)
{
    struct process newProcess;
    int init_size = strlen(str);
	char delim[] = "\t";
	char *ptr = strtok(str, delim);
    
    int i = 0;
	while(ptr != NULL)
	{

        char *my_string = malloc(MAXCHAR);
        strcpy(my_string, ptr);
        if(i == 0)
        {
            newProcess.id = atoi(my_string);
            i ++;
        }
        else if (i == 1)
        {
            newProcess.arrivaltime = atoi(my_string);
            i ++;
        }
        else if (i == 2)
        {
            newProcess.runningtime = atoi(my_string);
            i ++;
        }
        else if (i == 3)
        {
            newProcess.priority = atoi(my_string);
            i ++;
        }

		ptr = strtok(NULL, delim);
        free(my_string);
	}

    addNodeToLikedlistEnd(list, newProcess);
}

void readFromFileAndFillList(struct linkedlist* list)
{
    FILE * pFile;
    char str[MAXCHAR];
    pFile = fopen("processes.txt", "r");

    if (pFile == NULL){
        printf("Could not open file");
        return;
    }

    long long index = 0;
    while (fgets(str, MAXCHAR, pFile) != NULL)
    {
        if(!startsWith(str,"#"))
        {
            loadProcess(str,list);
        }
        index ++;
    }
    fclose(pFile);
}

void writeInFile()
{
    
}
