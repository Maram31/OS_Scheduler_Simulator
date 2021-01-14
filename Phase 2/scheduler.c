#include "headers.h"
#include "priority_queue.h"
#include "circular_queue.h"
//#include "buddy_memory.h"
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

int RR(int quant);

struct LinkedList ready_queue = {NULL, NULL, 0};

// state 0 for started log
// state 1 for stopped log
// state 2 for finished log
// state 3 for resumed log
void reportProcessToLogFile(struct Process *P, int state, int clk);

FILE *schedulerLogFile;
FILE *schedulerPerfFile;

FILE *memoryLogFile;

//Temporary till process generator works
//struct LinkedList processes = {NULL, NULL, 0};
//

//Ready and running queues to help in context switching
//struct LinkedList ready_queue = {NULL, NULL, 0};
struct LinkedList running_queue = {NULL, NULL, 0};
struct LinkedList finished_queue = {NULL, NULL, 0};

//Initial version of Round Robin scheduling algorithm with no process communication
void finish_handler(int signum);

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

    initClk();
    Buddy(128);
    //TODO implement the scheduler :)
    //upon termination release the clock resources.
    
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
    printf("Scheduler: Message Queue ID = %d\n", msgq_id);

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
        
        //TODO
        //Add Algorithm function call
    }

    fclose(memoryLogFile);
    
    fclose(schedulerLogFile);
    fclose(schedulerPerfFile);
    //destroyClk(true);
    return 1;
}
struct LinkedList ready_queue_SRTN = {NULL, NULL, 0};

