#include "headers.h"
struct process              //struct to hold the information of each process
{
    int arrivaltime;
    int priority;
    int runningtime;
    int id;
};
struct msgbuff
{
    long mtype;
    struct process P;
};
// state 0 for started log
// state 1 for stopped log
// state 2 for finished log
// state 3 for resumed log
void reportProcessToLogFile(struct process* P, int state, int clk);

FILE * schedulerLogFile;
FILE * schedulerPerfFile;

int main(int argc, char * argv[])
{
     initClk();
    //TODO implement the scheduler :)
    //upon termination release the clock resources.

    
    schedulerLogFile = fopen("scheduler.log", "w");
    schedulerPerfFile = fopen("scheduler.perf", "w");
    fprintf(schedulerLogFile,"#At time x process y state arr w total z remain y wait k \n");

    key_t key_id;
    int msgq_id, rec_val;

    key_id = ftok("keyfile", 65);
    msgq_id = msgget(key_id, 0666 | IPC_CREAT);

    if (msgq_id == -1)
    {
        perror("Error in create message queue");
        exit(-1);
    }
    printf("Scheduler: Message Queue ID = %d\n", msgq_id);
    struct msgbuff message;

    if(!strcmp(argv[1], "1"))
    {
        printf("\nScheduler: Non-preemptive Highest Priority First (HPF) \n");
        //TODO
        //Add Algorithm function call
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

    while (1)
    {
        rec_val = msgrcv(msgq_id, &message, sizeof(message.P), 0, !IPC_NOWAIT);

        if (rec_val == -1)
            perror("Error in receive");
        else
            printf("\nProcess with id %d and arrival time %d was received at time %d\n", message.P.id, message.P.arrivaltime, getClk());
    }
    
  
    


    fclose(schedulerLogFile);
    fclose(schedulerPerfFile);
    //destroyClk(true);
    return 1;
}
void reportProcessToLogFile(struct process* P, int state, int clk)
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