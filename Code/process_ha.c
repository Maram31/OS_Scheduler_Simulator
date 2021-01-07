#include "headers.h"

int remainingtime;

int main(int agrc, char * argv[])
{
    initClk();
    printf("\nProcess with running time %d received at %d\n", atoi(argv[1]), getClk());
    remainingtime =  atoi(argv[1]);
    
    while (remainingtime > 0)
    {
        sleep(1);
        remainingtime --;
    }

    printf("\nProcess with running time %d finished at %d\n", atoi(argv[1]), getClk());
    
    destroyClk(false);
    
    return 0;
}