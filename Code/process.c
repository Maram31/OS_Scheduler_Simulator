#include "headers.h"

/* Modify this file as needed*/
int remainingtime;

int main(int agrc, char * argv[])
{
    initClk();
    int pid = atoi(argv[1]);
    int arrival_time = atoi(argv[2]);
    int running_time = atoi(argv[3]);
    printf("Created Process ID: %d\tArrival time: %d\tRunning Time: %d\n", pid, arrival_time, running_time);
    //TODO it needs to get the remaining time from somewhere
    int start_time = getClk();
    remainingtime = running_time;
    //remainingtime -= 1;
    while (getClk() - start_time != running_time)
    {
    }
    //sleep(1);
    int x = getClk();
    printf("Process %d finished execution\tcurrent time is %d\nTerminating...\n", pid, x);

    while(1);

    destroyClk(false);
    
    return 0;
}
