#include "headers.h"
#include "priority_queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define null 0
#define MAXCHAR 300

/*--------------------------------------- FUNCTIONS START HERE------------------------------------------------*/
//struct Process pcs[10];             // global

void readFromFileAndFillList(struct LinkedList* list);
void loadProcess(char str[], struct LinkedList* list);
void addNodeToLikedlistEnd(struct LinkedList* list, struct Process processToAdd);
bool startsWith(const char *a, const char *b);


void readFromFileAndFillList(struct LinkedList* list)
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

bool startsWith(const char *a, const char *b)
{
   if(strncmp(a, b, strlen(b)) == 0) return 1;
   return 0;
}

void addNodeToLikedlistEnd(struct LinkedList* list, struct Process processToAdd)
{
    struct Node * nodeToAdd = (struct Node *) malloc(sizeof(struct Node));  //create new node and assign the process to it.
    nodeToAdd->processInfo = processToAdd;
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

    list->size++;
}

void loadProcess(char str[], struct LinkedList* list)
{
    struct Process newProcess;
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
            newProcess.arrivalTime = atoi(my_string);
            i ++;
        }
        else if (i == 2)
        {
            newProcess.executionTime = atoi(my_string);
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


/*---------------------------------------MAIN STARTS HERE------------------------------------------------*/



int main()
{
    




}



/*---------------------------------------ARCHIVED CODE START HERE------------------------------------------------*/




// PREVIOUS EDIT, KEEP IF NEEDED
/*
void sort(int noofpcs)
{
     for (int i = 0; i < noofpcs-1; i++)
    {
        for (int j = 0; j < noofpcs-i-1; j++)
        {
            if(pcs[j].remainingTime > pcs[j+1].remainingTime)
            {
                struct Process temp;                        /// Is this possible? to put a struct = struct?
                temp = pcs[j];
                pcs[j] = pcs[j+1];
                pcs[j+1] = temp;
            }
        }
        
    }
}


//PREVIOUSLY IN MAIN, WHEN IMPLEMENTED AS ARRAY


/*THIS FILE IS A GENRAL DEMO OF WHAT SRTN SCHEDULAR IS LIKE, NOT THE FINAL OR WORKING IMPLEMENTATION*/
    /*
    int process_id, arrival_time, run_time, num_processes;

    // Assume we have a DS array with number of processes = num_processes
    // each with ID, AT, & RT

    num_processes= 10;
    int processes [10];
    int ids[10];
    int ats[10]; int rts[10];

    

    for (int i = 0; i < num_processes-1; i++)
    {
        pcs[i].id = ids[i];
        pcs[i].arrivalTime = ats[i];
        pcs[i].executionTime = rts[i];
    }


    //Bubble sort according to execution/running time

    for (int i = 0; i < num_processes-1; i++)
    {
        for (int j = 0; j < num_processes-i-1; j++)
        {
            if(pcs[j].executionTime > pcs[j+1].executionTime)
            {
                struct Process temp;                        /// Is this possible? to put a struct = struct?
                temp = pcs[j];
                pcs[j] = pcs[j+1];
                pcs[j+1] = temp;
            }
        }
        
    }

    //Simulation starts
    int index_of_running;
    while(1)
    {
        int t = getClk;
        printf("This time cycle is %d \n",t);
        bool istherenewarrival = false;
        for (int i = 0; i < num_processes-1; i++)
        {
            if (pcs[i].arrivalTime == t )             // if process is to arrive in this time slot
            {
                istherenewarrival = true;
                index_of_running = i;
                pcs[i].waitingTime = t - pcs[i].arrivalTime; // time waited to start
                pcs[i].isRunning = true;
                pcs[i].remainingTime--;

                if (pcs[i].remainingTime == 0)
                {
                    pcs[i].isFinished = true;
                }
                pcs[i].isRunning = false;
            }
            sort(num_processes);            // sorts processes according to new remaining time
        }

        if(!istherenewarrival)
        {

        }


    }
  
  */
