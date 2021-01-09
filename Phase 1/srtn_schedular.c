#include "headers.h"
#include "priority_queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define null 0
#define MAXCHAR 300


void readFromFileAndFillList(struct LinkedList* list);
void loadProcess(char str[], struct LinkedList* list);
void addNodeToLikedlistEnd(struct LinkedList* list, struct Process processToAdd);
bool startsWith(const char *a, const char *b);

/*---------------------------------------MAIN STARTS HERE------------------------------------------------*/


int main()
{

    // 1. Read the input files.
    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    // 3. Initiate and create the scheduler and clock processes.
    // 4. Use this function after creating the clock process to initialize clock
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    // 6. Send the information to the scheduler at the appropriate time.
    // 7. Clear clock resources


    struct LinkedList ProcessList = {NULL, NULL, 0};
    struct LinkedList ready_queue = {NULL, NULL, 0};

    initClk();
    int x = getClk();       // gets time
    printf("current time is %d\n", x);


    // Assume for now that process list is going to be filled
    // Will use a message queue to get the variables and create process structs

    readFromFileAndFillList(&ProcessList);

    // SRTN LOGIC
    // if single process, run it noe
    // every clk check if new process came
    // if a process came and has run time less than current process rem time
    // then sleep the first process 
    // and fork the new one
    // & so on   


    struct Node * iterator = ProcessList.head;
    struct Node * current = NULL;
    struct Node * temp = NULL;
    int timer;
    bool currently_occupied = false;
    int id_occupied;
    int arrived;
    while(1)
    {
        arrived = 0;
        timer = getClk();
        pid_t pid;
        // Put process in my ready queue of processes
        // while not if so that is several processes arrive ta same time
        while(iterator->processInfo.arrivalTime == timer)
        {
            insert_srtn(&ready_queue, iterator->processInfo);
            bubbleSort_srtn(&ready_queue);
            iterator = iterator->next; 
            arrived++;
        }
    
        // TEMP -> TO ITERATE ON READY_QUEUE
        temp = ready_queue.head; // MOMKEN MALHASH LOZZOOM 
        //CURRENT TELLS IF A PROCESS IS BEING HANDLED RN
        // If current is null, or not, --> handle
        if (!current && !currently_occupied)         
        {
            current = ready_queue.head;
            pid = fork();

            if(pid == 0)
            {
                //printf("current time is %d\tprocess with id %d is now forked\n", x, previous_head->processInfo.id);
                    
                    //Prepare parameters to be sent to process
                    char id_param [MAXCHAR] ; 
                    sprintf(id_param, "%d", current-> processInfo.id);
                    char running_time_param [MAXCHAR];
                    sprintf(running_time_param, "%d", current-> processInfo.executionTime);
                    char arrival_time_param [MAXCHAR];
                    sprintf(arrival_time_param, "%d", current-> processInfo.executionTime);
                    //
                    
                printf("IN SRTN, current time is %d\tprocess with id %d is now forked\n", timer, current->processInfo.id);
                return execl("./process.out", "./process.out", id_param, arrival_time_param, running_time_param,(char*)NULL);


            }
            else if(pid == -1)
            {
                printf("\nCould not create process!");
                sleep(2);
                exit(-1);
            }
            else                // nothing to do if parent right?
            {
                /* code */
            }
            


        }
        else if (current)
        {

        }
    







    }

    





}

//*--------------------------------------- FUNCTIONS START HERE------------------------------------------------*/
//struct Process pcs[10];             // global


// Called by loadProcess 
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
//Load process called by readfromFIleAndFillList
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


bool startsWith(const char *a, const char *b)
{
   if(strncmp(a, b, strlen(b)) == 0) return 1;
   return 0;
}


// Called initially in main to fill list
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
