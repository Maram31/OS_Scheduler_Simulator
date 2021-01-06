#include "headers.h"
#include <signal.h>

/* Modify this file as needed*/
int remaining_time;
int waiting_time;
int prev_pause;

void pause_handler(int signum);
void resume_handler(int signum);

int main(int agrc, char * argv[])
{

    initClk();
    waiting_time = 0;
    signal(SIGUSR1, pause_handler);
    signal(SIGCONT, resume_handler);

    int pid = atoi(argv[1]);
    int arrival_time = atoi(argv[2]);
    int running_time = atoi(argv[3]);
    int start_time = atoi(argv[4]);
    remaining_time = running_time;
    //printf("Process ID: %d\tStart time: %d\n", pid, start_time);
    //printf("Ok\n");
    //printf("Inside process current time is %d\n", getClk());
    //printf("Created Process ID: %d\tArrival time: %d\tRunning Time: %d\n", pid, arrival_time, running_time);
    //TODO it needs to get the remaining time from somewhere
    //int start_time = getClk();
    
    int prev_time = getClk();
    //remainingtime -= 1;

    //while (1)//remainingtime > 0)//getClk() - start_time != running_time)
    //{
        /*while(getClk() - prev_time == 0);
        prev_time = getClk();
        */
       /*
       while(getClk() - prev_time == 0);
       //if(getClk() - prev_time)
        remainingtime -= 1;
        prev_time = getClk();
        if(remainingtime == 0)
            break;
        */
    //}

    while (getClk() - waiting_time - start_time < running_time)
    {
    }
    

    //sleep(1);
    int x = getClk();

    //exit(pid);
    
    printf("Process %d finished execution\tcurrent time is %d\nTerminating and sending signal to parent...\n", pid, getClk());

    //while(1);

    destroyClk(false);
    kill(getppid(), SIGUSR1);
    exit(0);
    return 0;
}

void pause_handler(int signum)
{
    //printf("\nOUCH....Signal #%d received\n",signum);
    //printf("Pauseing at %d\n", getClk());
    int x = getClk();
    prev_pause = x;
    raise(SIGSTOP);
    signal(SIGUSR1, pause_handler);
}

void resume_handler(int signum)
{
    //printf("Resuming at %d\n",getClk());
    
    waiting_time += getClk() - prev_pause;
    //raise(SIGCONT);
    signal(SIGUSR2, resume_handler);
}
