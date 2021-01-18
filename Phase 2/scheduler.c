#include "headers.h"
#include "priority_queue.h"
#include "circular_queue.h"
#include "vector_of_vectors.h"
#include "map.h"
#include <math.h>
#include "math.h"

#define MAXCHAR 300

struct msgbuff
{
    long mtype;
    struct Process P;
};

key_t key_id;
int msgq_id, rec_val;
struct msgbuff message;
bool endReceive = false;
int busy = 0;
int handler_finished = 0;

struct Process currentProcess;
struct Process tempProcess; // nd 56
short algorithmNumber;

float *WTA;    //array to store weighted turnaround for every process
int total_waiting = 0;
float total_wta = 0;
float total_runtime = 0;
int processes_count = 0;
float utilization;
int total_turnaround = 0;
float total_weighted_turnaround = 0;

//Time quantum used for Round Robin
int time_quantum;

int running_removed = 0; //false

void HPF();
void runProcessHPF(int clk);
void handler(int signum);
void recievingHandler(int signum);

void SRTN();
void runProcessSRTN(int clk);
struct Node * ptr_head;
struct Node * current_process_ptr;
struct Process previousProcess;

int RR(int quant);
void RRstats();

struct LinkedList ready_queue = {NULL, NULL, 0};

struct LinkedList waiting_list = {NULL, NULL, 0};
// state 0 for started log
// state 1 for stopped log
// state 2 for finished log
// state 3 for resumed log
void reportProcessToLogFile(struct Process *P, int state, int clk);

FILE *schedulerLogFile;
FILE *schedulerPerfFile;
FILE *memoryLogFile;


//Ready and running queues to help in context switching
//struct LinkedList ready_queue = {NULL, NULL, 0};
struct LinkedList running_queue = {NULL, NULL, 0};
struct LinkedList finished_queue = {NULL, NULL, 0};

struct LinkedList forked_queue = {NULL, NULL, 0};

//Initial version of Round Robin scheduling algorithm with no process communication
void finish_handler(int signum);
void pauser(int signum);

//######################Memory
// Size of vector of pairs
int size;

struct vector_of_vectors arr = {NULL, NULL, 0 };

struct map mp = {NULL};

void Buddy(int s);
int allocate(int s, int process_id);
void deallocate(int id, int process_id);
void print();
//##########################

int main(int argc, char *argv[])
{
    WTA = (float *) malloc(sizeof(float*));

    algorithmNumber = atoi(argv[1]);
    signal(SIGUSR1, handler);
    signal(SIGUSR2, recievingHandler);
    //signal(SIGSTOP, recievingHandler);

    initClk();
    Buddy(1024);
        
    memoryLogFile = fopen("memory.log", "w");
    fprintf(memoryLogFile, "#At time x allocated y bytes for process z from i to j\n");


    schedulerLogFile = fopen("scheduler.log", "w");
    schedulerPerfFile = fopen("scheduler.perf", "w");
    fprintf(schedulerLogFile, "#At time x process y state arr w total z remain y wait k \n");

    key_id = ftok("keyfile", 65);
    msgq_id = msgget(key_id, 0666 | IPC_CREAT);

    if (msgq_id == -1)
    {
        perror("Error in create message queue");
        exit(-1);
    }
    //printf("Scheduler: Message Queue ID = %d\n", msgq_id);

    //To make the scheduler waits until all processes terminates
    pid_t wpid;
    int status = 0;
    while ((wpid = wait(&status)) > 0)
        ;

    if (!strcmp(argv[1], "1"))
    {
        printf("\nScheduler: Non-preemptive Highest Priority First (HPF) \n");
        //TODO
        //Add Algorithm function call
        HPF();
    }

    else if (!strcmp(argv[1], "2"))
    {
        printf("\nScheduler: Shortest Remaining time Next (SRTN) \n");
        //TODO
        //Add Algorithm function 
        SRTN();
    }

    else if (!strcmp(argv[1], "3"))
    {
        printf("\nScheduler: Round Robin (RR) with Quantum %d \n", atoi(argv[2]));
        RR(atoi(argv[2]));
        
    }

    fclose(memoryLogFile);
    
    fclose(schedulerLogFile);
    fclose(schedulerPerfFile);
    destroyClk(false);
    return 1;
}
struct LinkedList ready_queue_SRTN = {NULL, NULL, 0};


