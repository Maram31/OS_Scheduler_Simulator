#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <signal.h>

#define BUF_SIZE 3                /* size of shared buffer */

#define MUTEX 0                   /* control access to critical section */
#define FULL 1                    /* counts full buffer slots */
#define EMPTY 2                   /* counts empty buffer slots */

int semid;

void handler(int signum);
int shmid_resources;
int shmid;
int shmid_num;
int shmid_consumers_count;
void *shmaddr_consumers_count;
int shmid_remove;
void *shmaddr_resources;

union semun {
	int val;                                        /* value for SETVAL */
	struct semid_ds *buf;	                	    /* buffer for IPC_STAT, IPC_SET */
	unsigned short *array;	                        /* array for GETALL, SETALL */
	struct seminfo *__buf;	                        /* buffer for IPC_INFO */
};

int sem_create(int nsems) {                     /* create semaphore */
	int  id;
	key_t key = 1234;
	int semflg = IPC_CREAT | 0666;
	id = semget(key, nsems, semflg);
	if(id < 0)
	{
		perror("semget:");
		exit (1);
	}
	return id;
}

void sem_initialise(int semno, int val) {           /* initialise semaphore */
	union semun un;
	un.val = val;
	if(semctl(semid, semno, SETVAL, un) < 0)
	{
		perror("semctl:");
		exit(2);
	}
}

void down(int semno) {          /* acquire resource */
	struct sembuf buf;
	buf.sem_num = semno;
	buf.sem_op = -1;
	buf.sem_flg = 0;
	if(semop(semid, &buf, 1) < 0) {
		perror("semop:");
		exit(2);
	}
}

void up(int semno) {            /* release resource */
	struct sembuf buf;
	buf.sem_num = semno;
	buf.sem_op = 1;
	buf.sem_flg = 0;
	if(semop(semid, &buf, 1) < 0)
	{
		perror("semop:");
		exit(2);
	}
}

int main (int argc, char *argv[])
{
    signal(SIGINT, handler);
    signal(SIGTSTP, handler);
    signal(SIGKILL, handler);

    key_t key_id = ftok("keyfile", 80);                                     
    shmid_resources = shmget(key_id, sizeof(int), IPC_CREAT | 0644);            /* create shared memory for resources */
    printf("shmid_resources: %d\n", shmid_resources);
    shmaddr_resources = shmat(shmid_resources, (void *)0, 0);
    if (shmaddr_resources == (void*)-1)
    {
        perror("Error in attach in consumer");
        exit(-1);
    }
    
    key_id = ftok("keyfile", 44);               
    shmid_consumers_count = shmget(key_id, sizeof(int), IPC_CREAT | 0644);      /* create shared memory for consumers' count */
    printf("shmid_consumers_count: %d\n", shmid_consumers_count);
    shmaddr_consumers_count = shmat(shmid_consumers_count, (void *)0, 0);
    if (shmaddr_consumers_count == (void*)-1)
    {
        perror("Error in attach in consumer");
        exit(-1);
    }
    (*(int*)shmaddr_consumers_count) ++;
    
    key_id = ftok("keyfile", 67);               
    shmid_remove = shmget(key_id, sizeof(int), IPC_CREAT | 0644);
    printf("shmid_remove: %d\n", shmid_remove);
    void *shmaddr_remove = shmat(shmid_remove, (void *)0, 0);
    if (shmaddr_remove == (void*)-1)
    {
        perror("Error in attach in consumer");
        exit(-1);
    }

    key_id = ftok("keyfile", 65);               
    shmid = shmget(key_id, BUF_SIZE*sizeof(int), IPC_CREAT | 0644);             /* create shared memory for buffer */
    printf("shmid: %d\n", shmid);

    key_id = ftok("keyfile", 60);               
    shmid_num = shmget(key_id, sizeof(int), IPC_CREAT | 0644);
    printf("shmid_num: %d\n", shmid_num);

    semid = sem_create(3);                                               /* creating 3 semaphores */
	if((*(int*)shmaddr_resources) == 0)
    {
        sem_initialise(MUTEX, 1);                                        /* semaphore mutex intialised to value of 1 */
        sem_initialise(FULL, 0);                                         /* semaphore full intialised to value of 0 */
        sem_initialise(EMPTY, BUF_SIZE);                                 /* semaphore empty intialised to value of BUF_SIZE */
    }

    void *shmaddr = shmat(shmid, (void *)0, 0);
    if (shmaddr == (void*)-1)
    {
        perror("Error in attach in consumer");
        exit(-1);
    }

    void *shmaddr_num = shmat(shmid_num, (void *)0, 0);
    if (shmaddr_num == (void*)-1)
    {
        perror("Error in attach in consumer");
        exit(-1);
    }
    (*(int*)shmaddr_resources) ++;

    int i;
    while(1)                                                             /* infinite loop */
    {
        if( (*(int*)shmaddr_num)> 0)
        {
            down(FULL);                                                  /* decrement count of full slots */
            sleep(rand()%10+1);
            down(MUTEX);                                                 /* enter critical section */
            int rem = (*(int*)shmaddr_remove);
            i = (*((int*)shmaddr+ rem));                                 /* take item from buffer */

            (*(int*)shmaddr_num) --;
            (*((int*)shmaddr+ rem)) = -1;                
            printf("Consumer: message recived with value %d\n", i);      /* consume item */
            rem = (rem+1) % BUF_SIZE;

            int j;  
            for(j = 0; j<BUF_SIZE; j++)                                  /* print out buffer contents */
            {
                printf("%d\t", (*((int*)shmaddr+ j)));
            }
            printf("\n");

            (*(int*)shmaddr_remove) = rem;
            up(MUTEX);                                                    /* leave critical section */
            up(EMPTY);                                                    /* increment count of empty slots */
        }
    }

	return 0;
}

void handler(int signum)
{
    (*(int*)shmaddr_resources) --;
    (*(int*)shmaddr_consumers_count) --;
    
    if((*(int*)shmaddr_resources) == 0)                                 /* clearing all resources if no one is using it anymore */
    {
        shmctl(shmid, IPC_RMID, (struct shmid_ds *)0);
        shmctl(shmid_num, IPC_RMID, (struct shmid_ds *)0);
        shmctl(shmid_consumers_count, IPC_RMID, (struct shmid_ds *)0);
        shmctl(shmid_resources, IPC_RMID, (struct shmid_ds *)0);
        shmctl(shmid_remove, IPC_RMID, (struct shmid_ds *)0);
        semctl(semid, 0, IPC_RMID);    
    }
    else if((*(int*)shmaddr_consumers_count) == 0)
    {
        shmctl(shmid_consumers_count, IPC_RMID, (struct shmid_ds *)0);
        shmctl(shmid_remove, IPC_RMID, (struct shmid_ds *)0);
    }
    raise(SIGTERM);                               
}