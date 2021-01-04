#include "headers.h"

/* Modify this file as needed*/
int remainingtime;

int main(int agrc, char * argv[])
{
    initClk();
    printf("Created Process ID: %s\tRemaining Time: %s\n", argv[1], argv[2]);
    //TODO it needs to get the remaining time from somewhere
    //remainingtime = ??;
    while (1)//remainingtime > 0)
    {
        // remainingtime = ??;
    }
    
    destroyClk(false);
    
    return 0;
}
