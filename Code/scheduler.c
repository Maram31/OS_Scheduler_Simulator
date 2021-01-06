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

int main(int argc, char * argv[])
{
     initClk();
    //TODO implement the scheduler :)
    //upon termination release the clock resources.

    FILE * schedulerLogFile;
    schedulerLogFile = fopen("scheduler.log", "w");
    FILE * schedulerPerfFile;
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
        rec_val = msgrcv(msgq_id, &message, sizeof(message.P), 0, !IPC_NOWAIT);

        if (rec_val == -1)
            perror("Error in receive");
        else
            printf("\nProcess with id %d and arrival time %d was received at time %d\n", message.P.id, message.P.arrivalTime, getClk());
    }


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
         printf("\nScheduler: Round Robin (RR) \n");
        //TODO
        //Add Algorithm function call
    }
    


    fclose(schedulerLogFile);
    fclose(schedulerPerfFile);
    //destroyClk(true);
    return 1;
}


/*
void HPF() {

    struct LinkedList ready_queue = {NULL, NULL, 0};

    struct Node * previous_head = NULL;  //malloc(sizeof(struct Node));
    struct Node * current_head = NULL;
    
    int flag_first_time = 0;

    int c = 0;

    while (1)
    {
        while (1)
        {
            rec_val = msgrcv(msgq_id, &message, sizeof(message.P), 0, !IPC_NOWAIT);

            if (rec_val == -1) {
                perror("Error in receive");
            }

            else {
                printf("\nProcess with id %d and arrival time %d was received at time %d with priority %d\n", message.P.id, message.P.arrivalTime, getClk(), message.P.priority);
                insertWithPriority(&ready_queue, message.P);
                printLinkedList(&ready_queue);
                printf("\n%d\n", c++);

            
            
            }
        }

    }
 
}

*/