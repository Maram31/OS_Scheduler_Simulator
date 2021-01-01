#include "headers.h"

struct Process {
	int id;
	int arrivalTime;
	int priority;
	bool isRunning;
	int executionTime;
    int remainingTime;
    int waitingTime;

    bool isFinished;
    bool isReady;
};


void sort(int noofpcs)
{
     for (int i = 0; i < noofpcs-1; i++)
    {
        for (int j = 0; j < noofpcs-i-1; j++)
        {
            if(pcs[j].remainingTime > pcs[j+1].remainingTime)
            {
                struct Process temp;                        /// Is this possible? to put a struct = struct?
                temp = pcs[j];
                pcs[j] = pcs[j+1];
                pcs[j+1] = temp;
            }
        }
        
    }
}

struct Process pcs[10];             // global

int main()
{
    /*Nadin: THIS FILE IS A GENRAL DEMO OF WHAT SRTN SCHEDULAR IS LIKE, NOT THE FINAL OR WORKING IMPLEMENTATION*/

    int process_id, arrival_time, run_time, num_processes;

    // Assume we have a DS array with number of processes = num_processes
    // each with ID, AT, & RT

    num_processes= 10;
    int processes [10];
    int ids[10];
    int ats[10]; int rts[10];

    

    for (int i = 0; i < num_processes-1; i++)
    {
        pcs[i].id = ids[i];
        pcs[i].arrivalTime = ats[i];
        pcs[i].executionTime = rts[i];
    }


    /*Bubble sort according to execution/running time*/

    for (int i = 0; i < num_processes-1; i++)
    {
        for (int j = 0; j < num_processes-i-1; j++)
        {
            if(pcs[j].executionTime > pcs[j+1].executionTime)
            {
                struct Process temp;                        /// Is this possible? to put a struct = struct?
                temp = pcs[j];
                pcs[j] = pcs[j+1];
                pcs[j+1] = temp;
            }
        }
        
    }

    /*Simulation starts*/
    int index_of_running;
    while(1)
    {
        int t = getClk;
        printf("This time cycle is %d \n",t);
        bool istherenewarrival = false;
        for (int i = 0; i < num_processes-1; i++)
        {
            if (pcs[i].arrivalTime == t )             // if process is to arrive in this time slot
            {
                istherenewarrival = true;
                index_of_running = i;
                pcs[i].waitingTime = t - pcs[i].arrivalTime; // time waited to start
                pcs[i].isRunning = true;
                pcs[i].remainingTime--;

                if (pcs[i].remainingTime == 0)
                {
                    pcs[i].isFinished = true;
                }
                pcs[i].isRunning = false;
            }
            sort(num_processes);            // sorts processes according to new remaining time
        }

        if(!istherenewarrival)
        {

        }


    }
    
    




}