//  3andi moshkela eno at a certain tie it will send the Success:end message
// w na lesa ma5alastesh filling the ready queue
// plus bey7ot men 3ando process el id bta3ha 0? Weird? 
void SRTN()
{
    struct LinkedList ProcessList = {NULL, NULL, 0};
    initClk();
    int x = getClk();       // gets time
    printf("current time is %d\n", x);

    pid_t pid_of_running;

    while (!endReceive /*|| !isEmpty(&ready_queue_SRTN)*/)
    {

    }      
    //To make the scheduler waits until all processes terminates
    pid_t wpid;
    int status = 0;
    while ((wpid = wait(&status)) > 0);
    presentid(&ready_queue_SRTN);
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


    while ((wpid = wait(&status)) > 0);
}
void runProcessHPF(int clk)
{
    if (isEmpty(&ready_queue) != 1 && busy == 0)
    {
        busy = 1;
        handler_finished = 0;
        currentProcess = ready_queue.head->processInfo;
        total_runtime += currentProcess.runTime;
        currentProcess.waitingTime = clk - currentProcess.arrivalTime;
        total_waiting += currentProcess.waitingTime;
        processes_count++;

        char running_time_param[MAXCHAR];
        sprintf(running_time_param, "%d", currentProcess.runTime);
        char start_time_param[MAXCHAR];
        sprintf(start_time_param, "%d", clk);
        
        pid_t pid = fork();
        if (pid == 0)
        {
            currentProcess.systempid = getpid();
            execl("./process_mh.out", "./process_mh.out", running_time_param, start_time_param, (char *)NULL);
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

        //printf("Process with id = %d will be removed from queue\n", currentProcess.id);

      
    }
}


void handler(int signum)
{
    //printf("Inside handler\n");
    busy = 0;
    int clk = getClk();


        
    //fprintf(schedulerLogFile, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n", getClk(), currentProcess.id, currentProcess.arrivalTime, currentProcess.runTime, currentProcess.remainingTime, currentProcess.waitingTime, turnaround_time, weighted_ta); 
    fprintf(schedulerLogFile, "At time %d process %d finished arr %d total %d remain %d \n", clk, currentProcess.id, currentProcess.arrivalTime, currentProcess.runTime, 0);
    if (algorithmNumber == 1)
    {
        int turnaround = clk - currentProcess.arrivalTime;
        float weighted_turnaround = (float)turnaround/(float)currentProcess.runTime;

        WTA[processes_count-1] = weighted_turnaround;

        total_turnaround += turnaround;
        total_weighted_turnaround += weighted_turnaround;

        handler_finished = 1;
        runProcessHPF(clk);
    }
    signal(SIGUSR1, handler);
}

int RR(int quant)
{ 
    signal(SIGUSR1, finish_handler);
    //signal(SIGCHLD, child_handler);
    //
    

    //Process ID for forking
    pid_t pid;

    
    //temp value
    time_quantum = quant;
    printf("time quantum: %d\n", time_quantum);
    //
    
    //Will be removed
    int final_size = 5;


    // Use this function after creating the clock process to initialize clock
    initClk();
    // To get time use this
    int x = getClk();
    
    
    //printf("List size: %d\n", processes.size);
    //Debug
    printf("RR current time is %d\n", x);
    //

    //Debug
    /*
    int ahmad = processes.size;
    struct Node* tempppoz = processes.head;
    while (ahmad > 0)
    {
        printf("Process ID %d\tArrival time %d\tRunning time %d\tStart time %d\tRemaining time %d\tWaiting time %d\tIs started %d\n",tempppoz->processInfo.id, tempppoz->processInfo.arrivalTime, tempppoz->processInfo.runTime, tempppoz ->processInfo.starttime, tempppoz->processInfo.remainingTime, tempppoz->processInfo.waitingTime, tempppoz ->processInfo.isStarted);
        tempppoz = tempppoz -> next;
        ahmad -= 1;
    }*/
    //


    //Variable to loop every 1 second
    int prev_time = x;
    //
    
    //Pointer to first process in linked list (Will be removed when process generator is finished)
    //struct Node* current_node = processes.head;
    //


    struct Node * previous_head = NULL;//malloc(sizeof(struct Node));
    struct Node * current_head = NULL;
    
    int flag_first_time = 0;

    int start_time_prev_process = -1;

    //Logic of algorithm
    while (1)
    {
        if(running_queue.head != NULL && (getClk() - running_queue.head->processInfo.previousstart == time_quantum))
        {
            //printf("current time is %d\tprocess with id %d has start time %d\n", x, running_queue.head->processInfo.id, running_queue.head->processInfo.starttime);
            struct Node * temp_node = running_queue.head;
            temp_node->processInfo.remainingTime -= time_quantum;
            removeFromQueue(&running_queue, temp_node->processInfo.id );
            
            //printf("Forked pid remove: %d\n", temp_node -> processInfo.systempid);
            //printf("Debug current time is %d\n", getClk());
            if(temp_node->processInfo.remainingTime > 0)
            {
                temp_node ->processInfo.previousstop = getClk();
                fprintf(schedulerLogFile, "At time %d process %d stopped arr %d total %d remain %d wait %d\n", getClk(), temp_node ->processInfo.id, temp_node ->processInfo.arrivalTime, temp_node ->processInfo.runTime, temp_node ->processInfo.remainingTime, temp_node->processInfo.waitingTime);
                insertToQueue(&ready_queue, temp_node->processInfo);
                //kill(temp_node->processInfo.systempid, SIGSTOP);
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
                printf("Mem start %d\n", temp_node->processInfo.mem_start);
                deallocate(temp_node->processInfo.mem_start, temp_node->processInfo.id);
                print();
                //printf("Goodbye process %d\n", temp_node -> processInfo.id);
                final_size -= 1;
                
                }
            //printf("Hellloo\n");
        }
        //
        //printf("Hello 2!\n");
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
                    sprintf(running_time_param, "%d", previous_head -> processInfo.runTime);
                    char arrival_time_param [MAXCHAR];
                    sprintf(arrival_time_param, "%d", previous_head -> processInfo.arrivalTime);
                    //
                    char start_time_param [MAXCHAR];
                    sprintf(start_time_param, "%d", getClk());
                   
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
                    previous_head ->processInfo.waitingTime = getClk() - previous_head->processInfo.arrivalTime;
                    //printf("Forked pid insert: %d\n", pid);
                    previous_head->processInfo.systempid = pid;
                    fprintf(schedulerLogFile, "At time %d process %d started arr %d total %d remain %d wait %d\n", getClk(), previous_head ->processInfo.id, previous_head ->processInfo.arrivalTime, previous_head ->processInfo.runTime, previous_head ->processInfo.runTime, previous_head->processInfo.waitingTime);
                    
                    int mem_start = allocate(previous_head->processInfo.memsize, previous_head->processInfo.id);
                    printf("Alloc mem start: %d\n", mem_start);
                    if(mem_start != -1)
                    {
                        previous_head->processInfo.mem_start = mem_start;
                    }
                    insertToQueue(&running_queue, previous_head->processInfo);
                    print();
                    currentProcess = previous_head->processInfo;
                }
            }
            else
            {
                previous_head->processInfo.waitingTime += getClk() - (previous_head->processInfo.previousstop) ;
                fprintf(schedulerLogFile, "At time %d process %d resumed arr %d total %d remain %d wait %d\n", getClk(), previous_head ->processInfo.id, previous_head ->processInfo.arrivalTime, previous_head ->processInfo.runTime, previous_head ->processInfo.remainingTime, previous_head->processInfo.waitingTime);
                insertToQueue(&running_queue, previous_head->processInfo);
                kill(previous_head->processInfo.systempid, SIGCONT);
                currentProcess = previous_head->processInfo;
                //kill(previous_head->processInfo.systempid, SIGUSR2);
                //printf("Resume\n");
            }
            

        }

        //printf("Hello 3!\n");
        

       //wait for a second to pass
       /*
        while(getClk() - prev_time == 0);
        x = getClk();
        prev_time = x;
        */
        //Condition to exit the loop (will be modified)
        if(running_queue.head == NULL && ready_queue.head == NULL /*&& final_size == 0)*/ && endReceive) 
        {
            //printf("zzz %d\n", getClk());
            break;
        }
        

    }
    //printf("Hello 4!\n");

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
    //printf("holllaaa\n");
    //fclose(schedulerLogFile);
    //fclose(schedulerPerfFile);

}
///////////////////////////////////////////////////////////////////////
void finish_handler(int signum)
{
    
    printf("A Process finished execution at %d\n", getClk());
    signal(SIGUSR1, finish_handler);    

}

