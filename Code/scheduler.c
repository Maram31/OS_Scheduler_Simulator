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
void runProcessHPF(int clk);
void handler(int signum);
void recievingHandler(int signum);

void SRTN();

struct LinkedList ready_queue = {NULL, NULL, 0};

// state 0 for started log
// state 1 for stopped log
// state 2 for finished log
// state 3 for resumed log
void reportProcessToLogFile(struct Process *P, int state, int clk);

FILE *schedulerLogFile;
FILE *schedulerPerfFile;

int main(int argc, char *argv[])
{
    algorithmNumber = atoi(argv[1]);
    signal(SIGUSR1, handler);
    signal(SIGUSR2, recievingHandler);

    initClk();
    //TODO implement the scheduler :)
    //upon termination release the clock resources.

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
        //TODO
        //Add Algorithm function call
    }

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

    while (!endReceive || isEmpty(&ready_queue) == 0)
    {
        runProcessHPF(getClk());
    }
    //To make the scheduler waits until all processes terminates
    pid_t wpid;
    int status = 0;
    while ((wpid = wait(&status)) > 0);
}
void runProcessHPF(int clk)
{
    if (isEmpty(&ready_queue) != 1 && busy == 0)
    {
        busy = 1;
        currentProcess = ready_queue.head->processInfo;
        currentProcess.waitingTime = clk - currentProcess.arrivalTime;
        char id_param[MAXCHAR];
        sprintf(id_param, "%d", currentProcess.id);
        char running_time_param[MAXCHAR];
        sprintf(running_time_param, "%d", currentProcess.runTime);
        char arrival_time_param[MAXCHAR];
        sprintf(arrival_time_param, "%d", currentProcess.arrivalTime);
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
    //(schedulerLogFile, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n", getClk(), currentProcess.id, currentProcess.arrivalTime, currentProcess.runTime, currentProcess.remainingTime, currentProcess.waitingTime, turnaround_time, weighted_ta); 
    //fprintf(schedulerLogFile, "At time %d process %d finished arr %d total %d remain %d \n", clk, currentProcess.id, currentProcess.arrivalTime, currentProcess.runTime, 0);
    if (algorithmNumber == 1)
    {
        runProcessHPF(clk);
    }
    signal(SIGUSR1, handler);
}

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
            //printf("\nScheduler: End Slot Receive message was received\n");
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

            }
        }
    }
    signal(SIGUSR2, recievingHandler);
}