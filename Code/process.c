#include "headers.h"

/* Modify this file as needed*/
int remainingtime;

int main(int agrc, char * argv[])
{
    initClk();
    int pid = atoi(argv[1]);
    int arrival_time = atoi(argv[2]);
    int running_time = atoi(argv[3]);
    //printf("Ok\n");
    printf("Inside process current time is %d\n", getClk());
    printf("Created Process ID: %d\tArrival time: %d\tRunning Time: %d\n", pid, arrival_time, running_time);
    //TODO it needs to get the remaining time from somewhere
    int start_time = getClk();
    remainingtime = running_time;
    int prev_time = getClk();
    //remainingtime -= 1;
    while (remainingtime > 0)//getClk() - start_time != running_time)
    {
        while(getClk() - prev_time == 0);
        prev_time = getClk();
        remainingtime --;
    }
    //sleep(1);
    int x = getClk();
    printf("Process %d finished execution\tcurrent time is %d\nTerminating...\n", pid, prev_time);

    while(1);

    destroyClk(false);
    
    return 0;
}
