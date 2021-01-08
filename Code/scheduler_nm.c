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
bool endReceive = false;
int busy = 0;
struct Process currentProcess;


void HPF();
void handler(int signum);
void SRTN();

// state 0 for started log
// state 1 for stopped log
// state 2 for finished log
// state 3 for resumed log
void reportProcessToLogFile(struct Process* P, int state, int clk);

FILE * schedulerLogFile;
FILE * schedulerPerfFile;

int main(int argc, char * argv[])
{
    
    signal(SIGUSR1, handler);

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

/*
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
*/

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
        SRTN();
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



/////////////////////////////////////////////////////////////////// TAKE FROM HERE 

//  3andi moshkela eno at a certain tie it will send the Success:end message
// w na lesa ma5alastesh filling the ready queue
// plus bey7ot men 3ando process el id bta3ha 0? Weird? 
void SRTN()
{
    struct LinkedList ProcessList = {NULL, NULL, 0};
    struct LinkedList ready_queue = {NULL, NULL, 0};
    initClk();
    int x = getClk();       // gets time
    printf("current time is %d\n", x);

    pid_t pid_of_running;

    while (!endReceive || !isEmpty(&ready_queue))
    {    
            rec_val = msgrcv(msgq_id, &message, sizeof(message.P), 0, IPC_NOWAIT);
            //printf("Recieving processes\n");
            if (rec_val == -1) {
                //perror("Error in receive");
            }
            else {
                    if(message.P.id==-1)                /////??????????? When do we set a pid = -1? Fen? 
                    {
                        endReceive = true;
                        printf("\nScheduler: End Receive message was received\n");
                    }
                    else 
                    {
                        fprintf(schedulerLogFile,"#At time %d process %d started arr %d total %d remain %d wait %d \n", getClk(), message.P.id,  message.P.arrivalTime, message.P.runTime, message.P.runTime, 0);
                        pid_t pid = fork();
                        if(pid == 0)
                        {
                            char buffer[10];
                            sprintf(buffer, "%d", message.P.runTime);
                            printf("\nProcess Initialization Succes with pid = %d\n", getpid()); 
                            execl("./process_ha.out", "./process_ha.out", buffer, (char*) NULL);
                        }
                        else if(pid == -1)
                        {
                            printf("\nProcess Initialization Error\n");
                            exit(-1); 
                        }
                        else
                        {
                            message.P.systempid = pid;          //?????? what is this for?
                            kill(pid, SIGSTOP);
                            insert_srtn(&ready_queue, message.P);
                            presentid(&ready_queue);
                        }
                    }
                }
                  
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////


void HPF() {

    struct LinkedList ready_queue = {NULL, NULL, 0};

    struct Node * previous_head = NULL;  //malloc(sizeof(struct Node));
    struct Node * current_head = NULL;
    
    int flag_first_time = 0;
    int c = 0;
    printf("List is %d\n", isEmpty(&ready_queue));

    while (!endReceive || isEmpty(&ready_queue) == 0)
    {
        
        if(!endReceive)
        {
            rec_val = msgrcv(msgq_id, &message, sizeof(message.P), 0, IPC_NOWAIT);

            if (rec_val == -1) {
                //perror("Error in receive");
            }
            else
            {
                if(message.P.id==-1)                /////??????????? When do we set a pid = -1? Fen? 
                {
                    endReceive = true;
                    printf("\nScheduler: End Receive message was received\n");
                }
                else
                {
                    //printf("\nScheduler: Process with id %d and arrival time %d was received at time %d\n", message.P.id, message.P.arrivalTime, getClk());
                    fprintf(schedulerLogFile,"#At time %d process %d started arr %d total %d remain %d wait %d \n", getClk(), message.P.id,  message.P.arrivalTime, message.P.runTime, message.P.runTime, 0);


                    pid_t pid = fork();
                    if(pid == 0){
                        char buffer[10];
                        sprintf(buffer, "%d", message.P.runTime);
                        printf("\nProcess Initialization Succes with pid = %d\n", getpid()); 
                        execl("./process_ha.out", "./process_ha.out", buffer, (char*) NULL);
                    }
                    else if(pid == -1)
                    {
                        printf("\nProcess Initialization Error\n");
                        exit(-1); 
                    }
                    else {
                        message.P.systempid = pid;
                        kill(pid, SIGSTOP);
                        insertWithPriority(&ready_queue, message.P);
                        //printf("List is %d\n", isEmpty(&ready_queue));
 
                        printLinkedList(&ready_queue);
                    }

                }
            }
        }

        if(isEmpty(&ready_queue) != 1 && busy == 0) {
            printf("Process will be taken from queue\n");
            busy = 1;
            currentProcess = ready_queue.head->processInfo;
            kill(currentProcess.systempid, SIGCONT);
           
            removeWithPriority(&ready_queue);
            
            printLinkedList(&ready_queue);

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
    fprintf(schedulerLogFile,"#At time %d process %d finished arr %d total %d remain %d \n", getClk(), currentProcess.id,  currentProcess.arrivalTime, currentProcess.runTime, 0);
}