void SRTN()
{
    int clk = getClk();       // gets time
    //int last_clk = clk-1;
    //printf("SRTN current time is %d\n", clk);
 
    while (!endReceive || isEmpty(&ready_queue) == 0 || handler_finished == 0)
    {
        ptr_head = ready_queue.head;
       
        clk = getClk();
        if(ptr_head != NULL)
        {
            if(busy == 0)
            {
                runProcessSRTN(clk);
            }
            else 
            {
                if(ptr_head->processInfo.remainingTime < currentProcess.remainingTime-(clk-currentProcess.previousstart))
                {
                    runProcessSRTN(clk);
                }
            }
            
        }
    } 
    RRstats();

}

void runProcessSRTN(int clk)
{
    struct Process prev_ptr;
    //printf("\nIn run");
   /* if(busy == 1)
    {
        flag = true; 
        prev_ptr = currentProcess;
        kill(prev_ptr.systempid, SIGUSR1);
        prev_ptr.previousstop = clk;
        prev_ptr.remainingTime = currentProcess.remainingTime - (clk - currentProcess.previousstart);
        fprintf(schedulerLogFile, "At time %d process %d stopped arr %d total %d remain %d wait %d\n", clk, prev_ptr.id, prev_ptr.arrivalTime, prev_ptr.runTime,prev_ptr.remainingTime, prev_ptr.waitingTime);
        insert_srtn(&ready_queue, prev_ptr);
    }
    currentProcess = ready_queue.head->processInfo;
    busy = 1;*/
    tempProcess = ready_queue.head->processInfo;
    if(tempProcess.isStarted == 0)
    {
        int mem_start = allocate(tempProcess.memsize, tempProcess.id);
        //printf("\n%d is mem start for process %d with memsize %d", mem_start, tempProcess.id, tempProcess.memsize);
        if(mem_start == -1) 
        {
            //printf("Memory not enough");
            tempProcess.mem_start = mem_start;
            tempProcess.previousstart = -1;
            tempProcess.isStarted = 0;
        }
        else
        {
            //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
            bool flag = false;
            handler_finished = 0;
            if(busy == 1)
            {
                flag = true; 
                prev_ptr = currentProcess;
                kill(prev_ptr.systempid, SIGUSR1);
                prev_ptr.previousstop = clk;
                prev_ptr.remainingTime = currentProcess.remainingTime - (clk - currentProcess.previousstart);
                fprintf(schedulerLogFile, "At time %d process %d stopped arr %d total %d remain %d wait %d\n", clk, prev_ptr.id, prev_ptr.arrivalTime, prev_ptr.runTime,prev_ptr.remainingTime, prev_ptr.waitingTime);
                insert_srtn(&ready_queue, prev_ptr);
            }
            currentProcess = ready_queue.head->processInfo;
            busy = 1;

            //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

            currentProcess.mem_start = mem_start;
            currentProcess.isStarted = 1;
            currentProcess.waitingTime = clk - currentProcess.arrivalTime;
            total_waiting += currentProcess.waitingTime;
            currentProcess.previousstart = clk;
            processes_count++;
            pid_t pid = fork();
            //printf("\nProcess with id %d forked", currentProcess.id);
            if (pid == 0)
            {
                char running_time_param[MAXCHAR];
                sprintf(running_time_param, "%d", currentProcess.runTime);
                char start_time_param[MAXCHAR];
                sprintf(start_time_param, "%d", clk);
                char id_param [MAXCHAR] ; 
                sprintf(id_param, "%d", currentProcess.id);       
                execl("./process.out", "./process.out", running_time_param, start_time_param, id_param,(char *)NULL);
            }
            else if (pid == -1)
            {
                exit(-1);
            }
            else
            {
                fprintf(schedulerLogFile, "At time %d process %d started arr %d total %d remain %d wait %d \n", clk, currentProcess.id, currentProcess.arrivalTime, currentProcess.runTime, currentProcess.runTime, currentProcess.waitingTime);
                currentProcess.systempid = pid;
                remove_head(&ready_queue); 
            }
        }   
    }
    else
    {
        bool flag = false;
        handler_finished = 0;
        if(busy == 1)
        {
            flag = true; 
            prev_ptr = currentProcess;
            kill(prev_ptr.systempid, SIGUSR1);
            prev_ptr.previousstop = clk;
            prev_ptr.remainingTime = currentProcess.remainingTime - (clk - currentProcess.previousstart);
            fprintf(schedulerLogFile, "At time %d process %d stopped arr %d total %d remain %d wait %d\n", clk, prev_ptr.id, prev_ptr.arrivalTime, prev_ptr.runTime,prev_ptr.remainingTime, prev_ptr.waitingTime);
            insert_srtn(&ready_queue, prev_ptr);
        }
        currentProcess = ready_queue.head->processInfo;
        busy = 1;

        remove_head(&ready_queue);
        currentProcess.previousstart = clk;
        currentProcess.waitingTime += clk - (currentProcess.previousstop) ;
        fprintf(schedulerLogFile, "At time %d process %d resumed arr %d total %d remain %d wait %d\n", clk,currentProcess.id, currentProcess.arrivalTime, currentProcess.runTime, currentProcess.remainingTime, currentProcess.waitingTime);
        kill(currentProcess.systempid, SIGCONT);
    }
    

      
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HPF()
{

    struct Node *previous_head = NULL; //malloc(sizeof(struct Node));
    struct Node *current_head = NULL;

    int flag_first_time = 0;
    int c = 0;

    //printf("List is %d\n", isEmpty(&ready_queue));

    while (!endReceive || isEmpty(&ready_queue) == 0 || handler_finished == 0) 
    {
        runProcessHPF(getClk());
    }
    //To make the scheduler waits until all processes terminates
    pid_t wpid;
    int status = 0;
    RRstats();
    /*
    utilization = (total_runtime / getClk()) * 100;
    float average_waiting = (float)total_waiting/(float)processes_count;

    float average_turnaround = (float)total_turnaround/(float)processes_count;

    float average_weighted_turnaround = (float)total_weighted_turnaround/(float)processes_count;
    

    float sum_dev = 0;

    for(int i = 0; i < processes_count; i++) {
        sum_dev += pow((WTA[i]-average_weighted_turnaround), 2);
    }

    float std_average_weighted_turnaround = sqrt(sum_dev / processes_count);

    fprintf(schedulerPerfFile, "CPU utilization = %0.2f%%\n", utilization);
    fprintf(schedulerPerfFile, "Avg WTA = %0.2f\n", average_weighted_turnaround);
    fprintf(schedulerPerfFile, "Avg Waiting = %0.2f\n", average_waiting);  
    fprintf(schedulerPerfFile, "Std WTA = %0.2f\n", std_average_weighted_turnaround);  
    */

    while ((wpid = wait(&status)) > 0);
}
void runProcessHPF(int clk)
{
    
    if (isEmpty(&ready_queue) != 1 && busy == 0)
    {
        currentProcess = ready_queue.head->processInfo;
        
        int mem_start = allocate(currentProcess.memsize, currentProcess.id);
        if(mem_start == -1) /////// Check again
        {
            return;
        }
        else
        {
            currentProcess.mem_start = mem_start;
        }

        busy = 1;
        handler_finished = 0;
        //currentProcess = ready_queue.head->processInfo;
        total_runtime += currentProcess.runTime;
        currentProcess.waitingTime = clk - currentProcess.arrivalTime;
        total_waiting += currentProcess.waitingTime;
        processes_count++;

        char running_time_param[MAXCHAR];
        sprintf(running_time_param, "%d", currentProcess.runTime);
        char start_time_param[MAXCHAR];
        sprintf(start_time_param, "%d", clk);
        char id_param [MAXCHAR] ; 
        sprintf(id_param, "%d", currentProcess.id);
                        
        
        pid_t pid = fork();
        if (pid == 0)
        {
            currentProcess.systempid = getpid();
            execl("./process.out", "./process.out", running_time_param, start_time_param, id_param,(char *)NULL);
        }
        else if (pid == -1)
        {
            //printf("\nProcess Initialization Error\n");
            exit(-1);
        }
        else
        {
            removeWithPriority(&ready_queue);
            fprintf(schedulerLogFile, "At time %d process %d started arr %d total %d remain %d wait %d \n", clk, currentProcess.id, currentProcess.arrivalTime, currentProcess.runTime, currentProcess.runTime, currentProcess.waitingTime);
        }
        //kill(currentProcess.systempid, SIGCONT);
      
    }
    
}


void handler(int signum)
{
    //printf("Inside handler\n");
    busy = 0;
    int clk = getClk();
    
    if (algorithmNumber == 1)
    {
        deallocate(currentProcess.mem_start, currentProcess.id);
        int turnaround = clk - currentProcess.arrivalTime;
        float weighted_turnaround = (float)turnaround/(float)currentProcess.runTime;

        currentProcess.weightedTA = weighted_turnaround;


        //WTA[processes_count-1] = weighted_turnaround;

        //total_turnaround += turnaround;
        //total_weighted_turnaround += weighted_turnaround;
        insertToQueue(&finished_queue, currentProcess);
        handler_finished = 1;
        fprintf(schedulerLogFile, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n", clk, currentProcess.id,  currentProcess.arrivalTime, currentProcess.runTime, currentProcess.remainingTime, currentProcess.waitingTime, turnaround, weighted_turnaround); 
        runProcessHPF(clk);
        
    }
    else if (algorithmNumber == 2)
    {
        handler_finished = 1;
        int turnaround_time = getClk() - currentProcess.arrivalTime;
        float weighted_ta = (float)turnaround_time / currentProcess.runTime;  
        currentProcess.weightedTA = weighted_ta;
        busy = 0;
        insertToQueue(&finished_queue, currentProcess);
        deallocate(currentProcess.mem_start, currentProcess.id);
        fprintf(schedulerLogFile, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n", clk, currentProcess.id,  currentProcess.arrivalTime, currentProcess.runTime, currentProcess.remainingTime, currentProcess.waitingTime, turnaround_time, weighted_ta); 
    }/* Has an alternative way in the function below , so both are okay
    else if (algorithmNumber == 3)
    {
        deallocate(currentProcess.mem_start, currentProcess.id);
        int turnaround_time = getClk() - currentProcess.arrivalTime;
        float weighted_ta = (float)turnaround_time / currentProcess .runTime;  
        currentProcess.weightedTA = weighted_ta;
        currentProcess.remainingTime = 0;
        insertToQueue(&finished_queue, currentProcess);
        fprintf(schedulerLogFile, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n", getClk(), temp_node ->processInfo.id, temp_node ->processInfo.arrivalTime, temp_node ->processInfo.runTime, temp_node ->processInfo.remainingTime, temp_node->processInfo.waitingTime, turnaround_time, weighted_ta); 
    }*/
    signal(SIGUSR1, handler);
}

//Author : Ahmed OSama
//Round Robin
int RR(int quant)
{ 
    //Process ID for forking
    pid_t pid;

    time_quantum = quant;
   
    int x = getClk();
    
    struct Node * previous_head = NULL;
    struct Node * current_head = NULL;
    
    int flag_first_time = 0;

    //Logic of algorithm
    while (1)
    {
        //This condition handles one of two cases, either pauses a process when its quantum finishes, 
        // or removes a process when it finishes its runtime completely
        if(running_queue.head != NULL && (getClk() - running_queue.head->processInfo.previousstart == time_quantum))
        {
            struct Node * temp_node = running_queue.head;
            temp_node->processInfo.remainingTime -= time_quantum;
            removeFromQueue(&running_queue, temp_node->processInfo.id );
            
            
            if(temp_node->processInfo.remainingTime > 0)
            {
                temp_node ->processInfo.previousstop = getClk();
                fprintf(schedulerLogFile, "At time %d process %d stopped arr %d total %d remain %d wait %d\n", getClk(), temp_node ->processInfo.id, temp_node ->processInfo.arrivalTime, temp_node ->processInfo.runTime, temp_node ->processInfo.remainingTime, temp_node->processInfo.waitingTime);
                insertToQueue(&ready_queue, temp_node->processInfo);
                kill(temp_node->processInfo.systempid, SIGUSR1);
            }
            else
            {
                int turnaround_time = getClk() - temp_node->processInfo.arrivalTime;
                float weighted_ta = (float)turnaround_time / temp_node -> processInfo .runTime;  
                temp_node ->processInfo .weightedTA = weighted_ta;
                temp_node ->processInfo.remainingTime = 0;
                insertToQueue(&finished_queue, temp_node -> processInfo);
                fprintf(schedulerLogFile, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n", getClk(), temp_node ->processInfo.id, temp_node ->processInfo.arrivalTime, temp_node ->processInfo.runTime, temp_node ->processInfo.remainingTime, temp_node->processInfo.waitingTime, turnaround_time, weighted_ta); 
                deallocate(temp_node->processInfo.mem_start, temp_node->processInfo.id);
            }
        }
        current_head = ready_queue.head;

        //This condition handles one of two cases, either Starts a process from begining and fork it,  
        // or makes a process continue after being paused
        //Move process from ready state to running state
        if(current_head != NULL && running_queue.head == NULL)
        {
            int time = getClk();

            removeFromQueue(&ready_queue, current_head-> processInfo.id );

            flag_first_time = current_head->processInfo.isStarted;
            
            current_head -> processInfo.starttime = time;  
            if(current_head ->processInfo.remainingTime - time_quantum < 0)
            {
                current_head -> processInfo.previousstart = time + current_head ->processInfo.remainingTime - time_quantum;

            }
            else
                current_head -> processInfo.previousstart = time ;
            current_head -> processInfo.isStarted = 1;
            
            previous_head = current_head;
            current_head = ready_queue.head;

            
            if(flag_first_time == 0)    //First time to run
            {

                //############################ Memory allocation
                //Inserted at the queue again if no enough memory found to allocate
                int mem_start = allocate(previous_head->processInfo.memsize, previous_head->processInfo.id);
                if(mem_start == -1) /////// Check again
                {
                    previous_head->processInfo.mem_start = mem_start;
                    previous_head->processInfo.previousstart = -1;
                    previous_head->processInfo.isStarted = 0;
                    insertToQueue(&ready_queue, previous_head->processInfo);

                }
                else
                {
                    previous_head->processInfo.mem_start = mem_start;

                    pid=fork();
                    
                    if(pid == 0){   //Child                        
                        //Prepare parameters to be sent to process
                        char id_param [MAXCHAR] ; 
                        sprintf(id_param, "%d", previous_head -> processInfo.id);
                        char running_time_param [MAXCHAR];
                        sprintf(running_time_param, "%d", previous_head -> processInfo.runTime);
                        char arrival_time_param [MAXCHAR];
                        sprintf(arrival_time_param, "%d", previous_head -> processInfo.arrivalTime);
                        //
                        char start_time_param [MAXCHAR];
                        sprintf(start_time_param, "%d", time);
                    
                        return execl("./process.out", "./process.out", running_time_param, start_time_param, id_param, (char*)NULL);
                    }
                    else if(pid == -1)  //Failed
                    {
                        deallocate(previous_head->processInfo.mem_start, previous_head->processInfo.id);
                        printf("\nProcess Forking Error\n");
                        exit(-1); 
                    }
                    else //Parent
                    {
                        
                        previous_head -> processInfo.starttime = time;
                        previous_head ->processInfo.waitingTime = time - previous_head->processInfo.arrivalTime;
                        previous_head->processInfo.systempid = pid;
                        fprintf(schedulerLogFile, "At time %d process %d started arr %d total %d remain %d wait %d\n", time, previous_head ->processInfo.id, previous_head ->processInfo.arrivalTime, previous_head ->processInfo.runTime, previous_head ->processInfo.runTime, previous_head->processInfo.waitingTime);
                        
                        insertToQueue(&running_queue, previous_head->processInfo);
                        currentProcess = previous_head->processInfo;
                    }
                }
            }
            else    //Resuming (Already forked)
            {
                previous_head->processInfo.waitingTime += time - (previous_head->processInfo.previousstop) ;
                fprintf(schedulerLogFile, "At time %d process %d resumed arr %d total %d remain %d wait %d\n", time, previous_head ->processInfo.id, previous_head ->processInfo.arrivalTime, previous_head ->processInfo.runTime, previous_head ->processInfo.remainingTime, previous_head->processInfo.waitingTime);
                insertToQueue(&running_queue, previous_head->processInfo);
                kill(previous_head->processInfo.systempid, SIGCONT);
                currentProcess = previous_head->processInfo;
                
            }
            

        }

        //Condition to exit the loop
        if(running_queue.head == NULL && ready_queue.head == NULL  && endReceive) 
        {
            break;
        }
        

    }

    RRstats();

}

//Author: Ahmed Osama
//Calculates the required stats for scheduler.perf
void RRstats()
{
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
        total_runtime += temp ->processInfo.runTime;
        temp = temp->next;
        j--;
    }
    float avg_waiting = (float)total_waiting / processes_size;
    float avg_ta = total_wta / processes_size;
    float utilization = total_runtime / (getClk()) * 100;

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
}

void recievingHandler(int signum)
{

    while (1)
    {
        rec_val = msgrcv(msgq_id, &message, sizeof(message.P), 0, !IPC_NOWAIT);
        if (message.P.id == -2)
        {   
            break;
        }
        else
        {
            if (message.P.id == -1)
            {
                endReceive = true;
                break;
                //printf("\nScheduler: End Receive message was received\n");
            }
            else
            {
                    if (algorithmNumber == 1)
                    {
                        insertWithPriority(&ready_queue, message.P);
                        //printf("List is %d\n", isEmpty(&ready_queue));
                        //printLinkedList(&ready_queue);
                    }
                    else if (algorithmNumber == 2)
                    {
                        message.P.isStarted = 0;
                        insert_srtn(&ready_queue, message.P);
                    }
                    else if(algorithmNumber == 3)
                    {
                        //This part guarantees that the assumption will always be correct in round robin
                        // Where if a process arrives and another is paused at the same time, the new process remain on queue
                        // top                        
                        if(ready_queue.size != 0 && getClk() - ready_queue.tail->processInfo.previousstart == time_quantum && ready_queue.tail->processInfo.previousstart != -1)
                        {
                            struct Process temp = ready_queue.tail->processInfo;
                            removeFromQueueBsck(&ready_queue);
                            insertToQueue(&ready_queue, message.P);  
                            insertToQueue(&ready_queue, temp);  
                        }
                        else
                        {
                            insertToQueue(&ready_queue, message.P);   
                        }                      
                    }

            }
        }
    }
    signal(SIGUSR2, recievingHandler);
}


//#########Memory Related Functions
//Author: Ahmed Osama
//Initializes the memory with a given size s
void Buddy(int s)
{
	// Maximum number of powers of 2 possible
	int n = ceil(log(s) / log(2));
	size = n + 1;	
	initialize_vov(&arr, n);
	struct vector* iterator = arr.tail_vov;
	push_back(iterator, 0, s-1);
	printf("Memory with size %d bytes has been initialized!\n", s);
	
}

//Author: Ahmed Osama
//Allocates a memory for a process with size s and id process_id
int allocate(int s, int process_id)
{

	// Calculate index in free list
	// to search for block if available
	int x = ceil(log(s) / log(2));
    int return_value;
	

	struct vector* iterator = arr.head_vov;
	int counter = 0;
	while (iterator->next != NULL)
	{
		if (counter == x)
		{
			break;
		}
		counter ++ ;
		iterator = iterator->next;
	}
	if(counter < x)
	{		
		iterator = NULL;
		//printf("Sorry, failed to allocate memory\n");
		return_value = -1;
	}
	
	// Block available
	if(iterator->head != NULL)
	{
	    
        int temp_first = back_first(iterator);
		int temp_second = back_second(iterator);
		pop_back(iterator);
		//printf("Memory from %d to %d allocated\n", temp_first, temp_second);
        fprintf(memoryLogFile, "At time %d allocated %d bytes for process %d from %d to %d\n", getClk(), temp_second-temp_first + 1, process_id, temp_first, temp_second);

        return_value = temp_first;
		
		insert_to_map(&mp, temp_first, temp_second -temp_first + 1); //my defined map
	}
	else
	{
		int i;
		// If not, search for a larger block
		iterator = iterator->next;
		for(i = x + 1; i < size; i++)
		{
			if(iterator->head != NULL)
				break;
			iterator = iterator->next;		
		}
		// If no such block is found
		// i.e., no memory block available
		if (i >= size)
		{
			//printf("Sorry, failed to allocate memory\n");
            return_value = -1;
		}		
		else
		{
			int temp_first = back_first(iterator);
			int temp_second = back_second(iterator);
			pop_back(iterator);

			i--;

			for(;i >= x; i--)
			{
				// Divide block into two halves
				int pair1_first = 	temp_first;
				int pair1_second = temp_first +(temp_second -temp_first) / 2;			
				int pair2_first = 	temp_first +(temp_second -temp_first + 1) / 2;
				int pair2_second = temp_second;

				iterator = iterator->previous;

				push_back(iterator, pair1_first, pair1_second);				
				push_back(iterator, pair2_first, pair2_second);
				
				temp_first = begin_first(iterator);
				temp_second = begin_second(iterator);
				
				erase_begin(iterator);
			}

			
			//printf("Memory from %d to %d allocated\n", temp_first, temp_second);
            fprintf(memoryLogFile, "At time %d allocated %d bytes for process %d from %d to %d\n", getClk(), temp_second-temp_first + 1, process_id, temp_first, temp_second);
			return_value = temp_first;

			insert_to_map(&mp, temp_first, temp_second -temp_first + 1); //my defined map
		}
	}
	return return_value;
}

//Author: Ahmed Osama
//Deallocates the momry of process allocated at memory address id and with process id = process_id
void deallocate(int id, int process_id)
{

	// If no such starting address available
	int check_exist = get_value_map(&mp, id);
	if(check_exist == -1)
	{
		printf("Sorry, invalid free request\n");
		return;
	}

	// Size of block to be searched
	int temp_log = get_value_map(&mp, id);
	int n = ceil(log(temp_log) / log(2));

	int i, buddyNumber, buddyAddress;
	int shadow_id = id;

	struct vector* iterator = arr.head_vov;
	int counter = 0;
	while (iterator->next != NULL)
	{
		if (counter == n)
		{
			break;
		}
		counter ++ ;
		iterator = iterator->next;
	}
	// Add the block in free list
	push_back(iterator, id, id + pow(2, n) - 1);						
	//printf("Memory block from %d to %.0f freed\n", id, id + pow(2, n) - 1);

    fprintf(memoryLogFile, "At time %d freed %.0f bytes for process %d from %d to %.0f\n", getClk(), pow(2, n), process_id, id, id + pow(2, n) - 1);
	// Calculate buddy number
	temp_log = get_value_map(&mp, id);

	buddyNumber = id / temp_log;

    int temp_size = temp_log;

	bool flag = true;
	while(n <= size && flag)
    {
        flag = false;

        if (buddyNumber % 2 != 0)
            buddyAddress = id - pow(2, n);
        else
            buddyAddress = id + pow(2, n);

        // Search in free list to find it's buddy
		int  my_size = iterator->size;
		struct vector_element * v_element = iterator->head;
		for(i = 0; i < my_size; i++)
        {
			if (v_element->first == buddyAddress)
            {
                flag = true;
                if (buddyNumber % 2 == 0)
                {
					iterator = iterator->next;
					push_back(iterator, id, id + 2 * (pow(2, n)) -1);
					//printf("Coalescing of blocks starting at %d and %d was done\n", id, buddyAddress);
                }
                else
                {
					iterator = iterator->next;
					push_back(iterator, buddyAddress, buddyAddress +2 * (pow(2, n)) - 1);
					//printf("Coalescing of blocks starting at %d and %d was done\n", buddyAddress, id);
                }
				iterator = iterator->previous;
				
                erase_at_index(iterator, i);
                erase_at_index(iterator,  iterator->size-1);
				
				break;
            }
			v_element = v_element->next;
        }
       
        if (buddyNumber % 2 == 0)
        {
            id = id;
        }
        else
        {
           id = buddyAddress;
        }
        temp_size = temp_size * 2;

        buddyNumber = id / (temp_size);

        n += 1;
		iterator = iterator->next;

    }
    // Remove the key existence from map
	remove_from_map(&mp, shadow_id);
}
//###############


