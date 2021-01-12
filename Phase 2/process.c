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
    int running_time = atoi(argv[3]);
    int start_time = atoi(argv[4]);
    
    //printf("\nProcess with id %d started at %d\n", pid, start_time);
    

    while (getClk() - waiting_time - start_time < running_time)
    {
    }
  
    printf("Process %d finished execution\tcurrent time is %d\nTerminating and sending signal to parent...\n", pid, getClk());

    destroyClk(false);
    kill(getppid(), SIGUSR1);
    //exit(0);
    return 0;
}

void pause_handler(int signum)
{
    int x = getClk();
    prev_pause = x;
    //printf("Pausing\n");
    raise(SIGSTOP);
    signal(SIGUSR1, pause_handler);
}

void resume_handler(int signum)
{
    //printf("Resuming\n");
    waiting_time += getClk() - prev_pause;
    signal(SIGUSR2, resume_handler);
}