//////////////////////////////////////////////////////

void recievingHandler(int signum)
{

    while (1)
    {
        rec_val = msgrcv(msgq_id, &message, sizeof(message.P), 0, !IPC_NOWAIT);
        //printf("Message Id: %d\n", message.P.id);
        /*
            if (rec_val == -1) {
                //perror("Error in receive");
                break;
            }
            */
        if (message.P.id == -2)
        {   
            break;
        }
        else
        {
            if (message.P.id == -1)
            {
                endReceive = true;
                //printf("Ended recieve\n");
                break;
                //printf("\nScheduler: End Receive message was received\n");
            }
            else
            {
                //printf("\nScheduler: Process with id %d and arrival time %d was received at time %d\n", message.P.id, message.P.arrivalTime, getClk());

                    if (algorithmNumber == 1)
                    {
                        insertWithPriority(&ready_queue, message.P);
                        //printf("List is %d\n", isEmpty(&ready_queue));
                        //printLinkedList(&ready_queue);
                    }
                    else if (algorithmNumber == 2)
                    {
                        insert_srtn(&ready_queue_SRTN, message.P);
                    }
                    else if(algorithmNumber == 3)
                    {
                        
                        //printf("Process ID:%d\n", message.P.id);
                        if(ready_queue.size != 0 && getClk() - ready_queue.tail->processInfo.previousstart == time_quantum && ready_queue.tail->processInfo.previousstart != -1)
                        {
                            //printf("Case 2:\n");
                            struct Process temp = ready_queue.tail->processInfo;
                            //printf("temp id: %d\n", temp.id);
                            removeFromQueueBsck(&ready_queue);
                            insertToQueue(&ready_queue, message.P);  
                            insertToQueue(&ready_queue, temp);  
                        }
                        else
                        {
                            //printf("Case 3:\n");
                            insertToQueue(&ready_queue, message.P);   
                        }
                    
                           
                    }

            }
        }
    }
    signal(SIGUSR2, recievingHandler);
}


//#########Memory
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
		printf("Sorry, failed to allocate memory\n");
		return_value = -1;
        //return - 1;
	}
	
	// Block available
	if(iterator->head != NULL)
	{
	    
        int temp_first = back_first(iterator);
		int temp_second = back_second(iterator);
		pop_back(iterator);
		
		printf("Memory from %d to %d allocated\n", temp_first, temp_second);
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
		//printf("debug i %d\n", i);
		// If no such block is found
		// i.e., no memory block available
		if (i >= size)
		{
			printf("Sorry, failed to allocate memory\n");
            return_value = -1;
			//return -1;
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

			
			printf("Memory from %d to %d allocated\n", temp_first, temp_second);
            fprintf(memoryLogFile, "At time %d allocated %d bytes for process %d from %d to %d\n", getClk(), temp_second-temp_first + 1, process_id, temp_first, temp_second);
			return_value = temp_first;

			insert_to_map(&mp, temp_first, temp_second -temp_first + 1); //my defined map
		}
	}
	return return_value;
}


void deallocate(int id, int process_id)
{

	// If no such starting address available
	int check_exist = get_value_map(&mp, id);
	if(check_exist == -1)
	{
		//cout << "Sorry, invalid free request\n";
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
	printf("Memory block from %d to %.0f freed\n", id, id + pow(2, n) - 1);

    fprintf(memoryLogFile, "At time %d freed %.0f bytes for process %d from %d to %.0f\n", getClk(), pow(2, n), process_id, id, id + pow(2, n) - 1);
	// Calculate buddy number
	temp_log = get_value_map(&mp, id);

	buddyNumber = id / temp_log;

    int temp_size = temp_log;

	bool flag = true;
	while(n <= size && flag) // may be modified to less than only
    {
        flag = false;


        if (buddyNumber % 2 != 0)
            buddyAddress = id - pow(2, n);
        else
            buddyAddress = id + pow(2, n);

		printf("%d\t%d\n", buddyAddress, buddyNumber);

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

					printf("Coalescing of blocks starting at %d and %d was done\n", id, buddyAddress);
                }
                else
                {
					iterator = iterator->next;
					push_back(iterator, buddyAddress, buddyAddress +2 * (pow(2, n)) - 1);

					printf("Coalescing of blocks starting at %d and %d was done\n", buddyAddress, id);
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
		//printf("Deboo\n");
		//print_vector(iterator);
		iterator = iterator->next;

    }
    // Remove the key existence from map
    //mp.erase(shadow_id);
	remove_from_map(&mp, shadow_id);
}

void print()
{
	
	struct vector* iterator = arr.head_vov;
	int counter = 0;
	while (iterator != NULL)
	{
		printf("i: %d\n", counter);
		print_vector(iterator);
		iterator = iterator->next;
		counter ++;
	}
	

}

//###############