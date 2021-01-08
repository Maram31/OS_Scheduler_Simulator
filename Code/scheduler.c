#include "headers.h"
#include "priority_queue.h"

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
struct Process currentProcess;
short algorithmNumber;

void HPF();
void handler(int signum);
void recievingHandler(int signum);

struct LinkedList ready_queue = {NULL, NULL, 0};

// state 0 for started log
// state 1 for stopped log
// state 2 for finished log
// state 3 for resumed log
void reportProcessToLogFile(struct Process* P, int state, int clk);

FILE * schedulerLogFile;
FILE * schedulerPerfFile;

int main(int argc, char * argv[])
{
    algorithmNumber = atoi(argv[1]);
    signal(SIGUSR1, handler);
    signal(SIGUSR2, recievingHandler);

    initClk();
    //TODO implement the scheduler :)
    //upon termination release the clock resources.

    
    schedulerLogFile = fopen("scheduler.log", "w");
    schedulerPerfFile = fopen("scheduler.perf", "w");
    fprintf(schedulerLogFile,"#At time x process y state arr w total z remain y wait k \n");




    key_id = ftok("keyfile", 65);
    msgq_id = msgget(key_id, 0666 | IPC_CREAT);

    if (msgq_id == -1)
    {
        perror("Error in create message queue");
        exit(-1);
    }
    printf("Scheduler: Message Queue ID = %d\n", msgq_id);
 
 
    //To make the scheduler waits until all processes terminates
    pid_t  wpid;
    int status = 0;
    while ((wpid = wait(&status)) > 0); 


    if(!strcmp(argv[1], "1"))
    {
        printf("\nScheduler: Non-preemptive Highest Priority First (HPF) \n");
        //TODO
        //Add Algorithm function call
        HPF();
    }

    else if(!strcmp(argv[1], "2"))
    {
        printf("\nScheduler: Shortest Remaining time Next (SRTN) \n");
        //TODO
        //Add Algorithm function call
    }

    else if(!strcmp(argv[1], "3"))
    {
        printf("\nScheduler: Round Robin (RR) with Quantum %d \n", atoi(argv[2]));
        //TODO
        //Add Algorithm function call
    }



    fclose(schedulerLogFile);
    fclose(schedulerPerfFile);
    //destroyClk(true);
    return 1;
}



void HPF() {


    struct Node * previous_head = NULL;  //malloc(sizeof(struct Node));
    struct Node * current_head = NULL;
    
    int flag_first_time = 0;
    int c = 0;
     
    //printf("List is %d\n", isEmpty(&ready_queue));

    while (!endReceive || isEmpty(&ready_queue) == 0)
    {
        
        

        if(isEmpty(&ready_queue) != 1 && busy == 0) {
            busy = 1;
            currentProcess = ready_queue.head->processInfo;
            currentProcess.waitingTime = getClk()- currentProcess.arrivalTime;
            kill(currentProcess.systempid, SIGCONT);
            
            //printf("Process with id = %d will be removed from queue\n", currentProcess.id);

            removeWithPriority(&ready_queue);
            fprintf(schedulerLogFile,"At time %d process %d started arr %d total %d remain %d wait %d \n", getClk(), currentProcess.id,  currentProcess.arrivalTime, currentProcess.runTime, currentProcess.runTime, currentProcess.waitingTime);

            
            //printLinkedList(&ready_queue);

            //printf("Process finished");
        }



        
    } 



    //To make the scheduler waits until all processes terminates
    pid_t  wpid;
    int status = 0;
    while ((wpid = wait(&status)) > 0); 
    
 
}


