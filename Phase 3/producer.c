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

#define MUTEX 0
#define FULL 1
#define EMPTY 2

int semid;

void handler(int signum);
int shmid;
int shmid_num;
int shmid_resources;
int shmid_producers_count;
int shmid_add;
void *shmaddr_resources;
void *shmaddr_producers_count;

union semun {
	int val;                                        /* value for SETVAL */
	struct semid_ds *buf;	                	    /* buffer for IPC_STAT, IPC_SET */
	unsigned short *array;	                        /* array for GETALL, SETALL */
	struct seminfo *__buf;	                        /* buffer for IPC_INFO */
};

int sem_create(int nsems) {                              /* create semaphore */
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

void sem_initialise(int semno, int val) {                /* initialise semaphore */
	union semun un;
	un.val = val;
	if(semctl(semid, semno, SETVAL, un) < 0)
	{
		perror("semctl:");
		exit(2);
	}
}

void down(int semno) {                                   /* acquire resource */
	struct sembuf buf;
	buf.sem_num = semno;
	buf.sem_op = -1;
	buf.sem_flg = 0;
	if(semop(semid, &buf, 1) < 0) {
		perror("semop:");
		exit(2);
	}
}

void up(int semno) {                                     /* release resource */
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
    shmid_resources = shmget(key_id, sizeof(int), IPC_CREAT | 0644);                /* create shared memory for resources */
    printf("shmid_resources: %d\n", shmid_resources);

    shmaddr_resources = shmat(shmid_resources, (void *)0, 0);
    if (shmaddr_resources == (void*)-1)
    {
        perror("Error in attach in producer");
        exit(-1);
    }

    key_id = ftok("keyfile", 90);               
    shmid_producers_count = shmget(key_id, sizeof(int), IPC_CREAT | 0644);          /* create shared memory for producers' count */
    printf("shmid_producers_count: %d\n", shmid_producers_count);

    shmaddr_producers_count = shmat(shmid_producers_count, (void *)0, 0);
    if (shmaddr_producers_count == (void*)-1)
    {
        perror("Error in attach in producer");
        exit(-1);
    }
    (*(int*)shmaddr_producers_count) ++;

    key_id = ftok("keyfile", 77);               
    shmid_add = shmget(key_id, sizeof(int), IPC_CREAT | 0644);
    printf("shmid_add: %d\n", shmid_add);

    void *shmaddr_add = shmat(shmid_add, (void *)0, 0);
    if (shmaddr_add == (void*)-1)
    {
        perror("Error in attach in producer");
        exit(-1);
    }

    key_id = ftok("keyfile", 65);               
    shmid = shmget(key_id, BUF_SIZE*sizeof(int), IPC_CREAT | 0644);                  /* create shared memory for buffer */
    printf("shmid: %d\n", shmid);

    key_id = ftok("keyfile", 60);               
    shmid_num = shmget(key_id, sizeof(int), IPC_CREAT | 0644);                      
    printf("shmid_num: %d\n", shmid_num);
    
    semid = sem_create(3);
    if((*(int*)shmaddr_resources) == 0)                                               /* creating 3 semaphores */
    {
        sem_initialise(MUTEX, 1);                                                     /* semaphore mutex intialised to value of 1 */
        sem_initialise(FULL, 0);                                                      /* semaphore full intialised to value of 0 */
        sem_initialise(EMPTY, BUF_SIZE);                                              /* semaphore empty intialised to value of BUF_SIZE */
    }
    void *shmaddr = shmat(shmid, (void *)0, 0);
    if (shmaddr == (void*)-1)
    {
        perror("Error in attach in producer");
        exit(-1);
    }

    void *shmaddr_num = shmat(shmid_num, (void *)0, 0);
    if (shmaddr_num == (void*)-1)
    {
        perror("Error in attach in producer");
        exit(-1);
    }

    (*(int*)shmaddr_resources) ++;

    int i = 100 * (*(int*)shmaddr_producers_count);
    printf("Producer %d\n", i);
    while(1)                                                                    /* infinite loop */
    {
            if((*(int*)shmaddr_num) < BUF_SIZE)
           {
                down(EMPTY);                                                    /* decrement count of empty slots */
                sleep(rand()%10+1);
                down(MUTEX);                                                    /* enter critical section */

                int add = (*(int*)shmaddr_add);                         
                (*(int*)shmaddr_num) ++;
                (*((int*)shmaddr+ add)) = i ++;                                 /* add item to buffer */
                printf("Producer: message sent with value %d\n", i-1);          /* produce item */
                add = (add+1) % BUF_SIZE;

                int j;
                for(j = 0; j<BUF_SIZE; j++)                                     /* print out buffer contents */
                {
                    printf("%d\t", (*((int*)shmaddr+ j)));
                }
                printf("\n");

                (*(int*)shmaddr_add) = add;
                up(MUTEX);                                                      /* leave critical section */
                up(FULL);                                                       /* increment count of full slots */
           }        
        
    }

	return 0;
}

void handler(int signum)
{
    (*(int*)shmaddr_resources) --;
    (*(int*)shmaddr_producers_count) --;

    if((*(int*)shmaddr_resources) == 0)                                         /* clearing all resources if no one is using it anymore */
    {
        shmctl(shmid, IPC_RMID, (struct shmid_ds *)0);
        shmctl(shmid_num, IPC_RMID, (struct shmid_ds *)0);
        shmctl(shmid_producers_count, IPC_RMID, (struct shmid_ds *)0);
        shmctl(shmid_resources, IPC_RMID, (struct shmid_ds *)0);
        shmctl(shmid_add, IPC_RMID, (struct shmid_ds *)0);
        semctl(semid, 0, IPC_RMID);    
    }
    else if((*(int*)shmaddr_producers_count) == 0)
    {
        shmctl(shmid_producers_count, IPC_RMID, (struct shmid_ds *)0);
        shmctl(shmid_add, IPC_RMID, (struct shmid_ds *)0);
    }
    raise(SIGTERM);                       
}