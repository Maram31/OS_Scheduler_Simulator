#include "headers.h"

int remainingtime;

int main(int agrc, char * argv[])
{
    initClk();
    printf("\nProcess with running time %d received at %d\n", atoi(argv[1]), getClk());
    remainingtime =  atoi(argv[1]);
    
    while (remainingtime > 0)
    {
        //printf("Remaining time is reduced\n");
        sleep(1);
        remainingtime --;
    }


    printf("\nProcess with running time %d finished at %d\n", atoi(argv[1]), getClk());
       
 
    destroyClk(false);

    //printf("Process finished with pid %d\n", getppid()); 
    kill(getppid(), SIGUSR1);
  
    return 0;
}