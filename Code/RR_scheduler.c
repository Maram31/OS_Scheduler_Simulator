#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <math.h>
#include "headers.h"
#include "circular_queue.h"
#define null 0
#define MAXCHAR 300


//Initial version of Round Robin scheduling algorithm with no process communication
void readFromFileAndFillList(struct LinkedList* list);
void loadProcess(char str[], struct LinkedList* list);
void addNodeToLikedlistEnd(struct LinkedList* list, struct processData processToAdd);
bool startsWith(const char *a, const char *b);
void child_handler(int signum);
void finish_handler(int signum);



int main(int argc, char * argv[])
{

    //Output files
    FILE * schedulerLogFile;
    schedulerLogFile = fopen("scheduler.log", "w");
    FILE * schedulerPerfFile;
    schedulerPerfFile = fopen("scheduler.perf", "w");
    fprintf(schedulerLogFile,"#At time x process y state arr w total z remain y wait k\n");
    
    signal(SIGUSR1, finish_handler);
    //signal(SIGCHLD, child_handler);
    //
    //Temporary till process generator works
    struct LinkedList processes = {NULL, NULL, 0};
    //

    //Process ID for forking
    pid_t pid;

    //Ready and running queues to help in context switching
    struct LinkedList ready_queue = {NULL, NULL, 0};
    struct LinkedList running_queue = {NULL, NULL, 0};
    struct LinkedList finished_queue = {NULL, NULL, 0};
    //
    //Temporary till process generator works
    readFromFileAndFillList(&processes);
    //

    //Time quantum used for Round Robin
    int time_quantum;
    //temp value
    time_quantum = 2;
    //

    // Use this function after creating the clock process to initialize clock
    initClk();
    // To get time use this
    int x = getClk();
    //

    //Debug
    printf("RR current time is %d\n", x);
    //
    //Variable to loop every 1 second
    int prev_time = x;
    //
    
    //Pointer to first process in linked list (Will be removed when process generator is finished)
    struct Node* current_node = processes.head;
    //


    struct Node * previous_head = NULL;//malloc(sizeof(struct Node));
    struct Node * current_head = NULL;
    
    int flag_first_time = 0;

    int start_time_prev_process = -1;

    //Logic of algorithm
    while (1)
    {
        //printf("here\n");
        //printf("While current time is %d\n", x);
        //Puts a process in the ready queue when it arrives (ie. arrival time = current time) (Will be removed when process generator is finished)
        if(current_node != NULL && x >= current_node ->processInfo.arrivaltime)
        {
            //printf("current time is %d\n", x);
            //printf("Id: %d\n",current_node->processInfo.id);
            //sleep(1);
            processes.size -= 1;
            insertToQueue(&ready_queue, current_node->processInfo);
            current_node = current_node->next;
        }
        //

        //printf("Here\n");
        //Move process from running state to ready state
        /*if(running_queue.head!= NULL)
            printf("Running queue head start time is %d\n", running_queue.head->processInfo.starttime);
        */
        if(running_queue.head != NULL && (getClk() - running_queue.head->processInfo.previousstart == time_quantum))
        {
            //printf("current time is %d\tprocess with id %d has start time %d\n", x, running_queue.head->processInfo.id, running_queue.head->processInfo.starttime);
            struct Node * temp_node = running_queue.head;
            temp_node->processInfo.remainingTime -= time_quantum;
            removeFromQueue(&running_queue, temp_node->processInfo.id );
            
            //printf("Forked pid remove: %d\n", temp_node -> processInfo.systempid);
            //printf("Debug current time is %d\n", x);
            if(temp_node->processInfo.remainingTime > 0)
            {
                temp_node ->processInfo.previousstop = getClk();
                fprintf(schedulerLogFile, "At time %d process %d stopped arr %d total %d remain %d wait %d\n", getClk(), temp_node ->processInfo.id, temp_node ->processInfo.arrivaltime, temp_node ->processInfo.runningtime, temp_node ->processInfo.remainingTime, temp_node->processInfo.waitingTime);
                insertToQueue(&ready_queue, temp_node->processInfo);
                //kill(temp_node->processInfo.systempid, SIGSTOP);
                kill(temp_node->processInfo.systempid, SIGUSR1);
            }
            else
            {
                int turnaround_time = getClk() - temp_node->processInfo.arrivaltime;
                float weighted_ta = (float)turnaround_time / temp_node -> processInfo .runningtime;  
                temp_node ->processInfo .weightedTA = weighted_ta;
                temp_node ->processInfo.remainingTime = 0;
                insertToQueue(&finished_queue, temp_node -> processInfo);
                fprintf(schedulerLogFile, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n", getClk(), temp_node ->processInfo.id, temp_node ->processInfo.arrivaltime, temp_node ->processInfo.runningtime, temp_node ->processInfo.remainingTime, temp_node->processInfo.waitingTime, turnaround_time, weighted_ta); 
                printf("Goodbye process %d\n", temp_node -> processInfo.id);
            }
            //printf("Hellloo\n");
        }
        //

        current_head = ready_queue.head;

        //Move process from ready state to running state
        if(current_head != NULL && running_queue.head == NULL)//&& running_queue.size == 0)// && (start_time_prev_process == -1 || getClk() - start_time_prev_process >= time_quantum))
        {
            start_time_prev_process = getClk();
            removeFromQueue(&ready_queue, current_head-> processInfo.id );
            //printf("Is starteddddd? %d\n", current_head->processInfo.isStarted);
            flag_first_time = current_head->processInfo.isStarted;
            
            
            current_head -> processInfo.starttime = getClk();
            if(current_head ->processInfo.remainingTime - time_quantum < 0)
            {
                current_head -> processInfo.previousstart = getClk() + current_head ->processInfo.remainingTime - time_quantum;

            }
            else
                current_head -> processInfo.previousstart = getClk() ;
            current_head -> processInfo.isStarted = 1;
            
            previous_head = current_head;
            current_head = ready_queue.head;

            //current_head ->processInfo.isRunning = 1;
            //printf("Is started? %d\n", previous_head->processInfo.isStarted);
            if(flag_first_time == 0)
            {
                pid=fork();
                
                if(pid == 0){
                    //printf("current time is %d\tprocess with id %d is now forked\n", x, previous_head->processInfo.id);
                    
                    //Prepare parameters to be sent to process
                    char id_param [MAXCHAR] ; 
                    sprintf(id_param, "%d", previous_head -> processInfo.id);
                    char running_time_param [MAXCHAR];
                    sprintf(running_time_param, "%d", previous_head -> processInfo.runningtime);
                    char arrival_time_param [MAXCHAR];
                    sprintf(arrival_time_param, "%d", previous_head -> processInfo.arrivaltime);
                    //
                    char start_time_param [MAXCHAR];
                    sprintf(start_time_param, "%d", getClk());
                    //fprintf(schedulerLogFile, "At time %d process %s started arr %s total %s remain %s wait %d\n", getClk(), id_param, arrival_time_param, running_time_param, running_time_param, 0);
                    //fprintf(schedulerLogFile, "bombom\n");
                    //fprintf(schedulerLogFile,"#At time x process y state arr w total z remain y wait k\n");
                    //Start the process and give it the required parameters
                    return execl("./process.out", "./process.out", id_param, arrival_time_param, running_time_param, start_time_param,(char*)NULL);
                }
                else if(pid == -1)
                {
                    printf("\nClock Initialization Error\n");
                    exit(-1); 
                }
                else
                {
                    
                    //current_head ->processInfo.isRunning = true;
                    previous_head -> processInfo.starttime = getClk();
                    previous_head ->processInfo.waitingTime = getClk() - previous_head->processInfo.arrivaltime;
                    //printf("Forked pid insert: %d\n", pid);
                    previous_head->processInfo.systempid = pid;
                    fprintf(schedulerLogFile, "At time %d process %d started arr %d total %d remain %d wait %d\n", getClk(), previous_head ->processInfo.id, previous_head ->processInfo.arrivaltime, previous_head ->processInfo.runningtime, previous_head ->processInfo.runningtime, previous_head->processInfo.waitingTime);
                    insertToQueue(&running_queue, previous_head->processInfo);
                }
            }
            else
            {
                previous_head->processInfo.waitingTime += getClk() - (previous_head->processInfo.previousstop) ;
                fprintf(schedulerLogFile, "At time %d process %d resumed arr %d total %d remain %d wait %d\n", getClk(), previous_head ->processInfo.id, previous_head ->processInfo.arrivaltime, previous_head ->processInfo.runningtime, previous_head ->processInfo.remainingTime, previous_head->processInfo.waitingTime);
                insertToQueue(&running_queue, previous_head->processInfo);
                kill(previous_head->processInfo.systempid, SIGCONT);
                //kill(previous_head->processInfo.systempid, SIGUSR2);
                //printf("Resume\n");
            }
            

        }

        /*
        //Debug
        if (current_node == NULL)
        {
            current_node = ready_queue.head;
            while (current_node != NULL)
            {
                printf("Id: %d\n",current_node->processInfo.id);
                current_node = current_node->next;
                if(current_node == ready_queue.head)
                    break;
                
            }
            break;
        }
        */
        //Debug
        struct Node * temp_node_debug = running_queue.head;
        if (temp_node_debug != NULL)
        {
            //temp_node_debug = ready_queue.head;
            while (temp_node_debug != NULL)
            {
                //printf("Running queue\tId: %d\tTime: %d\n",temp_node_debug->processInfo.id, getClk());
                temp_node_debug = temp_node_debug->next;
                if(temp_node_debug == running_queue.head)
                    break;
                
            }
            //break;
        }
        
        

       //wait for a second to pass
        while(getClk() - prev_time == 0);
        x = getClk();
        prev_time = x;

        //Condition to exit the loop (will be modified)
        if(running_queue.head == NULL && ready_queue.head == NULL && processes.size == 0)
        {
            //printf("zzz\n");
            break;
        }
        

    }

    struct Node* temp = finished_queue.head;
    int total_waiting = 0;
    float total_wta = 0;
    float total_runtime = 0;
    int processes_size = finished_queue.size;
    int j = processes_size;
    while(j != 0)
    {
        total_waiting += temp ->processInfo.waitingTime;
        total_wta += temp->processInfo.weightedTA;
        total_runtime += temp ->processInfo.runningtime;
        temp = temp->next;
        j--;
    }
    float avg_waiting = (float)total_waiting / processes_size;
    float avg_ta = total_wta / processes_size;
    float utilization = total_runtime / (getClk() - 1) * 100;

    double sum_dev = 0;
    temp = finished_queue.head;
    j = processes_size;
    while(j != 0)
    {
        sum_dev += (temp -> processInfo.weightedTA - avg_ta) * (temp -> processInfo.weightedTA - avg_ta) ;
        temp = temp -> next;
        j--;
    }
    double std_dev = sqrt(sum_dev / processes_size);

    fprintf(schedulerPerfFile, "CPU utilization = %0.2f%%\n", utilization);
    fprintf(schedulerPerfFile, "Avg WTA = %0.2f\n", avg_ta);
    fprintf(schedulerPerfFile, "Avg Waiting = %0.2f\n", avg_waiting);
    fprintf(schedulerPerfFile, "Std WTA = %0.2f\n", std_dev);
    //printf("holllaaa\n");
    fclose(schedulerLogFile);
    fclose(schedulerPerfFile);


    
}

void finish_handler(int signum)
{
    
    printf("A Process finished execution at %d\n", getClk());
    signal(SIGUSR1, finish_handler);    

}

void child_handler(int signum)
{
    int pid, stat_loc;

    //printf("Child has sent a SIGCHLD signal #%d\n",signum);

    //printf("Child has sent a SIGCHLD signal #%d\n",signum);

    pid = wait(&stat_loc);
    if(pid!= 0)
    {
        if(!(stat_loc & 0x00FF))
            printf("Process %d finished execution\tcurrent time is %d\nTerminating...\n", stat_loc>>8, getClk());
    }
    signal(SIGCHLD, child_handler);    

}


bool startsWith(const char *a, const char *b)
{
   if(strncmp(a, b, strlen(b)) == 0) return 1;
   return 0;
}

void addNodeToLikedlistEnd(struct LinkedList* list, struct processData processToAdd)
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

    list->size ++;
}

void loadProcess(char str[], struct LinkedList* list)
{
    struct processData newProcess;
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
            newProcess.remainingTime = atoi(my_string);//Important
            i ++;
        }
        else if (i == 3)
        {
            newProcess.priority = atoi(my_string);
            i ++;
        }
        //newProcess.isRunning = false;
        
		ptr = strtok(NULL, delim);
        free(my_string);
	}
    newProcess.isStarted = 0;

    addNodeToLikedlistEnd(list, newProcess);
}


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

