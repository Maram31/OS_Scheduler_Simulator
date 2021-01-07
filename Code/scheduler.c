#include "headers.h"
#include "priority_queue.h"



struct msgbuff
{
    long mtype;
    struct Process P;
};

key_t key_id;
int msgq_id, rec_val;
struct msgbuff message;

void HPF();

// state 0 for started log
// state 1 for stopped log
// state 2 for finished log
// state 3 for resumed log
void reportProcessToLogFile(struct Process* P, int state, int clk);

FILE * schedulerLogFile;
FILE * schedulerPerfFile;

int main(int argc, char * argv[])
{
    bool endReceive = false;
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


    while (1)
    {
        if(!endReceive)
        {
            rec_val = msgrcv(msgq_id, &message, sizeof(message.P), 0, IPC_NOWAIT);

            if (rec_val == -1) {
                //perror("Error in receive");
            }
            else
            {
                if(message.P.id==-1)
                {
                    endReceive = true;
                    printf("\nScheduler: End Receive message was received\n");
                }
                else
                {
                    printf("\nScheduler: Process with id %d and arrival time %d was received at time %d\n", message.P.id, message.P.arrivalTime, getClk());
                    
                    pid_t pid = fork();
                    if(pid == 0){
                        char buffer[10];
                        sprintf(buffer, "%d", message.P.runTime);
                        printf("\nProcess Initialization Succes with pid = %d\n", getpid()); 
                        return execl("./process_ha.out", "./process_ha.out", buffer, (char*) NULL);
                    }
                    else if(pid == -1)
                    {
                        printf("\nProcess Initialization Error\n");
                        exit(-1); 
                    }

                }
            }
        }
        else
        {
            break;
        }
        
    }
    //To make the scheduler waits until all processes terminates
    pid_t  wpid;
    int status = 0;
    while ((wpid = wait(&status)) > 0); 


    if(!strcmp(argv[1], "1"))
    {
        printf("\nScheduler: Non-preemptive Highest Priority First (HPF) \n");
        //TODO
        //Add Algorithm function call
        //HPF();
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

    struct LinkedList ready_queue = {NULL, NULL, 0};

    struct Node * previous_head = NULL;  //malloc(sizeof(struct Node));
    struct Node * current_head = NULL;
    
    int flag_first_time = 0;

    int c = 0;

    while (1)
    {    

            rec_val = msgrcv(msgq_id, &message, sizeof(message.P), 0, IPC_NOWAIT);
            //printf("Recieving processes\n");

            if (rec_val == -1) {
                //perror("Error in receive");
            }

            else {
                printf("Message Recieved\n");


                    insertWithPriority(&ready_queue, message.P);
                    
                    printLinkedList(&ready_queue);


                }
            
            
            
        
    }
 
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
