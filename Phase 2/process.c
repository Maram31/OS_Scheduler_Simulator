/*
* Author: Ahmed Osama
* Synchronized the process with the clock to calculate the remaining time accurately with no tolerance
*/

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

    int pid = atoi(argv[3]); ///To be removed
    int running_time = atoi(argv[1]);
    int start_time = atoi(argv[2]);    

    while (getClk() - waiting_time - start_time < running_time)
    {
    }
  
    //printf("Process %d finished execution\tcurrent time is %d\nTerminating and sending signal to parent...\n", pid, getClk());

    destroyClk(false);
    kill(getppid(), SIGUSR1);
    return 0;
}

void pause_handler(int signum)
{
    int x = getClk();
    prev_pause = x;
    raise(SIGSTOP);
    signal(SIGUSR1, pause_handler);
}

void resume_handler(int signum)
{
    waiting_time += getClk() - prev_pause;
    signal(SIGUSR2, resume_handler);
}