void reportProcessToLogFile(struct Process* P, int state, int clk)
{

    switch (state)
    {
    case 0:
        //fprintf(schedulerLogFile,"#At time %d process %d started arr %d total %d remain %d wait %d \n", clk, P->id,  P->arrivaltime, P->runningtime, P->waitingTime);
        break;
    case 1:
        //fprintf(schedulerLogFile,"#At time %d process %d stopped arr %d total %d remain %d wait %d \n", clk, P->id,  P->arrivaltime, P->runningtime, P->waitingTime);
        break;
    case 2:
        //fprintf(schedulerLogFile,"#At time %d process %d finished arr %d total %d remain %d wait %d \n", clk, P->id,  P->arrivaltime, P->runningtime, P->waitingTime);
        break;
    case 3:
        //fprintf(schedulerLogFile,"#At time %d process %d resumed arr %d total %d remain %d wait %d \n", clk, P->id,  P->arrivaltime, P->runningtime, P->waitingTime);
        break;
    default:
        break;
    }
}


void handler(int signum) {
    //printf("Inside handler\n");
    busy = 0;
    int clk = getClk();
    fprintf(schedulerLogFile,"At time %d process %d finished arr %d total %d remain %d \n", clk, currentProcess.id,  currentProcess.arrivalTime, currentProcess.runTime, 0);
    
    if(algorithmNumber == 1) {
        if(isEmpty(&ready_queue) != 1 && busy == 0) {
            busy = 1;
            currentProcess = ready_queue.head->processInfo;
            currentProcess.waitingTime = clk- currentProcess.arrivalTime;
            kill(currentProcess.systempid, SIGCONT);
            
            //printf("Process with id = %d will be removed from queue\n", currentProcess.id);

            removeWithPriority(&ready_queue);
            fprintf(schedulerLogFile,"At time %d process %d started arr %d total %d remain %d wait %d \n", clk, currentProcess.id,  currentProcess.arrivalTime, currentProcess.runTime, currentProcess.runTime, currentProcess.waitingTime);
        }
    }

    signal(SIGUSR1, handler);

}


void recievingHandler(int signum) {

    while(1)
    {
            rec_val = msgrcv(msgq_id, &message, sizeof(message.P), 0, !IPC_NOWAIT);
            //printf("Message Id: %d\n", message.P.id);
            /*
            if (rec_val == -1) {
                //perror("Error in receive");
                break;
            }
            */
            if(message.P.id == -2)
            {
                //printf("\nScheduler: End Slot Receive message was received\n");
                break;
            }
            else
            {
                if(message.P.id == -1)
                {
                    endReceive = true;
                    //printf("Ended recieve\n");
                    break;
                    //printf("\nScheduler: End Receive message was received\n");
                }
                else
                {
                    //printf("\nScheduler: Process with id %d and arrival time %d was received at time %d\n", message.P.id, message.P.arrivalTime, getClk());


                    pid_t pid = fork();
                    if(pid == 0) {
                        /*
                        char buffer[10];
                        sprintf(buffer, "%d", message.P.runTime);
                        //printf("\nProcess Initialization Succes with pid = %d\n", getpid()); 
                        execl("./process_ha.out", "./process_ha.out", buffer, (char*) NULL);
                        */

                            char id_param [MAXCHAR] ; 
                            sprintf(id_param, "%d", currentProcess.id);
                            char running_time_param [MAXCHAR];
                            sprintf(running_time_param, "%d", currentProcess.runTime);
                            char arrival_time_param [MAXCHAR];
                            sprintf(arrival_time_param, "%d", currentProcess.arrivalTime);
                            //
                            char start_time_param [MAXCHAR];
                            sprintf(start_time_param, "%d", getClk());

                            execl("./process_mh.out", "./process_mh.out", id_param, arrival_time_param, running_time_param, start_time_param,(char*)NULL);
                    }

                    else if(pid == -1) {
                        //printf("\nProcess Initialization Error\n");
                        exit(-1); 
                    }

                    else {
                        message.P.systempid = pid;
                        //kill(pid, SIGSTOP);
                        kill(pid, SIGUSR1);
                        message.P.waitingTime = getClk(); 
                        
                        if(algorithmNumber == 1) {
                            insertWithPriority(&ready_queue, message.P);
                            //printf("List is %d\n", isEmpty(&ready_queue));
                            //printLinkedList(&ready_queue);
                        }

                    }

                }
            }
    }
    signal(SIGUSR2, recievingHandler);
}