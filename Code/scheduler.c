#include "headers.h"


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
         printf("\nScheduler: Round Robin (RR) \n");
        //TODO
        //Add Algorithm function call
    }
    


    fclose(schedulerLogFile);
    fclose(schedulerPerfFile);
    //destroyClk(true);
    return 1;
}
